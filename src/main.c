// Copyright (C) 2013. Brandon Edens. All Rights Reserved
// Author: Brandon Edens
// 2013-02-13

#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "serial_port.h"
#include "log.h"

/** The default baud rate to use for the serial reader. */
#define DEFAULT_BAUD (57600)

/** Maximum number of characters available for filename path */
#define FILENAME_NCHARS (1024)

/** The number of nanoseconds in a given microsecond. */
#define NSEC_PER_MSEC (1000000L)

/** Flag that indicates we're to enter multicast mode. */
static bool multicast = false;

/** Pointer to the program name. */
static char *prog_name;

/** Flag that indicates whether or not the software should terminate. */
static volatile bool shutdown = false;

static void termination_handler(int signum);
static bool file_exists(char *filename);


/****************************************************************************/

static bool file_exists(char *filename) {
    if (filename == NULL) {
        return false;
    }
    return access(filename, R_OK) == 0;
}

/** Properly shutdown the software upon receiving term signal. */
static void termination_handler(int signum) {
    shutdown = true;
}

static void usage(void) {
    printf("Usage:\n");
    printf("%s --serial=/dev/ttyUSB0 [--baud=57600]\n", prog_name);
}

int main(int argc, char *argv[]) {
    prog_name = argv[0];
    if (argc == 1) {
        usage();
        return EXIT_SUCCESS;
    }
    static struct option longopts[] = {
        {"serial",    required_argument, NULL, 's'},
        {"baud",      optional_argument, NULL, 'b'},
        {"multicast", no_argument,       NULL, 'm'},
        {NULL,        0,                 NULL, 0  }
    };
    int baud_rate = DEFAULT_BAUD;
    char serial_port[FILENAME_NCHARS];
    memset(serial_port, 0, sizeof(serial_port));
    int ch;
    while ((ch = getopt_long(argc, argv, "s:b:m", longopts, NULL)) != -1) {
        switch (ch) {
            case 's':
                strncpy(serial_port, optarg, FILENAME_NCHARS);
                break;
            case 'b':
                baud_rate = atoi(optarg);
                break;
            case 'm':
                multicast = true;
            default:
                usage();
        }
    }
    if (*serial_port == 0) {
        fprintf(stderr, "You must specify a valid serial port.\n");
        usage();
        return EXIT_FAILURE;
    } else if (!file_exists(serial_port)) {
        fprintf(stderr, "Serial port %s does not exist.\n", serial_port);
        usage();
        return EXIT_FAILURE;
    }

    // Register signal handler for proper program termination
    struct sigaction new_action;
    struct sigaction old_action;
    new_action.sa_handler = termination_handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;

    sigaction(SIGINT, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN) {
        sigaction(SIGINT, &new_action, NULL);
    }
    sigaction(SIGHUP, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN) {
        sigaction(SIGHUP, &new_action, NULL);
    }
    sigaction(SIGTERM, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN) {
        sigaction(SIGTERM, &new_action, NULL);
    }

    // Open the serial port.
    serial_port_open(serial_port, baud_rate);

    // Begin sleeping while serial port operates.
    log_info("Entering the sleep loop.");
    struct timespec req;
    req.tv_sec = 0;
    req.tv_nsec = 10 * NSEC_PER_MSEC;
    do {
        nanosleep(&req, NULL);
    } while (!shutdown);

    // Close serial port and clean up.
    serial_port_close();
    return EXIT_SUCCESS;
}
