/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   intuv.h
 * Author: David
 *
 * Created on 14 July 2016, 15:51
 */

#ifndef INTUV_H
#define INTUV_H

#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>


//define IO enable pin
#define enIO 2

//define FIFO pins
#define FIFO_nRS 3
#define FIFO_nW	23
#define FIFO_D0 5
#define FIFO_D1 25
#define FIFO_D2 7
#define FIFO_D3 24
#define FIFO_D4 27
#define FIFO_D5 17
#define FIFO_D6 4
#define FIFO_D8 22
#define FIFO_nEF 18
#define FIFO_nFF 12


/*	
PIN	GPIO		Function
5	3	Output	RS
7	4	Output	D6
29	5	Output	D0
26	7	Output	D2
32	12	Input	FF	
8	14	-		UART TX
10	15	-		UART RX
11	17	Output	D5
12	18	Input	EF
15	22	Output	D8
16	23	Output	W
18	24	Output	D3
22	25	Output	D1
13	27	Output	D4
3	2	Output	Enable converter
31	6	
24	8	-		CEN0
21	9	-		MISO
19	10	-		MOSI
23	11	-		CLK
33	13	
36	16	
35	19	
38	20	
37	26	
40	21
*/


void setup_io(); // initialise pointers
void init_io(); // setup input / output pins
void IO_ON(); // enable level converters
void IO_OFF(); // disable level converters
void reset_FIFO(); // reset FIFO
void reset_FIFO(unsigned char en); // pull reset high / low

void write_FIFO_byte(unsigned int inByte); //write byte to FIFO, includes translation to match circuit wiring
unsigned char read_FF(); //read status of full flag
unsigned char read_EF(); //read status of empty flag
unsigned int convert_byte(unsigned char inByte); // convert input byte to 32bit gpio value for faster setup






#endif /* INTUV_H */

