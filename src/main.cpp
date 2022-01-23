
#include "main.h"

String clientId = "BlenderRodriguez";

#include <Wire.h>


#define SDA 2
#define SCL 0

#define mcp1Address 0x20
//define log thread period
// #define LOG_PERIOD 10



Adafruit_MCP23X17 mcp1;



//mqtt vars
float waterCurrentPH;

bool Ch1;
bool Ch2;
bool Ch3;
bool Ch4;

int relativeDose1;// in milliliters per milliliters
int relativeDose2;// in milliliters per milliliters
int relativeDose3; // in milliliters per milliliters
int relativeDose4; // in milliliters per milliliters
int absoluteDose; // how much water we dose each time we blend the nutrients.

String whenDose; //dateTime (when to dose?)
String lastDose; //dateTime
String currentTime;//dateTime

// String payload;
extDcMotor motor1(0, 1);
extDcMotor motor2(2, 3);
extDcMotor motor3(4, 5);
extDcMotor motor4(6, 7);

void callback(char* topic, byte* payload, unsigned int length) {
  bool _Ch1 = Ch1;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    bool _Ch1 = 1;
    digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
    motor1.forward();
    Serial.print(F("\tCh1: ") + String(_Ch1));
    MQTTclient.publish(String(F("actuators/") + clientId + F("/Ch1/state")).c_str(), String(_Ch1).c_str());
  } else {
    bool _Ch1 = 0;
    digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
    motor1.stop();
    Serial.print(F("\tCh1: ") + String(_Ch1));
    MQTTclient.publish(String(F("actuators/") + clientId + F("/Ch1/state")).c_str(), String(_Ch1).c_str());
    Ch1 = _Ch1;
  }
  if ((char)payload[0] == 'ON') {
    // digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // // but actually the LED is on; this is because
    // // it is active low on the ESP-01)
    motor1.forward();
    Serial.print(F("\tCh1: ") + String(_Ch1));
    MQTTclient.publish(String(F("actuators/") + clientId + F("/Ch1/state")).c_str(), String(_Ch1).c_str());
  } else {
    bool _Ch1 = 0;
    digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
    motor1.stop();
    Serial.print(F("\tCh1: ") + String(_Ch1));
    MQTTclient.publish(String(F("actuators/") + clientId + F("/Ch1/state")).c_str(), String(_Ch1).c_str());
  }
}


void threadPublishCallback() {
  float _CurrentPh;
  bool _Ch1;
  bool _Ch2;
  bool _Ch3;
  bool _Ch4;

  Serial.print(F("threadPublishCallback: ") + String(millis() / 1000));

  Serial.print(F("\tCurrentPh: ") + String(_CurrentPh));
  MQTTclient.publish(String(F("sensors/") + clientId + F("/CurrentPh")).c_str(), String(_CurrentPh).c_str());

  // Serial.print(F("\tCh1: ") + String(_Ch1));
  // MQTTclient.publish(String(F("actuators/") + clientId + F("/Ch1/state")).c_str(), String(_Ch1).c_str());
  // MQTTclient.subscribe(String(F("actuators/") + clientId + F("/Ch1/command")).c_str());
  //
  // // MQTTclient.publish(String(F("actuators/") + clientId + F("/Ch2/state")).c_str(), String(_Ch2).c_str());
  // Serial.print(F("\tCh2: ") + String(_Ch3));
  // MQTTclient.subscribe(String(F("actuators/") + clientId + F("/Ch2/command")).c_str());
  // MQTTclient.publish(String(F("actuators/") + clientId + F("/Ch2/state")).c_str(), String(_Ch3).c_str());
  //
  // Serial.print(F("\tCh3: ") + String(_Ch3));
  // MQTTclient.subscribe(String(F("actuators/") + clientId + F("/Ch3/command")).c_str());
  // MQTTclient.publish(String(F("actuators/") + clientId + F("/Ch/state")).c_str(), String(_Ch3).c_str());
  //
  // Serial.print(F("\tCh4: ") + String(_Ch4));
  // MQTTclient.subscribe(String(F("actuators/") + clientId + F("/Ch4/command")).c_str());
  // MQTTclient.publish(String(F("actuators/") + clientId + F("/Ch4/state")).c_str(), String(_Ch4).c_str());
  //mqtt vars
  String  _payload = "{\"clientId\":";
          _payload += (String)clientId;

          _payload += ",\"CurrentPh\":";
          _payload += (float)_CurrentPh;

          _payload += ",\"Ch1\":";
          _payload += (bool)_Ch1;

          _payload += ",\"Ch2\":";
          _payload += (bool)_Ch2;

          _payload += ",\"Ch3\":";
          _payload += (bool)_Ch3;

          _payload += ",\"Ch4\":";
          _payload += (bool)_Ch4;
          // float currentDose;
          // float targetDose;
          //
          // String whenDose; //dateTime (when to dose?)
          // String lastDose; //dateTime
          // String currentTime;//dateTime
          // _payload += ",\"currentDose\":";
          // _payload += (float)currentDose;
          // _payload += ",\"targetDose\":";
          // _payload += (float)targetDose;
          //
          // _payload += ",\"whenDose\":";
          // _payload += (String)whenDose;
          // _payload += ",\"lastDose\":";
          // _payload += (String)lastDose;

          Serial.println(F("\ttime: ") + String(timeClient.getEpochTime()));
          MQTTclient.publish(String(F("sensors/") + clientId + F("/debug/connected")).c_str(), String(timeClient.getEpochTime()).c_str());

          _payload += "}";

          MQTTclient.publish(String(F("sensors/") + clientId + F("/json")).c_str(), _payload.c_str());
}

