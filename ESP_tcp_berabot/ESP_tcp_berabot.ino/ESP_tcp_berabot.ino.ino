
#include <ESP8266WiFi.h>

#define packTimeout 5 // ms (if nothing more on UART, then send packet)
#define bufferSize 8192
#define verboseEnable//Comment for run on printer (include some M117 messages for display)

//VARS/////////////////////////////////////////////////
uint8_t buf1[bufferSize];
uint8_t i1 = 0;

uint8_t buf2[bufferSize];
uint8_t i2 = 0;

bool message = false;
///////////////////////////////////////////////////////

const char* ssid     = "CCAM01";//"Casa01";
const char* password = "w1r3l355@jitter";//"gordinocaca";
const int port = 1234; //9876
WiFiServer server(port);
WiFiClient client;

void setup() {
  Serial.begin(250000);
  delay(10);

  // We start by connecting to a WiFi network
#ifdef verboseEnable
  Serial.print("Connecting to ");
  Serial.println(ssid);
#endif

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);

    
    #ifdef verboseEnable
        Serial.print(".");
    #endif
      }
    #ifdef verboseEnable
      Serial.println("Connected!!");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      Serial.print("Port: ");
      Serial.println(port);
      Serial.println("Starting TCP server");
    #else
      delay(5000);
      char buffer[50];
      Serial.println(";");
      sprintf(buffer, "M117 IP:%s:%d", WiFi.localIP().toString().c_str(), port);
      Serial.println(buffer); 
#endif

  server.begin(); // start TCP server
}

int value = 0;

void loop() {
  if (!client.connected()) { // if client not connected
    client = server.available(); // wait for it to connect
    message = true;
    return;
  }

  if (message) Serial.println("M117 Client Connected"); // here we have a connected client
  message = false;

  if (client.available()) {
    while (client.available()) {
      buf1[i1] = (uint8_t)client.read();
      if (i1 < bufferSize - 1) i1++;
    }
    // now send to UART:
    Serial.write(buf1, i1);
    i1 = 0;
  }

  if (Serial.available()) {

    // read the data until pause:

    while (1) {
      if (Serial.available()) {
        buf2[i2] = (char)Serial.read(); // read char from UART
        if (i2 < bufferSize - 1) i2++;
      } else {
        //delayMicroseconds(packTimeoutMicros);
        delay(packTimeout);
        if (!Serial.available()) {
          break;
        }
      }
    }

    // now send to WiFi:
    client.write((char*)buf2, i2);
    i2 = 0;
  }


}


