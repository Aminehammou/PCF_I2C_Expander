/**
 * @file ObjectSyntax.ino
 * @brief Démontre l'utilisation des broches comme des objets.
 *
 * Cet exemple montre comment utiliser la syntaxe orientée objet pour
 * contrôler des broches individuelles, ce qui rend le code plus lisible
 * et plus proche de la manipulation des broches natives d'Arduino.
 *
 * Matériel requis :
 * - Une carte Arduino ou compatible.
 * - Un expandeur de port PCF8574.
 * - Une LED et sa résistance.
 * - Un bouton poussoir.
 *
 * Connexions :
 * - VCC du PCF8574 -> 5V ou 3.3V
 * - GND du PCF8574 -> GND
 * - SDA du PCF8574 -> Broche SDA de la carte
 * - SCL du PCF8574 -> Broche SCL de la carte
 * - Broche P0 du PCF8574 -> Anode de la LED -> Résistance -> GND
 * - Broche P7 du PCF8574 -> Une patte du bouton -> GND
 */

#include <Arduino.h>
#include <Wire.h>
#include "PCF8574.h"

const uint8_t PCF8574_ADDRESS = 0x20;
PCF8574 pcf(PCF8574_ADDRESS);

// Obtenir des objets pour des broches spécifiques.
// La syntaxe 'auto' est pratique ici.
auto led = pcf.getPin(0);
auto button = pcf.getPin(7);

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Exemple de syntaxe objet pour PCF8574");

  pcf.begin();
  if (!pcf.isConnected()) {
    Serial.println("Erreur: PCF8574 non trouvé !");
    while(1);
  }

  // Configurer les broches en utilisant les objets.
  // Note: OUTPUT ne fait rien de spécial, c'est l'état par défaut pour l'écriture.
  led.pinMode(OUTPUT);
  button.pinMode(INPUT);

  Serial.println("Configuration terminée. Prêt à tester.");
}

void loop() {
  // Lire l'état du bouton et contrôler la LED.
  if (button.digitalRead() == LOW) {
    led.digitalWrite(LOW); // Allumer la LED
  } else {
    led.digitalWrite(HIGH); // Éteindre la LED
  }
}