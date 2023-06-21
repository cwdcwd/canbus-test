// CAN Send Example
//

#include <mcp_can.h>
#include <SPI.h>

// Hardware pin definitions
int UVOUT = A9;   // Output from the sensor
int REF_3V3 = A8; // 3.3V power on the board

//CWD-- can manager
MCP_CAN CAN0(10); // Set CS to pin 10

// Takes an average of readings on a given pin
// Returns the average
int averageAnalogRead(int pinToRead)
{
  byte numberOfReadings = 8;
  unsigned int runningValue = 0;

  for (int x = 0; x < numberOfReadings; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return (runningValue);
}

byte *floatToArray(float f) {
  byte *b = new byte[4];
  memcpy(b, &f, 4);
  // b[0] = (byte)(f >> 24);
  // b[1] = (byte)(f >> 16);
  // b[2] = (byte)(f >> 8);
  // b[3] = (byte)(f /*>> 0*/);
  return b;
}

// The Arduino Map function but for floats
// From: http://forum.arduino.cc/index.php?topic=3922.0
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float takeReading()
{
  int uvLevel = averageAnalogRead(UVOUT);
  int refLevel = averageAnalogRead(REF_3V3);
  Serial.print("UV Level: ");
  Serial.print(uvLevel);
  Serial.print(" / Ref Level: ");
  Serial.println(refLevel);

  // Use the 3.3V power pin as a reference to get a very accurate output value from sensor
  float outputVoltage = 3.3 / refLevel * uvLevel;

  float uvIntensity = mapfloat(outputVoltage, 0.0, 3.3, 0.0, 15.0);  // Convert the voltage to a UV intensity level
  float uvMappedByIOLevel = mapfloat(uvLevel, 0.0, 4095, 0.0, 15.0); // Convert the voltage to a UV intensity level

  Serial.print("output: ");
  Serial.print(refLevel);

  Serial.print(" sensor output: ");
  Serial.print(uvLevel);

  Serial.print(" / sensor voltage: ");
  Serial.print(outputVoltage);
  Serial.print(" / uvMappedByIOLevel: ");
  Serial.print(uvMappedByIOLevel);

  Serial.print(" / UV Intensity (mW/cm^2): ");
  Serial.print(uvIntensity);

  Serial.println();

  return uvIntensity;
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Setup UV Pins");
  pinMode(UVOUT, INPUT);
  pinMode(REF_3V3, INPUT);

  // Initialize MCP2515 running at 8MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");

  CAN0.setMode(MCP_NORMAL); // Change to normal mode to allow messages to be transmitted
}

byte data[8] = {0x00, 0x01, 0x02, 0x03, 0xFF, 0xFE, 0xFD, 0xFC};

void loop()
{
  float uvIntensity = takeReading();

  byte *b = floatToArray(uvIntensity);

  for(int i = 0; i < 4; i++) {
    data[i] = b[i];
  }

  // send data:  ID = 0x100, Standard CAN Frame, Data length = 8 bytes, 'data' = array of data bytes to send
  byte sndStat = CAN0.sendMsgBuf(0x100, 0, 8, data);
  Serial.println(sndStat);

  if (sndStat == CAN_OK) {
    Serial.println("Message Sent Successfully!");
  } else {
    Serial.println("Error Sending Message...");
  }
  delay(1000); // send data per 1000ms
}



