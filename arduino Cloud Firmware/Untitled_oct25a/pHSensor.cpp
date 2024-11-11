

#include "pHSensor.hpp"

RS485ModbuspH::RS485ModbuspH(uint8_t rxPin, uint8_t txPin, long baudRate)
    : rxPin(rxPin), txPin(txPin), baudRate(baudRate) {}

void RS485ModbuspH::begin() {
    Serial2.begin(baudRate, SERIAL_8N1, rxPin, txPin);
    // Serial.println("RS485 Modbus communication initialized.");
}

void RS485ModbuspH::sendPacket(byte* packet, size_t length) {
    Serial2.write(packet, length);
    Serial.print("Sent: ");
    for (size_t i = 0; i < length; i++) {
        // Serial.print("0x");
        // Serial.print(packet[i], HEX);
        // Serial.print(" ");
    }
    // Serial.println();
    delay(10);  // Wait for transmission to complete
}

float RS485ModbuspH::receivePhPacket() {
    byte recvBuffer[20];
    int index = 0;

    if (Serial2.available() > 0) {
        while (Serial2.available()) {
            recvBuffer[index++] = Serial2.read();
            if (index >= 20) break;  // Exit if we've read enough bytes
        }
        if (index == 10){
          for (int i = 0; i < index; i++ ) {
            recvBuffer[i] = recvBuffer[i+1];
          }
          index = 9;
        }

        return printReceivedData(recvBuffer, index);
    } else {
        // Serial.println("No data received.");
        return -1;  // Return -1 to indicate no data
    }
}

float RS485ModbuspH::printReceivedData(byte* buffer, int length) {
 
    Serial.print("Received: ");
    for (int i = 0; i < length; i++) {
        // Serial.print("0x");
        // Serial.print(buffer[i], HEX);
        // Serial.print(" ");
    }
    Serial.println();
    delay(100);
    
    if (length == 9) {

       uint16_t pHvalue = (buffer[3] << 8) | buffer[4];  // Combine two bytes
       // Convert register value to ph (divide by 100 )
       float pH = pHvalue / 100.0;
       // Serial.print("pH: ");
       // Serial.print(pH);
       // Serial.println(" ");
      int temp_value = (buffer[5] << 8) | buffer[6];  // Combine high and low byte
      float temperature = temp_value / 10.0;  // Temperature is in 10x scale
      // Serial.print("Temperature: ");
      // Serial.println(temperature);
      // Serial.println("\n");
        
      return pH;  // Return oxygen concentration
    } 
     else {
        // Serial.println("Incomplete or invalid data received.");
        return -1;  // Return -1 for invalid data
    }
}

float RS485ModbuspH::parseFloat(byte* buffer) {
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

void RS485ModbuspH::receivePhdeviation(){
  byte recBuffer[20];
    int index_1 = 0;

    if (Serial2.available() > 0) {
        while (Serial2.available()) {
            recBuffer[index_1++] = Serial2.read();
            if (index_1 >= 20) break;  // Exit if we've read enough bytes
        }
        // if (index_1 == 10){
        //   for (int i = 0; i < index_1; i++ ) {
        //     recBuffer[i] = recBuffer[i+1];
        //   }
        //   index_1 = 9;
        // }

        // return printReceivedData(recvBuffer, index);
    }

}

