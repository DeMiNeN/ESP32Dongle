#include <Wire.h>
#include <SSD1306Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

SSD1306Wire display(0x3c, 26, 25);

const int hallpin = 32;  //OLED Display

int swPin = 33;

int count = 0; //magnet sensor
int lastHallState = HIGH; //magnet sensor

bool state = 0; //LINE

// WiFi Setting
const char* ssid     = "xxxxxxxxxxx";
const char* password = "xxxxxxxxxxx";

// LINE Notify Setting
const char* host = "xxxxxxxxxxx";
const char* token = "xxxxxxxxxxx"; //Your API key
const char* message = "薬を飲みました！"; //メッセージの内容 

void setup() {
  Serial.begin(115200);    //magnet sensor
  pinMode(hallpin, INPUT); //magnet sensor
  display.init();
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 0, "Count: " + String(count));
  display.display();

  pinMode(swPin,INPUT);
  connectWiFi();
}

void loop() {
  int hallState = digitalRead(hallpin);

     if (hallState == LOW && lastHallState == HIGH) {
       count++;
       updateDisplay();

      state = digitalRead(swPin);

        if (state) {            //もしくは(state == 1)。stateは０だったら(!state)を書く
         send_line();
        }
  }

  lastHallState = hallState;

}

void updateDisplay() {
  display.clear();
  display.drawString(0, 0, "Count: " + String(count));
  display.display();
}

void connectWiFi(){
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  //Start WiFi connection
  WiFi.begin(ssid, password);

  //Check WiFi connection status
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
}

void send_line() {
  //Access to HTTPS (SSL communication)
  WiFiClientSecure client;
  //Required when connecting without verifying the server certificate
  client.setInsecure();

  Serial.println("Try");

  //SSL connection to Line's API server (port 443: https)
  if (!client.connect(host, 443)) {
    Serial.println("Connection failed");
    return;
  }

  Serial.println("Connected");

  // Send request
  String query = String("message=") + String(message);
  String request = String("") +
    "POST /api/notify HTTP/1.1\r\n" +
    "Host: " + host + "\r\n" +
    "Authorization: Bearer " + token + "\r\n" +
    "Content-Length: " + String(query.length()) +  "\r\n" + 
    "Content-Type: application/x-www-form-urlencoded\r\n\r\n" +
    query + "\r\n";
  client.print(request);
 
  // Wait until reception is complete
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }
  
  String line = client.readStringUntil('\n');
  Serial.println(line);
}
