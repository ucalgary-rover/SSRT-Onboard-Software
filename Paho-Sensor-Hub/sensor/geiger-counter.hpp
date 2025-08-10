#ifndef GEIGER_COUNTER_H
#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define PORT "/dev/serial/by-id/usb-1a86_USB_Serial-if00-port0" // replace with actual port
#define BAUD_RATE B115200   // based on geiger counter specs

int setup();
unsigned char readGeiger(int serialPort);

#endif
