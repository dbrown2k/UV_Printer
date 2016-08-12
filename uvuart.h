/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   uvuart.h
 * Author: David
 *
 * Created on 14 July 2016, 08:51
 */


#ifndef uvuart_H //use header guard to prevent muntiple copies
	#define uvuart_H
//these include files should have their own header guards
#include <stdio.h>
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART
#include "CRC-ATM-8.h"		//call CRC function header
#include <bcm2835.h>


//class uvuart{

//public: //public functions to simplify the interface to the printer head

	//commands to control the print head
	
//private:	
void start_UART();
void config_UART();
void flush_UART();
void stop_UART();
void tx_UART(unsigned char tx_buffer[5], int nBytes);
unsigned char  rx_UART(unsigned char rx_buffer2[]); //returns pointer need function to parse the length of the array


//};

#endif /* uvuart_H */

