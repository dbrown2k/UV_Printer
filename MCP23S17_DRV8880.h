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
void stepAB(unsigned char command);



//component functions for SPI functions
void digitalSPIWrite(unsigned char addr, unsigned char reg, unsigned char data);
void bitWrite(unsigned char * inByte, unsigned char nBit, unsigned char x);
void stepA();
void stepB();
void stepAB();
void setEnable(unsigned char EN);
void setDirA(unsigned char DIR);
void setDirB(unsigned char DIR);
void setTorqueA(unsigned char TRQ);
void setTorqueB(unsigned char TRQ);
void setStepSizeA(unsigned char sSize);
void setStepSizeB(unsigned char sSize);

#endif /* MCP23S17_DRV8880_H */

