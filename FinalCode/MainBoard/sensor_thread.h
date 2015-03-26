/**
 * Sensor Data Collection Thread
 * Stribog - Balloon Computer
 * Team Up
 * Author: Cody Hyman
 */

#ifndef _SENSOR_THREAD_H_
#define _SENSOR_THREAD_H_

#include "ch.h"
#include "hal.h"
#include "chmtx.h"

#include "Drivers/bmp280.h"
#include "Drivers/lsm303.h"
#include "Drivers/ms5607.h"
#include "Drivers/si70x0.h"
#include "Drivers/tmp275.h"
#include "Drivers/rtd.h"

typedef struct
{
    mutex_t mtx; // Mutex
    float tempBmp;
    float temp275;
    float tempRtd;
    float pressBmp;
    float pressMpxm;
    float pressMs5607;
    float humdInt, humdExt, humd6030;
    float accX, accY, accZ;
    float magX, magY, magZ;
}sensorData_t;

// Sensor Thread
typedef struct
{
    uint16_t sleepTime;
    bool running;
    sensorData_t data;
    I2CDriver *i2c;
    ADCDriver *adc;
    
    bmp280_t bmp280;
    lsm303_t lsm303;
    ms5607_t ms5607;
    si70x0_t si7020;
    rtd_t rtd;
    tmp275_t tmp275;
}sensorThread_t;


msg_t sensorThread(void *arg);
msg_t sensorThreadStop(sensorThread_t *thread);
int8_t getSensorData(sensorThread_t *thread, sensorData_t *buffer);

#endif