#include <ArduinoHttpClient.h>
#include <SPI.h>
#include <Ethernet2.h>
#include <ArduinoJson.h>

// NEtwork configuration
byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0xFA, 0x91};
// Setup the httpclient
EthernetClient eth;

void setup() {
  // Serial connection setup
  Serial.begin(115200);
  // Ethernet connection setup
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    while(true){
      // If the network fails, no use to continue
    }
  }
}

void loop() {
  // Maintain ethernet connection
  switch (Ethernet.maintain())
  {
    case 1:
      Serial.println("Error: renewed fail");
      break;
    case 2:
      Serial.println("Renewed success");
      break;
    case 3:
      Serial.println("Error: rebind fail");
      break;
    case 4:
      Serial.println("Rebind success");
      break;
    default:
      break;
  }

  delay(5000);
  // JSON responses can get long, String makes it so it can catch all of them
  String resp;
  // If the ethernetclient disconnects, send new request later on
  if (!eth.connected()) {
    resp = httpGetRequest("demo.robustperception.io",9090,"/api/v1/query?query=up{job=\"node\"}","application/json", &resp);
    jsonToData(&resp);
  }
  delay(5000);
}

String httpGetRequest(const char *host, uint16_t port, const char * URLpath, const char* contType){
  HttpClient http = HttpClient(eth,host,port); // Start httpClient
  int statusCode = 0; //Var to catch errors
  int err = 0;
  String response;
  // With library
  http.beginRequest(); // Start HTTP request
  //Create the basis of the HTTP request: target,port,path,type,auth
  err = http.get(URLpath);
  // Set content-type
  http.sendHeader(HTTP_HEADER_CONTENT_TYPE,contType);
  http.endRequest(); // Terminate request and send
  if(err == 0){ // See if there are problems with the request
     statusCode = http.responseStatusCode();
    if (statusCode >= 200 && err <=299){ // Check http status code
      response = http.responseBody(); // Fetch response body
    }else{
      Serial.println("Wrong status code (<200 or >299):");
      Serial.println(statusCode);
    }
  }else{
    Serial.println("Connection failed:");
    Serial.println(err);
  }
  http.stop();
  return response;
}

void jsonToData(const String *response){
  // Buffer size needs to be picked well, can overload memory size causing arduino to freeze
  StaticJsonBuffer<300> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(*response);
  const char *status = root["status"];
  Serial.println("JSON output:");
  Serial.println(status);
}
