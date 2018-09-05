#include "GPIO.h"
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
// For GPIO PORTS
#define IN   0
#define OUT  1
#define LOW  0
#define HIGH 1

#define PIN27  21
#define PIN26  20
#define PIN25  16
#define PIN24  12
#define BUFFER_MAX 3
#define VALUE_MAX    35
#define DIRECTION_MAX 35

using namespace std;

GPIO::GPIO()
{
    GPIOExport(PIN24);
    GPIOExport(PIN25);
    GPIOExport(PIN26);
    GPIOExport(PIN27);
	/*
	 * Set GPIO directions
	 */
	GPIODirection(PIN24, OUT);
	GPIODirection(PIN25, OUT);
    GPIODirection(PIN26, OUT);
    GPIODirection(PIN27, OUT);

}




void GPIO::gpioOn()
{

	 GPIOWrite(PIN24 , 0);
	 GPIOWrite(PIN25 , 0);
	 GPIOWrite(PIN26 , 0);
	 GPIOWrite(PIN27 , 0);
}


void  GPIO:: gpioOff  ()
{

	 GPIOWrite(PIN24 , 1);
	 GPIOWrite(PIN25 , 1);
	 GPIOWrite(PIN26 , 1);
     GPIOWrite(PIN27 , 1);
}



int  GPIO:: GPIOExport(int pin)
{

	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open export for writing!\n");
		return(-1);
	}

	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}

int  GPIO::GPIOUnexport(int pin)
{
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open unexport for writing!\n");
		return(-1);
	}

	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}

int  GPIO:: GPIODirection(int pin, int dir)
{
	static const char s_directions_str[]  = "in\0out";

	char path[DIRECTION_MAX];
	int fd;

	snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio direction for writing!\n");
		return(-1);
	}

	if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3)) {
		fprintf(stderr, "Failed to set direction!\n");
		return(-1);
	}

	close(fd);
	return(0);
}


int  GPIO::GPIOWrite(int pin, int value)
{
	static const char s_values_str[] = "01";

	char path[VALUE_MAX];
	int fd;

	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio value for writing!\n");
		return(-1);
	}

	if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1)) {
		fprintf(stderr, "Failed to write value!\n");
		return(-1);
	}

	close(fd);
	return(0);
}



GPIO::~GPIO()
{

    GPIOUnexport(PIN24);
    GPIOUnexport(PIN25);
    GPIOUnexport(PIN26);
    GPIOUnexport(PIN27);

}
