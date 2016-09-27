

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <WiFiManager.h>

#include <RCSwitch.h>
#ifdef ESP8266
extern "C" {
#include "user_interface.h"
}
#endif
RCSwitch mySwitch = RCSwitch();

MDNSResponder mdns;

// Replace with your network credentials
//const char* ssid = "";
//const char* password = "";

char* myHostname = "RCSwitch_ESP8266";

ESP8266WebServer server(80);

// replace with your values
char* housecode[] = {"10101", "10101", "10101"};
char* socketcodes[] = {"00001", "00010", "00100"};
char* socketnames[] = {"Subwoofer", "SchlafzimmerTV", "LampeWZ"};
int numofsockets = sizeof(socketcodes)/4;

// you can write your own css and html code (head) here
String css = "body {background-color:#ffffff; color: #000000; font-family: 'Century Gothic', CenturyGothic, AppleGothic, sans-serif;}h1 {font-size: 2em;}a{font-family: Century Gothic, CenturyGothic, AppleGothic, sans-serif; font-size: 2em; text-decoration: none; background-color: #EEEEEE; color: #333333; padding: 2px 6px 2px 6px; border-top: 1px solid #CCCCCC; border-right: 1px solid #333333; border-bottom: 1px solid #333333; border-left: 1px solid #CCCCCC;} p{ font-family: Century Gothic, CenturyGothic, AppleGothic, sans-serif; font-size: 2em;}";
String head1 = "<!DOCTYPE html> <html> <head> <meta name=\"viewport\" content=\"initial-scale=1.0, minimum-scale=1.0, maximum-scale=1.0, user-scalable=no\"> <title>Steckdosensteuerung</title> <style>";
String head2 = "</style></head><body><center>";
String header = head1 + css + head2;
String body = "";
String website(String h, String b){
  String complete = h+b;
  return complete;
}

void setup(void){
  WiFiManager wifiManager;
  // if you want to modify body part of html start here
  body = "<h1>Steckdosensteuerung</h1>";
  // socket names and buttons are created dynamical
  for(int i = 0; i < numofsockets; i++){
    String namesocket = socketnames[i];
    body = body + "<p>" + namesocket + " </p> <a href=\"socket" + String(i) + "On\">AN</a>&nbsp;<a href=\"socket" + String(i) + "Off\">AUS</a>";
  }
  body += "</center></body>";
  mySwitch.enableTransmit(2);
  delay(1000);
  Serial.begin(115200);
  wifi_station_set_hostname(myHostname);
  //WiFi.begin(ssid, password);
  wifiManager.autoConnect();
  Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // serial output of connection details
  Serial.println("");
  Serial.print("Connected");
  //Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  #ifdef ESP8266
  Serial.print("wifi_station_get_hostname: ");
  Serial.println(wifi_station_get_hostname());
  #endif
  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  // this page is loaded when accessing the root of esp8266´s IP
  server.on("/", [](){
    String webPage = website(header, body);
    server.send(200, "text/html", webPage);
  });
  // pages for all your sockets are created dynamical
  for(int i = 0; i < numofsockets; i++){
    String pathOn = "/socket"+String(i)+"On";
    const char* pathOnChar = pathOn.c_str();
    String pathOff = "/socket"+String(i)+"Off";
    const char* pathOffChar = pathOff.c_str();
    
    server.on(pathOnChar, [i](){
      String webPage = website(header, body);
      server.send(200, "text/html", webPage);
      mySwitch.switchOn(housecode[i], socketcodes[i]);
      delay(1000);
    });
    
    server.on(pathOffChar, [i](){
      String webPage = website(header, body);
      server.send(200, "text/html", webPage);
      mySwitch.switchOff(housecode[i], socketcodes[i]);
      delay(1000);
    });
    
  }
  server.begin();
  Serial.println("HTTP server started");
}
void loop(void){
  server.handleClient();
}
