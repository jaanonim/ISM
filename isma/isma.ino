ADC_MODE(ADC_VCC);
#include <NTPClient.h>

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Servo.h>
#include <EEPROM.h>

#include "ArduinoJson.h"

#include "creds.h" //connection credentials

IPAddress staticIP(192, 168, 1, 151); // ESP static ip
IPAddress gateway(192, 168, 1, 1);    // IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);   // Subnet mask
IPAddress dns(8, 8, 8, 8);            // DNS

WiFiClient client;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "192.168.1.150", 3600);

const uint16_t port = 2693;
const IPAddress server(192, 168, 1, 150);

bool connectionAuthorized = false;

int led = 16; // D0

int p1 = 14;     // D5
int p2 = 12;     // D6
int karmPin = 5; // D1
int pinTem = 4;  // D2

bool isKarm;
float vcc;

String formattedDate;
String dayStamp;
String timeStamp;

String p1timeS = "00:00";
String p1timeE = "00:00";
String p2timeS = "00:00";
String p2timeE = "00:00";
String ktime = "00:00";

bool vP1 = false;
bool vP2 = false;

float lastTemp;

Servo myservo;

OneWire oneWire(pinTem);
DallasTemperature sensors(&oneWire);

struct SplitResult
{
  String code;
  String message;
};

//-----Logging-------------------------

String LogValues()
{
  return (String)("p1s: " + p1timeS + " p1e: " + p1timeE + " p2s: " + p2timeS + " p2e: " + p2timeE + " ktime:" + ktime);
}

void NormalLog()
{
  vcc = (float)ESP.getVcc() / 1000;
  Serial.print("# ");
  Serial.print(timeClient.getFormattedTime());
  Serial.println(" V=" + (String)vcc);
}

//-----ClientUtilitis-------------------------

String getData()
{
  String zegar = timeClient.getFormattedTime();
  sensors.requestTemperatures();
  float temperatura = (sensors.getTempCByIndex(0));

  lastTemp = temperatura;

  String data = "{ \"Time\": \"";
  data += zegar;
  data += "\", \"Voltage\": ";
  data += vcc;
  data += ", \"Temp\": ";
  data += temperatura;
  data += ", \"P1\": { \"status\": ";
  if (vP1)
  {
    data += "true";
  }
  else
  {
    data += "false";
  }
  data += ", \"on\": \"";
  data += p1timeS;
  data += "\", \"off\": \"";
  data += p1timeE;
  data += "\" }, \"P2\": { \"status\": ";
  if (vP2)
  {
    data += "true";
  }
  else
  {
    data += "false";
  }
  data += ", \"on\": \"";
  data += p2timeS;
  data += "\", \"off\": \"";
  data += p2timeE;
  data += "\" }, \"karm\": \"";
  data += ktime;
  data += "\" }";
  return data;
}

String SetTimers(String s)
{
  digitalWrite(led, 0);
  Serial.println("Getting Values: ");

  StaticJsonDocument<400> buffer;
  DeserializationError error = deserializeJson(buffer, s);

  if (error)
  {
    digitalWrite(led, 1);
    Serial.println("Deserialize json failed.");
    return "Deserialize json failed";
  }

  JsonObject doc = buffer.as<JsonObject>();

  bool valid = false;
  bool save = false;

  // -------- TIMERS --------

  if (doc.containsKey("ktime"))
  {
    ktime = (String)doc["ktime"];
    valid = true;
    save = true;
  }
  if (doc.containsKey("p1timeS"))
  {
    p1timeS = (String)doc["p1timeS"];
    valid = true;
    save = true;
  }
  if (doc.containsKey("p1timeE"))
  {
    p1timeE = (String)doc["p1timeE"];
    valid = true;
    save = true;
  }
  if (doc.containsKey("p2timeS"))
  {
    p2timeS = (String)doc["p2timeS"];
    valid = true;
    save = true;
  }
  if (doc.containsKey("p2timeE"))
  {
    p2timeE = (String)doc["p2timeE"];
    valid = true;
    save = true;
  }

  // -------- ACTIONS --------

  if (doc.containsKey("karm"))
  {
    valid = true;
    if (doc["karm"])
    {
      karmie();
    }
  }
  if (doc.containsKey("p1"))
  {
    if (vP1 != doc["p1"])
    {
      vP1 = !vP1;
      if (vP1)
      {
        digitalWrite(p1, 0);
        Serial.println("p1-ON");
      }
      else
      {
        digitalWrite(p1, 1);
        Serial.println("p1-OFF");
      }
    }
    valid = true;
  }
  if (doc.containsKey("p2"))
  {
    if (vP2 != doc["p2"])
    {
      vP2 = !vP2;
      if (vP2)
      {
        digitalWrite(p2, 0);
        Serial.println("p2-ON");
      }
      else
      {
        digitalWrite(p2, 1);
        Serial.println("p2-OFF");
      }
    }
    valid = true;
  }

  // -------------------------

  if (!valid)
  {
    digitalWrite(led, 1);
    Serial.println("Invalid data");
    return "Invalid data";
  }

  if (save)
  {
    Serial.print("New Values: ");
    Serial.println(LogValues());
    SaveAllTimes();
  }

  digitalWrite(led, 1);
  return "";
}

