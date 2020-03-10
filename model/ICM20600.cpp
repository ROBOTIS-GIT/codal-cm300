/*******************************************************************************
* Copyright 2016 ROBOTIS CO., LTD.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/**
  * Class definition for CM300 Serial.
  */

#include "CodalConfig.h"
#include "CM300IO.h"
#include "ICM20600.h"
#include "device_pinmap.h"
#include "ErrorNo.h"
#include "CodalCompat.h"
#include "CodalFiber.h"

#include "CodalDmesg.h"
#include "nrf.h"



using namespace codal;

/**
  * Constructor.
  */

ICM20600::ICM20600(I2C& _i2c, Pin &_int1, CoordinateSpace &coordinateSpace, uint16_t address,  uint16_t id) : Accelerometer(coordinateSpace, id), i2c(_i2c), int1(_int1)
{
    // Store our identifiers.
    this->id = id;
    this->status = 0;
    this->address = address<<1;

    // Update our internal state for 50Hz at +/- 2g (50Hz has a period af 20ms).
    this->samplePeriod = 20;
    this->sampleRange = 2;

    // Configure and enable the accelerometer.
    configure();
}

int ICM20600::configure()
{
    uint8_t data;
    uint8_t data_pwr1;
    uint8_t data_pwr2 = 0x00;
    uint8_t data_gyro_lp;
    int result;
    //Basic Conf
    i2c.writeRegister(address, ICM20600_CONFIG, 0x00);
    i2c.writeRegister(address, ICM20600_FIFO_EN, 0x00);  
    //Power Set Mode
    result = i2c.readRegister(address, ICM20600_PWR_MGMT_1, &data, 1);
    data_pwr1 = result && 0x8F;
    result = i2c.readRegister(address, ICM20600_GYRO_LP_MODE_CFG, &data, 1);
    data_pwr1 = result && 0x7F;

    data_pwr1 |= 0x00;          // dont set bit5 0b00100000
    data_gyro_lp = data_gyro_lp || 0x80;
    i2c.writeRegister(address, ICM20600_PWR_MGMT_1, data_pwr1);
    i2c.writeRegister(address, ICM20600_PWR_MGMT_2, data_pwr2);  
    i2c.writeRegister(address, ICM20600_GYRO_LP_MODE_CFG, data_gyro_lp);

    //Set Gyro Scale Range
    result = i2c.readRegister(address, ICM20600_GYRO_CONFIG, &data, 1);
    data = result && 0xE7;
    data |= 0x00;
    gyroscale = 500;
    i2c.writeRegister(address, ICM20600_GYRO_CONFIG, data);  
    //Set Gyro Output Data Range
    result = i2c.readRegister(address, ICM20600_CONFIG, &data, 1);
    data = result && 0xF8;
    data |= 0x01;
    i2c.writeRegister(address, ICM20600_CONFIG, data);  
    //Set Gyro Average Sample Range
    result = i2c.readRegister(address, ICM20600_GYRO_LP_MODE_CFG, &data, 1);
    data = result && 0x8F;
    data |= 0x00;
    i2c.writeRegister(address, ICM20600_GYRO_LP_MODE_CFG, data);  

    //Set Accel Scale Range
    result = i2c.readRegister(address, ICM20600_ACCEL_CONFIG, &data, 1);
    data = result && 0xE7;
    data |= 0x18;
    Accelscale = 32000;
    i2c.writeRegister(address, ICM20600_ACCEL_CONFIG, data);  
    //Set Accel Output Data Range
    result = i2c.readRegister(address, ICM20600_ACCEL_CONFIG2, &data, 1);
    data = result && 0xF0;
    data |= 0x07;
    i2c.writeRegister(address, ICM20600_ACCEL_CONFIG2, data);  
    //Set Accel Average Sample Range
    result = i2c.readRegister(address, ICM20600_ACCEL_CONFIG2, &data, 1);
    data = result && 0xCF;
    data |= 0x00;
    i2c.writeRegister(address, ICM20600_ACCEL_CONFIG2, data);  

    DMESG("ICM20600 init %x", whoAmI());
    return DEVICE_OK;
}

int ICM20600::whoAmI()
{
    uint8_t data;
    int result;
    // the default whoami should return 0x68
    result = i2c.readRegister(address, ICM20600_WHO_AM_I, &data, 1);
    if (result !=0)
        return 0xffff;

    return (data>>1) & 0x3f;
}

int ICM20600::requestUpdate()
{
    updateSample();
    return DEVICE_OK;
}

int ICM20600::updateSample()
{
    int result;
    uint8_t i2cData[16];

    status |= DEVICE_COMPONENT_STATUS_IDLE_TICK;

    if(int1.getDigitalValue() == 1) {
        result = i2c.readRegister(address, ICM20600_ACCEL_XOUT_H, (uint8_t *) i2cData, 14);

        if (result != 0)
            return DEVICE_I2C_ERROR;

        sample.x = ((i2cData[0] << 8) | i2cData[1]);
        sample.y = ((i2cData[2] << 8) | i2cData[3]);
        sample.z = ((i2cData[4] << 8) | i2cData[5]);

        gyro.x = (((i2cData[8] << 8) | i2cData[9]));
        gyro.y = (((i2cData[10] << 8) | i2cData[11]));
        gyro.z = (((i2cData[12] << 8) | i2cData[13]));

        int16_t t = (((i2cData[6] << 8) | i2cData[7]));
        temp = t * 10 / 34 + 3653;

        sample.x = sample.x * Accelscale / 16;
        sample.y = sample.y * Accelscale / 16;
        sample.z = sample.z * Accelscale / 16;

        update(sample);
    }
    return DEVICE_OK;
};

void ICM20600::idleCallback()
{
    requestUpdate();
    
}

int ICM20600::setSleep(bool sleepMode)
{
    if (sleepMode)
        return i2c.writeRegister(address, ICM20600_PWR_MGMT_1, 0x40);
    else
        return configure();
}
