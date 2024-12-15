// WRITING DATA

// #include <SPI.h>
// #include <Adafruit_PN532.h>

// // Define SPI pins
// #define SCK 18    // SPI Clock
// #define MOSI 27   // SPI MOSI pin
// #define MISO 19   // SPI MISO pin
// #define SS 5      // SPI Chip Select pin

// Adafruit_PN532 nfc(SS);  // Create PN532 instance with SS pin

// // Name and Permission Data
// char name[] = "andrei";   // 8 characters max
// bool permission = false;   // True = Permission Granted, False = Denied

// void setup() {
//   Serial.begin(115200);
//   Serial.println("Initializing NFC Module...");

//   // Initialize the SPI bus and PN532
//   SPI.begin(SCK, MISO, MOSI, SS); // Use hardware SPI
//   nfc.begin();  // Initialize NFC module over SPI

//   // Check if the NFC module is connected
//   uint32_t versiondata = nfc.getFirmwareVersion();
//   if (!versiondata) {
//     Serial.println("Didn't find PN53x board");
//     while (1);  // Halt
//   }

//   // Configure the board to read NFC tags
//   nfc.SAMConfig();
//   Serial.println("NFC Module Initialized!");
// }

// void loop() {
//   Serial.println("Waiting for an NFC card...");

//   uint8_t success;
//   uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
//   uint8_t uidLength;

//   // Check if an NFC card is present
//   success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

//   if (success) {
//     Serial.println("Card detected!");

//     // Print UID
//     Serial.print("UID Length: "); Serial.println(uidLength);
//     Serial.print("UID Value: ");
//     for (uint8_t i=0; i < uidLength; i++) {
//       Serial.print(" 0x"); Serial.print(uid[i], HEX);
//     }
//     Serial.println();

//     // Store name and permission data to the NFC card
//     writeDataToCard(uid, uidLength, name, permission);  // Corrected function call

//     delay(1000);  // Wait for 1 second before looking for another card
//   }
// }

// void writeDataToCard(uint8_t* uid, uint8_t uidLen, char* name, bool permission) {
//   uint8_t block[16];

//   // Prepare data for name (first 8 bytes)
//   for (int i = 0; i < 8; i++) {
//     block[i] = name[i];
//   }

//   // Add permission flag (convert boolean to byte)
//   block[8] = permission ? 1 : 0;  // Store 1 for true, 0 for false
  
//   // Fill remaining bytes with 0s (if necessary)
//   for (int i = 9; i < 16; i++) {
//     block[i] = 0;
//   }

//   // Default MIFARE Classic key (6 bytes, all 0xFF)
//   uint8_t keyData[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

//   // Authenticate the block before writing
//   uint8_t success = nfc.mifareclassic_AuthenticateBlock(uid, uidLen, 4, 0x60, keyData);  // Authenticate block 4 with key A
//   if (success) {
//     // Write the block to the card
//     success = nfc.mifareclassic_WriteDataBlock(4, block);  // Write data to block 4
//     if (success) {
//       Serial.println("Data written to NFC card!");
//     } else {
//       Serial.println("Failed to write data to NFC card.");
//     }
//   } else {
//     Serial.println("Authentication failed. Cannot write data.");
//   }
// }


// READING DATA

// #include <SPI.h>
// #include <Adafruit_PN532.h>

// // Define SPI pins
// #define SCK 18    // SPI Clock
// #define MOSI 27   // SPI MOSI pin
// #define MISO 19   // SPI MISO pin
// #define SS 5      // SPI Chip Select pin

// Adafruit_PN532 nfc(SS);  // Create PN532 instance with SS pin

// void setup() {
//   Serial.begin(115200);
//   Serial.println("Initializing NFC Module...");

//   // Initialize the SPI bus and PN532
//   SPI.begin(SCK, MISO, MOSI, SS); // Use hardware SPI
//   nfc.begin();  // Initialize NFC module over SPI

//   // Check if the NFC module is connected
//   uint32_t versiondata = nfc.getFirmwareVersion();
//   if (!versiondata) {
//     Serial.println("Didn't find PN53x board");
//     while (1);  // Halt
//   }

//   // Configure the board to read NFC tags
//   nfc.SAMConfig();
//   Serial.println("NFC Module Initialized!");
// }

// void loop() {
//   Serial.println("Waiting for an NFC card...");

//   uint8_t success;
//   uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
//   uint8_t uidLength;

//   // Check if an NFC card is present
//   success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

//   if (success) {
//     Serial.println("Card detected!");

//     // Print UID
//     Serial.print("UID Length: "); Serial.println(uidLength);
//     Serial.print("UID Value: ");
//     for (uint8_t i = 0; i < uidLength; i++) {
//       Serial.print(" 0x"); Serial.print(uid[i], HEX);
//     }
//     Serial.println();

//     // Read data from block 4
//     readDataFromCard(uid, uidLength);

//     delay(1000);  // Wait for 1 second before looking for another card
//   }
// }

// void readDataFromCard(uint8_t* uid, uint8_t uidLen) {
//   uint8_t block[16];
//   uint8_t success;

//   // Default MIFARE Classic key (6 bytes, all 0xFF)
//   uint8_t keyData[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

//   // Authenticate the block before reading
//   success = nfc.mifareclassic_AuthenticateBlock(uid, uidLen, 4, 0x60, keyData);  // Authenticate block 4 with key A
//   if (success) {
//     // Read the block from the card
//     success = nfc.mifareclassic_ReadDataBlock(4, block);  // Read data from block 4
//     if (success) {
//       // Print the read data
//       Serial.println("Data read from NFC card:");

//       // Extract name (first 8 bytes)
//       char name[9] = {0};  // 8 characters max + null terminator
//       for (int i = 0; i < 8; i++) {
//         name[i] = (char)block[i];
//       }
//       name[8] = '\0';  // Ensure null termination

//       // Extract permission flag (byte 9)
//       bool permission = block[8] == 1;

//       // Display the extracted data
//       Serial.print("Name: ");
//       Serial.println(name);
//       Serial.print("Permission: ");
//       Serial.println(permission ? "Granted" : "Denied");
//     } else {
//       Serial.println("Failed to read data from NFC card.");
//     }
//   } else {
//     Serial.println("Authentication failed. Cannot read data.");
//   }
// }