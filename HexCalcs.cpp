#include "HexCalcs.h"
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* Calculations to generate hex codes for passing to I2C and other devices
 * for configuration processes
 */

#define		NULLbyte	(0x00)

#define		IODIRA		(0x00)	// MCP23x17 I/O Direction Register
#define		IODIRB		(0x01)	// 1 = Input (default), 0 = Output
#define		GPIOA		(0x12)	// MCP23x17 GPIO Port Register
#define		GPIOB		(0x13)	// Value on the Port - Writing Sets Bits in the Output Latch
#define		STLOAD		(0x40)
#define		CLKlow		(0x00)
#define		CLKLOAD		(0x80)
#define		ADDRFreq	(0x27)	// address I2C of the frequency generator IO expander

#define		ADDRtrig	(0x1C)	// trigger laser MAX5215 DAC I2C address
#define		REFTrig		(3.3)	// trigger DAC reference voltage
#define		USERCONFIG	(0x08)	//
#define		CODELOAD	(0x01)	// read code and load into registers

#define		ADDRTEC		(0x1F)	// address of the TEC MAX5215 DAC
#define		REFTEC		(1.25)	// TEC DAC reference voltage

#define		DAC8ADDR	(0x10)	// 8x DAC I2C address
#define		DAC8REF		(0x23)	// 8x DAC REF to 4.096v
#define		DAC8BASE	(0x80)	// x8 MAX 5825 DAC base adderss
#define		DAC8SET		(0x98)	// apply DAC voltages

#define		ADCADDR		(0x68)	// MCP3421 ADC I2C addr


unsigned long tempword = 0; //variable byte holder


//
// this section is for setting the frequency generator
//
void calcFreq(float freq) //frequency in MHz
{
	long double temp = ceil((freq / 125) * exp2(32)); //MEMS oscillator @ 125MHz
	tempword = (unsigned long)temp;
}


//extract byte from 32bit word
unsigned char pullByte(unsigned long inword, int byteno) 
{
	unsigned char *p = (unsigned char*)&inword;
	//use p[0],p[1],p[2],p[3] to access the bytes.
	
	unsigned char outByte = p[byteno]; //copy required byte
	
	return outByte;
}


// need to map the data to the correct pins
unsigned char byteTranspose(int byteno) 
{
	unsigned char temp = pullByte(tempword, byteno);
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
	
	//std::cout << (int)outByte << std::endl;
	
}


//setup GPIO for frequency generator
void initFreq() 
{
	unsigned char setA[] = {0x0C, ADDRFreq, IODIRA, 0x00, NULLbyte};
	tx_UART(setA, 5);
	returnCheck(); // error check the return data
	
	unsigned char setB[] = {0x0C, ADDRFreq, IODIRB, 0x00, NULLbyte};
	tx_UART(setB, 5);
	returnCheck(); // error check the return data
}


