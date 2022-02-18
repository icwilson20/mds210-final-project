/**

   Web Server for IoT Greenhouse
   Author: Magnolia Maurer
   Date: 2/1/2022
   Description:
   Last-revised:

*/

#include "DHTesp.h" // Click here to get the library: http://librarymanager/All#DHTesp
#include <Arduino.h>
#include "heltec.h"

#include <WiFi.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include <WebServer.h>
#include <stdio.h>
#include <string.h>
#include "arduino_secrets.h"


#define BAND    915E6  //you can set band here directly,e.g. 868E6,915E6

String outgoing;              // outgoing message

byte localAddress = 0xAB;     // address of this device
byte destination = 0xBC;      // destination to send to
String myPlantData[5];        //data received from device

byte msgCount = 0;            // count of outgoing messages
long lastSendTime = 0;        // last send time
int interval = 180000;        // interval between sends

bool waterOn = false;
String water_plants = "0";
bool turnOnMusic = false;
String play_song = "1";
bool openWindow = false;
String open_window = "2";
bool closeWindow = false;
String close_window = "3";

bool firsttime = false; //prevents re-watering on autorefresh

// Network credentials
char ssid[] = SECRET_SSID;  //  your network NAME
char pass[] = SECRET_PASS;  //  your network PASSWORD

String apiKey = SECRET_APIKEY; // open weather map API KEY

// resource and parameter values for GET request
String resource = "/data/2.5/onecall";  // One Call API
String resource2 = "/geo/1.0/reverse";  // Reverse Geocoding API
String latitud = "39.48349985077955";   // cooredinates for Moench hall
String longitud = "-87.3236898594648";
/* comma-delimited list (without spaces) */
String excludeList = "";    // options: current,minutely,hourly,daily,alerts
String units = "imperial";  // options: standard (K), metric (C) and imperial (F)
String limit = "5";
int APIPressure;

char APIserver[] = "api.openweathermap.org";

WiFiClient client;
WebServer server(80);
double APITemperature;
int APIHumidity;
String content;

// Set web server port number to 80
String header;
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
const long oneminute = 60000;
int numminutes = 0;
int previousAPImillis = 0;
int humidity;

bool musicPlaying = false;

//Check if header is present and correct
bool is_authentified() {
  Serial.println("Enter is_authentified");
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Authentification Successful");
      return true;
    }
  }
  Serial.println("Authentification Failed");
  return false;
} // end is_authentified

