#include <WiFiNINA.h>
#include <Arduino_JSON.h>
#include <Arduino_MKRIoTCarrier.h>
#include "arduino_secrets.h"

MKRIoTCarrier carrier;

char ssid[] = SECRET_SSID;
char password[] = SECRET_PASS;
char api_key[] = API_KEY;
char hue_bridge[] = HUE_BRIDGE;

//default light for startup
int lightNum = 2;
String lightName = "George";

//tracks state for toggling lights off and on
bool light_state = true;
String print_txt = "";
String command = "";


int status = WL_IDLE_STATUS;
WiFiClient client;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid,password);
    delay(1000);
  }
  Serial.println("Connected to wifi");

  CARRIER_CASE = true;
  carrier.begin();
  carrier.display.setRotation(0);
}

void loop() {
  delay(100);
  carrier.Buttons.update();

  if (carrier.Button1.onTouchDown()) {
    light_state = true;
    int rand_c = random(65000);
    Serial.print("button0 ");
    Serial.println(rand_c);
    carrier.display.fillScreen(ST77XX_MAGENTA);
    carrier.display.setCursor(30, 60);
    carrier.display.setTextColor(ST77XX_WHITE);
    carrier.display.setTextSize(2);
    carrier.display.print("Random Color");
    carrier.display.setCursor(40, 110);
    carrier.display.println(rand_c);
    carrier.display.setCursor(30, 160);
    carrier.display.println(lightName);
    colorUpdate(rand_c, 5, light_state);
    delay(500);

  }

  if (carrier.Button2.onTouchDown()) {
    if (lightNum == 2){
      lightNum = 4;
      lightName = "Mary";
    } else {
      lightNum = 2;
      lightName = "George";
    }
    Serial.print("button3 ");
    Serial.println(lightName);
    carrier.display.fillScreen(ST77XX_BLUE);
    carrier.display.setCursor(30, 60);
    carrier.display.setTextColor(ST77XX_WHITE);
    carrier.display.setTextSize(2);
    carrier.display.print("Set to");
    carrier.display.setCursor(30, 110);
    carrier.display.println(lightName);
    delay(500);

  }
  
  if (carrier.Button4.onTouchDown()) {
    Serial.print("button4 off");
    if (light_state) {
      light_state = false;
      print_txt = " off";
    } else {
      light_state = true;
      print_txt = " on";
    }
    carrier.display.fillScreen(ST77XX_BLACK);
    carrier.display.setCursor(30, 60);
    carrier.display.setTextColor(ST77XX_WHITE);
    carrier.display.setTextSize(2);
    carrier.display.print("Turning light");
    carrier.display.setCursor(20, 110);
    carrier.display.print(lightName);
    carrier.display.println(print_txt);
    colorUpdate(15000, 5, light_state);
    delay(500);
  }
  
}


void colorUpdate(int color, int transition, bool state) {
  if (state) {
  command = "{\"on\":true,\"hue\":"+String(color)+",\"sat\":255,\"bri\":255,\"transitiontime\":"+String(transition)+"}";
  } else {
    //this command does double duty since I didn't want to duplicate the machinery when I added the ability to toggle the light on and off
    command = "{\"on\":false}";
  }
  if (client.connect(hue_bridge, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request: first building the URL
    client.print("PUT ");
    client.print(api_key);
    client.print("/lights/");
    client.print(lightNum);
    client.println("/state HTTP/1.1");
    //submit headers
    client.println("Connection: keep-alive");
    client.print("Host: ");
    client.println(hue_bridge);
    client.print("Content-Length: ");
    client.println(command.length());
    client.println("Content-Type: text/plain;charset=UTF-8");
    client.println(); // blank line before body
    client.println(command); // Hue command
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("unable to connect");
  }
  delay(1000);
}