// Thread threadCurrentLog = Thread();
// StaticThreadController<1> threadController (&threadCurrentLog);



boolean reconnect() // Called when client is disconnected from the MQTT server
{
  if (MQTTclient.connect(clientId.c_str()))
  {
    timeClient.update(); // Is this the right time to update the time, when we lost connection?

    Serial.print(F("reconnect: ") + String(millis() / 1000));
    Serial.println(F("\ttime: ") + String(timeClient.getEpochTime()));
    MQTTclient.publish(String(F("sensors/") + clientId + F("/debug/connected")).c_str(), String(timeClient.getEpochTime()).c_str());

    threadPublishCallback(); // Publish after reconnecting, probably not necesary with small LOGPERIOD
  }

  return MQTTclient.connected();
}

String generateClientIdFromMac() // Convert the WiFi MAC address to String
{
  byte _mac[6];
  String _output = "";

  WiFi.macAddress(_mac);

  for (int _i = 5; _i > 0; _i--)
  {
    _output += String(_mac[_i], HEX);
  }

  return _output;
}



String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i)
  {
    result += String(mac[i], 16);
    if (i < 5) result += ':';
  }
  return result;
}

void setup()
{
  Serial.begin(115200);
  // Serial.print("SDA: ");Serial.print(SDA);
  // Serial.print(" SCL: ");Serial.println(SCL);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  clientId += F("-") + generateClientIdFromMac();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Wire.begin(SDA, SCL);

  if (!mcp1.begin_I2C(0x20))
  {
    Serial.println(F("MCPERR"));
    while(1);
  }
  else
  {
    Serial.println("Setup channel 1");
    motor1.begin(&mcp1);
    motor1.stop();
    Serial.println("Setup channel 2");
    motor2.begin(&mcp1);
    motor2.stop();
    Serial.println("Setup channel 3");
    motor3.begin(&mcp1);
    motor3.stop();
    Serial.println("Setup channel 4");
    motor4.begin(&mcp1);
    motor4.stop();
  }

  MQTTclient.setServer(mqtt_server, mqtt_port);
  MQTTclient.setCallback(callback);

  threadPublish.enabled = true;
  threadPublish.setInterval(LOGPERIOD);
  threadPublish.onRun(threadPublishCallback);
  if (!MQTTclient.connected()) {
    reconnect();
  }
}

void loop()
{
  // Serial.println("arrived at loop");
  if (!MQTTclient.connected())
{
  unsigned long _now = millis();

  if (_now - lastReconnectAttempt > 5000) // try to reconnect every 5000 milliseconds
  {
    lastReconnectAttempt = _now;

    if (reconnect())
    {
      lastReconnectAttempt = 0;
    }
  }
}
else
{
  MQTTclient.loop();
}

// Run threads, this makes it all work on time!
threadController.run();
}