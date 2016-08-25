//This sketch is for use with the NANO 3 Arduino board

//Include compile header files
#include <Wire.h> //I2C communication
#include <SHT2x.h> //humidity & temp sensor


//define global pin assignments
//analog pins A0 = 14 to A7 = 21
const int mirSS   =  2;
const int mirLD   =  3;
const int mirCLK  =  4;
const int trigEN  =  5;
//6
const int focSTOP =  7;
const int focB    =  8;
const int focA    =  9;
const int focEN   =  10; 
const int uvSTAT  =  11;
const int uvEN    =  12;
const int tecEN   =  13;
const int therm1  =  14; //A0
const int therm2  =  15; //A1
const int tecMON  =  16; //A2
                          //A3
// SDA = A4 / 18
// SCL = A5 / 19
//const int uvPWR   =  20; //A6
const int trigPHOT = 21; //A7


//Define variables (global) - read bit from byte for bool values DO NOT use Bool on Arduino
//Return values, 0x01 = error bit, 0x00 = no error bit, other byte values specific errors


uint8_t bool_0 = 0; //boolean holder for information that cannot be easily called from pins
//speedSet = bit 0; //has the motor speed been set?
//MCP3421Set = bit 1; //has the MCP3421 been configured?
// = bit 2;
// = bit 3;
// = bit 4;
// = bit 5;
// = bit 6;
// = bit 7;


// generic read buffer 
uint8_t rBuffer[4];

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*-----------------------------------DEFINE FUNCTIONS--------------------------------------*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
// Add completed functions to the case list in the void loop to allow access


/*=========================================================================================*/
/*-------------------------------------Read sensors----------------------------------------*/
/*=========================================================================================*/


/*=========================================================================================*/
//001/0x01 enable or disable the TEC
/*=========================================================================================*/
uint8_t tecSet(uint8_t state){
  
  if (state == 0x01){
    digitalWrite(tecEN, HIGH);
    return 0x00; //ok
  }
  else {
    digitalWrite(tecEN, LOW);
    return 0x00; //ok
  }
  return 0x01; //not ok - error bit
  
}


/*=========================================================================================*/
//002/0x02 check status of stop limits for focus motor
/*=========================================================================================*/
uint8_t chkStop(){
  
  rBuffer[0] =  digitalRead(focSTOP);
  return 0x00; //OK
}



/*=========================================================================================*/
//003/0x03 move focus motor - use rPi to remember the initial position and increment logic
/*=========================================================================================*/
uint8_t focMove(uint8_t pos){ 
  //provide initial position, then 
  //need to store in config file on rPi
  
  
  if (digitalRead(focSTOP) == LOW){
     digitalWrite(focA, bitRead(pos,0)); //set position
     digitalWrite(focB, bitRead(pos,1));
     
     digitalWrite(focEN, HIGH); //then enable motor
     delay(5); //wait for motor to respond, minimum 1us
     digitalWrite(focEN, LOW); //then switch off the motor
     
     return 0x00; //ok
  }

  else {return 0x01;} //not ok - Error bit
  //if zero then the stop was enabled, check limit stop explicitly
}





/*=========================================================================================*/
//004/0x04 read status of motor speed lock
/*=========================================================================================*/
uint8_t checkLock(){

  rBuffer[0] =  digitalRead(mirLD);
  return 0x00; //OK
}



/*=========================================================================================*/
//005/0x05 set motor speed (may or may not be running)
/*=========================================================================================*/
//mirror motor; 9 poles, 12 magnets, 3 phases, divide no. magnets by phases to get pulse multiplier; 4
//Min PWM frequency = 75Hz -> 19Hz motor speed

uint8_t speedMotor(uint8_t mSpeed){ // motor speed in Hertz
    
    if (mSpeed > 19){
      tone(mirCLK, ((int)mSpeed * 4)); // convert to PMW frequency required for requested Hz
      bitWrite(bool_0,0,1); //speedSet = 1;
      return 0x00;
    }
    else {
      bitWrite(bool_0,0,0); //speedSet = 0;
      return 0x01; //return 1 to indicate error speed to slow
    }
  
}


