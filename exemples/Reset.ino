/**
 * @file Reset.ino
 * @brief Démontre comment réinitialiser le PCF8574 à son état par défaut.
 *
 * Cet exemple montre comment utiliser la fonction reset() pour ramener
 * toutes les broches du PCF8574 à leur état initial (entrées, niveau HAUT).
 *
 * Scénario :
 * 1. Une LED sur la broche P0 est allumée (en la mettant à l'état BAS).
 * 2. Après un délai, la fonction reset() est appelée.
 * 3. La broche P0 retourne à l'état HAUT (entrée), ce qui éteint la LED.
 *
 * Matériel requis :
 * - Une carte Arduino ou compatible.
 * - Un expandeur de port PCF8574.
 * - Une LED et sa résistance.
 *
 * Connexions :
 * - VCC du PCF8574 -> 5V ou 3.3V
 * - GND du PCF8574 -> GND
 * - SDA du PCF8574 -> Broche SDA de la carte
 * - SCL du PCF8574 -> Broche SCL de la carte
 * - Broche P0 du PCF8574 -> Anode de la LED -> Résistance -> GND
 */

#include <Arduino.h>
#include <Wire.h>
#include "PCF8574.h"

const uint8_t PCF8574_ADDRESS = 0x20;
const uint8_t LED_PIN = 0;

PCF8574 pcf(PCF8574_ADDRESS);

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Exemple de réinitialisation pour PCF8574");

  pcf.begin();
  if (!pcf.isConnected()) {
    Serial.println("Erreur: PCF8574 non trouvé !");
    while(1);
  }
}

void loop() {
  Serial.println("Allumage de la LED (P0 = LOW)...");
  pcf.write(LED_PIN, LOW);
  delay(3000);

  Serial.println("Réinitialisation du PCF8574...");
  pcf.reset();
  Serial.println("La LED devrait être éteinte (P0 est maintenant une entrée, état HAUT).");
  delay(3000);
}