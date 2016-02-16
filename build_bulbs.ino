#include <Wire.h>
#include <ESP8266WiFi.h>

const char* ssid     = "Wifi Network Name";
const char* password = "Wifi Network Password";

const char* host = "build_status_endpoint_host.com";
const char* path = "/endpoint/path/on/host";

const int STATUS_LENGTH = 4; // number of statuses
const int pins[STATUS_LENGTH] = {0, 4, 13, 12}; // pin addresses

void setup() {
  Serial.begin(115200);
  delay(100);

  for (int i = 0; i < STATUS_LENGTH; i++) {
    pinMode(pins[i], OUTPUT);
  }
  initWiFi();
}

void initWiFi() {
  WiFi.disconnect();
  
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(WiFi.status());
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

boolean statuses[STATUS_LENGTH] = {false, false, false, false};

void loop() {

  fetchBuildStatuses(statuses);
  Serial.print("Got statuses: ");
  Serial.println(statuses[0]);
  Serial.println(statuses[1]);
  Serial.println(statuses[2]);
  Serial.println(statuses[3]);
  lightBulbsFromStatuses(statuses);
  
  delay(10000);
}

void fetchBuildStatuses(boolean *statuses) {

  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  Serial.print("Requesting URL: ");
  Serial.println(path);
  
  // This will send the request to the server
  client.print(String("GET ") + path + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  int timeout = millis() + 5000;
  while (client.available() == 0) {
    if (timeout - millis() < 0) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()){
    String line = client.readStringUntil('\r');
    line.replace(" ", "");
    Serial.println("Got line: " + line);

    if (line.indexOf('{') != -1) {
      statuses[0] = getValueForKey(line, "JSON Key 1");
      statuses[1] = getValueForKey(line, "JSON Key 2");
      statuses[2] = getValueForKey(line, "JSON Key 3");
      statuses[3] = getValueForKey(line, "JSON Key 4");
      return;
    }
    
  }
  
  Serial.println();
  Serial.println("closing connection");

  return;
}

int getValueForKey(String jsonString, String key) {
  
  int objStart = jsonString.indexOf('{');
  int objEnd = jsonString.indexOf('}');
  int startPos = objStart + 1;
  
  while(startPos < objEnd) {
    int keyEnd = jsonString.indexOf(':', startPos);
    String thisKey = jsonString.substring(startPos, keyEnd);
    thisKey.replace("\"", "");

    int valStart = keyEnd + 1;
    int valEnd = jsonString.indexOf(',', valStart);
    if (valEnd == -1) {
      valEnd = objEnd;
    }
    String val = jsonString.substring(valStart, valEnd);
    val.replace(" ", "");
    
    if (thisKey == key) {    
      return val.toInt();
    }
    
    startPos = valEnd + 1;
  }

  return -1;
  
}

void lightBulbsFromStatuses(boolean *statuses) {
    for (int i = 0; i < STATUS_LENGTH; i++) {
      lightBulb(pins[i], statuses[i]);
    }
}

void lightBulb(int pin, boolean light) {
  if (light) {
    digitalWrite(pin, HIGH);
  } else {
    digitalWrite(pin, LOW);
  }
}
