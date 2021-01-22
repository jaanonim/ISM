ADC_MODE(ADC_VCC);
#include <NTPClient.h>

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>

/*
const char *ssid     = "AndroidAP";
const char *password = "";
*/

const char *ssid = "PGM";
const char *password = "Psalm121";

IPAddress staticIP(192, 168, 0, 11); //ESP static ip
IPAddress gateway(192, 168, 0, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);  //Subnet mask
IPAddress dns(8, 8, 8, 8);           //DNS

int led = 15; //D0 D8

int pinTem = 14;   //D5
int alarmPin = 13; //D7
int pompaPin = 12; //D6
bool isWater;

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

float vcc;

WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "0.pl.pool.ntp.org", 3600);
ESP8266WebServer server(80);

OneWire oneWire(pinTem);
DallasTemperature sensors(&oneWire);

//-----Logging-------------------------

String LogValues()
{
  return (String)("time: " + pTime + " fn: " + fn + " len: " + len + " rtime: " + rTime + " leftDays: " + leftDays + " alarmLen: " + lenA + " alarmIsAble: " + alarmIsAble);
}

//-----HTML----------------------------

String MainHtml(String title)
{
  String zegar = timeClient.getFormattedTime();

  String html = "<!DOCTYPE html> <html lang=\"pl\"> <head> <meta charset=\"utf-8\"> <title>I.S.M.P.</title> <meta http-equiv=\"X-Ua-Compatible\" content=\"IE=edge,chrome=1\"> <!--[if lt IE 9]> <script src=\"//cdnjs.cloudflare.com/ajax/libs/html5shiv/3.7.3/html5shiv.min.js\"></script> <![endif]--> <style> body { background-color: #444; color: #fff; font-size: 20px; } h2 { color: #fff; font-size: 22px; padding 10px; } a { Color: #ffae0d; cursor: pointer; text-decoration: none; } input { cursor: pointer; text-align: center; text-decoration: none; color: #fff; padding: 5px; margin: 5px; } input[type=submit] { width: 200px; height: 40px; font-size: 20px; border: 2px solid #ffcf2f; background-color: #ffae0d; border-radius: 5px; } input[type=time] { width: 90px; height: 20px; font-size: 18px; border: 2px solid #ffcf2f; background-color: #ffae0d; border-radius: 5px; } input[type=number] { width: 70px; height: 20px; font-size: 18px; border: 2px solid #ffcf2f; background-color: #ffae0d; border-radius: 5px; } input[type=number]:hover { border: 2px solid #ffcf2f; background-color: #ee9d0c; } input[type=time]:hover { border: 2px solid #ffcf2f; background-color: #ee9d0c; } input[type=submit]:hover { border: 2px solid #ffcf2f; background-color: #ee9d0c; } .logo { background-color: #666; text-align: center; font-size: 30px; padding: 25px; width: 1000px; margin-left: auto; margin-right: auto; } .infoBar { font-size: 18px; text-align: center; background-color: #ffbe1e; padding: 10px; min-width: 1050px; } .infoCointenr { margin-left: auto; margin-right: auto; width: 1000px; padding: 0 25px 0 25px; } .info { float: left; width: 220px; padding: 10px; background-color: #ffae0d; margin: 2px 5px 2px 5px; border-radius: 5px; } .conteiner { background-color: #555; text-align: center; padding: 25px; width: 1000px; margin-left: auto; margin-right: auto; } .footer { font-size: 14px; background-color: #333; text-align: center; padding: 10px; width: 1030px; margin-left: auto; margin-right: auto; } .content { background-color: #666; padding: 10px; width: 980px; margin-left: auto; margin-right: auto; border-radius: 10px; margin: 10px 0 10px 0; } .left { float: left; padding: 10px; width: 310px; margin: 5px; height: 90px; } .right { float: left; padding: 10px; width: 610px; min-height: 90px; margin: 3px; border-width: 0 0 0 2px; border-color: #fff; border-style: solid; } .on { width: 34px; height: 20px; margin-left: auto; margin-right: auto; background-color: green; border-radius: 5px; } .off { width: 34px; height: 20px; margin-left: auto; margin-right: auto; background-color: red; border-radius: 5px; } </style> </head> <body> <div class=\"logo\"> I.S.M.P.<br> <spam style=\"font-size: 22px\">Inteligentny System Zarządzania Roślinami</spam> </div> <div class=\"infoBar\"> <div class=\"infoCointenr\"> <div class=\"info\">";
  html += title;
  html += "</div> <div class=\"info\"> Alarm: ";
  if (alarmIsAble)
  {
    html += "on";
  }
  else
  {
    html += "off";
  }
  html += " </div> <div class=\"info\"> Napięcie: ";
  html += vcc;
  html += "V </div> <div class=\"info\"> Godzina: ";
  html += zegar;
  html += " </div> <div style=\"clear: both;\"></div> </div> </div> <div class=\"conteiner\">";

  return html;
}

