
#include "FirstScript.hpp"

RS485Modbus modbus(RXD2, TXD2, 4800);

RS485ModbuspH modebus_pH(RX_PH, TX_PH, 4800);

void getScripts::setup() {
  // Serial.begin(115200);
  modebus_pH.begin();
  delay(100);
  modbus.begin();
  getScripts::deviationpHValue();
  delay(100);
  // getScripts::atmosphPressure();
  // delay(100);
  // getScripts::zeroPointCalOxygenSens();
}

float getScripts::getOxygenSensorValue() {
  byte sendBuffer[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x06, 0xC5, 0xC8 };  // Query Frame
  modbus.sendPacket(sendBuffer, sizeof(sendBuffer));
  delay(100);

  float oxygenConcentration = modbus.receivePacket();
  if (oxygenConcentration >= 0) {
    // Serial.print("Received Oxygen Concentration: ");
    // Serial.println(oxygenConcentration);  // Display the concentration
  }
  delay(100);
  return oxygenConcentration;
}

float getScripts::getpHSensorValue() {
  byte sendpHBuffer[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B };
  // byte sendpHBuffer[] = { 0x01, 0x05, 0x00, 0x50, 0x00, 0x0A, 0x09, 0xDC };
  modebus_pH.sendPacket(sendpHBuffer, sizeof(sendpHBuffer));
  delay(100);

  float pHvalue = modebus_pH.receivePhPacket();
  if (pHvalue >= 0) {
    // Serial.print("Received pH value: ");
    // Serial.println(pHvalue);  // Display the ph value
  }
  delay(100);
  return pHvalue;
}

void getScripts::deviationpHValue() {
  byte sendpHBuffer1[] = { 0x01, 0x06, 0x00, 0x50, 0x00, 0x0A, 0x09, 0xDC };
  modebus_pH.sendPacket(sendpHBuffer1, sizeof(sendpHBuffer1));
  delay(100);
  modebus_pH.receivePhdeviation();
}

// void getScripts::zeroPointCalOxygenSens() {
//   byte sendOxygenBuffer[] = { 0x01, 0x06, 0x10, 0x10, 0x00, 0x00, 0x4D, 0x0F };  // Query Frame
//   modbus.sendPacket(sendOxygenBuffer, sizeof(sendOxygenBuffer));
//   delay(100);
// }

// void getScripts::atmosphPressure() {
//   byte sendOpressureBuffer[] = { 0x01, 0x06, 0x10, 0x22, 0x27, 0x97, 0x77, 0x5E };  // Query Frame
//   modbus.sendPacket(sendOpressureBuffer, sizeof(sendOpressureBuffer));
//   delay(100);
// }