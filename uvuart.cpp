#include "uvuart.h"



/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

// to access serial terminal use the following as root or sudo;
// minicom -b 11500 -o -D /dev/ttyAMA0
//Define header files (additional functions)

//At bootup, pins 8 and 10 are already set to UART0_TXD, UART0_RXD (ie the alt0 function) respectively
int uart0_filestream = -1;




void start_UART() //initialise the UART stream
{
//OPEN THE UART Ref. http://www.raspberry-projects.com/pi/programming-in-c/uart-serial-port/using-the-uart
//The flags (defined in fcntl.h):
//	Access modes (use 1 of these):
//		O_RDONLY - Open for reading only.
//		O_RDWR - Open for reading and writing.
//		O_WRONLY - Open for writing only.
//
//	O_NDELAY / O_NONBLOCK (same function) - Enables nonblocking mode. When set read requests on the file can return immediately with a failure status
//											if there is no input immediately available (instead of blocking). Likewise, write requests can also return
//											immediately with a failure status if the output can't be written immediately.
//
//	O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device to become the controlling terminal for the process.
uart0_filestream = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
if (uart0_filestream == -1)
{
	//ERROR - CAN'T OPEN SERIAL PORT
	perror("Error - Unable to open UART.  Ensure it is not in use by another application");
}	
	
}




void config_UART() //configure the UART baud etc.
{
//CONFIGURE THE UART
//The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
//	Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
//	CSIZE:- CS5, CS6, CS7, CS8
//	CLOCAL - Ignore modem status lines
//	CREAD - Enable receiver
//	IGNPAR = Ignore characters with parity errors
//	ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
//	PARENB - Parity enable
//	PARODD - Odd parity (else even)
struct termios options;
tcgetattr(uart0_filestream, &options);
options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;		//<Set baud rate
options.c_iflag = IGNPAR;
options.c_oflag = 0;
options.c_lflag = 0;
tcflush(uart0_filestream, TCIFLUSH);
tcsetattr(uart0_filestream, TCSANOW, &options);	
	
}


void flush_UART()
{
	tcflush(uart0_filestream, TCIFLUSH);
}



void stop_UART() //close UART stream
{
//----- CLOSE THE UART -----
close(uart0_filestream);	
}




void tx_UART(unsigned char data[5], int nBytes) //always going to be sending a set number of bytes
{
	unsigned char tx_buffer[6];
	
	for (int i = 0; i < nBytes; i++) { tx_buffer[i] = data[i];} //transfer data to buffer
	
	//calculate CRC, append to buffer and increment number of bytes
	tx_buffer[5] = computeCRC(data, nBytes);
	++nBytes;
	
	//----- TX BYTES -----
	if (uart0_filestream != -1)
	{
		int count = write(uart0_filestream, &tx_buffer[0], nBytes);		//Filestream, bytes to write, number of bytes to write
		if (count < 0)
		{
			perror("UART TX error");
		}
	}
	
	delay(2);

}





void rx_UART(unsigned char rx_buffer[]) //
{
//----- CHECK FOR ANY RX BYTES -----
	if (uart0_filestream != -1)
	{
		// Read up to 255 characters from the port if they are there
		
		int rx_length = read(uart0_filestream, (void*)rx_buffer, 6);		//Filestream, buffer to store in, number of bytes to read (max)
		if (rx_length < 0)
		{
			//An error occured (will occur if there are no bytes)
		}
		else if (rx_length == 0)
		{
			//No data waiting
		}
		else
		{
			//Bytes received
			//rx_buffer[rx_length] = '\0';
			//printf("%i bytes read : %s\n", rx_length, rx_buffer);
		}
	}
	
}