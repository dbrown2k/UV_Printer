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
#include <iostream>
#include "uvuart.h" //double-quotes tell the compiler that this is a header file we are supplying
#include "intuv.h" // FIFO IO setup
#include "MCP23S17_DRV8880.h" //SPI stepper motor controller
#include "Focus.h" // bi polar micro motor - focus controller
#include "HexCalcs.h" // calcs for setting up components that use hex streams


//function definitions



//global variables
unsigned int stepAcount = 0; //global store for position of print head
unsigned int stepBcount = 0; //global store for board position
unsigned char focMotor = 0x00; //use the first two bits; 00, 01, 10, 11 (0-3)


int main(int argc, char **argv)
{
  	//define variable used in function
	CalulateTable_CRC8(); //
	
	// Initialisation steps
	start_UART();
	config_UART();
	setup_io(); // Set up gpio pointer for direct register access
	init_io(); // setup input / output pins
	IO_ON(); // enable level converters
	setup_spi(); //setup SPI
	init_MCP23S17(); //initialise MCP23S17 (SPI stepper controller)
	initFreq();
  
	
	//'main' function program here
	
	
	
	/*
	focMotor = focusStep(focMotor, 250); //test focus stepper
	focMotor = focusStep(focMotor, -250);	
	*/
	
	
	
	
	//define array for received data
	unsigned char pointer_temp[] = {0, 0, 0, 0, 0, 0};
	//read buffer and place data into supplied array
	rx_UART(pointer_temp);
	
	
	setFreq(0.5);
	
	
	/*
	int steps;
	std::cout << "Please enter an number of steps: ";
	std::cin >> steps;
	
 
  //test SPI stepper
  setEnable(1);
  
  setStepSizeA(1);
  
  for (int i = 0; i < steps; i++)
  {
   stepA();
   delay(1);
  }
	
  setStepSizeB(16);
  
  for (int i = 0; i < steps; i++)
  {
   stepB();
   delay(1);
   
  }
  
  setEnable(0);
  
  
  unsigned char setval4[] = {0x0D, 0x00, 0x00, 0x00, 0x00}; //disable trigger laser
  tx_UART(setval4, 5);
  
  
  std::cout << readStop();
  
  */
  //shut down systems
  stop_UART();
  IO_OFF();
  
 
} // main
 
 


