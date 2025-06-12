#include <DFRobot_MCP23017.h>
#include <Wire.h>
#include <SPI.h>
#include "gfx_conf.h"  
#include "IO_GPIO.h"
#include "FirstScript.h"

#define ARDUINO_CLOUD
#if defined (ARDUINO_CLOUD)
#include "thingProperties.h"
#endif

/* Script running parameters*/
#define INTERVAL         0.5
#define OFF_DELAY   (5*1000)
#define MAX_SCRIPTS     3
#define DISPLAY_SCRIPTS 2

//Script 1
#define PH_PPM1          7
#define OXYGEN_PPM1      5
#define TOTAL_RUN_TIME1  120

//Script 2
#define PH_PPM2         7.4
#define OXYGEN_PPM2      9
#define TOTAL_RUN_TIME2 180

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

int scriptIndex = 0;
uint8_t countScript = 1;
int selectedScript = 1;  // Variable to track selected script (1 or 2)
bool isRunning = false;  // Flag to track if the script is running

// For debouncing buttons
bool downButtonPressed = false;
bool upButtonPressed = false;
bool selectButtonPressed = false;
bool startButtonPressed = false;
bool stopButtonPressed = false;

GPIOManager gpio;
getScripts script;

const char* scriptNames[MAX_SCRIPTS] = {
  "1.Microorganism",
  "2.Bacillus thuringiensis kurstaki",
  "3.Sterilization Script"
};

// Function prototypes for your scripts
void script1();
void script2();
void script3();
void script4();
void script5();

// // Array of function pointers
void (*scripts[MAX_SCRIPTS])() = { script1, script2, script3};//For Three Scripts
// void (*scripts[MAX_SCRIPTS])() = { script1, script2, script3, script4, script5 }; //For Five Scripts

// void doThisOnDisconnect(){
//   /* add your custom code here */
//   WiFi.begin(SECRET_SSID,SECRET_OPTIONAL_PASS);
// }

// static unsigned long lastCloudCheck = 0;
// const unsigned long cloudCheckInterval = 30000;
// unsigned long lastCloudConnected = millis();

// WiFi.onEvent([](WiFiEvent_t event) {
//   if (event == SYSTEM_EVENT_STA_DISCONNECTED) {
//     Serial.println("WiFi Disconnected! Attempting to reconnect...");
//     WiFi.begin(SECRET_SSID, SECRET_OPTIONAL_PASS);
//   }
// });

void checkCloudConnection() {
  static unsigned long lastAttemptTime = 0;
  const unsigned long retryDelay = 10000; // Retry every 10 seconds

  if (millis() - lastAttemptTime > retryDelay) {
    lastAttemptTime = millis();
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Trying to reconnect to Arduino Cloud...");
      ArduinoCloud.disconnect();
      initProperties();
      ArduinoCloud.begin(ArduinoIoTPreferredConnection);
      // testData = "lastTime:" + String(lastAttemptTime);
      ArduinoCloud.update();
      // ArduinoCloud.attachThing(DEVICE_LOGIN_NAME);
    } else {
      Serial.println("WiFi not connected. Waiting for reconnection...");
    }
  }
}

void checkConnection()
{
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");
    WiFi.disconnect();
    WiFi.begin(SECRET_SSID, SECRET_OPTIONAL_PASS);
    delay(1000);  // Give time to reconnect
  }

  if (!ArduinoCloud.connected()) {
    // Serial.println("Cloud disconnected. Attempting to reconnect...");
    checkCloudConnection();  // You already defined this function
  }
}

void setup() {
  Serial.begin(115200);
  delay(1500);

#if defined (ARDUINO_CLOUD)
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  // ArduinoCloud.addCallback(ArduinoIoTCloudEvent::DISCONNECT, doThisOnDisconnect);
  ArduinoCloud.printDebugInfo();
  logEvents = "Time , pH(actual), pH(Traget) ,Oxygen(actual), Oxygen(target),P1(Stimulant),P2(Acid),P3(Base)";
#endif

  tft.begin();
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1.5);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  
  drawUI();
   
  gpio.setup();
  script.setup();
  delay(500);
}

