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

#include "eeprom.h"
#include <Wire.h>

Eeprom::Eeprom(uint8_t address){
  this->address = address;
  //check so that wire is only initalized once
  if (TWCR == 0)
  {
    Wire.begin();
  }
}

//clear the whole eeprom. My eeprom is prefilled with ones, so it will be filled with ones.
void Eeprom::clear(){
  for (unsigned int i = 0 ; i < this->length() ; i++) {
    update(i, 0xFF);
  }
}

//returns the length of the eeprom. return only the variable declared in eeprom.h
unsigned int Eeprom::length(){
  return this->size;
}

//checks if the byte is already stored in the eeprom. If it´s already stored nothing will be done
void Eeprom::update(unsigned int eeaddress, uint8_t val){
  if( readByte(eeaddress) != val ){
      writeByte(eeaddress, val);
    }
}

//reads a byte from the eeprom
//Address has to be split, because one byte cannot address the whole rom.
byte Eeprom::readByte(unsigned int eeaddress) 
{
  byte rdata = 0xFF;
 
  Wire.beginTransmission(this->address);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
 
  Wire.requestFrom(this->address,1);
 
  if (Wire.available()) rdata = Wire.read();
 
  return rdata;
}

//writes a byte in the eeprom
void Eeprom::writeByte(unsigned int eeaddress, uint8_t data) 
{
  Wire.beginTransmission(this->address);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.write(data);
  Wire.endTransmission();
  delay(5);
}

//function writes string over several pages if necessary. Per write operation only one page can be written which is 64 byte long.
//16 bytes are stored per memory operation, since the Arduino buffer is 32 bytes in size and 2 bytes must still be used for the address per call, 16 bytes are used for simpler calculating.
void Eeprom::writeString(unsigned int eeaddress, String data, uint8_t breakByte) 
{
  int laenge = data.length()+1; // length + 1 for the delimiter
  int countchar = 0;
  int pagespacefirst = 64-(eeaddress%64); //calculate the memory space in the first page that is used
  int rounds;
  int address = eeaddress; //address for the storage operation

  //calculate how many pages are needed
  if ((laenge-pagespacefirst) <= 0){
    rounds = 0;
  } else {
    rounds = ((laenge-pagespacefirst)/64)+1;
  }

  //Specifies how many bytes must be written in the last page. 
  int lastpagechars = (laenge-pagespacefirst)%64;

  if (laenge+eeaddress < this->length()){ //check if string fits into eeprom
    for (int i = 0; i <= rounds; i++){ //loop through all required pages

      //calculate how many bytes are available in this page
      int size;
      if (i == 0){
        if (laenge < pagespacefirst){
          size = laenge;
        } else {
          size = pagespacefirst; 
        }
      } else if (rounds == i){
        size = lastpagechars;
      } else {
        size = 64;
      }

      //number of available 16-byte blocks per page
      int blocks;
      if (size/16.0 < 0.0){
        blocks = 1;
      } else if (size%16 == 0) {
        blocks = size/16.0;
      } else { 
        blocks = (size/16.0)+1;
      }

      //loop through all blocks in the page
      for (int x = 0; x < blocks; x++){

        //calculate how much has to be written into a block
        int blocksize;
        if (size > 16){
          blocksize = 16;
          size -= 16;
        } else {
          blocksize = size;
        }

        //initiate write operation for a 16-byte block
        Wire.beginTransmission(this->address);
        Wire.write((int)((address) >> 8));   // MSB
        Wire.write((int)((address) & 0xFF)); // LSB

        //loop for each letter
        for (int y = 0; y < blocksize; y++){
          //if the string has been written, the breakbyte should be appended
          if (i == rounds && x+1 == blocks && y+1 == blocksize){
            Wire.write(breakByte);
          } else { //otherwise write a single letter from the string
            char partofstring = data[countchar];
            Wire.write(partofstring);
            countchar++;
          }
          //If it was the last pass of this block, it should be sent.
          if (y+1 == blocksize){
            Wire.endTransmission();
            delay(6);
          }
        }
        //enlarge address by the blocksize
        address+=blocksize;
      }
      
    }
  }
}

