#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <inttypes.h>

#define ADDRESS_ACCELEROMETER 0x19
#define ADDRESS_COMPASS 0x1e

#define LSM303_REGISTER_ACCEL_CTRL_REG1_A 0x20
#define LSM303_REGISTER_ACCEL_OUT_X_L_A 0x28
#define LSM303_REGISTER_ACCEL_OUT_X_H_A 0x29
#define LSM303_REGISTER_ACCEL_OUT_Y_L_A 0x2A
#define LSM303_REGISTER_ACCEL_OUT_Y_H_A 0x2B
#define LSM303_REGISTER_ACCEL_OUT_Z_L_A 0x2C
#define LSM303_REGISTER_ACCEL_OUT_Z_H_A 0x2D


int main()
{
	int fd;
	int adapter_nr = 1;	/* probably dynamically determined */
	char filename[20];

	snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
	fd = open(filename, O_RDWR);
	if (fd == -1) {
		perror("open");
		exit(1);
	}

	if (ioctl(fd, I2C_SLAVE, ADDRESS_ACCELEROMETER) == -1) {
		perror("ioctl");
		exit(1);
	}

	if (i2c_smbus_write_byte_data(fd, LSM303_REGISTER_ACCEL_CTRL_REG1_A, 0x57) < 0) {
		perror("i2c_smbus_write_byte_data");
		exit(1);
	}

	uint16_t x, y, z;
	for (;;) {
		int result;

		result = i2c_smbus_read_byte_data(fd, LSM303_REGISTER_ACCEL_OUT_X_H_A);
		if (result == -1) {
			perror("i2c_smbus_read_byte_data");
			exit(1);
		}
		x = result;
		x <<= 8;

		result = i2c_smbus_read_byte_data(fd, LSM303_REGISTER_ACCEL_OUT_X_L_A);
		if (result == -1) {
			perror("i2c_smbus_read_byte_data");
			exit(1);
		}
		x |= result;

		printf("x accel: %" PRId16 "\n", (int16_t) x);
		usleep(100000);
	}

	return 0;
}
