/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CRC-ATM-8.h
 * Author: David
 *
 * Created on 24 July 2016, 10:00
 */

#ifndef CRC_ATM_8_H
#define CRC_ATM_8_H


void CalulateTable_CRC8(); //fill table
unsigned char computeCRC(unsigned char message[], int nBytes);


#endif /* CRC_ATM_8_H */

