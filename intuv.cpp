#include "intuv.h"

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


//
// re-arrange the table to match the hardware laser power to the required value
//
/*
GPIO Function		Ohms
7	Output	D2	U6	200
25	Output	D1	U7	100
5	Output	D0	U8	69.8
24	Output	D3	U5	50
22	Output	D8	U4	20
4	Output	D6	U1	15
27	Output	D4	U3	10
17	Output	D5	U2	6.19 */
static unsigned char FIFOtable[8] = {FIFO_D2, FIFO_D1, FIFO_D0, FIFO_D3, 
										FIFO_D8, FIFO_D6, FIFO_D4, FIFO_D5};



void setup_io()
{
bcm2835_init();

}

//
// Initialise pins
//
void init_io()
{
//set outputs
  bcm2835_gpio_fsel(FIFO_nRS, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(FIFO_nW, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(FIFO_D0, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(FIFO_D1, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(FIFO_D2, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(FIFO_D3, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(FIFO_D4, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(FIFO_D5, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(FIFO_D6, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(FIFO_D8, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(FIFO_nEF, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(FIFO_nFF, BCM2835_GPIO_FSEL_OUTP);
  
   
//set outputs LOW
  bcm2835_gpio_clr(FIFO_D0);
  bcm2835_gpio_clr(FIFO_D1);
  bcm2835_gpio_clr(FIFO_D2);
  bcm2835_gpio_clr(FIFO_D3);
  bcm2835_gpio_clr(FIFO_D4);
  bcm2835_gpio_clr(FIFO_D5);
  bcm2835_gpio_clr(FIFO_D6);
  bcm2835_gpio_clr(FIFO_D8);
  bcm2835_gpio_clr(enIO);
  
  
//Set FIFO controls to high (active low)
  bcm2835_gpio_set(FIFO_nRS); //switch FIFO RS HIGH
  bcm2835_gpio_set(FIFO_nW); //switch FIFO W HIGH
  
  
 //set inputs  
  bcm2835_gpio_fsel(FIFO_nEF, BCM2835_GPIO_FSEL_INPT);
  bcm2835_gpio_fsel(FIFO_nFF, BCM2835_GPIO_FSEL_INPT);
  
  
  //pull inputs low
  bcm2835_gpio_set_pud(FIFO_nEF, BCM2835_GPIO_PUD_DOWN);
  bcm2835_gpio_set_pud(FIFO_nFF, BCM2835_GPIO_PUD_DOWN);
  
}

//
//	Enable level converters
//
void IO_ON()
{
	// Switch on level converters
	bcm2835_gpio_set(enIO); //switch HIGH
}

//
//	Disable level converters
//
void IO_OFF()
{
	// Switch off level converters
	bcm2835_gpio_clr(enIO); //switch LOW
}

//
// reset the FIFO data sheet notes minimum delay of ~40ns 
//
void reset_FIFO()
{
	bcm2835_gpio_clr(FIFO_nRS); //switch FIFO RS LOW
	delay(1); //wait to reset
	bcm2835_gpio_set(FIFO_nRS); //switch FIFO RS HIGH
}



//
//set the FIFO based on supplied inByte value and table of pins
//
void write_FIFO_byte(unsigned char inByte)
{
	for (unsigned char i = 0; i <= 8; i++) //toggle pins
	{
		unsigned char temp = inByte; //need temporary value to allow shift
		if ( ((temp >> i) & 1) == 0) //read bit value at position i in Byte
		{
			bcm2835_gpio_clr(FIFOtable[i]);
		}
		else
		{
			bcm2835_gpio_set(FIFOtable[i]);
		}
	}
	
	//write value to FIFO, clock low, clock high
	bcm2835_gpio_clr(FIFO_nW);
	bcm2835_gpio_set(FIFO_nW);
}

//
// read the status flags, output NOT (~) to give high if case positive
//
unsigned char read_FF()
{
	return (~bcm2835_gpio_lev(FIFO_nEF) & 0x01);
}

unsigned char read_EF()
{
	return (~bcm2835_gpio_lev(FIFO_nFF) & 0x01);
}
