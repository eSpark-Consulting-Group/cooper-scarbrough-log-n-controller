#include "OxygenSensor.h"
#include "pHSensor.h"

#define RXD2 4
#define TXD2 5

#define RX_PH 6
#define TX_PH 7

class getScripts {
public:
  void setup();
  float getOxygenSensorValue();
  float getpHSensorValue();

private:
  void deviationpHValue();
  void zeroPointCalOxygenSens();
  void atmosphPressure();
};