String MainEndHtml()
{
  return "</div> <div class=\"footer\"> I.S.M.P.&copy; 2019 <br> <spam style=\"font-size: 12px\"><br>Intelligent System Managmet Plants | Inteligentny System Zarządzania Roślinami</spam> </div> </body> </html>";
}

//-----HTML-out-----------------------

String indexHtml()
{
  sensors.requestTemperatures();
  float temperaturaIn = (sensors.getTempCByIndex(inIndex));
  float temperaturaOut = (sensors.getTempCByIndex(outIndex));

  String html = MainHtml("Witamy!");

  html += "<div class=\"content\"> <div class=\"left\"> <h2>Alarm:</h2> <div class=\"";
  if (alarmIsAble)
  {
    html += "on";
  }
  else
  {
    html += "off";
  }
  html += "\"></div> </div> <div class=\"right\"> <a href=\"/ismp/on\">on</a> / <a href=\"/ismp/off\">off</a> <br> Czas alarmu: ";
  html += lenA;
  html += " </div> <div style=\"clear: both;\"></div> </div> <div class=\"content\"> <div class=\"left\"> <h2>Temperatura:</h2> </div> <div class=\"right\"> </br> Wewnątrz: ";
  html += temperaturaIn;
  html += " </br> Na zewnątrz: ";
  html += temperaturaOut;
  html += " </div> <div style=\"clear: both;\"></div> </div> <div class=\"content\"> <div class=\"left\"> <h2>Podlewanie:</h2> <div class=\"";
  if (pump)
  {
    html += "on";
  }
  else
  {
    html += "off";
  }
  html += "\"></div> </div> <div class=\"right\"> O gdzinie: ";
  html += pTime;
  html += " <br> Co ";
  html += fn;
  html += " dni. | Pozostało: ";
  html += leftDays;
  html += " dni. <br> Czas podlewania: ";
  html += len;
  html += " <br><br> <a href=\"/ismp/podlej\">Podlej</a> <br> <a href=\"/ismp/onp\">on</a> / <a href=\"/ismp/offp\">off</a> </div> <div style=\"clear: both;\"></div> </div> <div class=\"content\"> <div class=\"left\"> <h2>Restart:</h2> </div> <div class=\"right\"> </br> Koniec dnia o godzinie: ";
  html += rTime;
  html += " </div> <div style=\"clear: both;\"></div> </div> <div class=\"content\"> <form action=\"/ismp/settime\"><input type=\"submit\" value=\"Zmień Wartości\"></form> </div>";

  html += MainEndHtml();

  return html;
}

