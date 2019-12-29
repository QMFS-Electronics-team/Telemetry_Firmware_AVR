
#include <mcp_can.h> //lib includes functions for reading/sending data on the can bus
#include <mcp_can_dfs.h>
#include <SPI.h> //lib used to provide high speed communication with the external CANBUS transiever and modules

//CANBUS node var
const int SPI_CS_PIN = 10; //Chip select pin
MCP_CAN CAN(SPI_CS_PIN);    // Set CS pin

//data for tranmission
enum info { RPM = 0, GEAR, TPS, SPEED_MPH, WATER_TEMP, AIR_TEMP, MAP, BATT_VOLT, AMBIENT_TEMP, CANBUS_ERROR, BUFFER_SIZE}; //using 'BUFFER_SIZE' as parameter for buffer size, enum can be extended to add more data
double dataArray[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //17 pieces of data collected from ECU can bus

//debug - uncomment to view the message in serial terminal
#define DEBUG //Use debugging tool print out information via the serial comms, disable when not debugging as it will cause issues with code latency
//comment out the define if you wish to not print out the serial messages

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200); //set up the serial (UART) with 19200 baud to communicate with terminal/transiever module. Note: Baudrate of transiever and arduino must match to synchronize, changing baudrate of transiever module possible using pixhawk manager
  //ECU is default able to communicate with 1Mbps ONLY
  while (CAN_OK != CAN.begin(CAN_1000KBPS))
  {
    Serial.println("CAN BUS Shield has failed. Init CAN BUS Shield again...");
    delay(500); //re-attempt to connect after a delay
  }
  Serial.println("CAN BUS Shield init ok!");
}

void loop() {
  // put your main code here, to run repeatedly:

  if (CAN_MSGAVAIL == CAN.checkReceive())           // check if data coming
  {
    unsigned char len = 0;
    unsigned char buf[8];
    CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

    unsigned int canId = CAN.getCanId();
    if (canId == 8192) //2000
    {
      dataArray[0] = ((((double)combineBytes(buf[0], buf[1])) / 14500) * 100); //RPM
      dataArray[1] = ((((double)combineBytes(buf[2], buf[3])) / 25600) * 100); //TPS
      dataArray[2] = (((double)combineBytes(buf[4], buf[5])) / 256); //Water Temp
      dataArray[3] = (((double)combineBytes(buf[6], buf[7])) / 256); //Air Temp
      dataArray[16] = ((double)combineBytes(buf[0], buf[1])); //RPM NO
    }
    else if (canId == 8193) //2001
    {

      dataArray[4] = (((double)combineBytes(buf[0], buf[1])) / 256); //MAP Kpa
      dataArray[5] = (((double)combineBytes(buf[2], buf[3])) / 1000); //Lambda
      dataArray[6] = ((double)combineBytes(buf[4], buf[5])) / 10; //Kph
      dataArray[7] = ((double)combineBytes(buf[6], buf[7])); //Oil P kpa
    }
    else if (canId == 8194) //2002
    {
      dataArray[8] = ((double)combineBytes(buf[0], buf[1])); // Fuel Pkpa
      dataArray[9] = ((double)combineBytes(buf[2], buf[3])); // Oil Temp
      dataArray[10] = ((double)combineBytes(buf[4], buf[5])) / 10; //Volts
      dataArray[11] = ((double)combineBytes(buf[6], buf[7])) / 10; // Fuel Con
    }
    else if (canId == 8195) //2003
    {
      dataArray[12] = ((double)combineBytes(buf[0], buf[1])); //gear
      dataArray[13] = ((double)combineBytes(buf[2], buf[3])) / 10; // Advance deg
      dataArray[14] = ((double)combineBytes(buf[4], buf[5])) / 100; //Injection ms
      dataArray[15] = ((double)combineBytes(buf[6], buf[7])) / 10; //Fuel Con L/100km
    }

  }
}

uint16_t combineBytes(byte left, byte right)
{
  uint16_t newnumber = (left << 8);
  newnumber = newnumber + right;
  return newnumber;
}
