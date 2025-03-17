#include <DFRobot_MCP23017.h>
#include "thingProperties.h"
#include <Wire.h>
#include <SPI.h>
#include "gfx_conf.h"  
#include "IO_GPIO.hpp"
#include "FirstScript.hpp"

/* Script running parameters*/
#define PH_PPM      7
#define OXYGEN_PPM  5
#define MAX_SCRIPTS 2
#define OFF_DELAY   (5*1000)

const unsigned long INTERVAL = 10;  // 10 minutes in milliseconds
const unsigned long TOTAL_RUN_TIME = 120;  // 90 minutes in milliseconds

/* variables*/
unsigned long startTime = 0;
unsigned long lastIntervalTime = 0;
unsigned long elsTime = 1 ;
unsigned long elapsedTime = 0;  // Store the elapsed time

uint16_t homeColor = TFT_RED;
uint16_t upColor = TFT_RED;
uint16_t downColor = TFT_RED;
uint16_t selectColor = TFT_RED;
uint16_t stopColor = TFT_RED;
uint16_t startColor = TFT_RED;

float pHvalues = 0;
float oxygenSensor = 0;

uint8_t acid;
uint8_t base;
uint8_t stimulant;

GPIOManager gpio;
getScripts script;

uint8_t countScript = 1;
int selectedScript = 1;  // Variable to track selected script (1 or 2)
bool isRunning = false;  // Flag to track if the script is running

// For debouncing buttons
bool downButtonPressed = false;
bool upButtonPressed = false;
bool selectButtonPressed = false;
bool startButtonPressed = false;
bool stopButtonPressed = false;

// Function prototypes for your scripts
void script1();
void script2();
void script3();
void script4();
void script5();

// // Array of function pointers
void (*scripts[MAX_SCRIPTS])() = { script1, script2};//For Two Scripts
// void (*scripts[MAX_SCRIPTS])() = { script1, script2, script3, script4, script5 }; //For Five Scripts

void setup() {
  Serial.begin(115200);
  delay(1500);
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
 
  tft.begin();
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1.5);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  
  drawUI();
   
  gpio.setup();
  
  script.setup();
  delay(500);
  // logEvents = "Time , pH(actual), pH(Traget) ,Oxygen(actual), Oxygen(target),P1(Stimulant),P2(Acid),P3(Base)";
}

void loop() {
   ArduinoCloud.update();
  // Your code here 
  colorChangeStates(); 

  delay(100);

  drawUI();

  delay(100);

  scripts[selectedScript - 1]();

  delay(10);  // Small delay for debounce purposes
}

void drawUI() {
  // Draw buttons and UI elements
  drawButton(30, 15, "Home", homeColor);
  drawButton(145, 15, "Up", upColor);
  drawButton(255, 15, "Down", downColor);
  drawButton(370, 15, "Select", selectColor);

  // Draw the Registry box
  tft.drawRect(20, 63, 440, 60, TFT_RED);  // Highlight selected
  tft.setTextColor(countScript == 1 ? TFT_GREEN : TFT_RED, TFT_BLACK);
  tft.setCursor(30, 77);
  tft.println("Microorganism #1 (script 1)");
  tft.setTextColor(countScript == 2 ? TFT_GREEN : TFT_RED, TFT_BLACK);
  tft.setCursor(30, 102);
  tft.println("Microorganism #2 (script 2)");

  // Draw Selected section
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.drawRect(20, 132, 200, 40, TFT_RED);
  tft.setCursor(30, 140);
  tft.println("Selected:");
  tft.setCursor(30, 155);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println(selectedScript == 1 ? "Microorganism #1" : "Microorganism #2");

  // Draw Status section
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.drawRect(260, 132, 200, 40, TFT_RED);
  tft.setCursor(270, 140);
  tft.println("Status:");
  tft.setCursor(270, 155);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println(isRunning ? "Running" : "Idle   ");

  uint8_t time = 0;
  drawInfoBox(20, 179, "Time", time, "/ ", TOTAL_RUN_TIME);
  drawInfoBox(180, 179, "pH", pHvalues, "/ ",PH_PPM);
  drawInfoBox(340, 179, "Oxygen", oxygenSensor, "/ ", OXYGEN_PPM);
  // displayValue();
  // Draw Run and Stop buttons
  // drawCircleRunButton(120, 250, "RUN", startColor);
  // drawCircleStopButton(340, 250, "STOP", stopColor);
   drawCircleRunButton(340, 250, "RUN", startColor);
   drawCircleStopButton(120, 250, "STOP", stopColor);
  // Draw Signal bar
  tft.drawRect(400, 300, 50, 120, TFT_RED);  // Signal box
}

