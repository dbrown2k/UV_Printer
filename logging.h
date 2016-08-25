/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   logging.h
 * Author: David
 *
 * Created on 11 August 2016, 21:08
 */

#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>
#include <stdlib.h>
#include <bcm2835.h>
#include "uvuart.h"


float reqTemp();
float reqHumd();
float readTEC();
float readTrigPhot();



#endif /* LOGGING_H */