void wateringPlants() {
  //updateWeather();
  Serial.println("Enter wateringPlants");
  if (firsttime) {
    firsttime = false;
    waterOn = true;
  } else {
    waterOn = false;
  }

  String header;
  if (!is_authentified()) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  } else {
    String content = "<!DOCTYPE html><html>";
    content += ("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    //content += "<meta http-equiv=\"refresh\" content=\"30\">";
    content += ("<link rel=\"icon\" href=\"data:,\">");
    content += ("<script src=\"https://kit.fontawesome.com/a7900b886b.js\" crossorigin=\"anonymous\"></script>");
    // CSS to style the on/off buttons
    // Feel free to change the background-color and font-size attributes to fit your preferences
    content += ("<style>html { font-family: Arial fantasy; font-size: 20px; display: inline-block; margin: 0px auto; text-align: center;}");
    content += (".buttonWater { background-color: #1AB5F0; border: 6xp dotted solid double light blue; color: #0C0089; padding: 10px 20px;");
    content += ("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}");
    content += (".buttonMusic {background-color: #AA7DE0;border: 6xp dotted solid double light blue; color: #3B0B48; padding: 10px 20px;");
    content += ("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}");
    content += (".buttonWindow {background-color: #31CB7C; border: 6xp dotted solid double light blue; color: #05480B; padding: 10px 20px;");
    content += ("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}</style></head>");


    // Web Page Heading
    content += ("<body><h1>ESP32 Web Server for IoT Greenhouse</h1>");
    //    if(APITemperature > getTemperature()){
    //      content +=("<i class=\"fas fa-temperature-high\"></i>");
    //    } else {
    //      content +=("<i class=\"fas fa-temperature-low\"></i>");
    //    }

    content += ("<i class=\"fab fa-canadian-maple-leaf\"></i>"); //leaf
    content += ("<i class=\"fas fa-sun\"></i>");
    content += ("<i class=\"fas fa-holly-berry\"></i>"); //holly
    content += ("<i class=\"fas fa-rainbow\"></i>");
    content += ("<i class=\"fas fa-leaf\"></i>"); //leaf
    content += ("<i class=\"fab fa-pagelines\"></i>"); //leaf
    content += ("<i class=\"fas fa-seedling\"></i>");
    content += ("<i class=\"fas fa-wind\"></i>");
    content += ("<i class=\"fas fa-tree\"></i>");
    content += ("<i class=\"fas fa-cloud-sun\"></i>");

    content += ("<i class=\"fab fa-canadian-maple-leaf\"></i>"); //leaf
    content += ("<i class=\"fas fa-sun\"></i>");
    content += ("<i class=\"fas fa-holly-berry\"></i>"); //holly
    content += ("<i class=\"fas fa-rainbow\"></i>");
    content += ("<i class=\"fas fa-leaf\"></i>"); //leaf
    content += ("<i class=\"fab fa-pagelines\"></i>"); //leaf
    content += ("<i class=\"fas fa-seedling\"></i>");
    content += ("<i class=\"fas fa-wind\"></i>");
    content += ("<i class=\"fas fa-tree\"></i>");
    content += ("<i class=\"fas fa-cloud-sun\"></i>");

    content += ("<i class=\"fab fa-canadian-maple-leaf\"></i>"); //leaf
    content += ("<i class=\"fas fa-sun\"></i>");
    content += ("<i class=\"fas fa-holly-berry\"></i>"); //holly
    content += ("<i class=\"fas fa-rainbow\"></i>");
    content += ("<i class=\"fas fa-leaf\"></i>"); //leaf
    content += ("<i class=\"fab fa-pagelines\"></i>"); //leaf
    content += ("<i class=\"fas fa-seedling\"></i>");
    content += ("<i class=\"fas fa-wind\"></i>");
    content += ("<i class=\"fas fa-tree\"></i>");
    content += ("<i class=\"fas fa-cloud-sun\"></i>");

    content += ("<p1><br>   </p1>");
    content += addGreenhouseData();

    content += ("<p5><br>Current Outside Temperature: ");
    content += (APITemperature);
    content += "*C </p6>";

    content += ("<p6><br>Current Outside Humidity: ");
    content += (APIHumidity);
    content += ("%</p7>");

    content += ("<p7><br>Current Outside Pressure: ");
    content += (APIPressure);
    content += " hPa</p8>";

    content += ("<p><a href=\"/music/on\"><button class=\"button buttonMusic\">Sing to the Plants</button></a></p>");
    content += ("<p><a href=\"/window/open\"><button class=\"button buttonWindow\">Open the Window</button></a></p>");
    content += ("<p><a href=\"/\"><button class=\"button buttonWater\">The plants were watered!</button></a></p>");

    content += "<p11>You can access this page until you <a href=\"/login?DISCONNECT=YES\">disconnect</a></p11></body></html>";

    server.send(200, "text/html", content); //display everything

  }

} // end wateringPlants

