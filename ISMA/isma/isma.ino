ADC_MODE(ADC_VCC);
#include <NTPClient.h>

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Servo.h>
#include <EEPROM.h>

#include "creds.h"
/*
const char *ssid     = "AndroidAP";
const char *password = "";
*/

IPAddress staticIP(192, 168, 0, 11); //ESP static ip
IPAddress gateway(192, 168, 0, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);  //Subnet mask
IPAddress dns(8,8,8,8);              //DNS

Servo myservo;

int led = 16; //D0

int p1 = 14;     //D5
int p2 = 12;     //D6
int karmPin = 5; //D1
int pinTem = 4;  //D2
bool isKarm;

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

float vcc;

WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "192.168.0.26", 3600);
ESP8266WebServer server(80);

OneWire oneWire(pinTem);
DallasTemperature sensors(&oneWire);

//-----Logging-------------------------

String LogValues()
{
  return (String)("p1s: " + p1timeS + " p1e: " + p1timeE + "p2s: " + p2timeS + " p2e: " + p2timeE + " ktime:" + ktime);
}

//-----HTML----------------------------

String MainHtml(String title)
{
  sensors.requestTemperatures();
  float temperatura = (sensors.getTempCByIndex(0));
  String zegar = timeClient.getFormattedTime();

  String html = "<!DOCTYPE html> <html lang=\"pl\"> <head> <meta charset=\"utf-8\"> <title>I.S.M.A.</title> <meta http-equiv=\"X-Ua-Compatible\" content=\"IE=edge,chrome=1\"> <!--[if lt IE 9]> <script src=\"//cdnjs.cloudflare.com/ajax/libs/html5shiv/3.7.3/html5shiv.min.js\"></script> <![endif]--> <style> body { background-color: #444; color: #fff; font-size: 20px; } h2 { color: #fff; font-size: 22px; padding 10px; } a { Color: #5cdc00; cursor: pointer; text-decoration: none; } input { cursor: pointer; text-align: center; text-decoration: none; color: #fff; padding: 5px; margin: 5px; } input[type=submit] { width: 200px; height: 40px; font-size: 20px; border: 2px solid #8efe00; background-color: #4bcb00; border-radius: 5px; } input[type=time] { width: 90px; height: 20px; font-size: 18px; border: 2px solid #8efe00; background-color: #4bcb00; border-radius: 5px; } input[type=time]:hover { border: 2px solid #7ded00; background-color: #3aba00; } input[type=submit]:hover { border: 2px solid #7ded00; background-color: #3aba00; } .logo { background-color: #666; text-align: center; font-size: 30px; padding: 25px; width: 1000px; margin-left: auto; margin-right: auto; } .infoBar { font-size: 18px; text-align: center; background-color: #6dfd00; padding: 10px; min-width: 1050px; } .infoCointenr { margin-left: auto; margin-right: auto; width: 1000px; padding: 0 25px 0 25px;} .info { float: left; width: 220px; padding: 10px; background-color: #5cdc00; margin: 2px 5px 2px 5px; border-radius: 5px; } .conteiner { background-color: #555; text-align: center; padding: 25px; width: 1000px; margin-left: auto; margin-right: auto; } .footer { font-size: 14px; background-color: #333; text-align: center; padding: 10px; width: 1030px; margin-left: auto; margin-right: auto; } .content { background-color: #666; padding: 10px; width: 980px; margin-left: auto; margin-right: auto; border-radius: 10px; margin: 10px 0 10px 0; } .left { float: left; padding: 10px; width: 310px; margin: 5px; height: 90px; } .right { float: left; padding: 10px; width: 610px; height: 90px; margin: 3px; border-width: 0 0 0 2px; border-color: #fff; border-style: solid; } .on { width: 34px; height: 20px; margin-left: auto; margin-right: auto; background-color: green; border-radius: 5px; } .off { width: 34px; height: 20px; margin-left: auto; margin-right: auto; background-color: red; border-radius: 5px; } </style> </head> <body> <div class=\"logo\"> I.S.M.A.<br> <spam style=\"font-size: 22px\">Inteligentny System Zarządzania Akwarium</spam> </div> <div class=\"infoBar\"> <div class=\"infoCointenr\"> <div class=\"info\">";
  html += title;
  html += "</div> <div class=\"info\"> Temperatura: ";
  html += temperatura;
  html += "&#176;C </div> <div class=\"info\"> Napięcie: ";
  html += vcc;
  html += "V </div> <div class=\"info\"> Godzina: ";
  html += zegar;
  html += " </div> <div style=\"clear: both;\"></div> </div> </div> <div class=\"conteiner\"> ";
  return html;
}

