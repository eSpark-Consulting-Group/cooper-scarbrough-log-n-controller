#include "OxygenSensor.hpp"

RS485Modbus::RS485Modbus(uint8_t rxPin, uint8_t txPin, long baudRate)
  : rxPin(rxPin), txPin(txPin), baudRate(baudRate) {}

void RS485Modbus::begin() {
  Serial1.begin(baudRate, SERIAL_8N1, rxPin, txPin);
  // Serial.println("RS485 Modbus communication initialized.");
}

void RS485Modbus::sendPacket(byte* packet, size_t length) {
  Serial1.write(packet, length);
  // Serial.print("Sent: ");
  for (size_t i = 0; i < length; i++) {
    // Serial.print("0x");
    // Serial.print(packet[i], HEX);
    // Serial.print(" ");
  }
  Serial.println();
  delay(10);  // Wait for transmission to complete
}

float RS485Modbus::receivePacket() {
  byte recvBuffer[20];
  int index = 0;

  if (Serial1.available() > 0) {
    while (Serial1.available()) {
      recvBuffer[index++] = Serial1.read();
      if (index >= 20) break;  // Exit if we've read enough bytes
    }

    return printReceivedData(recvBuffer, index);
  } else {
    // Serial.println("No data received.");
    return -1;  // Return -1 to indicate no data
  }
}

float RS485Modbus::printReceivedData(byte* buffer, int length) {
  Serial.print("Received: ");
  for (int i = 0; i < length; i++) {
    Serial.print("0x");
    Serial.print(buffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  if (length >= 9) {
    float oxygenSaturation = parseFloat(buffer + 3);
    float oxygenConcentration = parseFloat(buffer + 7);
    float temperature = parseFloat(buffer + 11);

    Serial.print("Dissolved Oxygen Saturation: ");
    Serial.println(oxygenSaturation * 100);
    Serial.print("Dissolved Oxygen Concentration: ");
    Serial.println(oxygenConcentration);
    Serial.print("Temperature: ");
    Serial.println(temperature);

    return oxygenConcentration - 0.8;  // Return oxygen concentration
  }

  else {
    Serial.println("Incomplete or invalid data received.");
    return -1;  // Return -1 for invalid data
  }
}

float RS485Modbus::parseFloat(byte* buffer) {
  union {
    byte b[4];
    float f;
  } data;
  data.b[0] = buffer[3];  // Big endian
  data.b[1] = buffer[2];
  data.b[2] = buffer[1];
  data.b[3] = buffer[0];
  return data.f;
}
