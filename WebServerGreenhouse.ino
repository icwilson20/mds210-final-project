/**
 *
 * Web Server for IoT Greenhouse
 * Author: Magnolia Maurer
 * Date: 2/1/2022
 * Description: 
 * Last-revised: 
 *
 */

#include "DHTesp.h" // Click here to get the library: http://librarymanager/All#DHTesp
#include <Arduino.h>
#include <heltec.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include <WebServer.h>

#define BAND    915E6  //you can set band here directly,e.g. 868E6,915E6

String outgoing;              // outgoing message

byte localAddress = 0xAB;     // address of this device
byte destination = 0xBC;      // destination to send to

byte msgCount = 0;            // count of outgoing messages
long lastSendTime = 0;        // last send time
int interval = 0;          // interval between sends

DHTesp dht;

// Network credentials
const char* ssid = "RHIT-OPEN";
const char* pass = "";
const char* userTemp = "temp";
String apiKey = "0d395ca59e5a4bd97ddaab6bb601594f";

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
double APITemperature;
int APIHumidity;
String content;

// Set web server port number to 80
//WiFiServer server(80);
WebServer server(80);
String header;
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
int humidity;

/** Comfort profile */
ComfortState cf;
/** Pin number for DHT11 data pin */
int dhtPin = 10;
const int ledPin = LED_BUILTIN;
int ledState = LOW; // LED initially off
int fan = 27;
int fanState = HIGH; //fan initially off
int hotTemp = 25; // initial temperature threshold
bool musicPlaying = false;

int getTemperature() {
  // Reading temperature for humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
  TempAndHumidity newValues = dht.getTempAndHumidity();
  // Check if any reads failed and exit early (to try again).
  if (dht.getStatus() != 0) {
    Serial.println("DHT11 error status: " + String(dht.getStatusString()));
    return false;
  }
  humidity = newValues.humidity;
  return newValues.temperature;
} // end getTemperature


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


void handleRoot() { //webpage for authorized users
  Serial.println("Enter handleRoot");

  String header;
  if (!is_authentified()) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  } else {
    String content ="<!DOCTYPE html><html>";
    content +=("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    content +=("<link rel=\"icon\" href=\"data:,\">");
    content +=("<script src=\"https://kit.fontawesome.com/a7900b886b.js\" crossorigin=\"anonymous\"></script>");
    // CSS to style the on/off buttons 
    // Feel free to change the background-color and font-size attributes to fit your preferences
    content +=("<style>html { font-family: Helvetica fantasy; font-size: 20px; display: inline-block; margin: 0px auto; text-align: center;}");
    content +=(".buttonWater { background-color: #1AB5F0; border: 6xp dotted solid double light blue; color: #0C0089; padding: 10px 20px;");
    content +=("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}");
    content +=(".buttonMusic {background-color: #AA7DE0;border: 6xp dotted solid double light blue; color: #3B0B48; padding: 10px 20px;");
    content +=("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}");
    content +=(".buttonWindow {background-color: #31CB7C; border: 6xp dotted solid double light blue; color: #05480B; padding: 10px 20px;");
    content +=("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}</style></head>");
              
              
    // Web Page Heading
    content +=("<body><h1>ESP32 Web Server for IoT Greenhouse</h1>");
//    if(APITemperature > getTemperature()){
//      content +=("<i class=\"fas fa-temperature-high\"></i>");
//    } else {
//      content +=("<i class=\"fas fa-temperature-low\"></i>");
//    }

    if(musicPlaying == true){
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
    content +=("<i class=\"fab fa-canadian-maple-leaf\"></i>"); //leaf
    content += ("<i class=\"fas fa-sun\"></i>");
    content += ("<i class=\"fas fa-holly-berry\"></i>"); //holly
    content += ("<i class=\"fas fa-rainbow\"></i>");
    content += ("<i class=\"fas fa-leaf\"></i>"); //leaf
    content += ("<i class=\"fab fa-pagelines\"></i>"); //leaf
    content += ("<i class=\"fas fa-seedling\"></i>");
    content += ("<i class=\"fas fa-wind\"></i>");
    content += ("<i class=\"fas fa-tree\"></i>");
    content += ("<i class=\"fas fa-cloud-sun\"></i>");
    
    content +=("<i class=\"fab fa-canadian-maple-leaf\"></i>"); //leaf
    content += ("<i class=\"fas fa-sun\"></i>");
    content += ("<i class=\"fas fa-holly-berry\"></i>"); //holly
    content += ("<i class=\"fas fa-rainbow\"></i>");
    content += ("<i class=\"fas fa-leaf\"></i>"); //leaf
    content += ("<i class=\"fab fa-pagelines\"></i>"); //leaf
    content += ("<i class=\"fas fa-seedling\"></i>");
    content += ("<i class=\"fas fa-wind\"></i>");
    content += ("<i class=\"fas fa-tree\"></i>");
    content += ("<i class=\"fas fa-cloud-sun\"></i>");
    
    content +=("<i class=\"fab fa-canadian-maple-leaf\"></i>"); //leaf
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
    
    content +=("<p1><br>   </p1>");

    content +=("<p2><br>Greenhouse Temperature: </p2>"); //print current Temp value
    content +=(getTemperature());
    
    content +=("<p3><br>Greenhouse Humidity: </p3>"); //print current humidity value
    content +=(humidity);

    content +=("<p4><br>Greenhouse Pressure: </p4>");



    content +=("<p5><br>Current Outside Temperature: </p5>");
    content +=(APITemperature);

    content +=("<p6><br>Current Outside Humidity: </p6>");
    content +=(APIHumidity);

    content += ("<p7><br>Current Outside Pressure: </p7>");
    content += (APIPressure);

    content += ("<p8><br>Soil Moisture Level: </p8>");

    content += ("<p9><br>Plants being watered? </p9>");

    content += ("<p10><br>Window open? </p10>");

    content += ("<p><a href=\"/\"><button class=\"button buttonMusic\">Sing to the Plants</button></a></p>");
    content += ("<p><a href=\"/\"><button class=\"button buttonWindow\">Open the Window</button></a></p>");
    content += ("<p><a href=\"/\"><button class=\"button buttonWater\">Water the Plants</button></a></p>");



//    content += "<form><label for=\"temp\">Enter new temp:</label><br><input type=\"text\" id=\"temp\" name=\"temp\"><br></form>";
    content += "<p11>You can access this page until you <a href=\"/login?DISCONNECT=YES\">disconnect</a></p11></body></html>";

  

//    if(server.hasArg("temp")){ //get input from user and assign it to hotTemp (temperature threshold)
//      String newTemp = server.arg("temp");
//      Serial.print("The new temp is ");
//      Serial.println(newTemp);
//      hotTemp = newTemp.toInt();
//      server.sendHeader("Location", "/");
//    } // end user input;

      //***********************************need to actually get user input

                    
    server.send(200, "text/html", content); //display everything

}
  
} // end handleRoot



