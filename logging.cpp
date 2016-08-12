#include "logging.h"

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#define		NULLbyte	(0x00)


float reqTemp()
{
	unsigned char setval[] = {0x0B, 0x01, NULLbyte, NULLbyte, NULLbyte};
	tx_UART(setval, 5); //send command
	
	delay(150); //need to wait for data to be fetched and transmitted before reading it
	
	unsigned char pointer_temp[] = {0, 0, 0, 0, 0, 0}; // define array for received data
	unsigned char errorVal = rx_UART(pointer_temp); //read buffer and place data into supplied array
	
	if (errorVal == 0x01) {perror("UART RX error");} // issue error if read fails
	
	//std::cout << (int)pointer_temp[1] << std::endl;
	//std::cout << (int)pointer_temp[2] << std::endl;
	
	float result = pointer_temp[1] + ((float)pointer_temp[2]/10);
	
	return result;
}


float reqHumd()
{
	unsigned char setval[] = {0x0B, 0x00, NULLbyte, NULLbyte, NULLbyte};
	tx_UART(setval, 5); //send command
	
	delay(150); //need to wait for data to reach the buffer before reading it
	
	unsigned char pointer_temp[] = {0, 0, 0, 0, 0, 0}; // define array for received data
	unsigned char errorVal = rx_UART(pointer_temp); //read buffer and place data into supplied array
	
	if (errorVal == 0x01) {perror("UART RX error");} // issue error if read fails
	
	//std::cout << (int)pointer_temp[1] << std::endl;
	//std::cout << (int)pointer_temp[2] << std::endl;
	
	float result = pointer_temp[1] + ((float)pointer_temp[2]/10);
	
	return result;
}