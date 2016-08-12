/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HexCalcs.h
 * Author: David
 *
 * Created on 10 August 2016, 09:40
 */

#ifndef HEXCALCS_H
#define HEXCALCS_H

#include <cmath>
#include <iostream>
#include <iomanip>
#include "uvuart.h"



void calcFreq(float freq);
unsigned char pullByte(unsigned long inword, int byteno);
unsigned char byteTranspose(int byteno);
void initFreq();
void setFreq(float freq);

void calcVolt(float volt);
void setVolt(unsigned char SelAdr, float volt);
void setTrigVolt(float volt);
void enTrigger(unsigned char en);

void setTECTemp(float temp);
void enTEC(unsigned char en);

void init8DAC();
void calcx8volt(float volt);
void set8DAC(float volts[8]);

void initADC();
float readADC();

#endif /* HEXCALCS_H */

