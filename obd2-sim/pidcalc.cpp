#include "pidcalc.h"

//no switch case used because if it´s compiled errors will occur
//call the specific function for the recieved pid
//store data in txdata array
//value is already calculated by the value function
void getReturnData(uint8_t pid, uint8_t txdata[8], float value){
  txdata[1] = 0x41;  // service type increased by 0x40
  txdata[2] = pid; // pid that has arrived can be used to send again
  txdata[0] = 0x03; //obd2 data length
  if (pid == 0x0C){
    txdata[0] = 0x04;
    getDataPid0C(txdata, value);
  } else if (pid == 0x0D){
    txdata[0] = 0x03;
    getDataPid0D(txdata, value);
  } else if (pid == 0x05){
    txdata[0] = 0x03;
    getDataPid05(txdata, value);
  } else if (pid == 0x0A){
    txdata[0] = 0x03;
    getDataPid0A(txdata, value);
  } else if (pid == 0x11){
    txdata[0] = 0x03;
    getDataPid11(txdata, value);
  } else if (pid == 0x0B){
    txdata[0] = 0x03;
    getDataPid0B(txdata, value);
  } else if (pid == 0x0F){
    txdata[0] = 0x03;
    getDataPid0F(txdata, value);
  } else if (pid == 0x00){
    txdata[0] = 0x06;
    getDataPid00(txdata);
  } else if (pid == 0x04){
    txdata[0] = 0x03;
    getDataPid04(txdata, value);
  } else if (pid == 0x0E){
    txdata[0] = 0x03;
    getDataPid0E(txdata, value);
  } else {
   
  }
}

//call the value function for the specific pid
//value is the raw potentiometer value
float getReturnValue(uint8_t pid,const int value){
  if(pid == 0x0C){
    return getValuePid0C(value);
  } else if (pid == 0x0D){
    return getValuePid0D(value);
  } else if (pid == 0x05){
    return getValuePid05(value);
  } else if (pid == 0x0A){
    return getValuePid0A(value);
  } else if (pid == 0x11){
    return getValuePid11(value);
  } else if (pid == 0x0B){
    return getValuePid0B(value);
  } else if (pid == 0x0F){
    return getValuePid0F(value);
  } else if (pid == 0x04){
    return getValuePid04(value);
  } else if (pid == 0x0E){
    return getValuePid0E(value);
  } else {
    return 0.0;
  }
}

//calculation can be found on the pid wiki page
void getDataPid0C(uint8_t txdata[],const float value){
  txdata[3] = (int)(value*4) >> 8;
  txdata[4] = (int)(value*4) & 0xFF;
}

void getDataPid0D(uint8_t txdata[],const float value){
  txdata[3] = (int)(value);
}

void getDataPid05(uint8_t txdata[],const float value){
  txdata[3] = (int)(value+40);
}

void getDataPid0A(uint8_t txdata[],const float value){
  txdata[3] = round(value/3.0);
}

void getDataPid11(uint8_t txdata[],const float value){
  txdata[3] = round(value/100.0*255.0);
}

void getDataPid0B(uint8_t txdata[],const float value){
  txdata[3] = (int)(value);
}

void getDataPid0F(uint8_t txdata[],const float value){
  txdata[3] = (int)(value+40);
}

void getDataPid00(uint8_t txdata[]){
  //bitwise indication whether pid is supported or not. B = 1011 means 1,3,4 is supported
  txdata[0] = 0x06;
  txdata[3] = 0x18;
  txdata[4] = 0x7D;
  txdata[5] = 0x80;
  txdata[6] = 0x00;
}

void getDataPid04(uint8_t txdata[],const float value){
  txdata[3] = round(value/100.0*255.00);
}

void getDataPid0E(uint8_t txdata[],const float value){
  txdata[3] = (value+64.0)/2.0;
}

float getValuePid0C(const int value){ //engine rpm
  //value is allowed between 0 and 16383.75. Steps: 0.25
  float min = 0, max = 16383.75;
  float rangeval = max - min;
  float exactvalue = (rangeval/POTRANGE)*value;
  return min+roundTo(exactvalue, 0.25);
}

float getValuePid0D(const int value){ //vehicle speed
  //value is allowed between 0 and 255.
  int min = 0, max = 255;
  int rangeval = max - min;
  return min+round((rangeval/POTRANGE)*value);
}

float getValuePid05(const int value){ //Engine coolant temperature
  //value is allowed between -40 and 215.
  int min = -40, max = 215;
  int rangeval = max - min;
  return min+round((rangeval/POTRANGE)*value);
}

float getValuePid0A(const int value){ //fuel pressure
  //value is allowed between 0 and 765.
  int min = 0, max = 765;
  int rangeval = max - min;
  return min+round((rangeval/POTRANGE)*value);
}

float getValuePid11(const int value){ //throttle position
  //value is allowed between 0 and 100.
  int min = 0, max = 100;
  int rangeval = max - min;
  return min+round((rangeval/POTRANGE)*value);
}

float getValuePid0B(const int value){ //MAP-Sensor
  //value is allowed between 0 and 255.
  int min = 0, max = 255;
  int rangeval = max - min;
  return min+round((rangeval/POTRANGE)*value);
}

float getValuePid0F(const int value){ //Air intake temperature
  //value is allowed between -40 and 215.
  int min = -40, max = 215;
  int rangeval = max - min;
  return min+round((rangeval/POTRANGE)*value);
}

float getValuePid04(const int value){ //Engine Load
  //value is allowed between 0 and 100.
  int min = 0, max = 100;
  int rangeval = max - min;
  return min+round((rangeval/POTRANGE)*value);
}

float getValuePid0E(const int value){ //Timing advance
  //value is allowed between -64 and 63.5. Steps: 0.5
  float min = -64.0, max = 63.5;
  float rangeval = max - min;
  float exactvalue = (rangeval/POTRANGE)*value;
  return min+roundTo(exactvalue, 0.5);
}


float roundTo(float numToRound, float step){
  if (step == 0){
    return numToRound;
  }

  float floor = ((long)(numToRound/step)) * step;
  float remainder = numToRound - floor;
  if (remainder >= step / 2){
    floor += step;
  }
  return floor;
}
