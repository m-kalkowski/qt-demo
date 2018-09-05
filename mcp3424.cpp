#include "mcp3424.h"
#include <linux/i2c-dev.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>

mcp3424::mcp3424(){

	fd = open("/dev/i2c-1", O_RDWR);
	if (fd == -1) {
		printf("error: open: %s\n", strerror(errno));

	}
     init(0x68, MCP3424_RESOLUTION_14);
	 set_conversion_mode(MCP3424_CONVERSION_MODE_CONTINUOUS);
}

void mcp3424::init(  uint8_t addr_, enum mcp3424_resolution res)
{
	addr = addr_;
	config = 0x00;
	err = MCP3424_OK;
	set_channel(MCP3424_CHANNEL_1);
	set_conversion_mode( MCP3424_CONVERSION_MODE_ONE_SHOT);
	set_pga(MCP3424_PGA_1X);
	set_resolution( res);
}

void mcp3424::set_conversion_mode(enum mcp3424_conversion_mode mode) {
	config &= ~0x10;
	config |= (mode << 4);
}
void mcp3424::set_channel(enum mcp3424_channel channel) {
	config &= ~0x60;
	config |= (channel << 5);
}
void mcp3424::set_pga(enum mcp3424_pga pga) {
	config &= ~0x03;
	config |= pga;
}
void mcp3424::set_resolution(enum mcp3424_resolution res) {
	config &= ~0x0c;
	config |= (res << 2);
}

void mcp3424::set_errstr(const char *format, ...) {
	va_list ap;
	va_start(ap, format);
	vsnprintf(errstr, MCP3424_ERR_LEN, format, ap);
	va_end(ap);
}

int mcp3424::get_raw( enum mcp3424_channel channel) {
	int rv;
	ssize_t n;
	uint8_t reading[4];
	unsigned int raw;

	rv = ioctl(fd, I2C_SLAVE, addr);
	if (rv == -1) {
		set_errstr("ioctl: %s", strerror(errno));
        err = MCP3424_ERR;
		return 0;
	}

	set_channel(channel);

	// if one shot, write ready bit to start new conversion on mcp3424
	if (((config >> 4) & 0x03) == MCP3424_CONVERSION_MODE_ONE_SHOT) {
		config |= (1 << 7);
	}
    void * mypointer = &config;
	write(fd, mypointer, 1);
/*	if (n < 1) {
		if (n == 0) {
			set_errstr( "failed to write config byte");
			err = MCP3424_WARN;
		} else if (n == -1) {
			set_errstr("write: %s", strerror(errno));
			err = MCP3424_ERR;
		}
		return 0;
	} */

	if (((config >> 4) & 0x03) == MCP3424_CONVERSION_MODE_ONE_SHOT) {
		config &= ~(1 << 7);
	}

	while (1) {
		n = read(fd, reading, 4);
		if (n < 4) {
			if (n >= 0) {
				set_errstr( "failed to read 4 byte reading");
				err = MCP3424_WARN;
			} else if (n == -1) {
				set_errstr( "read: %s", strerror(errno));
				err = MCP3424_ERR;
			}
			return 0;
		}

		// loop until ready bit is 0 (new reading)
		if (((config >> 2) & 0x03) == MCP3424_RESOLUTION_18) {
			if ((reading[3] >> 7) == 0) {
				break;
			}
		} else {
			if ((reading[2] >> 7) == 0) {
				break;
			}
		}
	}

	switch ((config >> 2) & 0x03) {
		case MCP3424_RESOLUTION_12:
			raw = ((reading[0] & 0x0f) << 8) | reading[1];
			break;
		case MCP3424_RESOLUTION_14:
			raw = ((reading[0] & 0x3f) << 8) | reading[1];
			break;
		case MCP3424_RESOLUTION_16:
			raw = (reading[0] << 8) | reading[1];
			break;
		case MCP3424_RESOLUTION_18:
			raw = ((reading[0] & 0x03) << 16) | (reading[1] << 8) | reading[2];
			break;
		default:
			set_errstr( "invalid resolution");
			err = MCP3424_ERR;
			return 0;
	}

	return raw;
}


void mcp3424::print(){

    for (int channel = 0; channel < 4; channel++) {
        res[channel] = get_raw( mcp3424_channel(channel));
        if (err) {
            printf("error: mcp3424_get_raw: %s\n", errstr);
            close(fd);
        }
        voltage[channel] =  ( (V_MAX * res[channel]) / RAW_MAX_14 );
    }

    for (int channel = 0; channel < 4 ; channel++) {
        printf("resolution[%d] = %u, v[%d] = %0.2f\n", channel, res[channel], channel, voltage[channel]);
    }
    printf("\n");

}

mcp3424::~mcp3424()
{
    close(fd);
}
