/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Focus.h
 * Author: David
 *
 * Created on 29 July 2016, 08:45
 */

#ifndef FOCUS_H
#define FOCUS_H

#include <stdlib.h>
#include <bcm2835.h>
#include "uvuart.h"

unsigned char focusToStop();
unsigned char focusStep(unsigned char lastpos, int steps);
unsigned char readStop();

#endif /* FOCUS_H */

