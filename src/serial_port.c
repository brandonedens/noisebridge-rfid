// Implementation of serial port handling
// Copyright (C) 2013 Noisebridge. All Rights Reserved
// Author: Brandon Edens
// Date: 2013-02-13

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include "serial_port.h"
#include "log.h"

/** Delay between read attempts. */
#define DELAY_READ_MSEC (10000)

/** Maximum number of characters available in the read buffer. */
#define BUF_NCHARS (8192)

/** Buffer to hold read serial data. */
char read_buf[BUF_NCHARS];

/** Index of current location being written to in the read buf. */
int read_buf_idx;

/** Index of current location being written to in the read buf. */
int read_buf_idx;

/** Handle to the file descriptor for the serial port. This value is 0 if the
 * serial port is not open.
 */
static volatile int serial_fd = 0;

/** Thread to continuously read data from the port. */
static pthread_t read_thread;

/** Disable the run thread. */
static volatile bool shutdown = false;

/** Function that reads data from the port until close or shutdown. */
static void *run_read_thread(void *unused);

/** Enable / disable blocking of the port for the given descriptor. */
void set_blocking(int fd, bool should_block);

int set_interface_attribs(int fd, int speed, int parity);


/****************************************************************************/

static void *run_read_thread(void *unused) {
    char buf[BUF_NCHARS];
    int avail = 0;
    ssize_t nread = 0;
    do {
        ioctl(serial_fd, FIONREAD, &avail);
        if (avail == 0) {
            usleep(DELAY_READ_MSEC);
            continue;
        }
        size_t to_read = avail < sizeof(buf) ? avail : sizeof(buf);
        nread = read(serial_fd, buf, to_read);
        if (nread > 0) {
            log_info("Read %ld bytes from serial port.", nread);
            log_info("Read data was: %s.", buf);
        }
    } while (!shutdown && serial_fd > 0);
    log_info("Exiting the run read thread shutdown: %d fd: %d",
             shutdown, serial_fd);
    return NULL;
}

/** Close open the serial port. */
void serial_port_close() {
    log_info("Shutting down serial port.");
    shutdown = true;
    if (serial_fd == 0) {
        log_err("Attempt to close already closed serial port.");
    }
    if (pthread_join(read_thread, NULL) != 0) {
        log_err("Failure to join the read thread.");
    }
    close(serial_fd);
    serial_fd = 0;
}

/** Open the serial port for the given filename with specified baud. */
void serial_port_open(char *filename, int baud_rate) {
    serial_fd = open(filename, O_RDWR | O_NOCTTY | O_NDELAY);
    log_debug("file descriptor is: %d", serial_fd);
    if (serial_fd < 0) {
        perror("Failure to open serial port.");
        exit(EXIT_FAILURE);
    }
    set_interface_attribs(serial_fd, baud_rate, 0);
    set_blocking(serial_fd, false);
    // Start the read thread.
    pthread_create(&read_thread, NULL, run_read_thread, NULL);
}

/** Enable or disable blocking for the serial port on the given file
 * descriptor.
 */
void set_blocking(int fd, bool should_block) {
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        log_err("Failure to %d from tggetattr", errno);
        return;
    }
    tty.c_cc[VMIN]  = should_block ? 1 : 0;
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        log_err("Failure to %d setting term attributes", errno);
    }
}

/** Set the interface attributes of the given file descriptor for a serial
 * port. Here we specify the speed of the port and the parity. A fuller
 * description of the options here can be found in the Linux Serial Programming
 * HOWTO.
 */
int set_interface_attribs(int fd, int speed, int parity) {
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        log_err("error %d from tcgetattr", errno);
        return -1;
    }
    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~IGNBRK;         // ignore break signal
    tty.c_lflag = 0;                // no signaling chars, no echo,
    // no canonical processing
    tty.c_oflag = 0;                // no remapping, no delays
    tty.c_cc[VMIN]  = 0;            // read doesn't block
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        log_err("error %d from tcsetattr", errno);
        return -1;
    }
    return 0;
}
