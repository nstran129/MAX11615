/**
 Modified library for MAX116115EEE+ in OLA firmware
 */
#include "MAX11615.h"
#include <Wire.h>

//Class constructor
MAX11615::MAX11615() {}

bool MAX11615::begin(uint8_t address, TwoWire &wirePort)
{
	_i2cPort = &wirePort; //Choose the wire port of the deivice
	_MAX11615Address = address; //Set the address of the device
	
	//make sure the TMP will acknowledge over I2c
	_i2cPort->beginTransmission((uint8_t)_MAX11615Address);
	if (_i2cPort->endTransmission() != 0)
	{
		return false;
	}	
	
//	if (detection() != true)
//	{
//		return false;
//	}
	return (true);
}

bool MAX11615::detection()
{
	init(4+2+1);
	uint16_t val1[8];
	uint16_t val2[8];
	int i;
	scan(val1);
	scan(val2);
	for (i=0;i<8;i++) {
		if (val1[i] != val2[i])
		{
			return false;
		}
	}
	return (true);
}

void MAX11615::write8(MAX11615Register reg, uint8_t value)
{
 	_i2cPort->beginTransmission((uint8_t)_MAX11615Address);
	_i2cPort->write(static_cast<uint8_t>(reg));
	_i2cPort->write(value);
	_i2cPort->endTransmission();
}

uint8_t MAX11615::read8(MAX11615Register reg)
{
	_i2cPort->beginTransmission((uint8_t)_MAX11615Address);
	_i2cPort->write(static_cast<uint8_t>(reg));
	_i2cPort->endTransmission();
	_i2cPort->requestFrom((uint8_t)_MAX11615Address, (uint8_t)1);
	return _i2cPort->read();
}


uint8_t MAX11615::init(uint8_t vRef)
{
	uint8_t ret = 0;
	// 0 - don't care
	// 1 - reset configuration register to default
	// 2 - unipolar
	// 3 - internal clock
	// 4 - SEL0 (vRef)
	// 5 - SEL1 (vRef)
	// 6 - SEL2 (vRef)
	vRef = (vRef<<4) & 0xf0;
	vRef |= 2; // do not reset the setup register
	ret += setup(vRef);
	// 0 - Single Ended
	// 1 to 4 - Channel Select:  7
	// 5 to 6 - Scan Mode: read channels up to selected channel
	uint8_t config_byte = 0x00;
	config_byte |= 1; // single ended mode
	config_byte |= ((1<<5) & (1<<6)); // SCAN bits: convert only the channel selected by CS bits
	ret += configuration(config_byte);
	return ret;
}

uint8_t MAX11615::setup(uint8_t data)
{
	data = data | 0x80; // make REG bit 7 = 1 (setup byte)
	_i2cPort->beginTransmission((uint8_t)_MAX11615Address);
	_i2cPort->write(data);
	if(_i2cPort->endTransmission() != 0)
	{
		return 1;
	}
	return 0;
}

uint8_t MAX11615::configuration(uint8_t data)
{
	data = data & (~0x80); // make REG bit 7 = 0 (configuration byte)
	_i2cPort->beginTransmission((uint8_t)_MAX11615Address);
	_i2cPort->write(data);
	if(_i2cPort->endTransmission() != 0)
	{
		return 1;
	}
	return 0;
}

//Read one channel
uint8_t MAX11615::ADCRead(uint8_t channel, uint16_t* val)
{
	uint8_t result[2] = {0,0};
	uint8_t configurationByte = ( (channel<<1) & 0x0e) | 0x61;
	configuration(configurationByte);
	// the conversion consists of two bytes per channel
	_i2cPort->beginTransmission((uint8_t)_MAX11615Address);
	_i2cPort->requestFrom((uint8_t)_MAX11615Address, 2);
	for(int i=0; i<2; i++){
			result[i] = _i2cPort->read();
	}
	if(_i2cPort->endTransmission() != 0)
	{
		return 1;
	}
	uint16_t value = 0;
	// cast to uint16_t is necessary to not loose the values by the left shift
	value =  (((uint16_t)result[0] & 0x000f) << 8); // MSB is returned first
	value += ((uint16_t)result[1] & 0x00ff); // read LSB
	*val = value;
	return 0;
}

//Read all channel
uint8_t MAX11615::scan(uint16_t* buffer)
{
	uint8_t ret = 0;
	uint8_t configurationByte = 0xf0;
	ret += configuration(configurationByte);
	// 2 bytes per channel. There are 8 channels
	for(uint8_t i = 0;i<8;i++){
		ret += ADCRead(i, buffer+i);
	}
	return ret;
}
