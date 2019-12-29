
#include <Wire.h> //lib used for I2C comms with the main PIC MCU
#include <mcp_can.h> //lib includes functions for reading/sending data on the can bus
#include <mcp_can_dfs.h>
#include <SPI.h> //lib used to provide high speed communication with the external CANBUS transiever and modules

//CANBUS node var
const int SPI_CS_PIN = 10; //Chip select pin
MCP_CAN CAN(SPI_CS_PIN);    // Set CS pin

//data for tranmission
enum info { RPM=0, GEAR, TPS, SPEED_MPH,WATER_TEMP, AIR_TEMP, MAP, BATT_VOLT, AMBIENT_TEMP, CANBUS_ERROR,BUFFER_SIZE}; //using 'BUFFER_SIZE' as parameter for buffer size, enum can be extended to add more data 
double dataArray[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //17 pieces of data collected from ECU can bus

//debug - uncomment to view the message in serial terminal
#define DEBUG //Use debugging tool print out information via the serial comms, disable when not debugging as it will cause issues with code latency
//comment out the define if you wish to not print out the serial messages

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
