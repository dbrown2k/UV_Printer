#include "MCP23S17_DRV8880.h"


/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


//MCP23S17 address 
#define    IODIRA    (0x00)      // MCP23x17 I/O Direction Register
#define    IODIRB    (0x01)      // 1 = Input (default), 0 = Output
#define    IOCON     (0x0A)      // MCP23x17 Configuration Register
#define    GPIOA     (0x12)      // MCP23x17 GPIO Port Register
#define    GPIOB     (0x13)      // Value on the Port - Writing Sets Bits in the Output Latch
#define    ADDR_ENABLE   (0b00001000) //0x08 Configuration register for MCP23S17
#define    StepperAddr   (0b01000000) //0x40
#define    GPPUA     (0x06)     //Pull up resistor A
#define    GPPUB     (0x16)     //Pull up resistor B

//settings bytes for DRV8880
unsigned char IODIR_A = 0x01; //stepper data byte Bank A
unsigned char GPPU_A = 0x01; //set input pull up resistors
// nFault = bit 7; = input (pull up)
// ENABLE = bit 6; = output
// DIR = bit 5; = output
// STEP = bit 4; = output
// M1 = bit 3; = output
// M0 = bit 2; = input/output
// TRQ1 = bit 1; = output
// TRQ0 = bit 0; = output

unsigned char IODIR_B = 0x02; //stepper data byte Bank A
unsigned char GPPU_B = 0x02; //set input pull up resistors
// nSleep = bit 7; = output
// nFault = bit 6; = input (pull up)
// DIR = bit 5; = output
// STEP = bit 4; = output
// M1 = bit 3; = output
// M0 = bit 2; = input/output
// TRQ1 = bit 1; = output
// TRQ0 = bit 0; = output

unsigned char GPIO_A = 0; //stepper data byte Bank A
unsigned char GPIO_B = 0; //stepper data byte Bank B


//set selected bit to value
//use case is to send the address to bitWrite(&GPIO_A,4,1);
void bitWrite(unsigned char * inByte, unsigned char nBit, unsigned char x)
{
	*inByte ^= (-x ^ (*inByte)) & (1 << nBit);
}



//initialise for spi
void setup_spi()
{
bcm2835_init();
bcm2835_spi_begin();
bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // The default
bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_65536);	  // 65536 = 4kHz 64 = 4MHz
bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                      // The default
bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);      // The default

}

//initialise the MCP23S171
void init_MCP23S17()
{
	digitalSPIWrite(StepperAddr, IOCON, ADDR_ENABLE);
	digitalSPIWrite(StepperAddr, GPPUA, GPPU_A); //set GPPU_A (pull up resistors)
    digitalSPIWrite(StepperAddr, GPPUB, GPPU_B); //set GPPU_B (pull up resistors)
	setDirA(0);
	setDirB(0);
	setTorqueA(2); //1 to 4 (25%, 50%, 75%, 100%)
	setTorqueB(2); //1 to 4 (25%, 50%, 75%, 100%)
	setStepSizeA(1); //1th, 2th, 4th, 8th, 16th
	setStepSizeB(1); //1th, 2th, 4th, 8th, 16th
	
}

//function to write data to MCP23S17
void digitalSPIWrite(unsigned char addr, unsigned char reg, unsigned char data) {
  
	unsigned char buf[3] = {addr, reg, data};
	
  bcm2835_spi_transfern((char*)buf, 3); //recast buf as (char*) 
  
}


void stepA(){ //send step pulse train _n_ 
  bitWrite(&GPIO_A,4,1); //ensure step is back in 1 value
  digitalSPIWrite(StepperAddr, GPIOA, GPIO_A); //set GPIO_A
  bitWrite(&GPIO_A,4,0); //ensure step is back in 0 value
  digitalSPIWrite(StepperAddr, GPIOA, GPIO_A); //set GPIO_A
}

void stepB(){ //send step pulse train _n_ 
  bitWrite(&GPIO_B,4,1); //ensure step is back in 1 value
  digitalSPIWrite(StepperAddr, GPIOB, GPIO_B); //set GPIO_B
  bitWrite(&GPIO_B,4,0); //ensure step is back in 0 value
  digitalSPIWrite(StepperAddr, GPIOB, GPIO_B); //set GPIO_B
}

void stepAB(){
  bitWrite(&GPIO_A,4,1); //ensure step is back in 1 value
  bitWrite(&GPIO_B,4,1); //ensure step is back in 1 value
  digitalSPIWrite(StepperAddr, GPIOA, GPIO_A); //set GPIO_A
  digitalSPIWrite(StepperAddr, GPIOB, GPIO_B); //set GPIO_B
  bitWrite(&GPIO_A,4,0); //ensure step is back in 0 value
  bitWrite(&GPIO_B,4,0); //ensure step is back in 0 value
  digitalSPIWrite(StepperAddr, GPIOA, GPIO_A); //set GPIO_A
  digitalSPIWrite(StepperAddr, GPIOB, GPIO_B); //set GPIO_B
}

