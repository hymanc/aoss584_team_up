/**
 * lsm303.h
 * LSM303 Accelerometer/Magnetometer Library
 * for ChibiOS
 * Author: Cody Hyman
 */

#ifndef _LSM303_H_
#define _LSM303_H_

#include "i2c_sensor.h"

// Software Buffer sizes
#define ACC_BUFFER_SIZE 16
#define MAG_BUFFER_SIZE 8

#define LSM303_ACC_ADDR 0b0011101
#define LSM303_MAG_ADDR 0b0011110
/**
 * Register Definitions
 */
// Accelerometer
#define WHO_AM_I_A		0x0F
#define ACT_THS_A		0x1E
#define ACT_DUR_A		0x1F
#define CTRL_REG1_A		0x20
#define CTRL_REG2_A		0x21
#define CTRL_REG3_A		0x22
#define CTRL_REG4_A		0x23
#define CTRL_REG5_A		0x24
#define CTRL_REG6_A		0x25
#define CTRL_REG7_A		0x26
#define STATUS_REG_A	0x27
#define OUT_X_L_A		0x28
#define OUT_X_H_A		0x29
#define OUT_Y_L_A		0x2A
#define OUT_Y_H_A		0x2B
#define OUT_Z_L_A		0x2C
#define OUT_Z_H_A		0x2D
#define FIFO_CTRL		0x2E
#define FIFO_SRC		0x2F
#define IG_CFG1_A		0x30
#define IG_SRC1_A		0x31
#define IG_THS_X1_A		0x32
#define IG_THS_Y1_A		0x33
#define IG_THS_Z1_A		0x34
#define IG_DUR1_A		0x35
#define IG_CFG2_A		0x36
#define IG_SRC2_A		0x37
#define IG_THS2_A		0x38
#define IG_DUR2_A		0x39
#define XL_REFERENCE	0x3A
#define XH_REFERENCE	0x3B
#define YL_REFERENCE	0x3C
#define YH_REFERENCE	0x3D
#define ZL_REFERENCE	0x3E
#define ZH_REFERENCE	0x3F

// Magnetometer
#define WHO_AM_I_M		0x0F
#define CTRL_REG1_M		0x20
#define CTRL_REG2_M		0x21
#define CTRL_REG3_M		0x22
#define CTRL_REG4_M		0x23
#define CTRL_REG5_M		0x24
#define STATUS_REG_M	0x27
#define OUT_X_L_M		0x28
#define OUT_X_H_M		0x29
#define OUT_Y_L_M		0x2A
#define OUT_Y_H_M		0x2B
#define OUT_Z_L_M		0x2C
#define OUT_Z_H_M		0x2D
#define TEMP_L_M		0x2E
#define TEMP_H_M		0x2F
#define INT_CFG_M		0x30
#define INT_SRC_M		0x31
#define INT_THS_L_M		0x32
#define INT_THS_H_M		0x33

// X, Y, Z channel
#define X_CH 0
#define Y_CH 1
#define Z_CH 2

typedef struct
{
    // TODO: Mutex?
    I2CSensor_t accSensor;
    I2CSensor_t magSensor;
    //int16_t acc_buffer[3][ACC_BUFFER_SIZE];
    //int16_t mag_buffer[3][MAG_BUFFER_SIZE];
    int8_t abuf_counter;
    int8_t mbuf_counter;
    uint8_t txaBuffer[10];
    uint8_t txmBuffer[10];
    uint8_t rxaBuffer[16];
    uint8_t rxmBuffer[16];
}lsm303_t;

msg_t lsm303_init(lsm303_t *lsm, I2CDriver *driver);
msg_t lsm303_stop(lsm303_t *lsm);
msg_t lsm303_readAcceleration(lsm303_t *lsm, uint8_t nread);
msg_t lsm303_readMagnetometer(lsm303_t *lsm, uint8_t nread);


#endif