String MainEndHtml()
{
  String html = " </div> <div class=\"footer\"> I.S.M.A.&copy; 2019 <br> <spam style=\"font-size: 12px\"><br>Intelligent System Managmet Aquarium | Inteligentny System Zarządzania Akwarium</spam> </div> </body> </html>";
  return html;
}

//-----HTML-out-----------------------

String indexHtml()
{
  String html = MainHtml("Witamy!");

  html += " <div class=\"content\"> <div class=\"left\"> <h2>P1:</h2> <div class=\"";

  if (vP1)
  {
    html += "on";
  }
  else
  {
    html += "off";
  }

  html += "\"></div> </div> <div class=\"right\"> <a href=\"/isma/onp1\">on</a> / <a href=\"/isma/offp1\">off</a><br><br>";
  html += p1timeS;
  html += " | ";
  html += p1timeE;
  html += " </div> <div style=\"clear: both;\"></div> </div> <div class=\"content\"> <div class=\"left\"> <h2>P2:</h2> <div class=\"";

  if (vP2)
  {
    html += "on";
  }
  else
  {
    html += "off";
  }

  html += "\"></div> </div> <div class=\"right\"> <a href=\"/isma/onp2\">on</a> / <a href=\"/isma/offp2\">off</a><br><br>";
  html += p2timeS;
  html += " | ";
  html += p2timeE;
  html += " </div> <div style=\"clear: both;\"></div> </div> <div class=\"content\"> <div class=\"left\"> <h2>Karmienie:</h2> </div> <div class=\"right\"> <a href=\"/isma/karm\">karm</a> <br><br> ";
  html += ktime;
  html += " </div> <div style=\"clear: both;\"></div> </div> <div class=\"content\"> <form action=\"/isma/settime\"><input type=\"submit\" value=\"Zmień zegary\"></form> </div>";

  html += MainEndHtml();

  return html;
}

String settimeHtml()
{
  String html = MainHtml("Ustawienia zegarów");

  html += " <form action=\"/isma/time\"> <div class=\"content\"> <div class=\"left\"> <h2>P1:</h2> </div> <div class=\"right\"> <br> <input type=\"time\" name=\"p1timeS\" value=\"";
  html += p1timeS;
  html += "\"> | <input type=\"time\" name=\"p1timeE\" value=\"";
  html += p1timeE;
  html += "\"> </div> <div style=\"clear: both;\"></div> </div> <div class=\"content\"> <div class=\"left\"> <h2>P2:</h2> </div> <div class=\"right\"> <br> <input type=\"time\" name=\"p2timeS\" value=\"";
  html += p2timeS;
  html += "\"> | <input type=\"time\" name=\"p2timeE\" value=\"";
  html += p2timeE;
  html += "\"> </div> <div style=\"clear: both;\"></div> </div> <div class=\"content\"> <div class=\"left\"> <h2>Karmienie:</h2> </div> <div class=\"right\"> <br> <input type=\"time\" name=\"ktime\" value=\"";
  html += ktime;
  html += "\"> </div> <div style=\"clear: both;\"></div> </div> <div class=\"content\"> <input type=\"submit\" value=\"Zapisz\"> </div> </form> ";

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
  html += "<br><br> <a href=\"/isma/\">home</a><br><br> </div>";

  html += MainEndHtml();

  return html;
}

