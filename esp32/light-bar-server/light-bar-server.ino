#include <WiFi.h>
#include <analogWrite.h>

//WiFi metadata
#define NETWORK_SSID "WiFi-SSID"
#define NETWORK_PASSWORD "WiFi-PASSWORD"

//pin output for leds, each led strip has warm and cold leds
#define _LEFT_WARM 13 
#define _LEFT_COLD 27
#define _RIGHT_WARM 33
#define _RIGHT_COLD 32

WiFiServer server(80);

//brightness levels:
int left_warm_level;
int left_cold_level;

int right_warm_level;
int right_cold_level;

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
  pinMode(_LEFT_WARM, OUTPUT);
  pinMode(_LEFT_COLD, OUTPUT);
  pinMode(_RIGHT_WARM, OUTPUT);
  pinMode(_RIGHT_COLD, OUTPUT);

//  analogWriteResolution(_LEFT_WARM, 12);
//  analogWriteResolution(_LEFT_COLD, 12);
//  analogWriteResolution(_RIGHT_WARM, 12);
//  analogWriteResolution(_LEFT_COLD, 12);
  
  Serial.begin(115200);
  delay(10);
  WiFi.begin(NETWORK_SSID, NETWORK_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  server.begin();

  //set level to maximum each time the controller is powered on
  left_warm_level = 255;
  left_cold_level = 255;
  
  right_warm_level = 255;
  right_cold_level = 255;

  //start with leds turned on
  power = true;
}

void loop() {

  if (power) {
    analogWrite (_LEFT_WARM, left_warm_level);
    analogWrite (_LEFT_COLD, left_cold_level);
    analogWrite (_RIGHT_WARM, right_warm_level);
    analogWrite (_RIGHT_COLD, right_cold_level);
  } else {
    analogWrite (_LEFT_WARM, 0);
    analogWrite (_LEFT_COLD, 0);
    analogWrite (_RIGHT_WARM, 0);
    analogWrite (_RIGHT_COLD, 0);
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
      client.print("\",\"left warm\": \"");
      client.print(left_warm_level);
      client.print("\",\"left cold\": \"");
      client.print(left_cold_level);
      client.print("\",\"right warm\": \"");
      client.print(right_warm_level);
      client.print("\",\"right cold\": \"");
      client.print(right_cold_level);
      client.print("\"}");
      client.println();
      
    } else if (HTTPrequestStartLine.startsWith("GET /brightness")) { //if the request was to set brightness

      //GET /brightness?left_cold=xxx&left_warm=xxx&right_cold=xxx&right_warm=xxx

      //get left brightness level from http request
      left_warm_level = HTTPrequestStartLine.substring(26,29).toInt();
      left_cold_level = HTTPrequestStartLine.substring(40,43).toInt();
      
      //get right brightness level form http request
      right_warm_level = HTTPrequestStartLine.substring(55,58).toInt();
      right_cold_level = HTTPrequestStartLine.substring(70,73).toInt();

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
