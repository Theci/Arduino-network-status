#include <JsonListener.h>
#include <JsonStreamingParser.h>
#include <HttpClient.h>
#include <SPI.h>
#include <Ethernet2.h>

// NEtwork configuration
byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0xFA, 0x91};

// Setup the httpclient
EthernetClient client;
HttpClient http(client);

// Setup JSON engine
JsonStreamingParser parser;
ExampleListener listener;

void setup() {
  // Serial connection setup
  Serial.begin(9600);
  // Ethernet connection setup
  Serial.println("Connecting to the internet.");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    while(true){
      // If the network fails, no use to continue
    }
  }

  Serial.println("Internet connection established.");
  printIPAddress();
  delay(500);

  // HTTP GET request setup
  Serial.println("Establishing server connection.");
  httpGetRequest("kelder.zeus.ugent.be",80,"/slotmachien/","application/json");
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
      printIPAddress();
      break;
    case 3:
      Serial.println("Error: rebind fail");
      break;
    case 4:
      Serial.println("Rebind success");
      printIPAddress();
      break;
    default:
      break;
  }

  // If the ethernetclient disconnects, send new request later on
  if (!client.connected()) {
    Serial.println();
    Serial.println("Waiting 10 seconds for next request ...");
    delay(10000);
    httpGetRequest("kelder.zeus.ugent.be",80,"/slotmachien/","application/json");
  }
}

void printIPAddress()
{
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }
  Serial.println();
}

void httpGetRequest(const char *host, uint16_t port, const char * URLpath, const char* contType){
  int err = 0; //Var to catch errors
  // With library
  http.beginRequest(); // Start HTTP request
  //Create the basis of the HTTP request: target,port,path,type,auth
  err = http.startRequest(host,port,URLpath,HTTP_METHOD_GET,NULL);
  // Set content-type
  http.sendHeader("content-type",contType);
  http.endRequest(); // Terminate request and send
  if(err == 0){
    err = http.responseStatusCode();
    if (err >= 200 && err <=299){
      // Skip over the response header
      err = http.skipResponseHeaders();
      if(err>=0){
        char c;
        // When there's data incoming or we are connected, output
        while(http.connected()||http.available()){
          // Print body
          if(http.available()){
            c = http.read;
            Serial.print(c);            
          }else{
          // If no data incoming, wait x miliseconds
            delay(50);
          }
        }
      }else{
        Serial.println("Couldn't skip headers:");
        Serial.println(err);
      }
    }else{
      Serial.println("Wrong status code (<200 or >299):");
      Serial.println(err);
    }
  }else{
    Serial.println("Connection failed:");
    Serial.println(err);
  }
  http.stop();
}