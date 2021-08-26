ADC_MODE(ADC_VCC);
#include <NTPClient.h>

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>

#include "ArduinoJson.h"

#include "creds.h"

IPAddress staticIP(192, 168, 0, 15); //ESP static ip
IPAddress gateway(192, 168, 0, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);  //Subnet mask
IPAddress dns(8, 8, 8, 8);           //DNS

WiFiClient client;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "192.168.0.26", 3600);

const uint16_t port = 2693;
const IPAddress server(192, 168, 0, 26);

bool connectionAuthorized = false;

int led = 15; //D0 D8

int pinTem = 14;   //D5
int alarmPin = 13; //D7
int pompaPin = 12; //D6
bool isWater;

float vcc;

int inIndex = 1;
int outIndex = 0;

String formattedDate;
String dayStamp;
String timeStamp;

String rTime = "23:59"; //save
String pTime = "00:00"; //save
int leftDays = 0;       //save
int len = 5;            //save
int fn = 1;             //save
int lenA = 2;           // długość alarmu  //save

bool alarmIsAble = false; //save
bool alarm = false;
bool pump = false;

float lastTempIn;
float lastTempOut;

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
  return (String)("time: " + pTime + " fn: " + String(fn) + " len: " + String(len) + " rtime: " + rTime + " leftDays: " + String(leftDays) + " alarmLen: " + String(lenA) + " alarmIsAble: " + String(alarmIsAble));
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
  float temperaturaIn = (sensors.getTempCByIndex(inIndex));
  float temperaturaOut = (sensors.getTempCByIndex(outIndex));

  lastTempIn = temperaturaIn;
  lastTempOut = temperaturaOut;

  String data = "{ \"Time\": \"";
  data += zegar;
  data += "\", \"Voltage\": ";
  data += vcc;
  data += ", \"Temp_in\": ";
  data += temperaturaIn;
  data += ", \"Temp_out\": ";
  data += temperaturaOut;
  data += ", \"Alarm\": { \"status\": ";
  if (alarmIsAble)
  {
    data += "true";
  }
  else
  {
    data += "false";
  }
  data += ", \"time\": ";
  data += lenA;
  data += " }, \"Polewanie\": { \"status\": ";
  if (pump)
  {
    data += "true";
  }
  else
  {
    data += "false";
  }
  data += ", \"time\": ";
  data += len;
  data += ", \"hour\": \"";
  data += pTime;
  data += "\", \"left\": ";
  data += leftDays;
  data += ", \"loop\": ";
  data += fn;
  data += " }, \"Reset\": \"";
  data += rTime;
  data += "\" }";
  return data;
}

String SetTimers(String s)
{
  digitalWrite(led, 1);
  Serial.println("Getting Values: ");

  StaticJsonDocument<400> buffer;
  DeserializationError error = deserializeJson(buffer, s);

  if (error)
  {
    digitalWrite(led, 0);
    Serial.println("Deserialize json failed.");
    return "Deserialize json failed";
  }

  JsonObject doc = buffer.as<JsonObject>();

  bool valid = false;
  bool save = false;

  // -------- TIMERS --------

  if (doc.containsKey("rtime"))
  {
    rTime = (String)doc["rtime"];
    valid = true;
    save = true;
  }
  if (doc.containsKey("ptime"))
  {
    pTime = (String)doc["ptime"];
    valid = true;
    save = true;
  }
  if (doc.containsKey("len"))
  {
    len = doc["len"];
    valid = true;
    save = true;
  }
  if (doc.containsKey("fn"))
  {
    fn = doc["fn"];
    valid = true;
    save = true;
  }
  if (doc.containsKey("lena"))
  {
    lenA = doc["lena"];
    valid = true;
    save = true;
  }

  // -------- ACTIONS --------

  if (doc.containsKey("podlej"))
  {
    valid = true;
    if (doc["podlej"])
    {
      podlej();
    }
  }
  if (doc.containsKey("alarm"))
  {
    if (alarmIsAble != doc["alarm"])
    {
      alarmIsAble = !alarmIsAble;
      SaveAlarm();
      if (alarmIsAble)
      {
        Serial.println("Alarm ON");
      }
      else
      {
        Serial.println("Alarm OFF");
      }
    }
    valid = true;
  }
  if (doc.containsKey("woda"))
  {
    if (pump != doc["woda"])
    {
      pump = !pump;
      if (pump)
      {
        digitalWrite(pompaPin, 0);
        Serial.println("Pompa ON");
      }
      else
      {
        digitalWrite(pompaPin, 1);
        Serial.println("Pompa OFF");
      }
    }
    valid = true;
  }

  // -------------------------

  if (!valid)
  {
    digitalWrite(led, 0);
    Serial.println("Invalid data");
    return "Invalid data";
  }

  if (save)
  {
    Serial.print("New Values: ");
    Serial.println(LogValues());
    SaveAllTimes();
  }

  digitalWrite(led, 0);
  return "";
}