void loop() {
#if defined(ARDUINO_CLOUD)
  ArduinoCloud.update();
  checkConnection();
#endif
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
  tft.drawRect(20, 63, 440, 60, TFT_RED);
  for (int i = 0; i < DISPLAY_SCRIPTS; i++)
  {
    int scriptNum = scriptIndex + i;
    if (scriptNum >= MAX_SCRIPTS)
    {
      tft.fillRect(30, 77 + (25 * i), 300, 20, TFT_BLACK);
      continue;
    }
    // Set text color based on selection
    tft.setTextColor(countScript == scriptNum + 1 ? TFT_GREEN : TFT_RED, TFT_BLACK);
    tft.fillRect(30, 77 + (25 * i), 300, 20, TFT_BLACK);
    tft.setCursor(30, 77 + (25 * i));
    tft.println(scriptNames[scriptNum]);
  }

  // Draw Selected section
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.drawRect(20, 132, 200, 40, TFT_RED);
  tft.setCursor(30, 140);
  tft.println("Selected:");
  tft.fillRect(30, 155, 180, 16, TFT_BLACK);
  tft.setCursor(60, 155);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println(selectedScript);

  // Draw Status section
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.drawRect(260, 132, 200, 40, TFT_RED);
  tft.setCursor(270, 140);
  tft.println("Status:");
  tft.setCursor(270, 155);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println(isRunning ? "Running" : "Idle   ");

  uint8_t time = 0;

  if(selectedScript == 1)
  {
    drawInfoBox(20, 179, "Time", time, "/ ", TOTAL_RUN_TIME1);
    drawInfoBox(180, 179, "pH", pHvalues, "/ ",PH_PPM1);
    drawInfoBox(340, 179, "Oxygen", oxygenSensor, "/ ", OXYGEN_PPM1);
  }
  else if(selectedScript == 2)
  {
    drawInfoBox(20, 179, "Time", time, "/ ", TOTAL_RUN_TIME2);
    drawInfoBox(180, 179, "pH", pHvalues, "/ ",PH_PPM2);
    drawInfoBox(340, 179, "Oxygen", oxygenSensor, "/ ", OXYGEN_PPM2);
  }
  else if(selectedScript == 3)
  {
    drawInfoBox(20, 179, "Time", time, "/ ", 0);
    drawInfoBox(180, 179, "pH", pHvalues, "/ ",0.00);
    drawInfoBox(340, 179, "Oxygen", oxygenSensor, "/ ", 0.00);
  }

  drawCircleRunButton(340, 250, "RUN", startColor);
  drawCircleStopButton(120, 250, "STOP", stopColor);

  // Draw Signal bar
  tft.drawRect(400, 300, 50, 120, TFT_RED);
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

void drawInfoBox(int x, int y, const char *label, uint8_t data, const char *value, float values) {
  tft.drawRect(x, y, 120, 50, TFT_RED);
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
  tft.fillRect(x + 60, y + 30, 50, 16, TFT_BLACK);
  tft.setCursor(x + 60, y + 30);
  tft.setTextColor(TFT_WHITE);
  tft.println(values,1);
}

void colorChangeStates() {
  // Handle button presses to switch between scripts
  if (!gpio.getUpGPIO()) {
    if (!upButtonPressed) {
      countScript--;
      if (countScript < 1) {
        countScript = MAX_SCRIPTS;
        scriptIndex = MAX_SCRIPTS - DISPLAY_SCRIPTS;
      } else if (countScript <= scriptIndex) {
        scriptIndex--;
        if (scriptIndex < 0) scriptIndex = 0;
      }
      upButtonPressed = true;
    }
  } else {
    upButtonPressed = false;
  }

  if (!gpio.getDownGPIO())
  {
    if (!downButtonPressed)
    {
      countScript++;
      if (countScript > MAX_SCRIPTS)
      {
        countScript = 1;
        scriptIndex = 0;
      }
      else if (countScript > scriptIndex + DISPLAY_SCRIPTS-1) 
      {
        scriptIndex++;
        if (scriptIndex > MAX_SCRIPTS - DISPLAY_SCRIPTS+1)
          scriptIndex = MAX_SCRIPTS - DISPLAY_SCRIPTS;
      }
      downButtonPressed = true;
    }
  }
  else
  {
    downButtonPressed = false;
  }

  if (!gpio.getSelectGPIO()) {
    if (!selectButtonPressed) {
      // selectedScript = constrain(countScript, 1, MAX_SCRIPTS);
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
#if defined(ARDUINO_CLOUD)
  ArduinoCloud.update();
#endif
}

void displayValue() {
  tft.fillRect(30, 179 + 30, 40, 10, TFT_BLACK);
  tft.setCursor(20 + 10, 179 + 30);
  tft.setTextColor(TFT_WHITE);
  tft.println(elapsedTime);

  tft.fillRect(180 + 10, 179 + 30, 40, 10, TFT_BLACK);
  tft.setCursor(180 + 10, 179 + 30);
  tft.setTextColor(TFT_WHITE);
  tft.println(pHvalues, 1);

  tft.fillRect(350, 179 + 30, 40, 10, TFT_BLACK);
  tft.setCursor(340 + 10, 179 + 30);
  tft.setTextColor(TFT_WHITE);
  tft.println(oxygenSensor, 1);
#if defined (ARDUINO_CLOUD)
  logEvents = String(elapsedTime) + ", " + String(pHvalues, 1) + "," + String(7) + "," + String(oxygenSensor, 1)+ "," + String(5) + ","+String(stimulant) +"," + String(base) + ","+ String(acid);
  Serial.println(String(elapsedTime) + ", " + String(pHvalues, 1) + "," + String(7) + "," + String(oxygenSensor, 1) + "," + String(5) + "," +String(stimulant) + "," + String(base) + ","+ String(acid));
  ArduinoCloud.update();
#endif
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
      checkConnection();
      
      unsigned long currentTime = millis() ;
      // Check if 10 minutes have passed since the last reading
      if (currentTime - lastIntervalTime >= INTERVAL * 60000) {
        elapsedTime = ((currentTime - lastIntervalTime) * elsTime) / 60000 ;
        elsTime ++;
        lastIntervalTime = currentTime;
        script1Running();
      }

      if (currentTime - startTime >= TOTAL_RUN_TIME1 * 60000) {
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

// SCRIPTS#2 IMPLEMENTATIONS
void script2()
{
  if (isRunning) {
    gpio.setAirPump_4();
    startTime = millis() ;
    lastIntervalTime = startTime;
    script1Running();
    while (isRunning)
    {  
      checkConnection();
      unsigned long currentTime = millis() ;
      // Check if 10 minutes have passed since the last reading
      if (currentTime - lastIntervalTime >= INTERVAL * 60000) {
        elapsedTime = ((currentTime - lastIntervalTime) * elsTime) / 60000 ;
        elsTime ++;
        lastIntervalTime = currentTime;
        script1Running();
      }

      if (currentTime - startTime >= TOTAL_RUN_TIME2 * 60000) {
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

// SCRIPTS#3 IMPLEMENTATIONS
void script3()
{
  while (isRunning)
  {
    checkConnection();
    gpio.setACmotor_5();
    gpio.setUVlight_6();

    if (!gpio.getStopGPIO()) {
      drawCircleRunButton(340, 250, "RUN", TFT_RED);
      drawCircleStopButton(120, 250, "STOP", TFT_GREEN);
      resetVariable();
      displayValue();
      gpio.resetPump();
    }
  }
}
void script4() {}
void script5() {}

// INNER IMPLEMETATION OF SCRIPT
void script1Running() {
  oxygenSensor = script.getOxygenSensorValue();
  pHvalues = script.getpHSensorValue();

  if (oxygenSensor > OXYGEN_PPM1) {
    gpio.setFoodSourcePump_1();
    stimulant = 1;
  }
  else{
    stimulant = 0;
    gpio.stopFoodSourcePump_1();
  }
  
  if (pHvalues < PH_PPM1) {
    gpio.setBasePump_2();
    gpio.stopAcidPump_3();
    acid = 0;
    base = 1;
  }
  else if (pHvalues > PH_PPM1) {
    gpio.setAcidPump_3();
    gpio.stopBasePump_2();
    acid = 1;
    base = 0;
  }
  else if(pHvalues == PH_PPM1){
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