void windowOpen() {
  //updateWeather();
  Serial.println("Enter windowOpen");
  openWindow = true;

  String header;
  if (!is_authentified()) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  } else {
    String content = "<!DOCTYPE html><html>";
    content += ("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    content += "<meta http-equiv=\"refresh\" content=\"30\">";
    content += ("<link rel=\"icon\" href=\"data:,\">");
    content += ("<script src=\"https://kit.fontawesome.com/a7900b886b.js\" crossorigin=\"anonymous\"></script>");
    // CSS to style the on/off buttons
    // Feel free to change the background-color and font-size attributes to fit your preferences
    content += ("<style>html { font-family: Arial fantasy; font-size: 20px; display: inline-block; margin: 0px auto; text-align: center;}");
    content += (".buttonWater { background-color: #1AB5F0; border: 6xp dotted solid double light blue; color: #0C0089; padding: 10px 20px;");
    content += ("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}");
    content += (".buttonMusic {background-color: #AA7DE0;border: 6xp dotted solid double light blue; color: #3B0B48; padding: 10px 20px;");
    content += ("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}");
    content += (".buttonWindow {background-color: #31CB7C; border: 6xp dotted solid double light blue; color: #05480B; padding: 10px 20px;");
    content += ("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}</style></head>");

    // Web Page Heading
    content += ("<body><h1>ESP32 Web Server for IoT Greenhouse</h1>");

    content += ("<i class=\"fab fa-canadian-maple-leaf\"></i>"); //leaf
    content += ("<i class=\"fas fa-sun\"></i>");
    content += ("<i class=\"fas fa-holly-berry\"></i>"); //holly
    content += ("<i class=\"fas fa-rainbow\"></i>");
    content += ("<i class=\"fas fa-leaf\"></i>"); //leaf
    content += ("<i class=\"fab fa-pagelines\"></i>"); //leaf
    content += ("<i class=\"fas fa-seedling\"></i>");
    content += ("<i class=\"fas fa-wind\"></i>");
    content += ("<i class=\"fas fa-tree\"></i>");
    content += ("<i class=\"fas fa-cloud-sun\"></i>");

    content += ("<i class=\"fab fa-canadian-maple-leaf\"></i>"); //leaf
    content += ("<i class=\"fas fa-sun\"></i>");
    content += ("<i class=\"fas fa-holly-berry\"></i>"); //holly
    content += ("<i class=\"fas fa-rainbow\"></i>");
    content += ("<i class=\"fas fa-leaf\"></i>"); //leaf
    content += ("<i class=\"fab fa-pagelines\"></i>"); //leaf
    content += ("<i class=\"fas fa-seedling\"></i>");
    content += ("<i class=\"fas fa-wind\"></i>");
    content += ("<i class=\"fas fa-tree\"></i>");
    content += ("<i class=\"fas fa-cloud-sun\"></i>");

    content += ("<i class=\"fab fa-canadian-maple-leaf\"></i>"); //leaf
    content += ("<i class=\"fas fa-sun\"></i>");
    content += ("<i class=\"fas fa-holly-berry\"></i>"); //holly
    content += ("<i class=\"fas fa-rainbow\"></i>");
    content += ("<i class=\"fas fa-leaf\"></i>"); //leaf
    content += ("<i class=\"fab fa-pagelines\"></i>"); //leaf
    content += ("<i class=\"fas fa-seedling\"></i>");
    content += ("<i class=\"fas fa-wind\"></i>");
    content += ("<i class=\"fas fa-tree\"></i>");
    content += ("<i class=\"fas fa-cloud-sun\"></i>");

    content += ("<p1><br>   </p1>");
    content += addGreenhouseData();

    content += ("<p5><br>Current Outside Temperature: ");
    content += (APITemperature);
    content += "*C </p5>";

    content += ("<p6><br>Current Outside Humidity: ");
    content += (APIHumidity);
    content += ("%</p6>");

    content += ("<p7><br>Current Outside Pressure: ");
    content += (APIPressure);
    content += " hPa</p8>";

    content += ("<p><a href=\"/music/on\"><button class=\"button buttonMusic\">Sing to the Plants</button></a></p>");
    content += ("<p><a href=\"/\"><button class=\"button buttonWindow\">Close the Window</button></a></p>");
    content += ("<p><a href=\"/watering/plants\"><button class=\"button buttonWater\">Water the Plants</button></a></p>");

    content += "<p11>You can access this page until you <a href=\"/login?DISCONNECT=YES\">disconnect</a></p11></body></html>";

    server.send(200, "text/html", content); //display everything

  }

} // end windowOpen


