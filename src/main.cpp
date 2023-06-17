#include <FlexCAN.h>

#define CAN_BUS_SPEED 500000 // Define the CAN bus speed (in bps)
#define CAN_MESSAGE_ID 0x123 // Define the CAN message ID

uint8_t canData[] = {0x01, 0x02, 0x03, 0x04, 0x05}; // Define the CAN message data
FlexCAN CANbus; // Initialize the CAN bus object

void setup()
{
  Serial.begin(9600);          // Initialize the Serial communication
  CANbus.begin(CAN_BUS_SPEED); // Initialize the CAN bus with the specified speed
}

void loop()
{
  CAN_message_t msg; // Create a CAN message object
  msg.id = CAN_MESSAGE_ID; // Set the message ID
  msg.len = sizeof(canData); // Set the message length
  memcpy(msg.buf, canData, sizeof(canData)); // Copy the data to the message payload
  CANbus.write(msg); // Send the CAN message
  Serial.println("CAN message sent!"); // Print a confirmation message
  delay(1000); // Wait for some time before sending the next message
}
