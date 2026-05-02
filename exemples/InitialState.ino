/**
 * @file InitialState.ino
 * @brief Démontre comment définir un état initial pour le PCF8574.
 *
 * Cet exemple montre comment utiliser le constructeur de la bibliothèque
 * pour définir un état de démarrage personnalisé pour les broches du PCF8574.
 *
 * Scénario :
 * - Un premier expandeur est initialisé avec l'état par défaut (toutes les broches en entrée).
 * - Un deuxième expandeur est initialisé avec un état personnalisé : les broches
 *   P0-P3 sont des sorties à l'état BAS, et P4-P7 sont des entrées.
 *
 * Matériel requis :
 * - Une carte Arduino ou compatible.
 * - Deux expandeurs PCF8574.
 *
 * Connexions :
 * - Les broches SDA, SCL, VCC, et GND de chaque PCF8574 sont connectées en parallèle.
 * - PCF8574 #1 : A0, A1, A2 connectées à GND (Adresse = 0x20).
 * - PCF8574 #2 : A0 connectée à VCC, A1 et A2 à GND (Adresse = 0x21).
 */

#include <Arduino.h>
#include <Wire.h>
#include "PCF8574.h"

// Définir les adresses uniques pour chaque expandeur.
const uint8_t PCF_ADDRESS_1 = 0x20;
const uint8_t PCF_ADDRESS_2 = 0x21;

// Définir un état initial personnalisé pour le deuxième expandeur.
// 0b11110000 -> P0-P3 = LOW (sorties), P4-P7 = HIGH (entrées)
const uint8_t CUSTOM_INITIAL_STATE = 0b11110000;

// Crée une instance pour le premier expandeur avec l'état par défaut (0xFF).
PCF8574 pcf1(PCF_ADDRESS_1);

// Crée une instance pour le deuxième expandeur avec un état initial personnalisé.
PCF8574 pcf2(PCF_ADDRESS_2, CUSTOM_INITIAL_STATE);

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Exemple d'état initial pour PCF8574");

  // Option: broches I2C personnalisées (ESP32/ESP8266)
  // #if defined(ARDUINO_ARCH_ESP32) || defined(ESP8266)
  //   pcf1.setPins(21, 22);
  //   pcf2.setPins(21, 22);
  // #endif

  // Initialiser les deux expandeurs.
  pcf1.begin();
  pcf2.begin();

  // Vérifier la connexion pour chaque expandeur.
  if (!pcf1.isConnected() || !pcf2.isConnected()) {
    Serial.println("Erreur: Un ou plusieurs PCF8574 non trouvés !");
    while(1);
  }

  Serial.println("Les deux expandeurs sont connectés.");

  // Lire et afficher l'état initial de chaque expandeur.
  uint8_t state1 = pcf1.readPortValue();
  uint8_t state2 = pcf2.readPortValue();

  Serial.print("État initial de l'expandeur 1 (défaut) : 0b");
  Serial.println(state1, BIN);

  Serial.print("État initial de l'expandeur 2 (personnalisé) : 0b");
  Serial.println(state2, BIN);
}

void loop() {
  // La boucle principale est vide, tout se passe dans le setup.
}