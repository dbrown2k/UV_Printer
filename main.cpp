/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: David
 *
 * Created on 14 March 2016, 10:13
 * 
 * Access from ARM Running Linux
 * 
 */
 
 
//#include <stdio.h> //header file that was included with the compiler
#include "uvuart.h" //double-quotes tell the compiler that this is a header file we are supplying
#include "intuv.h" 
#include "MCP23S17_DRV8880.h"



//function definitions



//global variables
unsigned int stepAcount = 0;
unsigned int stepBcount = 0;
unsigned char focMotor = 0; //use the first two bits; 00, 01, 10, 11 (0-3)


int main(int argc, char **argv)
{
  	//define variable used in function
	CalulateTable_CRC8(); //
	
	// Initialisation steps
  setup_io(); // Set up gpio pointer for direct register access
  init_io(); // setup input / output pins
  IO_ON(); // enable level converters
  setup_spi(); //setup SPI
  init_MCP23S17(); //initialise MCP23S17
 
  
	
	//'main' function program here
	start_UART();
	config_UART();
	
	unsigned char setval1[] = {0x09, 0x1C, 0x08, 0x00, 0x10}; //setup the trigger laser DAC configuration
	unsigned char setval2[] = {0x09, 0x1C, 0x01, 0x57, 0x0C}; //set trigger laser DAC to 1.7v
	unsigned char setval3[] = {0x0D, 0x00, 0x00, 0x00, 0x00}; //enable trigger laser
	
	//tx_UART(setval1, 5);
	
	//tx_UART(setval2, 5);
	
	tx_UART(setval3, 5);
	
	
	//define array for recieved data
	unsigned char pointer_temp[] = {0, 0, 0, 0, 0, 0};
	//read buffer and place data into supplied array
	//rx_UART(pointer_temp);
	
	//tx_UART(pointer_temp, 7);
	
		
	    
	
	stop_UART();

  
 
  /* //test SPI stepper
  setEnable(1);
  
  setStepSizeA(1);
  
  for (int i = 0; i < 500; i++)
  {
   stepA();
   delay(10);
  }
	
  setStepSizeB(16);
  
  for (int i = 0; i < 500; i++)
  {
   stepB();
   delay(10);
   
  }
  
  setEnable(0);
  
  
  
  
  
  //IO_OFF();
  */
 
} // main
 
 


