#include "obd2.h"
#include "pidcalc.h"

//try to start the CAN interface with the correct baudrate
//endless loop because device can be bussy
void initInterface(MCP_CAN& CAN){
  while (CAN_OK!=CAN.begin(CAN_500KBPS)){
    delay(100);
  }
}

//set filter to get messages only from a specific range
void setFilter(MCP_CAN& CAN){
  /*
   * Filter and mask:
   * Masks are 2 and filters are 6
   * All masks and filters must be set.
   * Specify as first mask and then the valid filters.
   */
  
  //mask indicates which bits should be considered when filtering. 8 says the last three bits don't matter.
  //The range 0x7E0 to 0x7E7 is filtered here
  CAN.init_Mask(0, 0, 0x7F8);
  CAN.init_Filt(0, 0, 0x7E0);
  CAN.init_Filt(1, 0, 0x7E0);

  //all bits should be checked here
  //here is looked after the id 0x7FF
  CAN.init_Mask(1, 0, 0x7FF);
  CAN.init_Filt(2, 0, 0x7DF);
  CAN.init_Filt(3, 0, 0x7DF);
  CAN.init_Filt(4, 0, 0x7DF);
  CAN.init_Filt(5, 0, 0x7DF);
  delay(100);
}


//check for new messages and if a new message recived save it in the reference variables. Only responseid the pid and in wich mode the obd reader wants the message is necessery.
bool checkMsg(MCP_CAN& CAN, uint8_t &dataType, uint8_t &pid, unsigned long &responseId){
  unsigned char len = 0;
  uint8_t rxbuf[8];
  if (CAN_MSGAVAIL == CAN.checkReceive()){
    CAN.readMsgBuf(&len, rxbuf);

    unsigned long queryId = CAN.getCanId(); //read CAN-ID to calculate the right id for the requested message.

    //check if extended frame CAN.isExtendedFrame();
    //check if request frame CAN.isRemoteRequest();
    
    if (queryId == 0x7DF){
      responseId = 0x7E8; //if broadcast id was sent then the main ecu id should be used fot the response.
    } else {
      responseId = queryId+8; //if it´s another id from the range 0x7E0 to 0x7Et then 8 should be added
    }
    if (len > 2){
      pid = rxbuf[2];
      dataType = rxbuf[1];
    }
    return true;
  } else {
    return false;
  }
}

//everything that has arrived via the checkmsg function will be used again in this function. Additionally the modified pid values 
void sendMsg(MCP_CAN& CAN, uint8_t dataType, uint8_t pid, unsigned long responseId, float responseValue){
  uint8_t txdata[] = {0x03, 0x41, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00};
  if (dataType == 0x01){
    txdata[1] = 0x41; // service type increased by 0x40
    txdata[2] = pid; // pid that has arrived can be used for the data that will be send

    getReturnData(pid, txdata, responseValue);
   
    //first canid, then if it´s a standard or extended frame, then the length of the data and at the end the data
    CAN.sendMsgBuf(responseId, 0, 8, txdata);
  }
}