/*=========================================================================================*/
//006/0x06 enable motor
/*=========================================================================================*/
uint8_t setMotor(uint8_t state){
  
  if (state == 0x01 && bitRead(bool_0,0) == 1){  //speedSet = bit 0
    digitalWrite(mirSS, LOW); //active low
    return 0x00;
  }
  else if (state == 0x00){
    digitalWrite(mirSS, HIGH);
    return 0x00;
  }
  else {return 0x01;} //return 1 to indicate error, need to set the speed before enabeling
}


/*=========================================================================================*/
//007/0x07 ----------------------generic write to MCP3421 over I2C-------------------------------*/
/*=========================================================================================*/

uint8_t genMCP3421w(uint8_t addr, uint8_t reg) { 
  Wire.beginTransmission(addr); //start write, I2C address
  Wire.write(reg);  //write to register
  
  uint8_t error = Wire.endTransmission();//return 1 to indicate error, ready for next command
  
  bitWrite(bool_0,1,1); //MCP3421Set = bit 1
  

return error; 
}

/*
//setup the MCP23017 expander
void SetMCP_OUT() {
  Wire.begin((uint8_t) 0x4E); //start write, hard coded address
  Wire.write((uint8_t) 0x00);  //write to bank A
  Wire.write((uint8_t) 0x00);  //set to output
  Wire.end();       //close transmission

  Wire.begin((uint8_t) 0x4E); //start write, hard coded address
  Wire.write((uint8_t) 0x01);  //write to bank B
  Wire.write((uint8_t) 0x00);  //set to output
  Wire.end();       //close transmission
}


//write data to bank of expander MCP23017
void WriteMCP_data(uint8_t data, bool bank) {
  Wire.begin((uint8_t) 0x4E); //start write, hard coded address
  
  if (bank == 0){ //1 = bank A, 0 = bank B
    Wire.write((uint8_t) 0x00);  //write to bank A
  }
  else {
    Wire.write((uint8_t) 0x01);  //write to bank B
  }

  Wire.write((uint8_t) data); //write data to bank
  Wire.end();       //close transmission
}
*/

/*=========================================================================================*/
//008/0x08 ----------------------generic read from MCP3421 over I2C-------------------------------*/
/*=========================================================================================*/

//ensure that the config is writen before making a read
uint8_t genMCP3421r(uint8_t addr) { 
  //ref. http://forum.arduino.cc/index.php?topic=211587.0
  //re. data sheet table 5-3 & figure 5-2
  
  uint8_t H, L, C;
  int8_t error = 0x01;

  if (bitRead(bool_0,1) == 1) { //MCP3421Set = bit 1
  Wire.beginTransmission(addr); //start write, I2C address
    
 
  Wire.requestFrom(addr, 3); //Tell slave we need to read 2bytes from the current register
  
  while (Wire.available() < 3){
    ; //wait
  }
  

    H = Wire.read(); //read High byte
    L = Wire.read(); // read Low byte
    C = Wire.read(); //read config data
  
  
  error = Wire.endTransmission(); //return 1 to indicate error, close transmission

  rBuffer[0] = H;
  rBuffer[1] = L;
  rBuffer[2] = C;
  }
  
  return error; 
}



/*=========================================================================================*/
//009/0x09 -------------------generic write to MAX_Device over I2C-------------------------------*/
/*=========================================================================================*/
// both the MAX5825 & MAX5217 use similar protocols, just different command registers

uint8_t genMAX_DACw(uint8_t addr, uint8_t reg, uint8_t dataH, uint8_t dataL) { 
  Wire.beginTransmission(addr); //start write, I2C address
  Wire.write(reg);  //write to register
  Wire.write(dataH);  //write data word
  Wire.write(dataL);  //write data word
  
  uint8_t error = Wire.endTransmission();//return 1 to indicate error, ready for next command
  
return error; //return 1 to indicate completion, ready for next command
}

/*=========================================================================================*/
//010/0x0A ------------------generic read from MAX_Device over I2C-------------------------------*/
/*=========================================================================================*/
// both the MAX5825 & MAX5217 use similar protocols, just different command registers