void drawButton(int x, int y, const char *label, uint16_t color) {
  tft.drawRect(x, y, 80, 40, color);
  tft.setCursor(x + 15, y + 15);
  tft.setTextColor(color);
  tft.print(label);
}

void drawCircleRunButton(int x, int y, const char *label, uint16_t color) {
  tft.fillCircle(x, y, 15, color);
  tft.setCursor(x - 50, y - 2);
  tft.setTextColor(TFT_WHITE);
  tft.print(label);
}

void drawCircleStopButton(int x, int y, const char *label, uint16_t color) {
  tft.fillCircle(x, y, 15, color);
  tft.setCursor(x + 25, y - 2);
  tft.setTextColor(TFT_WHITE);
  tft.print(label);
}

void drawInfoBox(int x, int y, const char *label, uint8_t data, const char *value, int values) {
  tft.drawRect(x, y, 140, 50, TFT_RED);
  tft.setCursor(x + 10, y + 10);
  tft.setTextColor(TFT_RED);
  tft.println(label);
  tft.setCursor(x + 10, y + 30);
  tft.setTextColor(TFT_WHITE);
  if(!data)
  {
    tft.println(0.00);
  }
  else
  {
    tft.println(data);
  }
  tft.setCursor(x + 50, y + 30);
  tft.setTextColor(TFT_WHITE);
  tft.println(value);
  tft.setCursor(x + 60, y + 30);
  tft.setTextColor(TFT_WHITE);
  tft.println(values);
}


void colorChangeStates() {
  // Handle button presses to switch between scripts
  if (!gpio.getUpGPIO()) {
    if (!upButtonPressed) {
      countScript--;
      if (countScript < 1) {
        countScript = MAX_SCRIPTS;
      }
      upButtonPressed = true;
    }
  } else {
    upButtonPressed = false;
  }

  if (!gpio.getDownGPIO()) {
    if (!downButtonPressed) {
      countScript++;
      if (countScript > MAX_SCRIPTS) {
        countScript = 1;
      }
      downButtonPressed = true;
    }
  } else {
    downButtonPressed = false;
  }
  
  if (!gpio.getSelectGPIO()) {
    if (!selectButtonPressed) {
      selectedScript = countScript;
      selectButtonPressed = true;
    }
  } else {
    selectButtonPressed = false;
  }

  // Handle Start button press to start the selected script
  if (!gpio.getStartGPIO()) {
    if (!startButtonPressed) {
      isRunning = true;
      startButtonPressed = true;
      // startTime = millis();

      // Call the selected script function
    }
  } else {
    startButtonPressed = false;
  }

  // Handle Stop button press to stop the script
  if (!gpio.getStopGPIO()) {
    if (!stopButtonPressed) {
      isRunning = false;
      stopButtonPressed = true;
    }
  } else {
    stopButtonPressed = false;
  }

  // Maintain button color change logic for display
  homeColor   = !gpio.getHomeGPIO() ? TFT_GREEN : TFT_RED;
  upColor     = upButtonPressed ? TFT_GREEN : TFT_RED;
  downColor   = downButtonPressed ? TFT_GREEN : TFT_RED;
  selectColor = selectButtonPressed ? TFT_GREEN : TFT_RED;
  startColor  = startButtonPressed ? TFT_GREEN : TFT_RED;
  stopColor   = stopButtonPressed ? TFT_GREEN : TFT_RED;
  ArduinoCloud.update();
}

