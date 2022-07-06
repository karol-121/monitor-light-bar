#include <WiFi.h>
#include <analogWrite.h>

//WiFi metadata
#define NETWORK_SSID "WiFi-SSID"
#define NETWORK_PASSWORD "WiFi-PASSWORD"

//change the variable names afterwards as they will not correspond to diodes in final product
#define LEFT_LEDSTRIP_OUTPUT 32
#define RIGHT_LEDSTRIP_OUTPUT 33

WiFiServer server(80);

//brightness levels:
int left_brightness_level;
int right_brightness_level;

//power, on/off
bool power;

//definition of http responses
void response(WiFiClient client, int type) {
  if (type == 0) { //return ok response
    client.println("HTTP/1.1 200 OK");
    client.println("Access-Control-Allow-Origin: *");
    client.println("Content-type:text/html");
    client.println();
  } else if (type == -1) { //return bad request response
    client.println("HTTP/1.1 400 Bad Request");
    client.println("Content-type:text/html");
    client.println();
  }
}

void setup(){
  pinMode(LEFT_LEDSTRIP_OUTPUT, OUTPUT);
  pinMode(RIGHT_LEDSTRIP_OUTPUT, OUTPUT);
  
  Serial.begin(115200);
  delay(10);
  WiFi.begin(NETWORK_SSID, NETWORK_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  server.begin();

  //set level to maximum each time the controller is powered on
  left_brightness_level = 255;
  right_brightness_level = 255;

  //start with leds turned on
  power = true;
}

void loop() {

  if (power) {
    analogWrite (LEFT_LEDSTRIP_OUTPUT, left_brightness_level);
    analogWrite (RIGHT_LEDSTRIP_OUTPUT, right_brightness_level);
  } else {
    analogWrite (LEFT_LEDSTRIP_OUTPUT, 0);
    analogWrite (RIGHT_LEDSTRIP_OUTPUT, 0);
  }
  
  
  WiFiClient client = server.available();
  if (client) {

    //getting and formatting first line of the request as only so much is needed
    String HTTPrequestStartLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if(c == '\n') {
          break;
        }
        HTTPrequestStartLine += c;
      }
    }

    //checking the request and handling it
    if (HTTPrequestStartLine.startsWith("GET /status")) { //if the request was to get status

      //print json encoded status about power, and brightness levels
      client.println("HTTP/1.1 200 OK");
      client.println("application/json");
      client.println();
      client.print("{\"power\": \"");
      client.print(power);
      client.print("\",\"left\": \"");
      client.print(left_brightness_level);
      client.print("\",\"right\": \"");
      client.print(right_brightness_level);
      client.print("\"}");
      client.println();
      
    } else if (HTTPrequestStartLine.startsWith("GET /brightness")) { //if the request was to set brightness

      //get left brightness level from http request
      left_brightness_level = HTTPrequestStartLine.substring(21,24).toInt();

      //get right brightness level form http request
      right_brightness_level = HTTPrequestStartLine.substring(31,34).toInt();
      
      response(client, 0);

    } else if (HTTPrequestStartLine.startsWith("GET /?power=1")) { //if the request was to turn on the power
      
      power = true;
      response(client, 0); //OK response
      
    } else if (HTTPrequestStartLine.startsWith("GET /?power=0")) { //if the request was to turn off the power
      
      power = false;
      response(client, 0); //OK response
      
    } else { //other request than these above
      
      //if none of request above has been send, return "bad request" response
      response(client, -1); //BAD REQUEST response
      
    }

    client.stop();
  }
}
