#define TINY_GSM_MODEM_SIM7600  // LUÔN ĐẶT Ở ĐÂY
#define DEBUG 1 

#include <Arduino.h>
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <TinyGPSPlus.h> 
#include <TimeLib.h>
#include <TimeAlarms.h>
#include <ArduinoJson.h>

// =========================================================
// === CẤU HÌNH SERIAL VÀ DEBUG ===
// =========================================================
#define SerialComm  Serial;

#ifdef DEBUG
  #define DEBUG_PRINT(...) SerialComm.print(__VA_ARGS__)
  #define DEBUG_PRINTLN(...) SerialComm.println(__VA_ARGS__)
#else
  #define DEBUG_PRINT(...)
  #define DEBUG_PRINTLN(...)
#endif

//----------------------khai bao GSM--------------------------
#define Serial_GMS Serial2 
const int SIM_BAUD = 115200;
const int SIM_RX_PIN = 16; // Nối với A7680C TXD
const int SIM_TX_PIN = 17; // Nối với A7680C RXD

const char apn[]       = "v-internet"; // THAY TÊN APN CỦA BẠN
const char gprsUser[]  = "";
const char gprsPass[]  = "";
const char* broker = "mqtt.thingsboard.cloud"; 
const int TB_PORT = 1883;
const char* tbToken = "mdP4E3CsmWON6zlh2MYJ"; // <<< THAY TOKEN CỦA BẠN!
const char* topicTelemetry = "v1/devices/me/telemetry";

TinyGsm modem(Serial_GMS); 
TinyGsmClient client(modem);
PubSubClient mqtt(client);

const long INTERVAL_MS = 10000; 
uint32_t lastRead = 0; 
long counter = 0; 

uint32_t lastReconnectAttempt = 0;
const long MQTT_RECONNECT_INTERVAL = 10000; 

const size_t JSON_DOC_SIZE = 300; 


//---------------------end khai bao GSN-----------------------

//--------------------khai bao AI esp32 cam------------------
// Serial 0: Giao tiếp với ESP32-CAM và Debug
HardwareSerial Serial_CAM(0); // Sử dụng UART 0, nhưng re-map pin để giải phóng pin 1/3 cho Debug.
const int COMM_RX_PIN = 5;
const int COMM_TX_PIN = 18; // Remap TX0 sang GPIO 21
const int COMM_BAUD = 115200; 
//------------------end khai bao AI esp32 cam----------------

// 
//--------------------khai bao HC_SR40-----------------------
const int TRIG_PIN = 14;
const int ECHO_PIN = 13; 
const float BIN_HEIGHT_CM = 50.0;
const float SENSOR_OFFSET_CM = 5.0;
float distance;
float fill_percent;
long measureDistanceCM();
float calculateFillPercent(long distance_cm);

//--------------------end khai bao HC_SR40------------------

//--------------------khai bao GPS--------------------------
#define GPS_BAUD 9600 // Tốc độ Baud mặc định của NEO-6M
#define Serial_GPS Serial1 
const int GPS_RX_PIN = 2; // NEO-6M TXD
const int GPS_TX_PIN = 4; // NEO-6M RXD 
TinyGPSPlus gps;

//--------------------end khai bao GPS----------------------
//--------------------khai bao timer-----------------------
#define step_50ms             100
#define step_250ms            250
#define step_1s               1
#define step_2s               2
#define step_5s               5
#define step_10s              10
#define step_30s              30
#define step_15s              15
#define step_1m               60
#define step_90s              90
#define step_5m               60*5
#define step_60m              60*60

unsigned long time_50msecond = 0;
unsigned long time_250msecond = 0;
unsigned long time_1second = 0;
unsigned long time_2second = 0;
unsigned long time_5second = 0;
unsigned long time_10second = 0;
unsigned long time_30second = 0;
unsigned long time_1minute = 0;
unsigned long time_90second = 0;
unsigned long time_5minute = 0;
unsigned long time_60minute = 0;
//--------------------end khai bao timer-------------------

// -----------------------------------------------------------------
String trashType;
float trash_height = 0;
String  time_GPS;
long location_long, location_lat;

// -----------------------------------------------------------------

void setup() {
  // 1. Khởi tạo Serial 0 (SerialComm) và Remap
  SerialComm.begin(COMM_BAUD); 
  delay(100);
  setupGPRS_MQTT();
  setup_GPS();
  setup_HC();
  setup_espcam();
}


// -----------------------------------------------------------------
// === LOOP (Hàm lặp lại) ===
// -----------------------------------------------------------------

void loop() {
  if (TimeReached(time_250msecond)) {
    SetNextTimeInterval(time_250msecond, step_250ms);
    HC_loop();
    mqtt.loop();
  }

  if (TimeReached(time_10second)) {
    SetNextTimeInterval(time_10second, step_10s*1000);
    serialEvent_cam();
    if (!mqtt.connected()) {
      DEBUG_PRINTLN("=== MQTT NOT CONNECTED ===");
      uint32_t t = millis();
      if (t - lastReconnectAttempt > MQTT_RECONNECT_INTERVAL) {
        lastReconnectAttempt = t;
        if (mqttConnect()) { lastReconnectAttempt = 0; }
      }
    }
  }
  if (TimeReached(time_30second)) {
    SetNextTimeInterval(time_30second, step_30akas*1000);
    smartDelay();
  }
  
  if (TimeReached(time_60second)) {
    SetNextTimeInterval(time_60second, step_60s*1000);
    sendTelemetryData();
  }
}