String settimeHtml()
{
  String html = MainHtml("Ustawienia zegarów");

  html += "<form action=\"/ismp/time\"> <div class=\"content\"> <div class=\"left\"> <h2>Podlewanie:</h2> </div> <div class=\"right\"> O godzinie:<input type=\"time\" name=\"time\" value=\"";
  html += pTime;
  html += "\"> <br> Powtarzaj co:<input type=\"number\" name=\"fn\" value=\"";
  html += fn;
  html += "\"> <br> Czas podlewania:<input type=\"number\" name=\"len\" value=\"";
  html += len;
  html += "\"> (s) </div> <div style=\"clear: both;\"></div> </div> <div class=\"content\"> <div class=\"left\"> <h2>Restart:</h2> </div> <div class=\"right\"> <br> <input type=\"time\" name=\"rtime\" value=\"";
  html += rTime;
  html += "\"> </div> <div style=\"clear: both;\"></div> </div> <div class=\"content\"> <div class=\"left\"> <h2>Alarm:</h2> </div> <div class=\"right\"> <br> Długość alarmu:<input type=\"number\" name=\"lena\" value=\"";
  html += lenA;
  html += "\"> (s) </div> <div style=\"clear: both;\"></div> </div> <div class=\"content\"> <input type=\"submit\" value=\"Zapisz\"> </div> </form>";

  html += MainEndHtml();

  return html;
}

String Error404Html()
{
  String message = "URI: ";
  message += server.uri();
  message += "<br>Method: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "<br>Arguments: ";
  message += server.args();
  message += "<br>";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "<br>";
  }

  String html = MainHtml("Error 404");

  html += "<div class=\"content\"> <h1> Error <spam style=\"color: red;\">404</spam> </h1> Nie znaleziono pliku!<br>";
  html += message;
  html += "<br><br> <a href=\"/ismp/\">home</a><br><br> </div>";

  html += MainEndHtml();

  return html;
}

//-----SERVER--------------------------

void handleNotFound()
{
  digitalWrite(led, 1);
  Serial.println("Request: 404");
  server.send(404, "text/html", Error404Html());
  digitalWrite(led, 0);
}

void handleRoot()
{
  digitalWrite(led, 1);
  Serial.println("Requset: /ismp/");
  server.send(200, "text/html", indexHtml());
  digitalWrite(led, 0);
}

void handleSetTmie()
{
  digitalWrite(led, 1);
  Serial.println("Requset: /ismp/settime");
  server.send(200, "text/html", settimeHtml());
  digitalWrite(led, 0);
}

void backHome()
{
  Serial.println("Redirect to: /ismp/");
  server.sendHeader("Location", "/ismp/", true);
  server.send(302, "text/plain", "");
}

void api()
{
  digitalWrite(led, 1);
  Serial.print("Requset: /ismp/api");
  if (server.method() == HTTP_GET)
  {
    String zegar = timeClient.getFormattedTime();
    sensors.requestTemperatures();
    float temperaturaIn = (sensors.getTempCByIndex(inIndex));
    float temperaturaOut = (sensors.getTempCByIndex(outIndex));

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

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Max-Age", "10000");
    server.sendHeader("Access-Control-Allow-Methods", "GET");
    server.sendHeader("Access-Control-Allow-Headers", "*");
    server.send(200, "application/json", data);
    Serial.println(" 200");
  }
  else
  {
    server.send(400, "text/plain", "");
    Serial.println(" 400");
  }
  digitalWrite(led, 0);
}

//-----handle---------------------------

String GetTimers()
{
  digitalWrite(led, 1);
  Serial.print("Getting Values: ");

  if (server.args() > 0)
  {
    for (uint8_t i = 0; i < server.args(); i++)
    {
      if (server.argName(i) == "time")
      {
        pTime = server.arg(i);
      }
      if (server.argName(i) == "rtime")
      {
        rTime = server.arg(i);
      }
      if (server.argName(i) == "len")
      {
        len = server.arg(i).toInt();
      }
      if (server.argName(i) == "fn")
      {
        fn = server.arg(i).toInt();
        leftDays = fn - 1;
      }
      if (server.argName(i) == "lena")
      {
        lenA = server.arg(i).toInt();
      }
    }
  }
  String data = LogValues();
  Serial.println(data);
  SaveAllTimes();
  digitalWrite(led, 0);
  return data;
}