void checkUpdate()
{
  sensors.requestTemperatures();
  float temperaturaIn = (sensors.getTempCByIndex(inIndex));
  float temperaturaOut = (sensors.getTempCByIndex(outIndex));

  if ((abs(lastTempIn - temperaturaIn) > 0.5) || (abs(lastTempOut - temperaturaOut) > 0.5))
  {
    String zegar = timeClient.getFormattedTime();
    lastTempIn = temperaturaIn;
    lastTempOut = temperaturaOut;
    String data = "{ \"Time\": \"";
    data += zegar;
    data += "\", \"Voltage\": ";
    data += vcc;
    data += ", \"Temp_in\": ";
    data += temperaturaIn;
    data += ", \"Temp_out\": ";
    data += temperaturaOut;
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
  //temperature
  sensors.requestTemperatures();
  float temperaturaIn = (sensors.getTempCByIndex(inIndex));
  float temperaturaOut = (sensors.getTempCByIndex(outIndex));

  //podlewanie
  if (timeClient.getFormattedTime() > pTime + ":00" && timeClient.getFormattedTime() < pTime + ":30")
  {
    if (!isWater)
    {
      if (leftDays <= 0)
      {
        podlej();
        leftDays = fn - 1;
      }
      else
      {
        leftDays--;
        SaveLeftDays();
      }
    }
    isWater = true;
  }

  //reset
  if (timeClient.getFormattedTime() > rTime + ":00" && timeClient.getFormattedTime() < rTime + ":30")
  {
    if (isWater || alarm)
    {
      isWater = false;
      alarm = false;
    }
  }

  //alarm
  if (temperaturaOut > -100 && temperaturaIn > -100 && round(temperaturaIn) < round(temperaturaOut) && alarmIsAble)
  {
    if (!alarm)
    {
      Alarm();
    }
    alarm = true;
  }

  digitalWrite(led, 0);
}

void Alarm()
{
  Serial.println("Alarm: ");
  bool b = true;
  digitalWrite(alarmPin, 1);
  for (int i = 0; i < lenA; i++)
  {
    Serial.print(".");
    digitalWrite(led, b);
    b = (!b);
    delay(1000);
  }
  digitalWrite(alarmPin, 0);
  Serial.println("");
}

void podlej()
{
  Serial.println("Podlwanie: ");
  bool b = true;
  digitalWrite(pompaPin, 0);
  for (int i = 0; i < len; i++)
  {
    digitalWrite(led, b);
    Serial.print(".");
    b = (!b);
    delay(1000);
  }
  digitalWrite(pompaPin, 1);
  Serial.println("");
}

//------EPPROM-----------------------------------------------------

//SAVE

void SaveAllTimes()
{
  Serial.print("Saving Values: ");
  SaveTime(rTime, 2);
  SaveTime(pTime, 6);
  SaveInt(leftDays, 10);
  SaveInt(fn, 18);
  SaveInt(lenA, 22);
  SaveBool(alarmIsAble, 26);
  SaveInt(len, 32);
  EEPROM.commit();
  Serial.println(LogValues());
  //adrres conunt = 13 ; end addres = 12
}

void SaveLeftDays()
{
  Serial.println("Saving LeftDatys: " + leftDays);
  SaveInt(leftDays, 10);
  EEPROM.commit();
}

void SaveAlarm()
{
  Serial.println("Saving Alarm: " + alarmIsAble);
  SaveBool(alarmIsAble, 26);
  EEPROM.commit();
}

//VOID SAVE
void SaveInt(int i, int addr)
{
  if (i > 9999 || i < 0)
  {
    Serial.println("ERROR out of range at addres" + addr);
    return;
  }
  String s = String(i);
  switch (s.length())
  {
  case 0:
  {
    s = "0000";
  }
  break;
  case 1:
  {
    s = "000" + s;
  }
  break;
  case 2:
  {
    s = "00" + s;
  }
  break;
  case 3:
  {
    s = "0" + s;
  }
  break;
  }
  s = String(s[0]) + String(s[1]) + ":" + String(s[2]) + String(s[3]);
  SaveTime(s, addr);
}

void SaveBool(bool b, int addr)
{
  if (b)
  {
    SaveInt(1, addr);
  }
  else
  {
    SaveInt(0, addr);
  }
}

void SaveTime(String s, int addr)
{
  // zajmuje 4 miejsca
  EEPROM.put(addr, (int)s[0]);
  EEPROM.put(addr + 1, (int)s[1]);
  EEPROM.put(addr + 2, (int)s[3]);
  EEPROM.put(addr + 3, (int)s[4]);
}

//READ

void ReadAllTimes()
{
  Serial.print("Reading Values: ");
  rTime = ReadTime(2);
  pTime = ReadTime(6);
  leftDays = ReadInt(10);
  fn = ReadInt(18);
  lenA = ReadInt(22);
  alarmIsAble = ReadBool(26);
  len = ReadInt(32);
  Serial.println(LogValues());
}

//VOID READ
bool ReadBool(int addr)
{
  int i = ReadInt(addr);
  if (i == 1)
  {
    return true;
  }
  else
  {
    return false;
  }
}

int ReadInt(int addr)
{
  String s = ReadTime(addr);
  s = String(s[0]) + String(s[1]) + String(s[3]) + String(s[4]);
  return s.toInt();
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

  //pins
  Serial.println();
  Serial.print("Pins Initialize...");
  pinMode(led, OUTPUT);
  pinMode(alarmPin, OUTPUT);
  pinMode(pompaPin, OUTPUT);

  digitalWrite(led, 0);
  digitalWrite(pompaPin, 1);
  Serial.println("ok");

  //WiFi
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
  WiFi.hostname("ismp.ism");
  WiFi.mode(WIFI_STA);
  digitalWrite(led, 0);
  delay(100);

  Serial.print("WiFi Initialize...");
  digitalWrite(led, 1);
  if (WiFi.begin(ssid, password))
  {
    Serial.println("ok");
  }
  else
  {
    Serial.println("failed");
  }
  digitalWrite(led, 0);
  delay(100);

  //WiFi Conecting
  Serial.println("Conecting:");
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(led, 1);
    delay(250);
    Serial.print(".");
    digitalWrite(led, 0);
    delay(250);
  }
  Serial.println();
  delay(100);
  //Time server
  digitalWrite(led, 1);
  Serial.print("Time server initialize...");
  timeClient.begin();
  Serial.println("ok");
  digitalWrite(led, 0);
  delay(100);

  //EEPROM
  digitalWrite(led, 1);
  Serial.print("EEPROM initialize...");
  EEPROM.begin(512);
  Serial.println("ok");
  digitalWrite(led, 0);
  delay(1000);

  //Read Times
  digitalWrite(led, 1);
  ReadAllTimes();
  digitalWrite(led, 0);
  delay(100);

  //IP
  digitalWrite(led, 1);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(led, 0);
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
    client.print("CON:ISMP");

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
