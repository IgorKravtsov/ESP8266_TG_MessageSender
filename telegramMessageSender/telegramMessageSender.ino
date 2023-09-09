#include <ESP8266WiFi.h>   
#include <FastBot.h>

#define BOT_TOKEN       "YOUR_BOT_TOKEN"
#define TG_GROUP_ID     "YOUR_TG_GROUP_ID"
#define HOST_TO_PING    "www.google.com"
#define PORT_TO_PING    443

FastBot bot(BOT_TOKEN);
WiFiClient client;

// Doesn't support 5Ghz networks
const char* wifiNetworks[][2] = {
  {"WIFI_NETWORK_1", "WIFI_PASSWORD_1"},
  {"WIFI_NETWORK_2", "WIFI_PASSWORD_2"},
  {"WIFI_NETWORK_3", "WIFI_PASSWORD_3"},
};

const int numNetworks = sizeof(wifiNetworks) / sizeof(wifiNetworks[0]);
int currentNetwork = 0;

void setup() {
  Serial.begin(9600);
  connectWiFi();
  checkInterntAndSendMessage();
}

void loop() {
  bot.tick();
}

void connectWiFi() {
  const char* wifiName = wifiNetworks[currentNetwork][0];
  const char* wifiPassword = wifiNetworks[currentNetwork][1];
  WiFi.begin(wifiName, wifiPassword);

  int tries = 0;
  int max_tries = 15;
  
  while (WiFi.status() != WL_CONNECTED && tries++ < max_tries) {
    delay(1000);
    Serial.print("Connecting to ");
    Serial.print(wifiName);
    Serial.println("...");
  }
  
  Serial.println("Connected to Wi-Fi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void checkInterntAndSendMessage() {
  bool isInternetConnected = false;

  while(!isInternetConnected) {
    if (WiFi.status() != WL_CONNECTED) {
      reconnectToWifi("Connection lost, trying the next network...");
    } else {
      if (client.connect(HOST_TO_PING, PORT_TO_PING)) {
        bot.sendMessage("Here we have the light!", TG_GROUP_ID);
        isInternetConnected = true;
      } else {
        reconnectToWifi("No internet, trying next network...");
      }
    }
  }
}

void reconnectToWifi(const char* errorMessage) {
  Serial.println(errorMessage);
  currentNetwork = (currentNetwork + 1) % numNetworks; // Switch to the next network
  connectWiFi(); // Try to connect to the new network
}