void checkUpdate()
{
  sensors.requestTemperatures();
  float temperatura = (sensors.getTempCByIndex(0));

  if (abs(lastTemp - temperatura) > 0.5)
  {
    String zegar = timeClient.getFormattedTime();
    lastTemp = temperatura;
    String data = "{ \"Time\": \"";
    data += zegar;
    data += "\", \"Voltage\": ";
    data += vcc;
    data += ", \"Temp\": ";
    data += temperatura;
    data += "}";
    Serial.print("[CLIENT] ");
    Serial.println("Update");
    client.print("DATA:" + data);
  }
}

String readMessage()
{
  if (client.available() == 0)
  {
    return "";
  }

  String msg = "";
  while (client.available() > 0)
  {
    msg += char(client.read());
  }
  Serial.print("[CLIENT] ");
  Serial.println(msg);
  return msg;
}

struct SplitResult getSplitResult(String s)
{
  int index = s.indexOf(':');
  SplitResult result;
  result.code = s.substring(0, index);
  result.message = s.substring(index + 1);
  return result;
}

//-----Pins--------------------------

void check()
{
  //-----P1-----
  if (timeClient.getFormattedTime() > p1timeS + ":00" && timeClient.getFormattedTime() < p1timeS + ":30")
  {
    digitalWrite(led, 0);
    digitalWrite(p1, 0);
    vP1 = true;
    Serial.println("p1-ON");
  }
  if (timeClient.getFormattedTime() > p1timeE + ":00" && timeClient.getFormattedTime() < p1timeE + ":30")
  {
    digitalWrite(led, 0);
    digitalWrite(p1, 1);
    vP1 = false;
    Serial.println("p1-OFF");
  }

  //-----P2------
  if (timeClient.getFormattedTime() > p2timeS + ":00" && timeClient.getFormattedTime() < p2timeS + ":30")
  {
    digitalWrite(led, 0);
    digitalWrite(p2, 0);
    vP2 = true;
    Serial.println("p2-ON");
  }
  if (timeClient.getFormattedTime() > p2timeE + ":00" && timeClient.getFormattedTime() < p2timeE + ":30")
  {
    digitalWrite(led, 0);
    digitalWrite(p2, 1);
    vP2 = false;
    Serial.println("p2-OFF");
  }

  //-----Karm------
  if (timeClient.getFormattedTime() > ktime + ":00" && timeClient.getFormattedTime() < ktime + ":30")
  {
    if (isKarm)
    {
      karmie();
    }
    isKarm = false;
  }
  else
  {
    isKarm = true;
  }

  digitalWrite(led, 1);
}

void karmie()
{
  digitalWrite(led, 0);
  Serial.println("Karmie...");

  myservo.attach(karmPin);
  myservo.write(180);
  delay(750);
  myservo.write(0);
  delay(750);
  myservo.write(180);
  delay(750);
  myservo.detach();
  digitalWrite(led, 1);
}

//-----EEPROM----------------------------------

void SaveAllTimes()
{
  Serial.print("Saving Values: ");
  SaveTime(p1timeS, 0);
  SaveTime(p1timeE, 4);
  SaveTime(p2timeS, 8);
  SaveTime(p2timeE, 12);
  SaveTime(ktime, 16);
  EEPROM.commit();
  Serial.println(LogValues());
  // adrres conunt = 20 ; end addres = 19
}

void SaveTime(String s, int addr)
{
  EEPROM.put(addr, (int)s[0]);
  EEPROM.put(addr + 1, (int)s[1]);
  EEPROM.put(addr + 2, (int)s[3]);
  EEPROM.put(addr + 3, (int)s[4]);
}

