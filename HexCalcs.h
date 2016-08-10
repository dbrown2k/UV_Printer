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
unsigned char pullByte(int byteno);
void initFreq();
void setFreq(float freq);



#endif /* HEXCALCS_H */