//-----SERVER--------------------------

void handleNotFound()
{
  digitalWrite(led, 0);
  Serial.println("Request: 404");
  server.send(404, "text/html", Error404Html());
  digitalWrite(led, 1);
}

void handleRoot()
{
  digitalWrite(led, 0);
  Serial.println("Requset: /isma/");
  server.send(200, "text/html", indexHtml());
  digitalWrite(led, 1);
}

void handleSetTmie()
{
  digitalWrite(led, 0);
  Serial.println("Requset: /isma/settime");
  server.send(200, "text/html", settimeHtml());
  digitalWrite(led, 1);
}

void api()
{
  digitalWrite(led, 0);
  Serial.print("Requset: /isma/api");
  if (server.method() == HTTP_GET)
  {
    sensors.requestTemperatures();
    float temperatura = (sensors.getTempCByIndex(0));
    String zegar = timeClient.getFormattedTime();

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
  digitalWrite(led, 1);
}

//-----handle---------------------------

String GetTimers()
{
  digitalWrite(led, 0);
  Serial.print("Getting Values: ");

  if (server.args() > 0)
  {
    for (uint8_t i = 0; i < server.args(); i++)
    {
      if (server.argName(i) == "p1timeS")
      {
        p1timeS = server.arg(i);
      }
      if (server.argName(i) == "p1timeE")
      {
        p1timeE = server.arg(i);
      }
      if (server.argName(i) == "p2timeS")
      {
        p2timeS = server.arg(i);
      }
      if (server.argName(i) == "p2timeE")
      {
        p2timeE = server.arg(i);
      }
      if (server.argName(i) == "ktime")
      {
        ktime = server.arg(i);
      }
    }
  }
  String data = LogValues();
  Serial.println(data);
  SaveAllTimes();
  digitalWrite(led, 1);
  return data;
}

void handleTimeApi()
{
  Serial.println("Requset: /isma/api/time");
  String data = GetTimers();
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Max-Age", "10000");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST");
  server.sendHeader("Access-Control-Allow-Headers", "*");
  server.send(200, "text/plain", data);
}

void handleTime()
{
  Serial.println("Requset: /isma/time");
  GetTimers();
  backHome();
}

//-----Pins--------------------------

void checkP()
{
  //-----P1-----
  if (timeClient.getFormattedTime() > p1timeS + ":00" && timeClient.getFormattedTime() < p1timeS + ":10")
  {
    digitalWrite(led, 0);
    digitalWrite(p1, 0);
    vP1 = true;
    Serial.println("p1-ON");
  }
  if (timeClient.getFormattedTime() > p1timeE + ":00" && timeClient.getFormattedTime() < p1timeE + ":10")
  {
    digitalWrite(led, 0);
    digitalWrite(p1, 1);
    vP1 = false;
    Serial.println("p1-OFF");
  }

  //-----P2------
  if (timeClient.getFormattedTime() > p2timeS + ":00" && timeClient.getFormattedTime() < p2timeS + ":10")
  {
    digitalWrite(led, 0);
    digitalWrite(p2, 0);
    vP2 = true;
    Serial.println("p2-ON");
  }
  if (timeClient.getFormattedTime() > p2timeE + ":00" && timeClient.getFormattedTime() < p2timeE + ":10")
  {
    digitalWrite(led, 0);
    digitalWrite(p2, 1);
    vP2 = false;
    Serial.println("p2-OFF");
  }

  //-----Karm------
  if (timeClient.getFormattedTime() > ktime + ":00" && timeClient.getFormattedTime() < ktime + ":10")
  {
    karm();
    isKarm = false;
  }
  else
  {
    isKarm = true;
  }
}

void karm()
{
  if (isKarm)
  {
    karmie();
  }
}

