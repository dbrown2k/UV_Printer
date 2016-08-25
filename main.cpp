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
#include <sys/mman.h>




//function definitions
void delay_small(unsigned int no);


//global variables
const unsigned int strip_width = 329; // No. scan pixels from laser focus diameter and scan length
const unsigned int FIFO_depth = 1024; // hardware limit of FIFO
const unsigned int Page_width_max = 13000; // hardware limit from bed width and focus diameter
const unsigned int step_strip_width = 2; // number of steps between scan lines, 5um per step

unsigned int stepAcount = 0; //global store for position of print head
unsigned int stepBcount = 0; //global store for board position
unsigned char focMotor = 0x00; //use the first two bits; 00, 01, 10, 11 (0-3)

unsigned int imageStrip[FIFO_depth][Page_width_max]; // Cache data for strip to memory, load using translation function



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
	
	setTrigVolt(1.15); //test trigger laser
	//std::cout << readTrigPhot() << std::endl; //read trigger laser photodiode
	//std::cout << readTrigPhot() << std::endl; //read trigger laser photodiode
	enTrigger(1);
	//std::cout << readTrigPhot() << std::endl; //read trigger laser photodiode
	//std::cout << readTrigPhot() << std::endl; //read trigger laser photodiode
	//delay(2000);
	
	
	focMotor = focusStep(focMotor, 5); //test focus stepper
	focMotor = focusStep(focMotor, -5);	
	
	
	setFreq(1.525); //sets the frequency in MHz (max is ~4MHz with current hardware
	
		
	float DAC8volts[] = {3.455, 3.363, 2.796, 3.020, 3.042, 1.638, 2.116, 1.727}; //from validation tests
	//float DAC8volts[] = {0, 0, 0, 0, 0, 0, 0, 0};
	set8DAC(DAC8volts);

	
	//std::cout << (float)readADC() << std::endl; // make reading from ADC	
	//std::cout << (float)reqTemp() << std::endl; // make reading from the temp sensor
	//std::cout << (float)reqHumd() << std::endl; // make reading from the humidity sensor
	
	
	//setTECTemp(25); // need to set the relationship between temp and DAC voltage
	//enTEC(1); //enable TEC
	//std::cout << readTEC() << std::endl; // make reading from the TEC monitor
	
	


	 //test SPI stepper
	setEnable(1);
	
	setStepSizeA(16);
	setStepSizeB(8);
	
	
	for (int i = 0; i < Page_width_max; i++)
	{
	 	stepA();
					 
			for (int k = 0; k < strip_width; k++)
			{
			write_FIFO_byte(imageStrip[k][i]); // load value into FIFO
			
			// move head during data load process, it takes time for motor 
			// to respond so use timer and counter to check and see if requesite
			// time has elapsed and if more steps are needed
			//
			if(k == strip_width / 2){stepA();} //second move
			}
			
			
	}
	
	
	setEnable(0);

	 

  
	
	/*
	speedMirMotor(65); //initial calculation motor speed (Hz)
	enMirMotor(1); // enable motor
	delay(20000); //20 seconds - time to stabalise speed
    std::cout << (int)checkLock(); // check if the PLL speed matches required frequency, never seen this work
	*/
	

	//uvTrigOveride(0);
	//std::cout << (int)uvLaserStat();
	
	
	/* print process consists of reading the EF, when empty load zero set
	 * watch for when this has been read out then reset and load data
	 * may be possible to set the EF as interupt to speed up. can use this
	 * method to determine the maximum size of data that can be loaded
	 * by the Pi within the time. Use second arduino to trigger the readout
	 * 
	 */
	
	/*
	//std::cout << (int)read_FF() << std::endl; // Read FIFO status flags
	//std::cout << (int)read_EF() << std::endl;
	
	// fill cache with data - increasing output
	//std::cout << (int)read_EF() << std::endl; 
	

	for (int i = 0; i < Page_width_max; i++)

	for (int i = 0; i < 1000; i++)

	{
		for (int k = 0; k < strip_width; k++)
		{
		imageStrip[k][i] = 150;
		}
	}
	


	
		// loop to read EF - system too sensitive to test by hand using the laser, seems to work with wire short
	for (int i = 0; i < 100; i++)
	{
		stepA(); //first part of step
		//std::cout << "LOAD FIFO" << std::endl;
		while (read_EF() == 1 && read_FF() == 0)
		{
			unsigned int step_count = step_strip_width;			 
			for (int k = 0; k < strip_width; k++)
			{
			write_FIFO_byte(imageStrip[k][0]); // load value into FIFO
			
			// move head during data load process, it takes time for motor 
			// to respond so use timer and counter to check and see if requesite
			// time has elapsed and if more steps are needed
			//
			
			
			}
		stepA(); //second part of step


	 
		}
		
		//while (i < 10) // readout signals for testing
		//{
		//std::cout << (float)read_EF() << " "; 	
		//std::cout << (float)read_FF() << std::endl; 
		//delay(500);
		//}
		

		//std::cout << "Wait to empty" << std::endl; 


		while (read_EF() == 0 && read_FF() == 0) // empty flag still low prior to start of first read
		{
		}
		while (read_EF() == 1 && read_FF() == 0) // empty flag triggered at end of read
		{
		}
		while (read_EF() == 1 && read_FF() == 1) // wait for reset to finish
		{

		}	
	}
	
	*/


	
  //shut down systems
  uvTrigOveride(0);
  enTrigger(0);
  enTEC(0);
  enMirMotor(0);
  stop_UART();
  IO_OFF();
  
 
} // main
 
 
void delay_small(unsigned int no)
{
	for (int i = 0; i < no; i++){}
}
