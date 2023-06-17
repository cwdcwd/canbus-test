#include <SPI.h>
#include <mcp_can.h>

// Define the CAN bus speed (in bps)
#define CAN_BUS_SPEED 500000

// Define the CAN message ID
#define CAN_MESSAGE_ID 0x123

// Define the CAN message data
uint8_t canData[] = {0x01, 0x02, 0x03, 0x04, 0x05};

// Initialize the MCP_CAN object
MCP_CAN CAN(10); // Set CS pin to pin 10 (you can change it if needed)

void setup()
{
  // Initialize the Serial communication
  Serial.begin(9600);

  // Initialize the CAN bus
  if (CAN.begin(CAN_500KBPS) == CAN_OK)
  {
    Serial.println("CAN bus initialized successfully!");
  }
  else
  {
    Serial.println("Error initializing CAN bus...");
  }
}

void loop()
{
  // Create a CAN message object
  CAN_MSG canMsg;

  // Set the message ID
  canMsg.can_id = CAN_MESSAGE_ID;

  // Set the message length
  canMsg.can_dlc = sizeof(canData);

  // Copy the data to the message payload
  memcpy(canMsg.data, canData, sizeof(canData));

  // Send the CAN message
  if (CAN.sendMsgBuf(canMsg.can_id, canMsg.can_dlc, canMsg.data) == CAN_OK)
  {
    Serial.println("CAN message sent!");
  }
  else
  {
    Serial.println("Error sending CAN message...");
  }

  // Wait for some time before sending the next message
  delay(1000);
}
