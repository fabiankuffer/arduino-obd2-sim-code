/* 
 * Website: http://www.hobbytronics.co.uk/arduino-external-eeprom
 * copyright www.hobbytronics.co.uk 2012
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version. see <http://www.gnu.org/licenses/>
 * 
 * parts of the script are copied from the website, but most of it is written by me.
 */
#ifndef eeprom_h
#define eeprom_h

#define MENUSIZE 9

#include "Arduino.h"
#include <inttypes.h>

  class Eeprom {
    private:
      int address;
      //titles stored in the rom.
      const String defaultTitles[MENUSIZE] = {
        "RPM",
        "Speed",
        "Kuehlwassertemperatur",
        "Kraftstoffdruck",
        "Gaspedalstellung",
        "MAP-Sensor",
        "Lufteinlasstemperatur",
        "Engine Load",
        "Timing advance"
      };
      //pids stored in the rom.
      const byte defaultPids[MENUSIZE]{
        0x0C,
        0x0D,
        0x05,
        0x0A,
        0x11,
        0x0B,
        0x0F,
        0x04,
        0x0E
      };
      
      //arrays that store the data from the eeprom
      String storedTitles[MENUSIZE];
      uint8_t storedPids[MENUSIZE];
      
      //size of the eeprom
      unsigned int size = 32768;
      void writeByte(unsigned int eeaddress, uint8_t data);
      void writeString(unsigned int eeaddress, String data, uint8_t breakByte = 0x00);
      String readString(unsigned int eeaddress, uint8_t breakByte = 0x00);
      void writePage(unsigned int eeaddresspage, String data);
      String readPage(unsigned int eeaddresspage);
      void clear();
      unsigned int length();
      void update(unsigned int eeaddress, uint8_t val);
    public:
      Eeprom(uint8_t address);
      uint8_t readByte(unsigned int eeaddress);
      void readMenu(String titles[], uint8_t pids[]);
      void getData(String titles[], uint8_t pids[]);
      bool diff(const String defaultTitles[],const String storedTitles[],const uint8_t defaultPids[],const uint8_t storedPids[]);
      void writeMenu(const String titles[],const uint8_t pids[]);
  };

#endif