uint8_t genMAX_DACr(uint8_t addr, uint8_t reg) { 
  //ref. http://forum.arduino.cc/index.php?topic=211587.0
  //re. data sheet Fig 6 - standard read sequence
  byte H, L, C;
  uint8_t error;

  Wire.beginTransmission(addr); //start write, I2C address
  Wire.write(reg);  //write to register address
  delay(100);

  Wire.requestFrom(addr, 2); //Tell slave we need to read 2bytes from the current register
  
  while (Wire.available() < 2){
    ; //wait
  }
    
    H = Wire.read();
    L = Wire.read();


  error = Wire.endTransmission();

 rBuffer[0] = H; //print out reading
 rBuffer[1] = L;
  
  
return error; //return 1 to indicate error
}

/*=========================================================================================*/
//011/0x0B --------------------------------get humidity or temp----------------------------------*/
/*=========================================================================================*/
uint8_t genSHT(uint8_t x){ //0 = return humidity, 1 = return temp
  float a;
   Wire.begin();
   
   if (x == 0){
    a = SHT2x.GetHumidity(); // %
   }
   else {
    a = SHT2x.GetTemperature(); // 'C
    }

    rBuffer[0] = (uint8_t)(a);  //get integer part of the value 3.141 -> 3
    rBuffer[1] = (uint8_t)(fmod(a,1)*10); //get decimal part ie 3.141 -> 0.141 *10 -> 14
    
    uint8_t error = Wire.endTransmission();//return 1 to indicate error, ready for next command
    
return error; //return 1 to indicate error
}


/*=========================================================================================*/
//012/0x0C ---------------------Write to MCP23017------------------------------------*/
/*=========================================================================================*/

// 

uint8_t genMCP_IOw(uint8_t addr, uint8_t reg, uint8_t dataH) { 
  Wire.beginTransmission(addr); //start write, I2C address
  Wire.write(reg);  //write to register
  Wire.write(dataH);  //write data word
  Wire.endTransmission();       //close transmission

  uint8_t error = Wire.endTransmission();//return 1 to indicate error, ready for next command
  
return error; //return 1 to indicate error
}

/*=========================================================================================*/
//013/0x0D enable or disable the Trigger laser
/*=========================================================================================*/

uint8_t trigLaser(uint8_t state){
  
    if (state == 0x01){
    digitalWrite(trigEN, HIGH);
    return 0x00; //ok
  }
  else if (state == 0x00){
    digitalWrite(trigEN, LOW);
    return 0x00; //ok
  }
  else {return 0x01;} //return 1 to indicate error
}


/*=========================================================================================*/
//014/0x0E read UV laser status
/*=========================================================================================*/

uint8_t uvLaserStat(){
  
  uint8_t reading = 0;
  
  for (uint8_t i = 0; i < 10; i++)
  {
    reading = digitalRead(uvSTAT);
    if(reading == 1)
        {
            break;
        }
  }
  
  rBuffer[0] = reading;
  
  return 0x00; //OK
}

/*=========================================================================================*/
//015/0x0F read trigger laser photodiode
/*=========================================================================================*/

uint8_t trigReadPhot(){

  float tmp = analogRead(trigPHOT) * (5.0 / 1023.0);
  
  rBuffer[0] = (uint8_t)(tmp);  //get integer part of the value
  rBuffer[1] = (uint8_t)(fmod(tmp,1)*10); //get decimal part 

  return 0x00;  
}



/*=========================================================================================*/
//016/0x10 UV laser enable
/*=========================================================================================*/

uint8_t uvLaserEN(uint8_t state){
  
  if (state = 0x01){
    digitalWrite(uvEN, HIGH);
    return 0x00; //OK
  }
  else {
    digitalWrite(uvEN, LOW);
    return 0x00; //OK
  }
  
  return 0x01; //return 1 to indicate error
}

/*=========================================================================================*/
//017/0x11 read TEC voltage / current
/*=========================================================================================*/

uint8_t TECread(){
  
  float tmp = analogRead(tecMON) * (5.0 / 1023.0);

  rBuffer[0] = (uint8_t)(tmp);  //get integer part of the value
  rBuffer[1] = (uint8_t)(fmod(tmp,1)*10); //get decimal part 

  return 0x00;
}



/*=========================================================================================*/
//018/0x12
/*=========================================================================================*/



//019/0x13
//020/0x14
//021/0x15
//022/0x16
//023/0x17
//024/0x18
//025/0x19
//026/0x1A
//027/0x1B
//028/0x1C
//029/0x1D
//030/0x1E
//031/0x1F
//032/0x20



