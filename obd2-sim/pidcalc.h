#ifndef pidcalc_h
#define pidcalc_h

  #include "Arduino.h"
  #include <inttypes.h>

  //how large the range of the potentiometers is.
  #define POTRANGE 923.0
  //call the specific getData or getValue function
  void getReturnData(uint8_t pid, uint8_t txdata[8],float value = 0);
  float getReturnValue(uint8_t pid,const int value);
  //getData stores the raw data in the passed array
  void getDataPid0C(uint8_t txdata[],const float value);
  void getDataPid0D(uint8_t txdata[],const float value);
  void getDataPid05(uint8_t txdata[],const float value);
  void getDataPid0A(uint8_t txdata[],const float value);
  void getDataPid11(uint8_t txdata[],const float value);
  void getDataPid0B(uint8_t txdata[],const float value);
  void getDataPid0F(uint8_t txdata[],const float value);
  void getDataPid00(uint8_t txdata[]);
  void getDataPid04(uint8_t txdata[],const float value);
  void getDataPid0E(uint8_t txdata[],const float value);
  //getValue are the human readable values
  float getValuePid0C(const int value);
  float getValuePid0D(const int value);
  float getValuePid05(const int value);
  float getValuePid0A(const int value);
  float getValuePid11(const int value);
  float getValuePid0B(const int value);
  float getValuePid0F(const int value);
  float getValuePid04(const int value);
  float getValuePid0E(const int value);
  float roundTo(float numToRound, float step = 0);
  
#endif
