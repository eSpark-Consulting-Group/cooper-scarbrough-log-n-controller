#ifndef RS485MODBUSPH_HPP
#define RS485MODBUSPH_HPP

#include <Arduino.h>  // Add this line
#include <HardwareSerial.h>

class RS485ModbuspH {
public:
    RS485ModbuspH(uint8_t rxPin, uint8_t txPin, long baudRate);
    void begin();
    void sendPacket(byte* packet, size_t length);
    float receivePhPacket();
    void receivePhdeviation();

private:
    uint8_t rxPin;
    uint8_t txPin;
    long baudRate;

    float printReceivedData(byte* buffer, int length);
    float parseFloat(byte* buffer);
};

#endif // RS485MODBUSPH_HPP