void setEnable(unsigned char EN){ //enable is called less often than flush, so pullup set here 
  bitWrite(&GPIO_A,6,EN); //enable / disable, on/off, 0/1
  bitWrite(&GPIO_B,7,1); //nsleep high = awake
  
  bitWrite(&GPIO_A,4,0); //ensure step is 0 value
  bitWrite(&GPIO_B,4,0); //ensure step is 0 value

  //then apply changes
  digitalSPIWrite(StepperAddr, GPIOA, GPIO_A); //set GPIO_A
  digitalSPIWrite(StepperAddr, GPIOB, GPIO_B); //set GPIO_B
  
}

void setDirA(unsigned char DIR){
  bitWrite(&GPIO_A,5,DIR); //DIR = 0/1
}

void setDirB(unsigned char DIR){
  bitWrite(&GPIO_B,5,DIR); //DIR = 0/1
}


void setTorqueA(unsigned char TRQ){
  if (TRQ == 1){
    bitWrite(&GPIO_A,0,1); //TRQ0 = 1
    bitWrite(&GPIO_A,1,1); //TRQ1 = 1
  }

  if (TRQ == 2){
    bitWrite(&GPIO_A,0,0); //TRQ0 = 0
    bitWrite(&GPIO_A,1,1); //TRQ1 = 1
  }  

  if (TRQ == 3){
    bitWrite(&GPIO_A,0,1); //TRQ0 = 1
    bitWrite(&GPIO_A,1,0); //TRQ1 = 0
  }  

  if (TRQ == 4){
    bitWrite(&GPIO_A,0,0); //TRQ0 = 0
    bitWrite(&GPIO_A,1,0); //TRQ1 = 0
  }
  
  
}

void setTorqueB(unsigned char TRQ){
  if (TRQ == 1){
    bitWrite(&GPIO_B,0,1); //TRQ0 = 1
    bitWrite(&GPIO_B,1,1); //TRQ1 = 1
  }

  if (TRQ == 2){
    bitWrite(&GPIO_B,0,0); //TRQ0 = 0
    bitWrite(&GPIO_B,1,1); //TRQ1 = 1
  }  

  if (TRQ == 3){
    bitWrite(&GPIO_B,0,1); //TRQ0 = 1
    bitWrite(&GPIO_B,1,0); //TRQ1 = 0
  }  

  if (TRQ == 4){
    bitWrite(&GPIO_B,0,0); //TRQ0 = 0
    bitWrite(&GPIO_B,1,0); //TRQ1 = 0
  }
  
  
}

void setStepSizeA(unsigned char sSize){ //motor A or B
  if (sSize == 1){
      bitWrite(&GPIO_A,2,0); //M0 = 0
      bitWrite(&GPIO_A,3,0); //M1 = 0
  }

    if (sSize == 2){
      bitWrite(&GPIO_A,2,0); //M0 = 0
      bitWrite(&GPIO_A,3,1); //M1 = 1
  }

    if (sSize == 4){
      bitWrite(&GPIO_A,2,1); //M0 = 1
      bitWrite(&GPIO_A,3,1); //M1 = 1
  }

    if (sSize == 8){
      bitWrite(&IODIR_A,2,1); //M0 = z (set to input)
      bitWrite(&GPIO_A,3,0); //M1 = 0
	  digitalSPIWrite(StepperAddr, IODIRA, IODIR_A); //set DIR_A
  }

    if (sSize == 16){
      bitWrite(&IODIR_A,2,1); //M0 = z (set to input)
      bitWrite(&GPIO_A,3,1); //M1 = 1
	  digitalSPIWrite(StepperAddr, IODIRA, IODIR_A); //set DIR_A
  }

	
}


void setStepSizeB(unsigned char sSize){ //motor B
  if (sSize == 1){
      bitWrite(&GPIO_B,2,0); //M0 = 0
      bitWrite(&GPIO_B,3,0); //M1 = 0
  }

    if (sSize == 2){
      bitWrite(&GPIO_B,2,0); //M0 = 0
      bitWrite(&GPIO_B,3,1); //M1 = 1
  }

    if (sSize == 4){
      bitWrite(&GPIO_B,2,1); //M0 = 1
      bitWrite(&GPIO_B,3,1); //M1 = 1
  }

    if (sSize == 8){
      bitWrite(&IODIR_B,2,1); //M0 = z (set to input)
      bitWrite(&GPIO_B,3,0); //M1 = 0
	  digitalSPIWrite(StepperAddr, IODIRB, IODIR_B); //set DIR_B
  }

    if (sSize == 16){
      bitWrite(&IODIR_B,2,1); //M0 = z (set to input)
      bitWrite(&GPIO_B,3,1); //M1 = 1
	  digitalSPIWrite(StepperAddr, IODIRB, IODIR_B); //set DIR_B
  }

  
}



//need to use timer values to interlace stepping events for head translation and writing data to the FIFO


//    bcm2835_spi_end();