void ReadAllTimes()
{
  Serial.print("Reading Values: ");
  p1timeS = ReadTime(0);
  p1timeE = ReadTime(4);
  p2timeS = ReadTime(8);
  p2timeE = ReadTime(12);
  ktime = ReadTime(16);
  Serial.println(LogValues());
}

String ReadTime(int addr)
{
  String s = "";
  char b;
  EEPROM.get(addr, b);
  s += b;
  EEPROM.get(addr + 1, b);
  s += b;
  s += ":";
  EEPROM.get(addr + 2, b);
  s += b;
  EEPROM.get(addr + 3, b);
  s += b;
  return s;
}

//----------------------------------------------------------------

void setup()
{
  Serial.begin(115200);

  delay(2000);

  // pins
  Serial.println();
  Serial.print("Pins Initialize...");
  pinMode(led, OUTPUT);
  pinMode(p1, OUTPUT);
  pinMode(p2, OUTPUT);

  digitalWrite(led, 1);
  digitalWrite(p1, 1);
  digitalWrite(p2, 1);
  Serial.println("ok");

  // servo
  Serial.print("Servo Initialize...");
  digitalWrite(led, 0);
  myservo.attach(karmPin);
  myservo.write(180);
  delay(750);
  myservo.detach();
  Serial.println("ok");
  digitalWrite(led, 1);
  delay(100);

  // WiFi
  Serial.print("WiFi Configuration...");
  digitalWrite(led, 1);
  WiFi.disconnect();
  if (WiFi.config(staticIP, gateway, subnet, dns))
  {
    Serial.println("ok");
  }
  else
  {
    Serial.println("failed");
  }
  WiFi.hostname("isma.ism");
  WiFi.mode(WIFI_STA);
  digitalWrite(led, 0);
  delay(100);

  Serial.print("WiFi Initialize...");
  digitalWrite(led, 0);
  if (WiFi.begin(ssid, password))
  {
    Serial.println("ok");
  }
  else
  {
    Serial.println("failed");
  }
  digitalWrite(led, 1);
  delay(100);

  // WiFi Conecting
  Serial.println("Conecting:");
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(led, 0);
    delay(250);
    Serial.print(".");
    digitalWrite(led, 1);
    delay(250);
  }
  Serial.println();
  delay(100);

  // Time server
  digitalWrite(led, 0);
  Serial.print("Time server initialize...");
  timeClient.begin();
  Serial.println("ok");
  digitalWrite(led, 1);
  delay(100);

  // EEPROM
  digitalWrite(led, 0);
  Serial.print("EEPROM initialize...");
  EEPROM.begin(24);
  Serial.println("ok");
  digitalWrite(led, 1);
  delay(100);

  // Read Times
  digitalWrite(led, 0);
  ReadAllTimes();
  digitalWrite(led, 1);
  delay(100);

  // IP
  digitalWrite(led, 0);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(led, 1);
}

void loop()
{
  timeClient.update();
  check();
  NormalLog();

  // ------------------------------------------

  if (!client.connected())
  {
    client.stop();
    connectionAuthorized = false;
    if (!client.connect(server, port))
    {
      Serial.println("Connection to host failed");
      delay(500);
      return;
    }
  }

  if (!connectionAuthorized)
  {
    client.print("CON:ISMA");

    unsigned long timeout = millis();
    while (client.available() == 0)
    {
      if (millis() - timeout > 5000)
      {
        Serial.print("[CLIENT] ");
        Serial.println("Server Timeout !");
        client.stop();
        connectionAuthorized = false;
        return;
      }
    }

    if (!readMessage().startsWith("OK"))
    {
      client.stop();
      connectionAuthorized = false;
      return;
    }
    Serial.print("[CLIENT] ");
    Serial.println("Connected");
    connectionAuthorized = true;
  }

  String msg = readMessage();
  if (msg != "")
  {
    SplitResult info = getSplitResult(msg);

    if (info.code == "PING")
    {
      client.print("OK:OK");
    }
    else if (info.code == "GET")
    {
      Serial.println(getData());
      client.print("DATA:" + getData());
    }
    else if (info.code == "SET")
    {
      String e = SetTimers(info.message);
      if (e == "")
      {
        client.print("OK:OK");
      }
      else
      {
        client.print("ERR:" + e);
      }
    }
    else
    {
    }
  }
  else
  {
    checkUpdate();
  }

  delay(500);
}
