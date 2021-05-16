#ifndef LSM6DS33_H__
#define LSM6DS33_H__

#include<xc.h> // processor SFR definitions

#define IMU_ADDR 0x6A<<1
#define WHO_AM_I 0x0F
#define CTRL1_XL 0x10
#define CTRL2_G 0x11
#define CTRL3_C 0x12
#define OUT_TEMP_L 0x20

void imu_setup();
void imu_read(unsigned char address, unsigned char reg, signed short * data, int len);

#endif