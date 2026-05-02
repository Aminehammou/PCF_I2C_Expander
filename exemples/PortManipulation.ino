/**
 * @file PortManipulation.ino
 * @brief Manipule toutes les broches du PCF8574 en même temps.
 *
 * Cet exemple montre comment utiliser les fonctions de port pour
 * écrire, lire et inverser l'état de toutes les 8 broches simultanément.
 *
 * Matériel requis :
 * - Une carte Arduino ou compatible
 * - Un expandeur de port PCF8574
 * - (Optionnel) 8 LEDs avec leurs résistances pour visualiser l'état du port.
 *
 * Connexions :
 * - VCC du PCF8574 -> 5V ou 3.3V de la carte
 * - GND du PCF8574 -> GND de la carte
 * - SDA du PCF8574 -> Broche SDA de la carte
 * - SCL du PCF8574 -> Broche SCL de la carte
 * - (Optionnel) Broches P0-P7 -> Anodes des LEDs -> Résistances -> GND
 */

#include <Arduino.h>
#include <Wire.h>
#include "PCF8574.h"

// Adresse I2C du PCF8574.
const uint8_t PCF8574_ADDRESS = 0x20;

// Crée une instance de la bibliothèque.
PCF8574 pcf(PCF8574_ADDRESS);

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Exemple PortManipulation pour PCF8574");

  // Option: broches I2C personnalisées (ESP32/ESP8266)
  // #if defined(ARDUINO_ARCH_ESP32) || defined(ESP8266)
  //   pcf.setPins(21, 22);
  // #endif

  pcf.begin();

  Serial.println("Démarrage de la séquence...");
}

void loop() {
  // 1. Écrire une valeur binaire (chenillard)
  Serial.println("\n--- Chenillard ---");
  for (int i = 0; i < 8; i++) {
    uint8_t portValue = 1 << i;
    pcf.writePort(portValue);
    delay(100);
  }

  // 2. Inverser toutes les broches
  Serial.println("\n--- Inversion du port (togglePort) ---");
  pcf.writePort(0b10101010); // Écrit un motif alterné
  delay(1000);

  pcf.togglePort(); // Inverse tous les bits
  delay(1000);

  // 3. Lire la valeur du port
  uint8_t readValue = pcf.readPortValue();
  Serial.print("Valeur lue sur le port : 0b");
  Serial.println(readValue, BIN);
  delay(2000);
}