//read string to breakbyte
String Eeprom::readString(unsigned int eeaddress, uint8_t breakByte){
  String returnvalue="";
  bool runwhile = true;
  while (runwhile){
    Wire.beginTransmission(this->address);
    Wire.write((int)((eeaddress) >> 8)); // MSB
    Wire.write((int)((eeaddress) & 0xFF)); // LSB
    Wire.endTransmission();
    Wire.requestFrom(this->address,1);
    if (Wire.available())
    {
      char charat = Wire.read();
      returnvalue+=charat;
      if (charat == breakByte){
        runwhile = false;
      } else {
        eeaddress++;
      }
    }
  }
  return returnvalue;
}

//write a full page in the eeprom. this time the address is the page address
void Eeprom::writePage(unsigned int eeaddresspage, String data) {
    for (byte c = 0; c < 4; c++){
      Wire.beginTransmission(this->address);
      Wire.write((int)((eeaddresspage+16*c) >> 8)); // MSB
      Wire.write((int)((eeaddresspage+16*c) & 0xFF)); // LSB
      for (byte x = 0; x < 16; x++){
        if((16*c+x) < data.length()){
          Wire.write(data[16*c+x]);
        } else {
          Wire.write(0x0);
        }
        Wire.endTransmission();

        delay(6);
      }
    }
}

//read a whole page from the eeprom and return it as string
String Eeprom::readPage(unsigned int eeaddresspage) {
    String returnvalue="";
    Wire.beginTransmission(this->address);
    Wire.write((int)(eeaddresspage >> 8)); // MSB
    Wire.write((int)(eeaddresspage & 0xFF)); // LSB
    Wire.endTransmission();
    Wire.requestFrom(this->address,64);
    int c = 0;
    for ( c = 0; c < 64; c++ )
        if (Wire.available()) returnvalue += Wire.read();
    return returnvalue;
}

//reads menu entry from eeprom and stores it in the title Array and pid array
void Eeprom::readMenu(String titles[], uint8_t pids[]){

  //structure of the menu entry
  //1.title
  //2.0x1F
  //3.PID as a string
  //4.0x1D
  
  String title = "", pid = "";
  unsigned int address= 0;
  bool pidPart = false;
  uint8_t titleCount=0;
  unsigned int eepromLength =  length();
  uint8_t readByteFromEeprom = 0xFF;
  
  do {
    //read byte
    readByteFromEeprom = readByte(address);
    //if it is the end of a menu entry the following should be done
    if (readByteFromEeprom == 0x1D){
      titles[titleCount] = title;
      pids[titleCount] = pid.toInt();
      titleCount++;
      pidPart = false;
      title = "";
      pid = "";
    } else if (readByteFromEeprom == 0x1F){ //if the title was read now everything should be stored in pid
      pidPart = true;
    } else {
      if (pidPart){
        pid += (char)readByteFromEeprom;
      } else {
        title += (char)readByteFromEeprom;
      }
    }
    address++;
  }while(address < eepromLength && readByteFromEeprom != 0xFF && titleCount < MENUSIZE);
}

//Ccheck if there are differences between the default values and the values in the eeprom
bool Eeprom::diff(const String defaultTitles[],const String storedTitles[],const uint8_t defaultPids[],const uint8_t storedPids[]){
  bool returnvalue = false;
  for (uint8_t i = 0; i < MENUSIZE && !returnvalue; i++){
    if(storedPids[i] != defaultPids[i] || storedTitles[i] != defaultTitles[i]){
      returnvalue = true;
    }
  }
  return returnvalue;
}

//writes the titles and the pids in the eeprom
void Eeprom::writeMenu(const String titles[],const uint8_t pids[]){
  unsigned int address = 0;
  for (uint8_t i = 0; i < MENUSIZE; i++){
    writeString(address, titles[i], 0x1F);
    address += titles[i].length()+1;
    writeString(address, String(pids[i]), 0x1D);
    address += String(pids[i]).length()+1;
  }
}

//stores titles and pids from the arrays in the eeprom
void Eeprom::getData(String titles[], uint8_t pids[]){
  if(readByte(0)==0xFF){
    writeMenu(this->defaultTitles,this->defaultPids);
  } else {
    readMenu(this->storedTitles, this->storedPids);
    if(diff(this->defaultTitles,this->storedTitles,this->defaultPids,this->storedPids)){
      writeMenu(this->defaultTitles,this->defaultPids);
    }
  }
  for (uint8_t i = 0; i < MENUSIZE; i++){
    titles[i] = this->defaultTitles[i];
    pids[i] = this->defaultPids[i];
  }
}