void displayValue() {
  
  // elapsedTime = (millis() - startTime) / 60000;
  tft.fillRect(30, 179 + 30, 40, 10, TFT_BLACK);
  tft.setCursor(20 + 10, 179 + 30);
  tft.setTextColor(TFT_WHITE);
  tft.println(elapsedTime);

  // delay(100);

  tft.fillRect(180 + 10, 179 + 30, 40, 10, TFT_BLACK);
  tft.setCursor(180 + 10, 179 + 30);
  tft.setTextColor(TFT_WHITE);
  tft.println(pHvalues, 1);

  // delay(100);

  tft.fillRect(350, 179 + 30, 40, 10, TFT_BLACK);
  tft.setCursor(340 + 10, 179 + 30);
  tft.setTextColor(TFT_WHITE);
  tft.println(oxygenSensor, 1);
 
  // delay(100);

  // logEvents = String(elapsedTime) + ", " + String(pHvalues, 1) + "," + String(7) + "," + String(oxygenSensor, 1)+ "," + String(5) + ","+String(stimulant) +"," + String(base) + ","+ String(acid);
  // Serial.println(String(elapsedTime) + ", " + String(pHvalues, 1) + "," + String(7) + "," + String(oxygenSensor, 1) + "," + String(5) + "," +String(stimulant) + "," + String(base) + ","+ String(acid));
  
  ArduinoCloud.update();
}

// SCRIPTS#1 IMPLEMENTATIONS
void script1() {
  if (isRunning) {
    gpio.setAirPump_4();
    gpio.setACmotor_5();
    gpio.stopUVlight_6();
    startTime = millis() ;
    lastIntervalTime = startTime;
    script1Running();
    while (isRunning) {
      
      unsigned long currentTime = millis() ;
      // Check if 10 minutes have passed since the last reading
      if (currentTime - lastIntervalTime >= INTERVAL * 60000) {
        elapsedTime = ((currentTime - lastIntervalTime) * elsTime) / 60000 ;
        elsTime ++;
        lastIntervalTime = currentTime;
        script1Running();
      }

      if (currentTime - startTime >= TOTAL_RUN_TIME * 60000) {
        Serial.println("Total time of 90 minutes has passed. Stopping the script.");
        resetVariable();
        displayValue();
        gpio.resetPump();
      }
      
      if((millis() - gpio.foodSourceTime) >= OFF_DELAY)
      {
        // Serial.println("FOOD STOPPEDDDDDDDDDDDDDDD");
        gpio.stopFoodSourcePump_1();
      }
      if((millis() - gpio.basePumpTime) >= OFF_DELAY)
      {
        // Serial.println("BASE STOPPEDDDDDDDDDDDDDDD");
        gpio.stopBasePump_2();
      }
      if((millis() - gpio.acidPumpTime) >= OFF_DELAY)
      {
        // Serial.println("ACID STOPPEDDDDDDDDDDDDDDD");
        gpio.stopAcidPump_3();
      }

      if (!gpio.getStopGPIO()) {
        drawCircleRunButton(340, 250, "RUN", TFT_RED);
        drawCircleStopButton(120, 250, "STOP", TFT_GREEN);
        resetVariable();
        displayValue();
        gpio.resetPump();
      }
    }
  }
}

// SCRIPTS#1 IMPLEMENTATIONS
void script2() {}
void script3() {}
void script4() {}
void script5() {}

// INNER IMPLEMETATION OF SCRIPT
void script1Running() {
  oxygenSensor = script.getOxygenSensorValue();
  pHvalues = script.getpHSensorValue();

  if (oxygenSensor > OXYGEN_PPM) {
    gpio.setFoodSourcePump_1();
    // displayValue();
    stimulant = 1;
  }else{
    stimulant = 0;
    gpio.stopFoodSourcePump_1();
    }
  
  if (pHvalues < PH_PPM) {
    gpio.setBasePump_2();
    gpio.stopAcidPump_3();
    // displayValue();
    base = 1; 
    
  }else{base = 0;}
  if (pHvalues > PH_PPM) {
    gpio.setAcidPump_3();
    gpio.stopBasePump_2();
    // displayValue();
    acid = 1;
  }else{acid = 0;}

  if(pHvalues == PH_PPM){
     gpio.stopAcidPump_3();
     gpio.stopBasePump_2();
     acid = 0;
     base = 0;
  }
  
  displayValue();
  delay(100);
   
}

void resetVariable() {
  isRunning = false;
  oxygenSensor = 0;
  startTime = 0;
  elapsedTime = 0;
  pHvalues = 0;
  elsTime = 0;
}

// void onLogEventsChange()  {
//   // Add your code here to act upon LogEvents change
// }