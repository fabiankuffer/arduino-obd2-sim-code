//orginally written by DFRobot. Rewrote some funtions. No license found.
//https://www.dfrobot.com/

#include "screen.h"
#include "screen_dfs.h"
#include <Wire.h>


Screen::Screen(uint8_t address){
  this->address = address;
  this->backlightval = LCD_NOBACKLIGHT;
}

void Screen::init(){
   if (TWCR == 0) // do this check so that Wire only gets initialized once
  {
    Wire.begin();
  }
  this->displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
  delay(50);
  // Now we pull both RS and R/W low to begin commands
  expanderWrite(this->backlightval);
  delay(1000);

  //put the LCD into 4 bit mode
  // this is according to the hitachi HD44780 datasheet
  // figure 24, pg 46

  // we start in 8bit mode, try to set 4 bit mode
  write4bits(0x03 << 4);
  delayMicroseconds(4500); // wait min 4.1ms

  // second try
  write4bits(0x03 << 4);
  delayMicroseconds(4500); // wait min 4.1ms

  // third go!
  write4bits(0x03 << 4);
  delayMicroseconds(150);

  // finally, set to 4-bit interface
  write4bits(0x02 << 4);

  // set # lines, font size, etc.
  command(LCD_FUNCTIONSET | this->displayfunction);

  // turn the display on with no cursor or blinking default
  this->displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  display();

  // clear it off
  clear();

  // Initialize to default text direction (for roman languages)
  this->displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

  // set the entry mode
  command(LCD_ENTRYMODESET | this->displaymode);

  home();
}

void Screen::expanderWrite(uint8_t data){
  Wire.beginTransmission(this->address);
  Wire.write((int)(data) | this->backlightval);
  Wire.endTransmission();
}


void Screen::write4bits(uint8_t value) {
  expanderWrite(value);
  pulseEnable(value);
}

void Screen::pulseEnable(uint8_t data){
  expanderWrite(data | En);  // En high
  delayMicroseconds(1);   // enable pulse must be >450ns
  expanderWrite(data & ~En); // En low
  delayMicroseconds(50);    // commands need > 37us to settle
}


inline void Screen::command(uint8_t value) {
  send(value, 0);
}


void Screen::send(uint8_t value, uint8_t mode) {
  uint8_t highnib=value&0xf0;
  uint8_t lownib=(value<<4)&0xf0;
  write4bits((highnib)|mode);
  write4bits((lownib)|mode);
}

void Screen::display(){
  this->displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | this->displaycontrol);
}

void Screen::clear(){
  command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void Screen::home(){
  command(LCD_RETURNHOME);  // set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void Screen::setCursor(uint8_t x, uint8_t y){
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if ( y > this->rows ) {
    y = this->rows-1;    // we count rows starting w/0
  }
  command(LCD_SETDDRAMADDR | (x + row_offsets[y]));
}

void Screen::clearLine(byte line){
  if (line >= 0 && line <= this->rows-1){
    setCursor(0,line);
    print(this->clearString);
  }
}


void Screen::turnBacklightOn(bool on){
  if(on){
    this->backlightval=LCD_BACKLIGHT;
    expanderWrite(0);
  } else {
    this->backlightval=LCD_NOBACKLIGHT;
    expanderWrite(0);
  }
}

void Screen::createChar(uint8_t location, uint8_t charmap[]){
  location &= 0x7; // we only have 8 locations 0-7
  command(LCD_SETCGRAMADDR | (location << 3));
  for (int i=0; i<8; i++) {
    write(charmap[i]);
  }
}

inline size_t Screen::write(uint8_t value) {
  send(value, Rs);
  return 1;
}

uint8_t Screen::getAddress(){
  return this->address;
}

void Screen::updateTitle(String title){
  clearLine(0);
  setCursor(0,0);
  print(title);
}

//functions to update the value line on the screen
void Screen::updateValue(float newValue){
  clearLine(1);
  setCursor(0,1);
  print(newValue);
}

void Screen::updateValue(int newValue){
  clearLine(1);
  setCursor(0,1);
  print(newValue);
}

void Screen::updateValue(char newValue){
  clearLine(1);
  setCursor(0,1);
  print(newValue);
}

void Screen::updateValue(String newValue){
  clearLine(1);
  setCursor(0,1);
  print(newValue);
}

void Screen::scrollText(String text, uint8_t x, uint8_t y){
  uint8_t strlength = text.length();
  uint8_t maxlength = cols-x;
  uint8_t moves = strlength - maxlength;
  if (strlength > maxlength){
    for (int i = 0; i <= moves; i++){
      setCursor(x,y);
      print(text.substring(0+i,maxlength+i));
      delay(1000);
    }
    setCursor(x,y);
    print(text.substring(0,maxlength));
  } else {
    setCursor(x,y);
    print(text);
  }
}

bool Screen::backlightOn(){
  if (backlightval == LCD_BACKLIGHT){
    return true;
  } else{
    return false;
  }
}
