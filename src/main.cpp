// CAN Send Example
//
#include <Arduino.h>
#include <mcp_can.h>
#include <SPI.h>

#include <StepperMotor.h>

// #define PIN_SLEEP 7 // CWD-- Sleep Pin
#define STEP01 0    // CWD-- Step Pin
#define DIR01 1     // CWD-- Direction Pin
// #define STEP02 3    // CWD-- Step Pin
// #define DIR02 4     // CWD-- Direction Pin

#define CAN0_INT 24
#define CAN0_CS 10
// Hardware pin definitions
int UVOUT = A9;   // Output from the sensor
int REF_3V3 = A8; // 3.3V power on the board

unsigned long lastRunUV = 0;

//CWD-- can manager
long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
byte data[8] = {0x00, 0x01, 0x02, 0x03, 0xFF, 0xFE, 0xFD, 0xFC};
char msgString[128]; // Array to store serial string

MCP_CAN CAN0(CAN0_CS); // Set CS to pin 10

//CWD-- stepper controller
StepperMotor *stepperMotor;

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

  Serial.print("---------------\noutput: ");
  Serial.print(refLevel);

  Serial.print(" sensor output: ");
  Serial.print(uvLevel);

  Serial.print(" / sensor voltage: ");
  Serial.print(outputVoltage);
  Serial.print(" / uvMappedByIOLevel: ");
  Serial.print(uvMappedByIOLevel);

  Serial.print(" / UV Intensity (mW/cm^2): ");
  Serial.print(uvIntensity);

  Serial.println("\n---------------\n");

  return uvIntensity;
}

void runUV() {
  float uvIntensity = takeReading();
  byte *b = floatToArray(uvIntensity);

  for (int i = 0; i < 4; i++)
  {
    data[i] = b[i];
  }

  // send data:  ID = 0x100, Standard CAN Frame, Data length = 8 bytes, 'data' = array of data bytes to send
  byte sndStat = CAN0.sendMsgBuf(0x100, 0, 8, data);
  Serial.println(sndStat);

  if (sndStat == CAN_OK)
  {
    Serial.println("Message Sent Successfully!");
  }
  else
  {
    Serial.println("Error Sending Message...");
  }

  lastRunUV = micros();
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Setup UV Pins");
  pinMode(UVOUT, INPUT);
  pinMode(REF_3V3, INPUT);

  pinMode(CAN0_INT, INPUT); // Configuring pin for /INT input
  stepperMotor = new StepperMotor();
  stepperMotor->init(STEP01, DIR01, 1000, true);
  stepperMotor->start();

  // Initialize MCP2515 running at 8MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");

  CAN0.setMode(MCP_NORMAL); // Change to normal mode to allow messages to be transmitted
}


void loop()
{
  stepperMotor->control();

  if (micros() - lastRunUV > 1000000){
    runUV();
  }

  if (!digitalRead(CAN0_INT)) {
    Serial.println("Message Received!");
    CAN0.readMsgBuf(&rxId, &len, rxBuf); // Read data: len = data length, buf = data byte(s)

    if ((rxId & 0x80000000) == 0x80000000) // Determine if ID is standard (11 bits) or extended (29 bits)
      sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    else
      sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);

    Serial.print(msgString);

    if ((rxId & 0x40000000) == 0x40000000)
    { // Determine if message is a remote request frame.
      sprintf(msgString, " REMOTE REQUEST FRAME");
      Serial.print(msgString);
    }
    else
    {
      for (byte i = 0; i < len; i++)
      {
        sprintf(msgString, " 0x%.2X", rxBuf[i]);
        Serial.print(msgString);
      }

      stepperMotor->changeDirection((stepperMotor->getDirection() == HIGH ? LOW : HIGH));
    }

    Serial.println();
  }
}



