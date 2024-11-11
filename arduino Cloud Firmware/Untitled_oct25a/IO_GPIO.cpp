#include <DFRobot_MCP23017.h>
#include "IO_GPIO.hpp"
// #include <Wire.h>
DFRobot_MCP23017 mcp(Wire, 0x20);  // Definition

void GPIOManager::setup() {
 
// Wire.begin();
// Wire.setClock(100000);
  while (mcp.begin() != 0) {
    Serial.println("Initialization of the chip failed, please confirm that the chip connection is correct!");
    delay(1000);
  }
   Serial.println("Initialization");
  mcp.pinMode(mcp.eGPA0, INPUT);
  mcp.pinMode(mcp.eGPA1, INPUT);
  mcp.pinMode(mcp.eGPA2, INPUT);
  mcp.pinMode(mcp.eGPA3, INPUT);
  mcp.pinMode(mcp.eGPA4, INPUT);
  mcp.pinMode(mcp.eGPA5, INPUT);
  mcp.pinMode(mcp.eGPB0, OUTPUT);
  mcp.pinMode(mcp.eGPB1, OUTPUT);
  mcp.pinMode(mcp.eGPB2, OUTPUT);
  mcp.pinMode(mcp.eGPB3, OUTPUT);
  mcp.pinMode(mcp.eGPB4, OUTPUT);
  mcp.pinMode(mcp.eGPB5, OUTPUT);
  mcp.pinMode(mcp.eGPB6, OUTPUT);
}

bool GPIOManager::readPin(uint8_t pin) {
  return (mcp.digitalRead(static_cast<DFRobot_MCP23017::ePin_t>(pin)) == 1);
}


// Reading Button States
bool GPIOManager::getHomeGPIO() {
  return readPin(mcp.eGPA3);
}
bool GPIOManager::getUpGPIO() {
  return readPin(mcp.eGPA0);
}
bool GPIOManager::getDownGPIO() {
  return readPin(mcp.eGPA1);
}
bool GPIOManager::getSelectGPIO() {
  return readPin(mcp.eGPA2);
}
bool GPIOManager::getStartGPIO() {
  return readPin(mcp.eGPA4);
}
bool GPIOManager::getStopGPIO() {
  return readPin(mcp.eGPA5);
}

// writting
void GPIOManager::resetPump() {
  mcp.digitalWrite(mcp.eGPB0, LOW);
  mcp.digitalWrite(mcp.eGPB1, LOW);
  mcp.digitalWrite(mcp.eGPB2, LOW);
  mcp.digitalWrite(mcp.eGPB3, LOW);
  mcp.digitalWrite(mcp.eGPB4, LOW);
  mcp.digitalWrite(mcp.eGPB5, LOW);
  mcp.digitalWrite(mcp.eGPB6, LOW);
  delay(100);
}


void GPIOManager::setFoodSourcePump_1() {
  mcp.digitalWrite(mcp.eGPB0, HIGH);
  delay(100);
}


void GPIOManager::setBasePump_2() {
  mcp.digitalWrite(mcp.eGPB1, HIGH);
  delay(100);
}

void GPIOManager::stopBasePump_2() {
  mcp.digitalWrite(mcp.eGPB1, LOW);
  delay(100);
}

void GPIOManager::setAcidPump_3() {
  mcp.digitalWrite(mcp.eGPB2, HIGH);
  delay(100);
}

void GPIOManager::stopAcidPump_3() {
  mcp.digitalWrite(mcp.eGPB2, LOW);
  delay(100);
}

void GPIOManager::setAirPump_4() {
  mcp.digitalWrite(mcp.eGPB3, HIGH);
  delay(100);
}

void GPIOManager::setACmotor_5() {
  mcp.digitalWrite(mcp.eGPB4, HIGH);
  delay(100);
}

void GPIOManager::setUVlight_6() {
  mcp.digitalWrite(mcp.eGPB5, HIGH);
  delay(100);
}
