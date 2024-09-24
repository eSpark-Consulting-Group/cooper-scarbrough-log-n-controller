#ifndef IO_GPIO_H
#define IO_GPIO_H

#include <DFRobot_MCP23017.h>

extern DFRobot_MCP23017 mcp;  // Declaration

class GPIOManager {
public:
  void setup();
  bool getHomeGPIO();
  bool getUpGPIO();
  bool getDownGPIO();
  bool getSelectGPIO();
  bool getStartGPIO();
  bool getStopGPIO();

  void setFoodSourcePump_1();
  void setBasePump_2();
  void setAcidPump_3();
  void setAirPump_4();
  void setACmotor_5();
  void setUVlight_6();
  void stopAcidPump_3();
  void stopBasePump_2();
  void resetPump();

private:
  bool readPin(uint8_t pin);
};

#endif  // IO_GPIO_H