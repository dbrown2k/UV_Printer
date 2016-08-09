/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MCP23S17_DRV8880.h
 * Author: David
 *
 * Created on 24 July 2016, 11:19
 */

#ifndef MCP23S17_DRV8880_H
	#define MCP23S17_DRV8880_H
#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

//initialise 
void setup_spi();


//initialise port multiplier
void init_MCP23S17();

/*set switch to enable/disable motors between steps to reduce power usage
function to take argument of direction (A/B), step size (A/B), number of steps (A/B)
for port A or B motors, to allow stepping of both at the same time.
*/
void stepAB(uint8_t command);



//component functions for SPI functions
void digitalSPIWrite(uint8_t addr, uint8_t reg, uint8_t data);
void bitWrite(uint8_t * inByte, uint8_t nBit, uint8_t x);
void stepA();
void stepB();
void stepAB();
void setEnable(uint8_t EN);
void setPwrSave(uint8_t EN);
void setDirA(uint8_t DIR);
void setDirB(uint8_t DIR);
void setTorqueA(uint8_t TRQ);
void setTorqueB(uint8_t TRQ);
void setStepSizeA(uint8_t sSize);
void setStepSizeB(uint8_t sSize);
void flushGPIO();

#endif /* MCP23S17_DRV8880_H */