void musicOn() {
  //updateWeather();
  Serial.println("Enter musicOn");
  turnOnMusic = true;
  firsttime = true;

  String header;
  if (!is_authentified()) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  } else {
    String content = "<!DOCTYPE html><html>";
    content += ("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    content += "<meta http-equiv=\"refresh\" content=\"30\">";
    content += ("<link rel=\"icon\" href=\"data:,\">");
    content += ("<script src=\"https://kit.fontawesome.com/a7900b886b.js\" crossorigin=\"anonymous\"></script>");
    // CSS to style the on/off buttons
    // Feel free to change the background-color and font-size attributes to fit your preferences
    content += ("<style>html { font-family: Arial fantasy; font-size: 20px; display: inline-block; margin: 0px auto; text-align: center;}");
    content += (".buttonWater { background-color: #1AB5F0; border: 6xp dotted solid double light blue; color: #0C0089; padding: 10px 20px;");
    content += ("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}");
    content += (".buttonMusic {background-color: #AA7DE0;border: 6xp dotted solid double light blue; color: #3B0B48; padding: 10px 20px;");
    content += ("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}");
    content += (".buttonWindow {background-color: #31CB7C; border: 6xp dotted solid double light blue; color: #05480B; padding: 10px 20px;");
    content += ("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}</style></head>");


    // Web Page Heading
    content += ("<body><h1>ESP32 Web Server for IoT Greenhouse</h1>");

    content += ("<i class=\"fas fa-music\"></i>"); //music note
    content += ("<i class=\"fas fa-volume-up\"></i>");
    content += ("<i class=\"fas fa-guitar\"></i>");
    content += ("<i class=\"fas fa-headphones-alt\"></i>");
    content += ("<i class=\"fas fa-sliders-h\"></i>");
    content += ("<i class=\"fas fa-drum\"></i>");
    content += ("<i class=\"fas fa-microphone-alt\"></i>");

    content += ("<i class=\"fas fa-music\"></i>"); //repeat
    content += ("<i class=\"fas fa-volume-up\"></i>");
    content += ("<i class=\"fas fa-guitar\"></i>");
    content += ("<i class=\"fas fa-headphones-alt\"></i>");
    content += ("<i class=\"fas fa-sliders-h\"></i>");
    content += ("<i class=\"fas fa-drum\"></i>");
    content += ("<i class=\"fas fa-microphone-alt\"></i>");

    content += ("<i class=\"fas fa-music\"></i>"); //repeat
    content += ("<i class=\"fas fa-volume-up\"></i>");
    content += ("<i class=\"fas fa-guitar\"></i>");
    content += ("<i class=\"fas fa-headphones-alt\"></i>");
    content += ("<i class=\"fas fa-sliders-h\"></i>");
    content += ("<i class=\"fas fa-drum\"></i>");
    content += ("<i class=\"fas fa-microphone-alt\"></i>");

    content += ("<i class=\"fas fa-music\"></i>"); //repeat
    content += ("<i class=\"fas fa-volume-up\"></i>");
    content += ("<i class=\"fas fa-guitar\"></i>");
    content += ("<i class=\"fas fa-headphones-alt\"></i>");
    content += ("<i class=\"fas fa-sliders-h\"></i>");
    content += ("<i class=\"fas fa-drum\"></i>");
    content += ("<i class=\"fas fa-microphone-alt\"></i>");


    content += ("<p1><br>   </p1>");
    content += addGreenhouseData();

    content += ("<p5><br>Current Outside Temperature: ");
    content += (APITemperature);
    content += "*C </p5>";

    content += ("<p6><br>Current Outside Humidity: ");
    content += (APIHumidity);
    content += ("%</p6>");

    content += ("<p7><br>Current Outside Pressure: ");
    content += (APIPressure);
    content += " hPa</p8>";

    content += ("<p><a href=\"/\"><button class=\"button buttonMusic\">Stop Playing Music</button></a></p>");
    content += ("<p><a href=\"/window/open\"><button class=\"button buttonWindow\">Open the Window</button></a></p>");
    content += ("<p><a href=\"/watering/plants\"><button class=\"button buttonWater\">Water the Plants</button></a></p>");

    content += "<p11>You can access this page until you <a href=\"/login?DISCONNECT=YES\">disconnect</a></p11></body></html>";

    server.send(200, "text/html", content); //display everything

  }

} // end musicOn