void karmHtml()
{
  Serial.println("Requset: /isma/karm");
  karmie();
  backHome();
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

//----------------------------------------------------------------

void nopI()
{
  digitalWrite(led, 0);
  vP1 = true;
  Serial.println("p1-ON");
  digitalWrite(p1, 0);
  delay(100);
  digitalWrite(led, 1);
  backHome();
}

void nopII()
{
  digitalWrite(led, 0);
  vP2 = true;
  Serial.println("p2-ON");
  digitalWrite(p2, 0);
  delay(100);
  digitalWrite(led, 1);
  backHome();
}

void offpI()
{
  digitalWrite(led, 0);
  vP1 = false;
  Serial.println("p1-ON");
  digitalWrite(p1, 1);
  delay(100);
  digitalWrite(led, 1);
  backHome();
}

void offpII()
{
  digitalWrite(led, 0);
  vP2 = false;
  Serial.println("p2-ON");
  digitalWrite(p2, 1);
  delay(100);
  digitalWrite(led, 1);
  backHome();
}

void backHome()
{
  Serial.println("Redirect to: /isma/");
  server.sendHeader("Location", "/isma/", true);
  server.send(302, "text/plain", "");
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
  //adrres conunt = 20 ; end addres = 19
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

  //pins
  Serial.println();
  Serial.print("Pins Initialize...");
  pinMode(led, OUTPUT);
  pinMode(p1, OUTPUT);
  pinMode(p2, OUTPUT);

  digitalWrite(led, 1);
  digitalWrite(p1, 1);
  digitalWrite(p2, 1);
  Serial.println("ok");

  //servo
  Serial.print("Servo Initialize...");
  digitalWrite(led, 0);
  myservo.attach(karmPin);
  myservo.write(180);
  delay(750);
  myservo.detach();
  Serial.println("ok");
  digitalWrite(led, 1);
  delay(100);

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

  //WiFi Conecting
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

  //MDNS
  digitalWrite(led, 0);
  Serial.print("MDNS responder initialize...");
  if (MDNS.begin("isma"))
  {
    Serial.println("ok");
  }
  else
  {
    Serial.println("failed");
  }
  digitalWrite(led, 1);
  delay(100);

  //HTTP server
  digitalWrite(led, 0);
  Serial.print("HTTP server initialize...");
  server.begin();
  Serial.println("ok");
  digitalWrite(led, 1);
  delay(100);

  //Time server
  digitalWrite(led, 0);
  Serial.print("Time server initialize...");
  timeClient.begin();
  Serial.println("ok");
  digitalWrite(led, 1);
  delay(100);

  //EEPROM
  digitalWrite(led, 0);
  Serial.print("EEPROM initialize...");
  EEPROM.begin(24);
  Serial.println("ok");
  digitalWrite(led, 1);
  delay(100);

  //Read Times
  digitalWrite(led, 0);
  ReadAllTimes();
  digitalWrite(led, 1);
  delay(100);

  //IP
  digitalWrite(led, 0);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(led, 1);

  //Server
  digitalWrite(led, 0);
  Serial.print("Server endpoint initialize...");

  server.on("/", backHome);
  server.on("/isma/", handleRoot);
  server.on("/isma/time", handleTime);
  server.on("/isma/settime", handleSetTmie);
  server.on("/isma/karm", karmHtml);
  server.on("/isma/onp1", nopI);
  server.on("/isma/onp2", nopII);
  server.on("/isma/offp1", offpI);
  server.on("/isma/offp2", offpII);
  server.on("/isma/api", api);
  server.on("/isma/api/time", handleTimeApi);

  server.onNotFound(handleNotFound);
  Serial.println("ok");
  digitalWrite(led, 1);
}

void loop()
{
  vcc = (float)ESP.getVcc() / 1000;
  timeClient.update();

  checkP();

  Serial.print("# ");
  Serial.print(timeClient.getFormattedTime());
  Serial.println(" V=" + (String)vcc);

  server.handleClient();
  MDNS.update();

  delay(500);
  digitalWrite(led, 1);
}