void handleTimeApi()
{
  Serial.println("Requset: /ismp/api/time");
  String data = GetTimers();
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Max-Age", "10000");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST");
  server.sendHeader("Access-Control-Allow-Headers", "*");
  server.send(200, "text/plain", data);
}

void handleTime()
{
  Serial.println("Requset: /ismp/time");
  GetTimers();
  backHome();
}
//-----Pins--------------------------

void check()
{
  //temperature
  sensors.requestTemperatures();
  float temperaturaIn = (sensors.getTempCByIndex(inIndex));
  float temperaturaOut = (sensors.getTempCByIndex(outIndex));

  //podlewanie
  if (timeClient.getFormattedTime() > pTime + ":00" && timeClient.getFormattedTime() < pTime + ":10")
  {
    podlej();
    isWater = true;
  }

  //reset
  if (timeClient.getFormattedTime() > rTime + ":00" && timeClient.getFormattedTime() < rTime + ":10")
  {
    if (isWater || alarm)
    {
      isWater = false;
      alarm = false;
    }
  }

  //alarm
  if (round(temperaturaIn) < round(temperaturaOut) && alarmIsAble)
  {
    Alarm();
    alarm = true;
  }
}

void Alarm()
{
  if (!alarm)
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
}

void podlejHtml()
{
  Serial.println("Podlwanie: ");
  bool b = true;
  backHome();
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

void podlej()
{
  if (!isWater)
  {
    if (leftDays <= 0)
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
      leftDays = fn - 1;
    }
    else
    {
      leftDays--;
      SaveLeftDays();
    }
  }
}

void On()
{
  digitalWrite(led, 1);
  alarmIsAble = true;
  SaveAlarm();
  Serial.println("Alarm ON");
  digitalWrite(led, 0);
  backHome();
}

void Off()
{
  digitalWrite(led, 1);
  alarmIsAble = false;
  SaveAlarm();
  Serial.println("Alarm ON");
  digitalWrite(led, 0);
  backHome();
}

void OnPump()
{
  digitalWrite(led, 1);
  digitalWrite(pompaPin, 0);
  pump = true;
  Serial.println("Pompa ON");
  digitalWrite(led, 0);
  backHome();
}

void OffPump()
{
  digitalWrite(led, 1);
  digitalWrite(pompaPin, 1);
  pump = false;
  Serial.println("Pompa OFF");
  digitalWrite(led, 0);
  backHome();
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
  WiFi.mode(WIFI_STA);
  WiFi.config(staticIP, subnet, gateway, dns);

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

  //MDNS
  digitalWrite(led, 1);
  Serial.print("MDNS responder initialize...");
  if (MDNS.begin("ismp"))
  {
    Serial.println("ok");
  }
  else
  {
    Serial.println("failed");
  }
  digitalWrite(led, 0);
  delay(100);

  //HTTP server
  digitalWrite(led, 1);
  Serial.print("HTTP server initialize...");
  server.begin();
  Serial.println("ok");
  digitalWrite(led, 0);
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

  //Server
  digitalWrite(led, 1);
  Serial.print("Server endpoint initialize...");

  server.on("/", backHome);
  server.on("/ismp/", handleRoot);
  server.on("/ismp/time", handleTime);
  server.on("/ismp/settime", handleSetTmie);
  server.on("/ismp/podlej", podlejHtml);
  server.on("/ismp/on", On);
  server.on("/ismp/off", Off);
  server.on("/ismp/onp", OnPump);
  server.on("/ismp/offp", OffPump);
  server.on("/ismp/api", api);
  server.on("/ismp/api/time", handleTimeApi);

  server.onNotFound(handleNotFound);

  Serial.println("ok");
  digitalWrite(led, 0);
}

void loop()
{
  vcc = (float)ESP.getVcc() / 1000;
  timeClient.update();

  check();

  Serial.print("# ");
  Serial.print(timeClient.getFormattedTime());
  Serial.println(" V=" + (String)vcc);

  server.handleClient();
  MDNS.update();

  delay(500);
  digitalWrite(led, 0);
}
