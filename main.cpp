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
 * /root/.netbeans/remote/192.168.20.27/x10-workstation-Windows-x86_64/E/Transporter/workspace/UV_Printer/dist/Debug/GNU-Linux/uv_printe
 */
 
 
//#include <stdio.h> //header file that was included with the compiler
#include <iostream>
#include "uvuart.h" //double-quotes tell the compiler that this is a header file we are supplying
#include "intuv.h" // FIFO IO setup
#include "MCP23S17_DRV8880.h" //SPI stepper motor controller
#include "Focus.h" // bi polar micro motor - focus controller
#include "HexCalcs.h" // calcs for setting up components that use hex streams; DACs & ADC
#include "logging.h" // logging of temp / humidity and other I/Os


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
	reset_FIFO();
	setup_spi(); //setup SPI
	init_MCP23S17(); //initialise MCP23S17 (SPI stepper controller)
	initFreq(); // initialise frequency generator
	init8DAC(); // initialise x8 UV laser DAC
	initADC(); //initialise ADC - UV laser current measure
  
	
	// 'main' function program here
	
	//setTrigVolt(1.1); //test trigger laser
	//enTrigger(1);
	//delay(2000);
	
	
	//focMotor = focusStep(focMotor, 5); //test focus stepper
	//focMotor = focusStep(focMotor, -5);	
	
	
	//setFreq(1.525); //sets the frequency in MHz
	
		
	//float DAC8volts[] = {3.455, 3.363, 2.796, 3.020, 3.042, 1.638, 2.116, 1.727}; //from validation tests
	//float DAC8volts[] = {0, 0, 0, 0, 0, 0, 0, 0};
	//set8DAC(DAC8volts);
	
	
	//std::cout << readADC() << std::endl; // make reading from ADC
	
	
	//std::cout << (int)read_FF() << std::endl; // Read FIFO status flags
	//std::cout << (int)read_EF() << std::endl;
	
	
	//std::cout << reqTemp() << std::endl; // make reading from the temp sensor
	//std::cout << reqHumd() << std::endl; // make reading from the humidity sensor
	
	
	//setTECTemp(25); // need to set the relationship between temp and DAC voltage
	//enTEC(1); //enable TEC
	//std::cout << readTEC() << std::endl; // make reading from the TEC monitor
	
	
	/* //test SPI stepper
	int steps;
	std::cout << "Please enter an number of steps: ";
	std::cin >> steps;
	
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
  
	*/
  
  
    
  
  //shut down systems
  enTrigger(0);
  enTEC(0);
  stop_UART();
  IO_OFF();
  
 
} // main
 
 


