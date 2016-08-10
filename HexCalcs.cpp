#include "HexCalcs.h"
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* Calculations to generate hex codes for passing to I2C and other devices
 * for configuration processes
 */


#define		IODIRA		(0x00)	// MCP23x17 I/O Direction Register
#define		IODIRB		(0x01)	// 1 = Input (default), 0 = Output
#define		GPIOA		(0x12)	// MCP23x17 GPIO Port Register
#define		GPIOB		(0x13)	// Value on the Port - Writing Sets Bits in the Output Latch
#define		STLOAD		(0x40)
#define		CLKlow		(0x00)
#define		CLKLOAD		(0x80)
#define		ADDRFreq	(0x27)	// address of the frequency generator IO expander


unsigned long frequencyword = 0; //variable for the frequency byte


void calcFreq(float freq) //frequency in MHz
{
	long double temp = ((freq / 125) * exp2(32));
	frequencyword = (unsigned long)temp;
}


unsigned char pullByte(int byteno)
{
	unsigned char *p = (unsigned char*)&frequencyword;
	//use p[0],p[1],p[2],p[3] to access the bytes.
	
	unsigned char temp = p[byteno]; //copy required byte
	unsigned char outByte = 0;
	
	//std::cout << (int)temp << std::endl;
	/*need to map the word to the pins
	 * out[7] = in[0]
	 * out[6] = in[4]
	 * out[5] = in[5]
	 * out[4] = in[6]
	 * out[3] = in[7]
	 * out[2] = in[1]
	 * out[1] = in[2]
	 * out[0] = in[3]
	 */
	// x = ((temp >> 0) & 1) calculates the value at position bit 0
	// outByte ^= (-x ^ outByte) & (1 << 7); applies the value of x to bit 7
	outByte ^= (-((temp >> 0) & 1) ^ outByte) & (1 << 7);
	outByte ^= (-((temp >> 4) & 1) ^ outByte) & (1 << 6);
	outByte ^= (-((temp >> 5) & 1) ^ outByte) & (1 << 5);
	outByte ^= (-((temp >> 6) & 1) ^ outByte) & (1 << 4);
	outByte ^= (-((temp >> 7) & 1) ^ outByte) & (1 << 3);
	outByte ^= (-((temp >> 1) & 1) ^ outByte) & (1 << 2);
	outByte ^= (-((temp >> 2) & 1) ^ outByte) & (1 << 1);
	outByte ^= (-((temp >> 3) & 1) ^ outByte) & (1 << 0);
	
	std::cout << (int)outByte << std::endl;
	return outByte;
}


void initFreq() //setup GPIO for frequency generator
{
	unsigned char setA[] = {0x0C, ADDRFreq, IODIRA, 0x00, 0x00};
	tx_UART(setA, 5);
	
	unsigned char setB[] = {0x0C, ADDRFreq, IODIRB, 0x00, 0x00};
	tx_UART(setB, 5);
}


void setFreq(float freq)
{
	calcFreq(freq); // calculate frequency
	
	unsigned char FQlow_CLKlow[] = {0x0C, ADDRFreq, GPIOA, CLKlow, 0x00};
	unsigned char FQhigh_CLKlow[] = {0x0C, ADDRFreq, GPIOA, CLKLOAD, 0x00};
	unsigned char CLKhigh[] = {0x0C, ADDRFreq, GPIOA, STLOAD, 0x00};
	unsigned char phase[] = {0x0C, ADDRFreq, GPIOB, 0x00, 0x00};
	unsigned char bit32_25[] = {0x0C, ADDRFreq, GPIOB, pullByte(3), 0x00};
	unsigned char bit24_17[] = {0x0C, ADDRFreq, GPIOB, pullByte(2), 0x00};
	unsigned char bit16_9[] = {0x0C, ADDRFreq, GPIOB, pullByte(1), 0x00};
	unsigned char bit8_1[] = {0x0C, ADDRFreq, GPIOB, pullByte(0), 0x00};
	
	std::cout << frequencyword << std::endl;
	//std::cout << pullByte(3) << std::endl;
	//std::cout << pullByte(2) << std::endl;
	//std::cout << pullByte(1) << std::endl;
	//std::cout << pullByte(0) << std::endl;
	
	tx_UART(FQlow_CLKlow, 5); //FQ_UD low & CLK low
	tx_UART(phase, 5); //phase data
	tx_UART(CLKhigh, 5); //CLK high - load
	tx_UART(FQlow_CLKlow, 5); //FQ_UD low & CLK low
	tx_UART(bit32_25, 5); //bit 32 through 25
	tx_UART(CLKhigh, 5); //CLK high - load
	tx_UART(FQlow_CLKlow, 5); //FQ_UD low & CLK low
	tx_UART(bit24_17, 5); //bit 24 through 17
	tx_UART(CLKhigh, 5); //CLK high - load
	tx_UART(FQlow_CLKlow, 5); //FQ_UD low & CLK low
	tx_UART(bit16_9, 5); //bit 16 through 9
	tx_UART(CLKhigh, 5); //CLK high - load
	tx_UART(FQlow_CLKlow, 5); //FQ_UD low & CLK low
	tx_UART(bit8_1, 5); //bit 8 through 1
	tx_UART(CLKhigh, 5); //CLK high - load
	tx_UART(FQlow_CLKlow, 5); //FQ_UD low & CLK low
	tx_UART(FQhigh_CLKlow, 5); //FQ_UD high & CLK low
	tx_UART(FQlow_CLKlow, 5); //FQ_UD low & CLK low
	
}







/*
unsigned char setval1[] = {0x09, 0x1C, 0x08, 0x00, 0x10}; //setup the trigger laser DAC configuration
	unsigned char setval2[] = {0x09, 0x1C, 0x01, 0x57, 0x0C}; //set trigger laser DAC to 1.7v
	unsigned char setval3[] = {0x0D, 0x01, 0x00, 0x00, 0x00}; //enable trigger laser
	
	tx_UART(setval1, 5);
	
	tx_UART(setval2, 5);
	
	tx_UART(setval3, 5);
 * 
 */




