/**
 * @file Blink.ino
 * @brief Fait clignoter une LED sur le PCF8574.
 *
 * Cet exemple montre comment utiliser la bibliothèque PCF8574 pour faire
 * clignoter une LED connectée à la broche P0 de l'expandeur.
 *
 * Matériel requis :
 * - Une carte Arduino ou compatible (ESP32, etc.)
 * - Un expandeur de port PCF8574
 * - Une LED
 * - Une résistance (ex: 220 Ohm)
 * - Des câbles de connexion
 *
 * Connexions :
 * - VCC du PCF8574 -> 5V ou 3.3V de la carte
 * - GND du PCF8574 -> GND de la carte
 * - SDA du PCF8574 -> Broche SDA de la carte (A4 sur Uno, 21 sur ESP32)
 * - SCL du PCF8574 -> Broche SCL de la carte (A5 sur Uno, 22 sur ESP32)
 * - Broche P0 du PCF8574 -> Anode (+) de la LED
 * - Cathode (-) de la LED -> Résistance -> GND
 */

#include <Arduino.h>
#include <Wire.h>
#include "PCF8574.h"

// Adresse I2C du PCF8574. Vérifiez la vôtre !
// 0x20 pour le PCF8574, 0x38 pour le PCF8574A.
const uint8_t PCF8574_ADDRESS = 0x20;

// Broche du PCF8574 à laquelle la LED est connectée.
const uint8_t LED_PIN = 0;

// Crée une instance de la bibliothèque.
PCF8574 pcf(PCF8574_ADDRESS);

void setup() {
  Serial.begin(115200);
  while (!Serial); // Attendre que le port série soit prêt (pour les cartes comme Leonardo/Micro)
  Serial.println("Exemple Blink pour PCF8574");

  // Initialise la communication avec le PCF8574.
  pcf.begin();

  // Vérifie si le composant est bien connecté.
  if (!pcf.isConnected()) {
    Serial.println("Erreur: PCF8574 non trouvé à l'adresse spécifiée !");
    Serial.println("Veuillez vérifier le câblage et l'adresse I2C.");
    while (1) {
      delay(100); // Bloque l'exécution.
    }
  }
  Serial.println("Le clignotement commence...");
}

void loop() {
  // Inverse l'état de la broche de la LED.
  pcf.toggle(LED_PIN);
  delay(1000);
}