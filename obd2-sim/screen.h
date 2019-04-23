//orginally written by DFRobot. Rewrote some funtions. No license found.
//https://www.dfrobot.com/
#ifndef screen_h
#define screen_h

#include "Arduino.h"
#include <inttypes.h>
#include "Print.h" 

  class Screen : public Print {
    private:
    
      //not beautiful but nothing else found to delete only one line
      String clearString = "                ";
      uint8_t address,
              cols = 16,
              rows = 2,
              backlightval,
              displaymode,
              displayfunction,
              displaycontrol;
              
      void clear();
      void command(uint8_t value);
      void expanderWrite(uint8_t data);
      void write4bits(uint8_t value);
      void pulseEnable(uint8_t data);
      void send(uint8_t value, uint8_t mode);
      void display();
      void home();
      void clearLine(uint8_t line);
    public:
      Screen(uint8_t address);
      void init();
      void turnBacklightOn(bool on);
      void createChar(uint8_t location, uint8_t charmap[]);
      void setCursor(uint8_t x, uint8_t y);
      virtual size_t write(uint8_t);
      uint8_t getAddress();
      void updateTitle(String title);

      //updateValue for different data types
      void updateValue(float newValue);
      void updateValue(int newValue);
      void updateValue(char newValue);
      void updateValue(String newValue);
      
      void scrollText(String text, uint8_t x, uint8_t y);
      bool backlightOn();
  };

#endif
