
/**
  Modified library for MAX11615EEE+ in OLA firmware
 */

#ifndef MAX11615_H
#define MAX11615_H

#include <Arduino.h>
#include <Wire.h>

/** @see Datasheet p.19 Table 6
  */
enum class MAX11615Register : uint8_t
{
	REF_VDD      =0x00,
	REF_EXTERNAL =0x02,
	REF_INTERNAL =0x04,
	ANANLOG_IN   =0x00,
	REF_OUT      =0x02,
	INT_REF_ON   =0x01,
};

class MAX11615
{
private:
	//This stores the requested i2c port
    TwoWire * _i2cPort = NULL;
	
	//This stores the i2c address
    uint8_t _MAX11615Address;	
public:
	MAX11615();
	
	bool begin(uint8_t address = 0x33, TwoWire &wirePort = Wire);
	bool detection();
	
	uint8_t init(uint8_t);
	uint8_t setup(uint8_t);
	uint8_t configuration(uint8_t);
	
	void write8(MAX11615Register reg, uint8_t value);
	uint8_t read8(MAX11615Register reg);
	
	uint8_t ADCRead(uint8_t, uint16_t*);
	uint8_t scan(uint16_t*);
};
#endif 
