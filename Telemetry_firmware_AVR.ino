
#include <mcp_can.h> //lib includes functions for reading/sending data on the can bus
#include <mcp_can_dfs.h>
#include <SPI.h> //lib used to provide high speed communication with the external CANBUS transiever and modules

//CANBUS node var
const int SPI_CS_PIN = 10; //Chip select pin
MCP_CAN CAN(SPI_CS_PIN);    // Set CS pin

//data for tranmission
#define printDebugMessage 0
#define printMessageForTransiever 1
enum infoECU { RPM = 0, TPS, WATER_TEMP, AIR_TEMP, MAP, LAMBDA, KPH, OIL_PRESSURE, FUEL_PRESSURE, OIL_TEMP, BATT_VOLT, FUEL_CONN_1, GEAR, ADVANCE_DEG, INJECTION_MS, FUEL_CONN_2, RPM_NO, BUFFER_SIZE}; //using 'BUFFER_SIZE' as parameter for buffer size, enum can be extended to add more data
double dataArray[BUFFER_SIZE] = {0}; //17 pieces of data collected from ECU can bus

//debug - uncomment to view the message in serial terminal
#define DEBUG //Use debugging tool print out information via the serial comms, disable when not debugging as it will cause issues with code latency
//comment out the define if you wish to not print out the serial messages and transmit data to transiever module

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200); //set up the serial (UART) with 19200 baud to communicate with terminal/transiever module. Note: Baudrate of transiever and arduino must match to synchronize, changing baudrate of transiever module possible using pixhawk manager
  //ECU is default able to communicate with 1Mbps ONLY
  while (CAN_OK != CAN.begin(CAN_1000KBPS))
  {
    Serial.println("CAN BUS Shield has failed. Init CAN BUS Shield again...");
    delay(500); //re-attempt to connect after a delay
  }
  Serial.println("CAN BUS Shield init at 1Mbps ok!");
}

void loop() {
  // put your main code here, to run repeatedly:

  readECUdata();

#ifdef DEBUG
  printOutECUdata(printDebugMessage); //print out data in certain way for readability for the user for debugging
#else
  printOutECUdata(printMessageForTransiever); //send message in csv format for the transiever module
#endif

  delay(1000);
}

void readECUdata() {
  
//data stored in following order first num is array index
    // 0 - RPM*
    // 1 - TPS*
    // 2 - Water temp C*
    // 3 - Air temp C*
    // 4 - MAP Kpa*
    // 5 - Lambda*
    // 6 - KPH
    // 7 - Oil P Kpa
    // 8 - Fuel P Kpa
    // 9 - Oil Temp
    // 10 - Volts
    // 11 - Fuel Con L/Hr
    // 12 - Gear
    // 13 - Advance Deg
    // 14 - Injection ms
    // 15 - Fuel Con L/100 Km
    //16 - RPM NO
    
  if (CAN_MSGAVAIL == CAN.checkReceive())           // check if data coming
  {
    unsigned char len = 0;
    unsigned char buf[8];
    CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

    unsigned int canId = CAN.getCanId();
    if (canId == 8192) //2000 - location of data which is spat out by the ECU for the following data
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

//@brief
// function prints out what is within the can bus array for debugging purposes
void printOutECUdata(int mode)
{

  printoutput("RPM:", (int)dataArray[RPM], mode);
  printoutput("TPS:", (int)dataArray[TPS], mode);
  printoutput("Water Temp:", (int)dataArray[WATER_TEMP], mode);
  printoutput("Air Temp:", (int)dataArray[AIR_TEMP], mode);
  printoutput("MAP Kpa:", (int)dataArray[MAP], mode);
  printoutput("Lambda:", (int)dataArray[LAMBDA], mode);
  printoutput("Kph:", (int)dataArray[KPH], mode);
  printoutput("Oil P kpa:", (int)dataArray[OIL_PRESSURE], mode);
  printoutput("Fuel Pkpa:", (int)dataArray[FUEL_PRESSURE], mode);
  printoutput("Oil Temp:", (int)dataArray[OIL_TEMP], mode);
  printoutput("Battery Volts:", (int)dataArray[BATT_VOLT], mode);
  printoutput("Fuel Con:", (int)dataArray[FUEL_CONN_1], mode);
  printoutput("gear:", (int)dataArray[GEAR], mode);
  printoutput("Advance deg:", (int)dataArray[ADVANCE_DEG], mode);
  printoutput("Injection ms:", (int)dataArray[INJECTION_MS], mode);
  printoutput("Fuel Con L/100km:", (int)dataArray[FUEL_CONN_2], mode);
  printoutput("RPM NO:", (int)dataArray[RPM_NO], mode);

  Serial.println(); //return carriage and newline

}

void printoutput(String var, int data, int mode) {
  switch (mode) {
    case printDebugMessage:
      Serial.print(var);
      Serial.print(data);
      Serial.print(" | ");
      break;
    case printMessageForTransiever:
    //print for csv format
      Serial.print(data);
      Serial.print(",");
      break;
  }
}
