/**
 * @file ReadMultiple.ino
 * @brief Démontre comment lire plusieurs broches en une seule fois.
 *
 * Cet exemple montre comment utiliser un masque binaire pour lire l'état
 * de plusieurs broches spécifiques simultanément, sans avoir à lire
 * chaque broche individuellement.
 *
 * Matériel requis :
 * - Une carte Arduino ou compatible.
 * - Un expandeur de port PCF8574.
 * - Deux boutons poussoirs.
 *
 * Connexions :
 * - VCC du PCF8574 -> 5V ou 3.3V
 * - GND du PCF8574 -> GND
 * - SDA du PCF8574 -> Broche SDA de la carte
 * - SCL du PCF8574 -> Broche SCL de la carte
 * - Broche P0 du PCF8574 -> Une patte du bouton 1
 * - Broche P7 du PCF8574 -> Une patte du bouton 2
 * - Les autres pattes des boutons -> GND
 */

#include <Arduino.h>
#include <Wire.h>
#include "PCF8574.h"

// Adresse I2C du PCF8574.
const uint8_t PCF8574_ADDRESS = 0x20;

// Définir les broches pour les boutons.
const uint8_t BUTTON1_PIN = 0;
const uint8_t BUTTON2_PIN = 7;

// Créer un masque pour lire uniquement ces deux broches.
// 1 << BUTTON1_PIN  -> 0b00000001
// 1 << BUTTON2_PIN  -> 0b10000000
// Le masque final est -> 0b10000001
const uint8_t READ_MASK = (1 << BUTTON1_PIN) | (1 << BUTTON2_PIN);

// Crée une instance de la bibliothèque.
PCF8574 pcf(PCF8574_ADDRESS);

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Exemple ReadMultiple pour PCF8574");

  // Option: broches I2C personnalisées (ESP32/ESP8266)
  // #if defined(ARDUINO_ARCH_ESP32) || defined(ESP8266)
  //   pcf.setPins(21, 22);
  // #endif

  pcf.begin();

  // Configurer les broches des boutons en entrée (active les pull-ups internes).
  pcf.pinMode(BUTTON1_PIN, INPUT);
  pcf.pinMode(BUTTON2_PIN, INPUT);

  Serial.println("Appuyez sur les boutons connectés à P0 et P7...");
}

void loop() {
  // Lit l'état des broches définies par le masque.
  uint8_t buttonStates = pcf.readMultiple(READ_MASK);

  // Vérifie l'état de chaque bouton en utilisant le résultat masqué.
  // Note : Un bouton appuyé connecte la broche à GND, donc l'état est LOW (0).
  bool button1_pressed = !(buttonStates & (1 << BUTTON1_PIN));
  bool button2_pressed = !(buttonStates & (1 << BUTTON2_PIN));

  Serial.print("Bouton 1 (P0): ");
  Serial.print(button1_pressed ? "APPUYE" : "Relâché");
  Serial.print(" | Bouton 2 (P7): ");
  Serial.println(button2_pressed ? "APPUYE" : "Relâché");

  delay(200);
}