void handleRoot() { //webpage for authorized users
  //updateWeather();
  Serial.println("Enter handleRoot");
  waterOn = false;
  turnOnMusic = false;
  openWindow = false;
  closeWindow = true;
  firsttime = true;

  String header;
  if (!is_authentified()) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  } else {
    String content = "<!DOCTYPE html><html>";
    content += ("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    content += "<meta http-equiv=\"refresh\" content=\"30\">";
    content += ("<link rel=\"icon\" href=\"data:,\">");
    content += ("<script src=\"https://kit.fontawesome.com/a7900b886b.js\" crossorigin=\"anonymous\"></script>");
    // CSS to style the on/off buttons
    // Feel free to change the background-color and font-size attributes to fit your preferences
    content += ("<style>html { font-family: Arial fantasy; font-size: 20px; display: inline-block; margin: 0px auto; text-align: center;}");
    content += (".buttonWater { background-color: #1AB5F0; border: 6xp dotted solid double light blue; color: #0C0089; padding: 10px 20px;");
    content += ("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}");
    content += (".buttonMusic {background-color: #AA7DE0;border: 6xp dotted solid double light blue; color: #3B0B48; padding: 10px 20px;");
    content += ("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}");
    content += (".buttonWindow {background-color: #31CB7C; border: 6xp dotted solid double light blue; color: #05480B; padding: 10px 20px;");
    content += ("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}</style></head>");


    // Web Page Heading
    content += ("<body><h1>ESP32 Web Server for IoT Greenhouse</h1>");

    content += ("<i class=\"fab fa-canadian-maple-leaf\"></i>"); //leaf
    content += ("<i class=\"fas fa-sun\"></i>");
    content += ("<i class=\"fas fa-holly-berry\"></i>"); //holly
    content += ("<i class=\"fas fa-rainbow\"></i>");
    content += ("<i class=\"fas fa-leaf\"></i>"); //leaf
    content += ("<i class=\"fab fa-pagelines\"></i>"); //leaf
    content += ("<i class=\"fas fa-seedling\"></i>");
    content += ("<i class=\"fas fa-wind\"></i>");
    content += ("<i class=\"fas fa-tree\"></i>");
    content += ("<i class=\"fas fa-cloud-sun\"></i>");

    content += ("<i class=\"fab fa-canadian-maple-leaf\"></i>"); //leaf
    content += ("<i class=\"fas fa-sun\"></i>");
    content += ("<i class=\"fas fa-holly-berry\"></i>"); //holly
    content += ("<i class=\"fas fa-rainbow\"></i>");
    content += ("<i class=\"fas fa-leaf\"></i>"); //leaf
    content += ("<i class=\"fab fa-pagelines\"></i>"); //leaf
    content += ("<i class=\"fas fa-seedling\"></i>");
    content += ("<i class=\"fas fa-wind\"></i>");
    content += ("<i class=\"fas fa-tree\"></i>");
    content += ("<i class=\"fas fa-cloud-sun\"></i>");

    content += ("<i class=\"fab fa-canadian-maple-leaf\"></i>"); //leaf
    content += ("<i class=\"fas fa-sun\"></i>");
    content += ("<i class=\"fas fa-holly-berry\"></i>"); //holly
    content += ("<i class=\"fas fa-rainbow\"></i>");
    content += ("<i class=\"fas fa-leaf\"></i>"); //leaf
    content += ("<i class=\"fab fa-pagelines\"></i>"); //leaf
    content += ("<i class=\"fas fa-seedling\"></i>");
    content += ("<i class=\"fas fa-wind\"></i>");
    content += ("<i class=\"fas fa-tree\"></i>");
    content += ("<i class=\"fas fa-cloud-sun\"></i>");

    content += ("<p1><br>   </p1>");
    content += addGreenhouseData();

    content += ("<p5><br>Current Outside Temperature: ");
    content += (APITemperature);
    content += "*C </p5>";

    content += ("<p6><br>Current Outside Humidity: </p6>");
    content += (APIHumidity);
    content += ("%</p6>");

    content += ("<p7><br>Current Outside Pressure: ");
    content += (APIPressure);
    content += " hPa</p8>";

    content += ("<p><a href=\"/music/on\"><button class=\"button buttonMusic\">Sing to the Plants</button></a></p>");
    content += ("<p><a href=\"/window/open\"><button class=\"button buttonWindow\">Open the Window</button></a></p>");
    content += ("<p><a href=\"/watering/plants\"><button class=\"button buttonWater\">Water the Plants</button></a></p>");

    content += "<p11>You can access this page until you <a href=\"/login?DISCONNECT=YES\">disconnect</a></p11></body></html>";

    server.send(200, "text/html", content); //display everything

  }

} // end handleRoot


