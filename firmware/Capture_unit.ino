#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// =====================================================
// WiFi Credentials
// =====================================================
const char* ssid = "Your_WiFi_Name_Here";
const char* password = "Your_WiFi_Password_Here";

// =====================================================
// MQTT
// =====================================================
const char* mqtt_server = "Your_Broker_Device_IP";

WiFiClient espClient;
PubSubClient client(espClient);

// =====================================================
// Topics
// =====================================================
const char* topicData   = "energy/data";
const char* topicStatus = "energy/status";

// =====================================================
// Timing
// =====================================================
unsigned long lastPublish = 0;
const unsigned long publishInterval = 5000;

// =====================================================
// Message Counter
// =====================================================
unsigned long msgId = 0;

// =====================================================
// LED
// =====================================================
const int LED_PIN = LED_BUILTIN;

// LED Modes
enum LedMode {
  WIFI_CONNECTING,
  MQTT_CONNECTING,
  SYSTEM_OK,
  SYSTEM_ERROR
};

LedMode currentLedMode = WIFI_CONNECTING;

// LED Timing
unsigned long ledPreviousMillis = 0;
bool ledState = false;
int blinkStep = 0;

// =====================================================
// LED Helper
// =====================================================
void setLED(bool on) {

  // ESP8266 onboard LED is ACTIVE LOW
  digitalWrite(LED_PIN, on ? LOW : HIGH);
}

// =====================================================
// LED Pattern Engine
// =====================================================
void updateLED() {

  unsigned long now = millis();

  switch (currentLedMode) {

    // ==========================================
    // FAST BLINK → WiFi Connecting
    // ==========================================
    case WIFI_CONNECTING:
      if (now - ledPreviousMillis >= 200) {
        ledPreviousMillis = now;
        ledState = !ledState;
        setLED(ledState);
      }
      break;

    // ==========================================
    // DOUBLE BLINK → MQTT Connecting
    // ==========================================
    case MQTT_CONNECTING:
      switch (blinkStep) {
        case 0:
          setLED(true);
          if (now - ledPreviousMillis >= 100) {
            ledPreviousMillis = now;
            blinkStep = 1;
          }
          break;
        case 1:
          setLED(false);
          if (now - ledPreviousMillis >= 100) {
            ledPreviousMillis = now;
            blinkStep = 2;
          }
          break;
        case 2:
          setLED(true);
          if (now - ledPreviousMillis >= 100) {
            ledPreviousMillis = now;
            blinkStep = 3;
          }
          break;
        case 3:
          setLED(false);
          if (now - ledPreviousMillis >= 700) {
            ledPreviousMillis = now;
            blinkStep = 0;
          }
          break;
      }
      break;

    // ==========================================
    // HEARTBEAT → System Healthy
    // ==========================================
    case SYSTEM_OK:
      if (!ledState && (now - ledPreviousMillis >= 5000)) {
        ledPreviousMillis = now;
        ledState = true;
        setLED(true);
      }
      if (ledState && (now - ledPreviousMillis >= 50)) {
        ledState = false;
        setLED(false);
      }
      break;

    // ==========================================
    // SOLID ON → Error
    // ==========================================
    case SYSTEM_ERROR:
      setLED(true);
      break;
  }
}

// =====================================================
// WiFi Setup
// =====================================================
void setup_wifi() {

  Serial.println("\n[WiFi] Connecting...");

  currentLedMode = WIFI_CONNECTING;

  WiFi.begin(ssid, password);

  unsigned long wifiStart = millis();

  while (WiFi.status() != WL_CONNECTED) {

    updateLED();

    yield();

    // 20 sec timeout
    if (millis() - wifiStart > 20000) {

      Serial.println("\n[WiFi] FAILED");

      currentLedMode = SYSTEM_ERROR;

      return;
    }
  }

  Serial.println("\n[WiFi] Connected");
  Serial.print("[WiFi] IP: ");
  Serial.println(WiFi.localIP());
}

// =====================================================
// MQTT Reconnect
// =====================================================
void reconnect() {

  currentLedMode = MQTT_CONNECTING;

  while (!client.connected()) {

    Serial.print("[MQTT] Connecting...");

    bool ok = client.connect(
                "ESP8266EnergyMonitor",
                topicStatus,
                0,
                true,
                "offline"
              );

    if (ok) {

      Serial.println("connected");

      client.publish(topicStatus, "online", true);

      currentLedMode = SYSTEM_OK;

    } else {

      Serial.print("failed, rc=");
      Serial.println(client.state());

      updateLED();

      delay(100);
    }
  }
}

// =====================================================
// Setup
// =====================================================
void setup() {

  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);

  setLED(false);

  Serial.println("\n=========================");
  Serial.println(" ENERGY MONITOR START");
  Serial.println("=========================");

  setup_wifi();

  client.setServer(mqtt_server, 1883);
}

// =====================================================
// Main Loop
// =====================================================
void loop() {

  updateLED();

  // WiFi Lost
  if (WiFi.status() != WL_CONNECTED) {

    currentLedMode = SYSTEM_ERROR;

    return;
  }

  // MQTT Lost
  if (!client.connected()) {

    reconnect();
  }

  client.loop();

  // ==========================================
  // Publish Scheduler
  // ==========================================
  if (millis() - lastPublish >= publishInterval) {

    lastPublish = millis();

    // ==========================================
    // JSON
    // ==========================================
    StaticJsonDocument<512> doc;

    // Dummy values
    float pv_voltage = random(400, 800) / 10.0;
    float pv_current = random(100, 400) / 10.0;

    float battery_voltage = random(300, 500) / 10.0;
    float battery_current = random(300, 500) / 10.0;

    float load_voltage = random(2200, 2300) / 10.0;
    float load_current = random(10, 200) / 10.0;

    float load_power = load_voltage * load_current;

    float power_factor = 0.95;

    float load_frequency = random(580, 601) / 10.0;

    // ==========================================
    // JSON Fields
    // ==========================================
    doc["msg_id"] = msgId++;

    doc["uptime_ms"] = millis();

    doc["pv_voltage"] = pv_voltage;
    doc["pv_current"] = pv_current;

    doc["battery_voltage"] = battery_voltage;
    doc["battery_current"] = battery_current;

    doc["load_voltage"] = load_voltage;
    doc["load_current"] = load_current;

    doc["load_power"] = load_power;

    doc["power_factor"] = power_factor;

    doc["load_frequency"] = load_frequency;

    // ==========================================
    // Serialize
    // ==========================================
    char buffer[512];

    serializeJson(doc, buffer);

    // ==========================================
    // Serial Debug
    // ==========================================
    Serial.println("\n[DATA]");
    Serial.println(buffer);

    // ==========================================
    // Publish
    // ==========================================
    bool ok = client.publish(topicData, buffer);

    if (ok) {

      Serial.println("[MQTT] Publish OK");

      currentLedMode = SYSTEM_OK;

    } else {

      Serial.println("[MQTT] Publish FAILED");

      currentLedMode = SYSTEM_ERROR;
    }

    Serial.println("----------------------");
  }
}