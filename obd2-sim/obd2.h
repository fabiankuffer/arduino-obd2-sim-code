#ifndef obd2_h
#define obd2_h

#include "Arduino.h"
#include <inttypes.h>
#include "mcp_can.h"

void initInterface(MCP_CAN& CAN);
void setFilter(MCP_CAN& CAN);
bool checkMsg(MCP_CAN& CAN, uint8_t &dataType, uint8_t &pid, unsigned long &responseId);
void sendMsg(MCP_CAN& CAN, uint8_t dataType, uint8_t pid, unsigned long responseId, float responseValue = 0);

#endif