void updateWeather() { //update weather from API
  // if you get a connection, report back via serial:
  if (client.connect(APIserver, 80)) {
    // Make a HTTP request:
    client.print("GET " + resource + "?");   // GET request

    // List of name-value pair paramaters, separated by &
    client.print("lat=" + latitud);
    client.print("&lon=" + longitud);

    client.print("&appid=" + apiKey);
    //    client.print("&exclude=" + excludeList);
    client.println("&units=" + units);
    client.println("Host: api.openweathermap.org");
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("unable to connect");
  }
  delay(1000);

  String line = "";
  if (client.connected()) {
    line = client.readStringUntil('\n');
    JSONVar myObject = JSON.parse(line);

    APITemperature = myObject["current"]["temp"];
    APITemperature = (APITemperature - 32.0) * (5.0 / 9.0); //convert to celsius
    APIHumidity = myObject["current"]["humidity"];
    APIPressure = myObject["current"]["pressure"];
  }
} //end updateWeater


void setup() {
  int counter = 0;
  Serial.begin(115200);

  Heltec.begin(false /*Display Enable*/, true /*LoRa Enable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  Serial.println("Heltec.LoRa Duplex");
  LoRa.setSpreadingFactor(8);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    counter++;
    if (counter >= 60) { //after 30 seconds timeout - reset board
      ESP.restart();
    }
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(1000);

  updateWeather();

  server.on("/", handleRoot);
  server.on("/login", handleLogin);
  server.on("/music/on", musicOn);
  server.on("/watering/plants", wateringPlants);
  server.on("/window/open", windowOpen);

  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent", "Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize);
  server.begin();
  Serial.println("HTTP server started");

} //end setup

void loop() {

  if (waterOn) {
    waterOn = false;
    String message = ""; //send water on
    sendMessage(water_plants);
    Serial.println("Sending " + message);
  }
  if (turnOnMusic) {
    turnOnMusic = false;
    String message = "Turn on music";
    sendMessage(play_song);
    Serial.println("Sending " + message);
  }
  if (openWindow) {
    openWindow = false;
    String message = "Open window";
    sendMessage(open_window);
    Serial.println("Sending " + message);
  }
  if (closeWindow) {
    closeWindow = false;
    String message = "Close window";
    sendMessage(close_window);
    Serial.println("Sending " + message);
  }

  if (currentTime - previousAPImillis >= oneminute) {
    previousAPImillis = currentTime;
    numminutes++;
    if (numminutes == 60) {
      numminutes = 0;
      updateWeather();
    }
  }

  onReceive(LoRa.parsePacket());

  server.handleClient();

  if (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print("wifi disconnected");
      ESP.restart();
    }

  }

} //end loop


void handleLogin() { //webpage for unauthorized users
  //updateWeather();
  turnOnMusic = false;
  waterOn = false;
  openWindow = false;
  closeWindow = false;
  String msg;
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")) {
    Serial.println("Disconnection");
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "ESPSESSIONID=0");
    server.send(301);
    return;
  }
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")) {
    if (server.arg("USERNAME") == "mds210" &&  server.arg("PASSWORD") == "Rh1t2O22") {
      server.sendHeader("Location", "/");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
      server.send(301);
      Serial.println("Log in Successful");
      return;
    }
    msg = "Wrong username/password! try again.";
    Serial.println("Log in Failed");
  }

  //send HTML response
  // Display the HTML web page
  String content = "<!DOCTYPE html><html>";

  content += ("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  content += "<meta http-equiv=\"refresh\" content=\"30\">";
  content += ("<link rel=\"icon\" href=\"data:,\">");
  content += ("<script src=\"https://kit.fontawesome.com/a7900b886b.js\" crossorigin=\"anonymous\"></script>");
  // CSS to style the on/off buttons
  // Feel free to change the background-color and font-size attributes to fit your preferences
  content += ("<style>html { font-family: Helvetica fantasy; font-size: 20px; display: inline-block; margin: 0px auto; text-align: center;}");
  content += (".buttonWater { background-color: #1AB5F0; border: 6xp dotted solid double light blue; color: #0C0089; padding: 10px 20px;");
  content += ("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}");
  content += (".buttonMusic {background-color: #AA7DE0;border: 6xp dotted solid double light blue; color: #3B0B48; padding: 10px 20px;");
  content += ("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}");
  content += (".buttonWindow {background-color: #31CB7C; border: 6xp dotted solid double light blue; color: #05480B; padding: 10px 20px;");
  content += ("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}</style></head>");


  // Web Page Heading
  content += ("<body><h1>ESP32 Web Server for IoT Greenhouse</h1>");

  if (musicPlaying == true) {
    content += ("<i class=\"fas fa-music\"></i>"); //music note
    content += ("<i class=\"fas fa-volume-up\"></i>");
    content += ("<i class=\"fas fa-guitar\"></i>");
    content += ("<i class=\"fas fa-headphones-alt\"></i>");
    content += ("<i class=\"fas fa-sliders-h\"></i>");
    content += ("<i class=\"fas fa-drum\"></i>");
    content += ("<i class=\"fas fa-microphone-alt\"></i>");

    content += ("<i class=\"fas fa-music\"></i>"); //repeat
    content += ("<i class=\"fas fa-volume-up\"></i>");
    content += ("<i class=\"fas fa-guitar\"></i>");
    content += ("<i class=\"fas fa-headphones-alt\"></i>");
    content += ("<i class=\"fas fa-sliders-h\"></i>");
    content += ("<i class=\"fas fa-drum\"></i>");
    content += ("<i class=\"fas fa-microphone-alt\"></i>");

    content += ("<i class=\"fas fa-music\"></i>"); //repeat
    content += ("<i class=\"fas fa-volume-up\"></i>");
    content += ("<i class=\"fas fa-guitar\"></i>");
    content += ("<i class=\"fas fa-headphones-alt\"></i>");
    content += ("<i class=\"fas fa-sliders-h\"></i>");
    content += ("<i class=\"fas fa-drum\"></i>");
    content += ("<i class=\"fas fa-microphone-alt\"></i>");

    content += ("<i class=\"fas fa-music\"></i>"); //repeat
    content += ("<i class=\"fas fa-volume-up\"></i>");
    content += ("<i class=\"fas fa-guitar\"></i>");
    content += ("<i class=\"fas fa-headphones-alt\"></i>");
    content += ("<i class=\"fas fa-sliders-h\"></i>");
    content += ("<i class=\"fas fa-drum\"></i>");
    content += ("<i class=\"fas fa-microphone-alt\"></i>");

  } else {
    content += ("<i class=\"fab fa-canadian-maple-leaf\"></i>"); //leaf
    content += ("<i class=\"fas fa-sun\"></i>");
    content += ("<i class=\"fas fa-holly-berry\"></i>"); //holly
    content += ("<i class=\"fas fa-rainbow\"></i>");
    content += ("<i class=\"fas fa-leaf\"></i>"); //leaf
    content += ("<i class=\"fab fa-pagelines\"></i>"); //leaf
    content += ("<i class=\"fas fa-seedling\"></i>");
    content += ("<i class=\"fas fa-wind\"></i>");
    content += ("<i class=\"fas fa-tree\"></i>");
    content += ("<i class=\"fas fa-cloud-sun\"></i>");

    content += ("<i class=\"fab fa-canadian-maple-leaf\"></i>"); //leaf
    content += ("<i class=\"fas fa-sun\"></i>");
    content += ("<i class=\"fas fa-holly-berry\"></i>"); //holly
    content += ("<i class=\"fas fa-rainbow\"></i>");
    content += ("<i class=\"fas fa-leaf\"></i>"); //leaf
    content += ("<i class=\"fab fa-pagelines\"></i>"); //leaf
    content += ("<i class=\"fas fa-seedling\"></i>");
    content += ("<i class=\"fas fa-wind\"></i>");
    content += ("<i class=\"fas fa-tree\"></i>");
    content += ("<i class=\"fas fa-cloud-sun\"></i>");

    content += ("<i class=\"fab fa-canadian-maple-leaf\"></i>"); //leaf
    content += ("<i class=\"fas fa-sun\"></i>");
    content += ("<i class=\"fas fa-holly-berry\"></i>"); //holly
    content += ("<i class=\"fas fa-rainbow\"></i>");
    content += ("<i class=\"fas fa-leaf\"></i>"); //leaf
    content += ("<i class=\"fab fa-pagelines\"></i>"); //leaf
    content += ("<i class=\"fas fa-seedling\"></i>");
    content += ("<i class=\"fas fa-wind\"></i>");
    content += ("<i class=\"fas fa-tree\"></i>");
    content += ("<i class=\"fas fa-cloud-sun\"></i>");
  } //end icon prints

  content += ("<p1><br>   </p1>");
  content += addGreenhouseData();

  content += ("<p5><br>Current Outside Temperature: ");
  content += (APITemperature);
  content += "*C </p5>";

  content += ("<p6><br>Current Outside Humidity: ");
  content += (APIHumidity);
  content += "%</p6>";

  content += ("<p7><br>Current Outside Pressure: ");
  content += (APIPressure);
  content += " hPa</p8>";

  content += "<form action='/login' method='POST'>Please log in to access Temperature Control<br>";
  content += "User:<input type='text' name='USERNAME' placeholder='user name'><br>";
  content += "Password:<input type='password' name='PASSWORD' placeholder='password'><br>";
  content += "<input type='submit' name='SUBMIT' value='Submit'></form><br>";

  content += ("</body></html>");

  server.send(200, "text/html", content);

} //end handleLogin


void sendMessage(String outgoing)
{
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
}

void onReceive(int packetSize)
{
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length

  String incoming = "";

  while (LoRa.available())
  {
    incoming += (char)LoRa.read();
  }

  if (incomingLength != incoming.length())
  { // check length for error
    Serial.println("error: message length does not match length");
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
    Serial.println("This message is not for me.");
    return;                             // skip rest of function
  }

  // if message is for this device, or broadcast, print details:
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Sent to: 0x" + String(recipient, HEX));
  Serial.println("Message ID: " + String(incomingMsgId));
  Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();

  char buffer[50];
  incoming.toCharArray(buffer, 50);
  //char myPlantData[5];
  String remaining = incoming;
  int comma = remaining.indexOf(',');
  int i = 0;
  while (comma != -1) {
    myPlantData[i] = remaining.substring(0, comma);
    remaining = remaining.substring(comma + 1);
    comma = remaining.indexOf(',');
    i++;
  }
  myPlantData[i] = remaining;
  Serial.println(incoming);
  Serial.println("Temp is: " + myPlantData[0] );
  Serial.println("Humidity is: " + myPlantData[1]);
  Serial.println("Moisture is: " + myPlantData[2]);
  Serial.println("Pressure is: " + myPlantData[3]);
  Serial.println("Altutude is: " + myPlantData[4]);

  //temp, humidity, moisture, pressure, altitude
} //end onRecieve


String addGreenhouseData() {
  String lines;
  lines += ("<p1><br> Location: Terre Haute, Indiana</p1>");
  lines += ("<p2><br>Greenhouse Temperature: "); //print current Temp value
  lines += (String)myPlantData[0];
  lines += "*C </p2>";

  lines += ("<p3><br>Greenhouse Humidity: </p3>"); //print current humidity value
  lines += (String)myPlantData[1];

  lines += ("<p4><br>Soil Moisture Level: </p4>");
  lines += (String)myPlantData[2];

  lines += ("<p5><br>Greenhouse Pressure: ");
  lines += (String)myPlantData[3];
  lines += " hPa </p5>";

  lines += ("<p6><br>Greenhouse Altitude: ");
  lines += (String)myPlantData[4];
  lines += " m</p6>";

  return lines;

}
