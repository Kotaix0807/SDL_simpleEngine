#ifndef ARDUINO_H
#define ARDUINO_H

#include <stdbool.h>

#define ARDUINO_PORT "/dev/ttyUSB0"

extern int serial_fd;
extern int light_level;

bool arduinoConnect();
void arduinoDisconnect();
void getLightLevel(int *light);

#endif