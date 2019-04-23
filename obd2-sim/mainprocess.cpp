#include "mainprocess.h"
#include "pidcalc.h"

MainProcess::MainProcess(){
}

//function to set all pins for this project
void MainProcess::initPins(){
  //set all button inputs
  for (uint8_t t = 0; t < NUMBEROFBUTTONS; t++){
    pinMode(this->menuButtonPins[t],LOW);
  }
  
  //set all switch inputs
  for (uint8_t t = 0; t < NUMBEROFSWITCHES; t++){
    pinMode(this->switchesPins[t],LOW);
  }
  
  //set pins for the multiplexer. One pin as input to read the value. The remaining pins as binary indication of which petentiometer is used.
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(A3,INPUT);
}

//check both switches. Screen object is needed because one switch toggles the backlight
void MainProcess::checkSwitches(Screen *display){
  //run through all switches and look at whether they are closed or open.
  //If it is switch 1, turn on or off the backlight.
  for (uint8_t t = 0; t < NUMBEROFSWITCHES; t++){
    if (digitalRead(this->switchesPins[t]) == HIGH){
      //turn on the backlight when the first switch is closed
      if (t == 1 && this->switchClosed[t] != true){
        display->turnBacklightOn(true);
      }
      this->switchClosed[t] = true;
    } else {
      //turn off the backlight when the first switch is open
      if (t == 1 && this->switchClosed[t] == true){
        display->turnBacklightOn(false);
      }
      this->switchClosed[t] = false;
    }
  }
}

//check which button was clicked. Only the first button that is tested will be returned.
//The script waits until the button is released again.
void MainProcess::checkButtons(){
  bool run = true;
  uint8_t button = 0;
  uint8_t t = 0;
  //run through all buttons
  do {
    if (digitalRead(menuButtonPins[t]) == HIGH){
      //as long as the button is pressed run this loop. Check for new obd messages and send messages.
      while (digitalRead(menuButtonPins[t]) == HIGH){}
      run = false;
      button=t+1;
    }
    t++;
  } while (run && t < NUMBEROFBUTTONS);
  this->pressedButton = button;
}

//read values from the potentiometers
void MainProcess::checkValues(Screen *display, float valuesPid[]){
  int valuessmoothing[NUMBEROFPOTIS];
  //read potentiometer multiple times to smooth the values
  for (int i = 0; i < VALUES_POTIS_SMOOTHING_ROUNDS; i++){
    //loop through all potentiometers
    for (int zaehler = 0; zaehler < NUMBEROFPOTIS; zaehler++){
      if (i == 0){
        valuessmoothing[zaehler] = 0;
      }
      //set the multiplexer for the right input
      digitalWrite(A1, bitRead(zaehler, 0));
      digitalWrite(A2, bitRead(zaehler, 1));
      digitalWrite(7, bitRead(zaehler, 2));
      digitalWrite(8, bitRead(zaehler, 3));
  
      valuessmoothing[zaehler] += analogRead(A3);
      //if its the last smoothing round calc the smoothed value
      if (i+1 == VALUES_POTIS_SMOOTHING_ROUNDS){
        valuessmoothing[zaehler] /= 10;

        //if its more then 973 or less then 50 set the max or the minimum value. Max and min value because every potentiometer has a different range
        if(valuessmoothing[zaehler] > 973){
          valuessmoothing[zaehler] = 973;
        } else if(valuessmoothing[zaehler] < 50) {
          valuessmoothing[zaehler] = 50;
        }

        //values should start at 0
        valuessmoothing[zaehler] -= 50;

        //if new value is of by more then one calculate new value for this specific pid
        if (this->valuesPotis[zaehler]+1 < valuessmoothing[zaehler] || this->valuesPotis[zaehler]-1 > valuessmoothing[zaehler]){
          this->valuesPotis[zaehler] = valuessmoothing[zaehler];
          this->valuesCalc[zaehler] = getReturnValue(this->storedPids[zaehler], this->valuesPotis[zaehler]);
          valuesPid[zaehler] = this->valuesCalc[zaehler];
          //it the pid is displayed on the screen update the screen
          if (zaehler == this->currentPos){
            display->updateValue(this->valuesCalc[zaehler]);
          }
        }
        
        valuessmoothing[zaehler] = 0;
      }
    }
    delay(1);
  }
}

void MainProcess::updateScreen(Screen *display){
  //if a button was pressed
  if (this->pressedButton != 0){
    switch(this->pressedButton){
      case 1:
        //if the left button was pressed the menu should go one to the left
        if (this->currentPos == 0){
          this->nextPos = MENUSIZE - 1;
        } else {
          this->nextPos = this->currentPos - 1;
        }
        break;
      case 2:
        //if the right button was pressed the menu should go one to the right
        if (this->currentPos == MENUSIZE - 1){
          this->nextPos = 0;
        } else {
          this->nextPos = this->currentPos + 1;
        }
        break;
      default:
        break;
    }
  }

  //if next pid should be shown or if the menu will be initialized then update tilte and value on the screen
  if (this->currentPos != this->nextPos || this->initMenu){
    display->updateTitle(this->storedTitles[this->nextPos]);
    display->updateValue(this->valuesCalc[this->nextPos]);
    this->currentPos = this->nextPos;
  }

  if (this->initMenu){
      this->initMenu = false;
  }
}

//write titles and pids that are stored in the eeprom into the two arrays in this object.
void MainProcess::setData(String titles[], uint8_t pids[]){
  for(uint8_t i = 0; i < MENUSIZE; i++){
    this->storedTitles[i] = titles[i];
    this->storedPids[i] = pids[i];
  }
}
