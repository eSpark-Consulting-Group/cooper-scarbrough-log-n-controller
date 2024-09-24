#ifndef RS485MODBUS_HPP
#define RS485MODBUS_HPP

#include <Arduino.h>  // Add this line
#include <HardwareSerial.h>

class RS485Modbus {
public:
  RS485Modbus(uint8_t rxPin, uint8_t txPin, long baudRate);
  void begin();
  void sendPacket(byte* packet, size_t length);
  float receivePacket();

private:
  uint8_t rxPin;
  uint8_t txPin;
  long baudRate;

  float printReceivedData(byte* buffer, int length);
  float parseFloat(byte* buffer);
};

#endif  // RS485MODBUS_HPP