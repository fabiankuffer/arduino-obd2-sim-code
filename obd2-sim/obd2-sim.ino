#include "mainprocess.h"
#include "screen.h"
#include "eeprom.h"
#include "obd2.h"
#include "mcp_can.h"

//definition of the spi pin for the can shield. Either 10 or 9
#define SPI_CS_PIN 10

//create objects for eeprom, display, can shield and mainfunctions
Screen display(0x3F);
Eeprom memory(0x50);
MainProcess mainFunctions;
MCP_CAN CAN(SPI_CS_PIN);

//global array with the calculated values for the pids to use it in other objects
float valuesPid[NUMBEROFPOTIS];
//list of pids in the same order as the values of the pids to know wich values belong to which pid
byte pids[9];

void setup() {
  //start the display
  display.init();

  //array of string to store the titles for the display
  String titles[9];

  //load all titles and pids from the eeprom and pass them to the mainfunction object for later use
  memory.getData(titles, pids);
  mainFunctions.setData(titles, pids);

  //Initialization of all needed pins
  mainFunctions.initPins();

  //start the can shield with the correct baudrate and set all filters
  initInterface(CAN);
  setFilter(CAN);

  //check both swichtes to see if the backlight needs to be turned off
  mainFunctions.checkSwitches(&display);
  //read values from the potis and display new values and store the values in the valuespid array
  mainFunctions.checkValues(&display, valuesPid);
  //initalisation of the screen content
  mainFunctions.updateScreen(&display);
}

void loop() {
  //read values from the potentiometers and display new values and store values in the valuespid array
  mainFunctions.checkValues(&display, valuesPid);

  //declare the variables to be filled with the checkMsg function
  unsigned long responseId;
  byte pid, dataType;

  //call the checkMsg function
  bool messageArrived = checkMsg(CAN, dataType, pid, responseId);

  //check if a new message has arrived
  if (messageArrived){

    //run through the valuespid array to get the correct value for the response
    byte loop = true;
    float responseValue = 0;
    for (byte i = 0; i < 9 && loop; i++){
      if (pid == pids[i]){
        responseValue = valuesPid[i];
        loop = false;
      }
    }
    //return response
    sendMsg(CAN, dataType, pid, responseId, responseValue);
  }


  //check both swichtes to see if the backlight needs to be turned off
  mainFunctions.checkSwitches(&display);
  //check buttons to see if the title needs to be updated
  mainFunctions.checkButtons();
  //if a button was pressed update screen
  mainFunctions.updateScreen(&display);
}
