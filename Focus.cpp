#include "Focus.h"

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

//functions to move the focus motor to the end stop and also to step move it
//we make use of the overflow of the unsigned char to allow us o continue to loop from 255 to 0

#define		NULLbyte	(0x00)

	/* bipolar stepper code and rubric index
	 * 0	0	=>	0
	 * 0	1	=>	1
	 * 1	1	=>	2
	 * 1	0	=>	3
	 */
unsigned char rubric[] = {0x00, 0x01, 0x03, 0x02}; //motor toggle key


//
// provide last position code , then move until stop position
//
unsigned char focusToStop(unsigned char lastpos)
{
	
	unsigned char pointer_temp[] = {0, 0, 0, 0, 0, 0}; //define array for received data
	unsigned char setval[] = {0x03, 0x00, 0x00, NULLbyte, NULLbyte}; //step command
	
	while (pointer_temp[0] == 0)
		{
			
				if (lastpos < 0x03)
				{
					lastpos = lastpos + 1;
				}
				else
				{
					lastpos = 0x00;
				}
						
			setval[1] = rubric[lastpos & 0x03];
			tx_UART(setval, 5); //send command
			delay(10);
			rx_UART(pointer_temp); //read buffer and place data into supplied array
		}
				
	return (lastpos & 0x03);
}


//
// provide last position code (2bit) and number of steps to move
//
unsigned char focusStep(unsigned char lastpos, int steps)
{
	
	unsigned char pointer_temp[] = {0, 0, 0, 0, 0, 0}; //define array for received data
	unsigned char setval[] = {0x03, 0x00, 0x00, NULLbyte, NULLbyte}; //step command
	

	
	for (int i = 0; i < abs(steps); i++)
	{
		if (pointer_temp[0] == 0)
		{
			if (steps < 0) //reverse
			{
				if (lastpos < 0x03)
				{
					lastpos = lastpos + 1;
				}
				else
				{
					lastpos = 0x00;
				}
								
			}
			else //forward
			{
				if (lastpos > 0x00)
				{
					lastpos = lastpos - 1;
				}
				else
				{
					lastpos = 0x03;
				}
				
			}
			
			setval[1] = rubric[lastpos & 0x03];
			tx_UART(setval, 5); //send command
			delay(10);
			rx_UART(pointer_temp); //read buffer and place data into supplied array
		}
			
		else
		{
			perror("Focus motor error");
		}
	}
	
	return (lastpos & 0x03);
}


//
// read focus stop status
//
unsigned char readStop()
{
	unsigned char setval[] = {0x02, NULLbyte, NULLbyte, NULLbyte, NULLbyte}; //step command
	tx_UART(setval, 5); //send command
	
	unsigned char pointer_temp[] = {0, 0, 0, 0, 0, 0}; //define array for received data
	delay(10);
	rx_UART(pointer_temp); //read buffer and place data into supplied array
	
	return pointer_temp[1];
}
