// Interface to the serial port system
// Copyright (C) 2013 Noisebridge. All Rights Reserved
// Author: Brandon Edens
// Date: 2013-02-13

#ifndef SERIAL_PORT_H_
#define SERIAL_PORT_H_

void serial_port_close();
void serial_port_open(char *filename, int baud_rate);

#endif /* SERIAL_PORT_H_ */