/*=========================================================================================*/
/* ----------------------CRC ATM-8 check------------------------------*/
/*=========================================================================================*/

uint8_t crcTable[256]; //storage of CRC lookup table

void CalulateTable_CRC8() //fill table
{
  uint8_t generator = 0x83; //ATM-8 CRC (8bit) code 
  // iterate over all byte values 0- 255
  for (int dividend = 0; dividend < 256; dividend++)
  {
    uint8_t currByte = dividend; 
    //calculate the CRC-8 value for current byte
    for (uint8_t bitSec = 0; bitSec < 8; bitSec++)
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


uint8_t computeCRC(uint8_t message[], int nBytes) //process byte string to get crc or make a test
{
  uint8_t crc = 0x0;
 
  for (int b = 0; b < nBytes; ++b)
  {
    //XOR in next input byte
    uint8_t data = message[b] ^ crc;
    //get current CRC value = remainder
    crc = crcTable[data];
  }
  
  return crc;
}



/*=========================================================================================*/
/* ----------------------Switch code to parse serial commands------------------------------*/
/*=========================================================================================*/
  
void processIncomingCommand ()
{
  //interface structure here to communicate with rPiZero and trigger functions in response

  int BuffSize = 6; // 6 byte buffer
  uint8_t Buffer[BuffSize]; // command string storage
  uint8_t returnBuffer[BuffSize]; //return buffer
  memset(returnBuffer,0,sizeof(returnBuffer)); //initialise array to zeros
  
  //command, in0, in1, in2, in3, checksum byte 

 /*
  -> recieve command
  read bytes into array
  parse command against checksum to confirm command
  <- notify valid/invalid based on checksum
  run command based on switch selection
  <- return command variable 
*/

//
// Read bytes into array
//
  Serial.readBytes(Buffer, BuffSize); //read 6 bytes into buffer array

//
//check CRC8 for valid transmitted data (6th byte is CRC code)
//
 if (computeCRC(Buffer, BuffSize) == 0)
   {
    
// 
// Check passes, process command,
// Run function based on the the command byte
//
     if (Buffer[0] == 0x01)
      {
       returnBuffer[0] = tecSet(Buffer[1]); 
     }
     
     //--------------------------------------------------- 
     else if (Buffer[0] == 0x02)
     {
       returnBuffer[0] = chkStop();
       returnBuffer[1] = rBuffer[0];
      }
     
     //---------------------------------------------------  
      else if (Buffer[0] == 0x03)
      {
       returnBuffer[0] = focMove(Buffer[1]);
      }
     
     //---------------------------------------------------  
      else if (Buffer[0] == 0x04)
      {
        if (checkLock() == 0x00)
       {
        returnBuffer[0] = 0x00;
        returnBuffer[1] = rBuffer[0];
       }
       else {returnBuffer[0] = 0x01;}
     }
     
     //--------------------------------------------------- 
     else if (Buffer[0] == 0x05)
     {
       returnBuffer[0] = speedMotor(Buffer[1]);
     }
     
     //--------------------------------------------------- 
     else if (Buffer[0] == 0x06)
     {
       returnBuffer[0] = setMotor(Buffer[1]);
      }
     
     //--------------------------------------------------- 
     else if (Buffer[0] == 0x07)
     {
       returnBuffer[0] = genMCP3421w(Buffer[1], Buffer[2]);
      }
     
     //--------------------------------------------------- 
     else if (Buffer[0] == 0x08)
      {
       if (genMCP3421r(Buffer[1]) == 0x00)
       {
        returnBuffer[0] = 0x00;
        returnBuffer[1] = rBuffer[0];
        returnBuffer[2] = rBuffer[1];
        returnBuffer[3] = rBuffer[2];
       }
       else {returnBuffer[0] = 0x01;}
      }
      
      //--------------------------------------------------- 
      else if (Buffer[0] == 0x09)
      {
        returnBuffer[0] = genMAX_DACw(Buffer[1], Buffer[2], Buffer[3], Buffer[4]);
      }
      
      //--------------------------------------------------- 
      else if (Buffer[0] == 0x0A)
      {
        if (genMAX_DACr(Buffer[1], Buffer[2]) == 0x00)
        {
          returnBuffer[0] = 0x00;
          returnBuffer[1] = rBuffer[0];
          returnBuffer[2] = rBuffer[1];
        }
        else {returnBuffer[0] = 0x01;}
      }
     
     //--------------------------------------------------- 
     else if (Buffer[0] == 0x0B)
      {
        if (genSHT(Buffer[1]) == 0x00)
        {
          returnBuffer[0] =  0x00;
          returnBuffer[1] = rBuffer[0];
          returnBuffer[2] = rBuffer[1];
        }
        else {returnBuffer[0] = 0x01;}
      }

      //--------------------------------------------------- 
     else if (Buffer[0] == 0x0C)
      {
      returnBuffer[0] = genMCP_IOw(Buffer[1], Buffer[2], Buffer[3]);
      }
      
      //--------------------------------------------------- 
      else if (Buffer[0] == 0x0D)
      {
      returnBuffer[0] = trigLaser(Buffer[1]);
      }
      
      //--------------------------------------------------- 
      else if (Buffer[0] == 0x0E)
      {
        if (uvLaserStat() == 0x00)
        {
          returnBuffer[0] = 0x00;
          returnBuffer[1] = rBuffer[0];
        }
        else {returnBuffer[0] = 0x01;} 
      }

      //--------------------------------------------------- 
      else if (Buffer[0] == 0x0F)
      {
      
        if (trigReadPhot() == 0x00)
        {
          returnBuffer[0] = 0x00;
          returnBuffer[1] = rBuffer[0];
          returnBuffer[2] = rBuffer[1];
        }
        else {returnBuffer[0] = 0x01;}  
      }

      //--------------------------------------------------- 
      else if (Buffer[0] == 0x10)
      {
      returnBuffer[0] = uvLaserEN(Buffer[1]);
      }

      //--------------------------------------------------- 
      else if (Buffer[0] == 0x11)
      {
        if (TECread() == 0x00)
        {
          returnBuffer[0] = 0x00;
          returnBuffer[1] = rBuffer[0];
          returnBuffer[2] = rBuffer[1];
        }
        else {returnBuffer[0] = 0x01;}   
      }

      //--------------------------------------------------- 
      else if (Buffer[0] == 0x12)
      {
      
      }

//
// House keeping confirm transmission and flush buffer for next command
//  
      returnBuffer[BuffSize-1] = computeCRC(returnBuffer, BuffSize-1); //calculate return CRC
      Serial.write(returnBuffer, BuffSize);  //write back CRC'd buffer with data, confirmation of execution or error
      Serial.flush(); //flush the buffer to avoid problems on next read
    }
    
  else { //there has been an error reading the command
      returnBuffer[0] = 0x6F; // value 111 command invalid
      returnBuffer[BuffSize-1] = computeCRC(returnBuffer, BuffSize-1); //calculate return CRC
      Serial.write(returnBuffer, BuffSize);  //write back CRC'd buffer with error (111 / 0x6F)
      Serial.flush(); //flush the buffer to avoid problems on next read
    }  
    
 } // end of processIncomingByte  



/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*----------------------------------INITIALIZE CODE----------------------------------------*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
//Void setup functions is to initialise pins etc. and only run once when power applied
void setup() {
  // put your setup code here, to run once:
  
//set pin directions I/O            access function written?
pinMode(trigPHOT, INPUT);           //
pinMode(tecMON, INPUT);             //
pinMode(therm1, INPUT);             //not used
pinMode(therm2, INPUT);             //not used
pinMode(focEN, OUTPUT);             //ok
digitalWrite(focEN, LOW);

pinMode(uvEN, OUTPUT);              //
digitalWrite(uvEN, LOW);     //FIFO read line pulled up by resistor,  
pinMode(uvSTAT, INPUT);             //

pinMode(tecEN, OUTPUT);             //ok
digitalWrite(tecEN, LOW);    //disable TEC at startup
pinMode(mirSS, OUTPUT);             //ok
digitalWrite(mirSS, HIGH);
pinMode(mirLD, INPUT);              //ok
pinMode(mirCLK, OUTPUT);            //ok
digitalWrite(mirCLK, LOW);
pinMode(trigEN, OUTPUT);             //ok
digitalWrite(trigEN, LOW);   //disable Trigger laser at startup
pinMode(focSTOP, INPUT);             //ok
pinMode(focB, OUTPUT);               //ok
digitalWrite(focB, LOW);
pinMode(focA, OUTPUT);               //ok
digitalWrite(focA, LOW);

CalulateTable_CRC8(); //generate CRC table

Wire.begin();  
Serial.begin(115200);

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif


// Initialisation settings, transfer these to Raspberry Pi code to allow tuning
/*
genMAX_DACw(0x1C, 0x08, 0x00, 0x10); //setup the trigger laser DAC configuration
genMAX_DACw(0x1C, 0x01, 0x57, 0x0C); //set trigger laser DAC to 1.7v

genMAX_DACw(0x1F, 0x08, 0x00, 0x10); //setup the TEC DAC configuration
genMAX_DACw(0x1F, 0x01, 0x99, 0x9C); //set TEC DAC to 1.0V ~25'C

//tecSet(0x01); //enable TEC
trigLaser(0x01); //enable trigger laser

//genMCP3421w(0x68, 0x0B); //setup ADC for single shot, 16bit, x8 gain


//set GPIO settings
genMCP_IOw(0x27, 0x00, 0x00); //set A as output
genMCP_IOw(0x27, 0x01, 0x00); //set B as output

//set frequency 5MHz
genMCP_IOw(0x27, 0x12, 0x00); //FQ_UD low & CLK low
genMCP_IOw(0x27, 0x13, 0x00); //phase data
genMCP_IOw(0x27, 0x12, 0x40); //CLK high - load
genMCP_IOw(0x27, 0x12, 0x00); //FQ_UD low & CLK low
genMCP_IOw(0x27, 0x13, 0x02); //bit 32 through 25
genMCP_IOw(0x27, 0x12, 0x40); //CLK high - load
genMCP_IOw(0x27, 0x12, 0x00); //FQ_UD low & CLK low
genMCP_IOw(0x27, 0x13, 0x9B); //bit 24 through 17
genMCP_IOw(0x27, 0x12, 0x40); //CLK high - load
genMCP_IOw(0x27, 0x12, 0x00); //FQ_UD low & CLK low
genMCP_IOw(0x27, 0x13, 0x7D); //bit 16 through 9
genMCP_IOw(0x27, 0x12, 0x40); //CLK high - load
genMCP_IOw(0x27, 0x12, 0x00); //FQ_UD low & CLK low
genMCP_IOw(0x27, 0x13, 0x1D); //bit 8 through 1
genMCP_IOw(0x27, 0x12, 0x40); //CLK high - load
genMCP_IOw(0x27, 0x12, 0x00); //FQ_UD low & CLK low
genMCP_IOw(0x27, 0x12, 0x80); //FQ_UD high & CLK low
genMCP_IOw(0x27, 0x12, 0x00); //FQ_UD low & CLK low 


genMAX_DACw(0x10, 0x23, 0x00, 0x00); //set REF to 4.096v
genMAX_DACw(0x10, 0x80, 0x00, 0x00); //upload DAC_0 setting
genMAX_DACw(0x10, 0x81, 0x00, 0x00); //upload DAC_1 setting
genMAX_DACw(0x10, 0x82, 0x00, 0x00); //upload DAC_2 setting
genMAX_DACw(0x10, 0x83, 0x00, 0x00); //upload DAC_3 setting
genMAX_DACw(0x10, 0x84, 0x00, 0x00); //upload DAC_4 setting
genMAX_DACw(0x10, 0x85, 0x00, 0x00); //upload DAC_5 setting
genMAX_DACw(0x10, 0x86, 0x00, 0x00); //upload DAC_6 setting
genMAX_DACw(0x10, 0x87, 0x00, 0x00); //upload DAC_7 setting
genMAX_DACw(0x10, 0x98, 0x00, 0x00); //apply settings to all DACs

*/
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*----------------------------------------MAIN CODE----------------------------------------*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
// Void loop function is main body of code
void loop() {
  // put your main code here, to run repeatedly:


// if serial data available, process it
  while (Serial.available () > 0) {
    processIncomingCommand (); //print back return error
  }
  // do other stuff here like testing digital input (button presses) ...




}
