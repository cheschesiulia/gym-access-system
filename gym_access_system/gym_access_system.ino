#include <SPI.h>
#include <Adafruit_PN532.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#include "config.h"

boolean fanOn = false;
#define FAN_PIN 4

// Define SPI pins
#define SCK 18   // SPI Clock
#define MOSI 27  // SPI MOSI
#define MISO 19  // SPI MISO
#define SS 5     // SPI Chip Select

#define RED_LED 23    // GPIO 23 for Red LED (Permission Denied)
#define GREEN_LED 22  // GPIO 22 for Green LED (Permission Granted)

// Create an instance of Adafruit_PN532 with SPI
Adafruit_PN532 nfc(SS);

void setup() {
  Serial.begin(115200);

  // Initialize LED pins
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  // Turn off both LEDs initially
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);

  // Init fan pin
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);

  connectToWiFi();

  delay(2000);

  Serial.println("Initializing NFC Reader with SPI...");

  // Initialize SPI with custom pins
  SPI.begin(SCK, MISO, MOSI, SS);
  SPI.setClockDivider(SPI_CLOCK_DIV64);

  // Initialize NFC module
  nfc.begin();

  // Check for the PN532 module
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Didn't find PN53x board. Check connections.");
    while (1); // Halt
  }

  // Print firmware version
  Serial.print("Found chip PN5"); Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);

  // Configure the NFC module
  nfc.SAMConfig();
  Serial.println("Waiting for an NFC card...");
}

void loop() {
  uint8_t uid[7];        // Buffer to hold the UID (7 bytes max for MiFare)
  uint8_t uidLength;     // Length of the UID

  // Try to read the card
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 1000)) { // Timeout of 1000ms
    Serial.println("NFC card detected!");

    // Print UID
    Serial.print("UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i = 0; i < uidLength; i++) {
      Serial.print(" 0x"); Serial.print(uid[i], HEX);
    }
    Serial.println();

    // Perform actions with the detected card (e.g., access control logic)
    readDataFromCard(uid, uidLength);

    delay(1000);
  } else {
    // No card detected
    Serial.println("No card detected within the timeout.");
  }

  // Check if fan state changed
  checkFanStateChanges();

  // Act accordingly
  fan();

  delay(1000); // Optional delay before polling again
}

void readDataFromCard(uint8_t* uid, uint8_t uidLen) {
  uint8_t block[16];
  uint8_t success;
  // Default MIFARE Classic key (6 bytes, all 0xFF)
  uint8_t keyData[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  // Authenticate the block before reading
  success = nfc.mifareclassic_AuthenticateBlock(uid, uidLen, 4, 0x60, keyData);  // Authenticate block 4 with key A
  if (success) {
    // Read the block from the card
    success = nfc.mifareclassic_ReadDataBlock(4, block);  // Read data from block 4
    if (success) {
      // Print the read data
      Serial.println("Data read from NFC card:");
      // Extract name (first 8 bytes)
      char name[9] = {0};  // 8 characters max + null terminator
      for (int i = 0; i < 8; i++) {
        name[i] = (char)block[i];
      }
      name[8] = '\0';  // Ensure null termination
      // Extract permission flag (byte 9)
      bool permission = block[8] == 1;
      // Display the extracted data
      Serial.print("Name: ");
      Serial.println(name);
      Serial.print("Permission: ");
      Serial.println(permission ? "Has membership" : "Doesnt have membership");
      
      // Send to server and control leds based on response
      sendNameToServer(name);
    } else {
      Serial.println("Failed to read data from NFC card.");
    }
  } else {
    Serial.println("Authentication failed. Cannot read data.");
  }
}

void sendNameToServer(String name) {
  // Print Wi-Fi status
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected, preparing to send data...");

    HTTPClient http;
    String url = "http://" SERVER_IP ":8000/api/entry_exit";  // Server URL with SERVER_IP
    Serial.print("Server URL: ");
    Serial.println(url);

    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"name\":\"" + name + "\"}";
    Serial.print("Payload: ");
    Serial.println(payload);

    int httpResponseCode = http.POST(payload);  // Send POST request
    Serial.print("HTTP Response Code: ");
    Serial.println(httpResponseCode);

    if (httpResponseCode == 200) {
      // Success
      String response = http.getString();  // Get the server response
      Serial.println("Server response received:");

      // Print the server response
      Serial.println(response);

      // Based on the server response, handle permission
      if (response.indexOf("Access granted") >= 0) {
        // Member entered the gym (Access granted)
        Serial.println("Access granted by server. Member entered the gym.");
        handlePermissionGranted();  // Handle permission granted
      } else if (response.indexOf("Member exited") >= 0) {
        // Member exited the gym
        Serial.println("Member exited the gym.");
        handlePermissionGranted();  // Handle exit but still valid
      } else if (response.indexOf("Access denied") >= 0) {
        // Invalid membership
        Serial.println("Access denied by server. Invalid membership.");
        handlePermissionDenied();  // Handle permission denied
      } else {
        // If there's an unexpected response
        Serial.println("Unexpected response from server: " + response);
      }
    } else {
      // Error with the HTTP request
      Serial.print("Error communicating with server. Response code: ");
      Serial.println(httpResponseCode);
    }

    http.end();  // Close the HTTP connection
    Serial.println("HTTP connection closed.");
  } else {
    // If WiFi is not connected
    Serial.println("WiFi disconnected. Cannot send data.");
  }

  delay(2000);
}

void checkFanStateChanges() {
    WiFiClientSecure client;
    client.setCACert(server_cert);

    if (client.connect(SERVER_IP, 8000)) {
        Serial.println("Connected to server for fan status!");

        String request = "GET /api/get_fan_status HTTP/1.1\r\n";
        request += "Host: " SERVER_IP "\r\n";
        request += "Connection: close\r\n\r\n";

        client.print(request);
        Serial.println("Request sent for fan status");

        // Citește răspunsul
        while (client.connected() || client.available()) {
            String response = client.readStringUntil('\n');
            Serial.println(response);
            if (response.indexOf("\"fan_on\": true") >= 0) {
                fanOn = true;
            } else if (response.indexOf("\"fan_on\": false") >= 0) {
                fanOn = false;
            }
        }

        client.stop();
    } else {
        Serial.println("Connection to server failed!");
    }
}

void fan() {
  Serial.println("Act fan");
  if (fanOn) {
    digitalWrite(FAN_PIN, HIGH);
  } else {
    digitalWrite(FAN_PIN, LOW);
  }
}

void connectToWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  WiFi.setSleep(false);
  Serial.println("Connected to WiFi!");
}

void handlePermissionGranted() {
  digitalWrite(GREEN_LED, HIGH);
  delay(2000);
  digitalWrite(GREEN_LED, LOW);
}

void handlePermissionDenied() {
  digitalWrite(RED_LED, HIGH);
  delay(2000);
  digitalWrite(RED_LED, LOW);
}