#include "CRC-ATM-8.h"

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

unsigned char crcTable[256]; //storage of CRC lookup table


void CalulateTable_CRC8() //fill table
{
  unsigned char generator = 0x83; //ATM-8 CRC (8bit) code 
  // iterate over all byte values 0- 255
  for (int dividend = 0; dividend < 256; dividend++)
  {
    unsigned char currByte = dividend; 
    //calculate the CRC-8 value for current byte
    for (unsigned char bitSec = 0; bitSec < 8; bitSec++)
    {
      if ((currByte & 0x80) != 0)
      {
        currByte <<= 1;
        currByte ^= generator;
      }
      else
      {
        currByte <<= 1;
      }
    }
    //store CRC value in lookup table
    crcTable[dividend] = currByte;
  }

  return;
}

//process byte string to get crc or make a test
unsigned char computeCRC(unsigned char message[], int nBytes) 
{
  unsigned char crc = 0x0;
 
  for (int b = 0; b < nBytes; ++b)
  {
    //XOR in next input byte
    unsigned char data = message[b] ^ crc;
    //get current CRC value = remainder
    crc = crcTable[data];
  }
  
  return crc;
}