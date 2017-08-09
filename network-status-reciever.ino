#include <SPI.h>
#include <Ethernet2.h>
#include <ArduinoJson.h>

// Setup the httpclient
EthernetServer server = EthernetServer(18080);

const char *status;

void setup() {
  Serial.begin(9600);
  while(!initEthernet()){
    // If init fails, keep retrying
  }
  server.begin();
}

void loop() {
  maintainEthernet();
  if(handleIcoming(&status)){
    setStatusLed(&status);
  }
}

bool initEthernet(){
  // Network configuration
  byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0xFA, 0x91};
  // Ethernet connection setup
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    return false;
  }
  return true;
  // IP 10.3.21.14
}

void maintainEthernet(){
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
}

bool skipRespHeaders(EthernetClient *client){
  char endOfHeaders[] = "\r\n\r\n";
  bool skipped = client->find(endOfHeaders);
  if(!skipped){
    Serial.println("Skipping headers fails");
  }
  return skipped;
}

bool handleIcoming(const char **status){
  EthernetClient client = server.available();
  if(client && skipRespHeaders(&client)){
    StaticJsonBuffer<32> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(client);
    if (!root.success()) {
      client.println("HTTP/1.1 500 ERROR");
      client.println("Content-Type: text/html");
      client.println("Connection: close");
      client.println();
      client.println("JSON failed to parse.");
      client.stop();
      return false;
    }
    *status = root["status"];
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.print("JSON parsed: ");
    client.println(*status);
    client.stop();
    return true;
  }

}

void setStatusLed(const char **const status){
  if(*status == "ok"){
    // Set the LED to green
  }else if(*status == "warning"){
    // Set the LED to orange
  }else if (*status == "critical"){
    // Set the LED to red
  }else{
    // By default turn amber -> unkown
  }
  /*
  Can you switch ... cases on char *? Look into it and if possible implement
  case "ok": // Set the LED to green
  break;
  case "warning": // Set the LED to orange
  break;
  case "critical": // Set the LED to red
  break;
  default: // By default turn amber -> unkown
  break;
  */
}