//calculate frequency code and send to frequency generator
void setFreq(float freq) 
{
	calcFreq(freq); // calculate frequency
	
	unsigned char FQlow_CLKlow[] = {0x0C, ADDRFreq, GPIOA, CLKlow, NULLbyte};
	unsigned char FQhigh_CLKlow[] = {0x0C, ADDRFreq, GPIOA, CLKLOAD, NULLbyte};
	unsigned char CLKhigh[] = {0x0C, ADDRFreq, GPIOA, STLOAD, NULLbyte};
	unsigned char phase[] = {0x0C, ADDRFreq, GPIOB, 0x00, NULLbyte};
	unsigned char bit32_25[] = {0x0C, ADDRFreq, GPIOB, byteTranspose(3), NULLbyte};
	unsigned char bit24_17[] = {0x0C, ADDRFreq, GPIOB, byteTranspose(2), NULLbyte};
	unsigned char bit16_9[] = {0x0C, ADDRFreq, GPIOB, byteTranspose(1), NULLbyte};
	unsigned char bit8_1[] = {0x0C, ADDRFreq, GPIOB, byteTranspose(0), NULLbyte};
	
	//std::cout << frequencyword << std::endl;
	//std::cout << byteTranspose(3) << std::endl;
	//std::cout << byteTranspose(2) << std::endl;
	//std::cout << byteTranspose(1) << std::endl;
	//std::cout << byteTranspose(0) << std::endl;
	
	tx_UART(FQlow_CLKlow, 5); //FQ_UD low & CLK low
	returnCheck(); // error check the return data
	tx_UART(phase, 5); //phase data
	returnCheck(); // error check the return data
	tx_UART(CLKhigh, 5); //CLK high - load
	returnCheck(); // error check the return data
	tx_UART(FQlow_CLKlow, 5); //FQ_UD low & CLK low
	returnCheck(); // error check the return data
	tx_UART(bit32_25, 5); //bit 32 through 25
	returnCheck(); // error check the return data
	tx_UART(CLKhigh, 5); //CLK high - load
	returnCheck(); // error check the return data
	tx_UART(FQlow_CLKlow, 5); //FQ_UD low & CLK low
	returnCheck(); // error check the return data
	tx_UART(bit24_17, 5); //bit 24 through 17
	returnCheck(); // error check the return data
	tx_UART(CLKhigh, 5); //CLK high - load
	returnCheck(); // error check the return data
	tx_UART(FQlow_CLKlow, 5); //FQ_UD low & CLK low
	returnCheck(); // error check the return data
	tx_UART(bit16_9, 5); //bit 16 through 9
	returnCheck(); // error check the return data
	tx_UART(CLKhigh, 5); //CLK high - load
	returnCheck(); // error check the return data
	tx_UART(FQlow_CLKlow, 5); //FQ_UD low & CLK low
	returnCheck(); // error check the return data
	tx_UART(bit8_1, 5); //bit 8 through 1
	returnCheck(); // error check the return data
	tx_UART(CLKhigh, 5); //CLK high - load
	returnCheck(); // error check the return data
	tx_UART(FQlow_CLKlow, 5); //FQ_UD low & CLK low
	returnCheck(); // error check the return data
	tx_UART(FQhigh_CLKlow, 5); //FQ_UD high & CLK low
	returnCheck(); // error check the return data
	tx_UART(FQlow_CLKlow, 5); //FQ_UD low & CLK low
	returnCheck(); // error check the return data
}


//
// This section for the MAX5215 DACs
//
void calcVolt(float volt, float Ref) //calculate trigger voltage code
{
	double temp = ceil((volt / Ref) * exp2(14)); //Reference voltage 3.3v, 14bits
	tempword = (unsigned long)temp << 2; //shift bits to move from 16bit to 14bits
	
	//std::cout << temp << std::endl;
}



// set the specified DAC laser voltage
void setVolt(unsigned char SelAdr, float volt, float Ref) 
{
	calcVolt(volt, Ref);
	
	unsigned char configTrig[] = {0x09, SelAdr, USERCONFIG, NULLbyte, 0x10}; //setup the trigger laser DAC configuration
	
	unsigned char byte1= pullByte(tempword, 1);
	unsigned char byte0= pullByte(tempword, 0);
	
	//std::cout << (int)byte1 << std::endl;
	//std::cout << (int)byte0 << std::endl;
		
	unsigned char setTrig[] = {0x09, SelAdr, CODELOAD, byte1, byte0}; //set trigger laser DAC to 1.122v
	
	tx_UART(configTrig, 5);
	returnCheck(); // error check the return data
	

	bcm2835_delay(1);


	
	tx_UART(setTrig, 5);
	returnCheck(); // error check the return data
}

//
// Setup the trigger laser
//
void setTrigVolt(float volt) //set the specified voltage to trigger DAC
{
	setVolt(ADDRtrig, volt, REFTrig);
}


//enable / disable trigger laser
void enTrigger(unsigned char en) 
{
	unsigned char swTrigger[] = {0x0D, en, NULLbyte, NULLbyte, NULLbyte}; 
	tx_UART(swTrigger, 5);
	returnCheck(); // error check the return data
}


//
// setup the TEC controller
//
void setTECTemp(float temp) //set temperature of the TEC DAC
{
	float volt = 1.25; // this needs updating to convert temperature to voltage at DAC
	setVolt(ADDRTEC, volt, REFTEC);
	returnCheck(); // error check the return data
}


// enable / disable the TEC
void enTEC(unsigned char en)
{
	unsigned char swTEC[] = {0x01, en, NULLbyte, NULLbyte, NULLbyte}; 
	tx_UART(swTEC, 5);
	returnCheck(); // error check the return data
}


//
// setup the x8 DAC for the UV laser
//
void init8DAC() //set REF to 4.096v
{
	unsigned char init8DAC[] = {0x09, DAC8ADDR, DAC8REF, NULLbyte, NULLbyte}; 
	tx_UART(init8DAC, 5);
	returnCheck(); // error check the return data
}


//calculate voltage code
void calcx8volt(float volt) 
{
	double temp = ceil((volt / 4.096) * exp2(12)); //Reference voltage 4.096v, 12bits
	tempword = (unsigned long)temp << 4; //shift bits to move from 16bit to 12bits
}


