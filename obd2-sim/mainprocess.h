#ifndef mainprocess_h
#define mainprocess_h

  #include "Arduino.h"
  #include <inttypes.h>
  #include "screen.h"
  #include "eeprom.h"

  #define NUMBEROFBUTTONS 2
  #define NUMBEROFSWITCHES 2
  #define NUMBEROFPOTIS 9
  #define MENUSIZE 9
  #define VALUES_POTIS_SMOOTHING_ROUNDS 10

  class MainProcess {
    private:
      bool switchClosed[2], initMenu = true;
      const uint8_t menuButtonPins[NUMBEROFBUTTONS] = {3,4};
      const uint8_t switchesPins[NUMBEROFSWITCHES] = {5,6};
      int valuesPotis[NUMBEROFPOTIS];
      float valuesCalc[NUMBEROFPOTIS];
      uint8_t currentPos, pressedButton = 0, nextPos = 0;
      String storedTitles[MENUSIZE];
      uint8_t storedPids[MENUSIZE];
    public:
      MainProcess();
      void setData(String titles[], byte pids[]);
      void checkButtons();
      void updateScreen(Screen *display);
      void checkValues(Screen *display, float valuesPid[]);
      void initPins();
      void checkSwitches(Screen *display);
  };
  
#endif