void updateWeather() { //update weather from API
  // if you get a connection, report back via serial:
  if (client.connect(APIserver, 80)) {
    // Make a HTTP request:
    client.print("GET " + resource + "?");   // GET request
    lastSendTime = millis();
    interval = 180000;

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
    APITemperature = (APITemperature-32.0)*(5.0/9.0);  //convert to celsius
    APIHumidity = myObject["current"]["humidity"];
    APIPressure = myObject["current"]["pressure"];
  }
} //end updateWeater



void setup() {

  Serial.begin(115200);
  dht.setup(dhtPin, DHTesp::DHT11); //setting up temperature sensor
  
  pinMode(ledPin, OUTPUT); //LED is an output
  pinMode(fan, OUTPUT); // fan is an output

  Heltec.begin(false /*Display Enable*/, false /*LoRa Enable*/, true /*Serial Enable*/); 

  digitalWrite(LED_BUILTIN, LOW);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(1000);
  
  server.on("/", handleRoot);
  server.on("/login", handleLogin);

  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent", "Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize);
  server.begin();
  Serial.println("HTTP server started");

} //end setup

void loop() {
  if(millis() - lastSendTime > interval){
      updateWeather();
  }

  server.handleClient();

   digitalWrite(fan, fanState); //assigns the fan the fanState
  int temp = getTemperature(); //reads the temperature
  delay(1000);
  if(temp < hotTemp){ //determines if its colder then threshold, fan off
    fanState = LOW;
  } else {
    fanState = HIGH;
  }

  delay (1000);

} //end loop


void handleLogin() { //webpage for unauthorized users

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
    String content ="<!DOCTYPE html><html>";
  
    content += "<form action='/login' method='POST'>Please log in to access Temperature Control<br>";
    content += "User:<input type='text' name='USERNAME' placeholder='user name'><br>";
    content += "Password:<input type='password' name='PASSWORD' placeholder='password'><br>";
    content += "<input type='submit' name='SUBMIT' value='Submit'></form><br>";

    content +=("</body></html>");

    server.send(200, "text/html", content);

    
        } //end handleLogin
         