//set the voltages based on array of 8 values
void set8DAC(float volts[8]) 
{
	unsigned char tmpaddr[] = {0x09, DAC8ADDR, NULLbyte, NULLbyte, NULLbyte};
	
	for (int i = 0; i < 8; i++)
	{
		calcx8volt(volts[i]);
		tmpaddr[2] = DAC8BASE + i;
		tmpaddr[3] = pullByte(tempword, 1);
		tmpaddr[4] = pullByte(tempword, 0);
		tx_UART(tmpaddr, 5);
		returnCheck(); // error check the return data
	}
	
	unsigned char vset[] = {0x09, DAC8ADDR, DAC8SET, NULLbyte, NULLbyte}; //apply settings to all DACs
	tx_UART(vset, 5);
	returnCheck(); // error check the return data
}


//
// setup the ADC for monitoring the UV laser current
//
void initADC()
{
	unsigned char initADC[] = {0x07, ADCADDR, 0x0B, NULLbyte, NULLbyte}; 
	tx_UART(initADC, 5);
	returnCheck(); // error check the return data
}


//initialise one-shot reading
float readADC() 
{
	unsigned char requestMeasure[] = {0x07, ADCADDR, 0x8B, NULLbyte, NULLbyte}; 
	tx_UART(requestMeasure, 5);
	returnCheck(); // error check the return data

	bcm2835_delay(100); //wait for reading to occur (at 16bit = 15 samples per second, so need at least 66ms
	
	unsigned char readCMD[]= {0x08, ADCADDR, NULLbyte, NULLbyte, NULLbyte};
	tx_UART(readCMD, 5);

	unsigned char pointer_temp[] = {0, 0, 0, 0, 0, 0}; // define array for received data
	unsigned char errorVal = rx_UART(pointer_temp); //read buffer and place data into supplied array
	
	if (errorVal == 0x01) {perror("UART RX error");} // issue error if read fails
	
	int x;
	//bit shift combine bytes into integer
	x = pointer_temp[1];      //send high to right most bits
	x = x << 8; // shift high over 8 bits
	x |= pointer_temp[2]; //OR low bits to total
	
	//std::cout << (int)pointer_temp[1] << std::endl;
	//std::cout << (int)pointer_temp[2] << std::endl;
	
	float current = (float)x * 0.0390625; //factor to convert from recorded value to mA

	return current;
}

//
// Check mirror motor speed lock
//
unsigned char checkLock()
{
	unsigned char initCheck[] = {0x04, NULLbyte, NULLbyte, NULLbyte, NULLbyte}; 
	tx_UART(initCheck, 5);
		

	bcm2835_delay(5); //wait for reading to occur


	
	unsigned char pointer_temp[] = {0, 0, 0, 0, 0, 0}; // define array for received data
	unsigned char errorVal = rx_UART(pointer_temp); //read buffer and place data into supplied array
	
	if (errorVal == 0x01) {perror("UART RX error");} // issue error if read fails
	
	unsigned char x = pointer_temp[1];      
	
	return x;
}

// Set motor speed min speed is 19Hz
void speedMirMotor(unsigned char speedIn)
{
	unsigned char initSpeed[] = {0x05, speedIn, NULLbyte, NULLbyte, NULLbyte}; 
	tx_UART(initSpeed, 5);
	returnCheck(); // error check the return data
	
}

// Enable mirror motor
void enMirMotor(unsigned char en)
{
	
	unsigned char initEn[] = {0x06, en, NULLbyte, NULLbyte, NULLbyte}; 
	tx_UART(initEn, 5);
	returnCheck(); // error check the return data
	
}


//
// UV status
//
unsigned char uvLaserStat()
{
	unsigned char initCheck[] = {0x0E, NULLbyte, NULLbyte, NULLbyte, NULLbyte}; 
	tx_UART(initCheck, 5);
		

	bcm2835_delay(120); //wait for reading to occur 

	
	unsigned char pointer_temp[] = {0, 0, 0, 0, 0, 0}; // define array for received data
	unsigned char errorVal = rx_UART(pointer_temp); //read buffer and place data into supplied array
	
	if (errorVal == 0x01) {perror("UART RX error");} // issue error if read fails
	
	unsigned char x = pointer_temp[1];      
	
	return x;
}

// trigger override = D12/uvEN active high
void uvTrigOveride(unsigned char en)
{
	unsigned char initEn[] = {0x10, en, NULLbyte, NULLbyte, NULLbyte}; 
	tx_UART(initEn, 5);
	returnCheck(); // error check the return data
}

