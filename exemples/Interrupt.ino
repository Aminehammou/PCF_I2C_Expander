/**
 * @file Interrupt.ino
 * @brief Démontre l'utilisation de la broche d'interruption du PCF8574.
 *
 * Cet exemple montre comment utiliser la broche INT du PCF8574 pour détecter
 * un changement d'état sur l'une de ses entrées sans avoir à interroger
 * (poll) constamment le composant.
 *
 * Matériel requis :
 * - Une carte Arduino/ESP32 avec une broche capable de gérer les interruptions.
 * - Un expandeur de port PCF8574.
 * - Un bouton poussoir.
 * - Une LED et sa résistance.
 * - Une résistance de rappel (pull-up) de 10k Ohm pour la ligne INT.
 *
 * Connexions :
 * - VCC du PCF8574 -> 5V ou 3.3V
 * - GND du PCF8574 -> GND
 * - SDA du PCF8574 -> Broche SDA de la carte
 * - SCL du PCF8574 -> Broche SCL de la carte
 * - Broche INT du PCF8574 -> Broche d'interruption de la carte (ex: D2 sur Uno, D15 sur ESP32)
 * - Broche INT du PCF8574 -> Résistance de 10k -> VCC (pull-up externe)
 * - Broche P7 du PCF8574 -> Une patte du bouton
 * - L'autre patte du bouton -> GND
 * - Broche P0 du PCF8574 -> Anode de la LED -> Résistance -> GND
 */

#include <Arduino.h>
#include <Wire.h>
#include "PCF8574.h"

// --- Configuration ---
const uint8_t PCF8574_ADDRESS = 0x20;
const uint8_t BUTTON_PIN = 7;
const uint8_t LED_PIN = 0;

// Définir la broche du microcontrôleur connectée à la broche INT du PCF8574.
// Assurez-vous que cette broche supporte les interruptions.
#if defined(ESP32)
  const int INTERRUPT_PIN = 15;
#else
  const int INTERRUPT_PIN = 2; // Pour Arduino Uno/Nano
#endif

// Crée une instance de la bibliothèque.
PCF8574 pcf(PCF8574_ADDRESS);

// Variable volatile pour être utilisée en toute sécurité dans l'ISR.
volatile bool interruptFlag = false;

/**
 * @brief Routine de Service d'Interruption (ISR).
 * Cette fonction est appelée lorsque la broche d'interruption passe à l'état BAS.
 * Elle ne fait que lever un drapeau. Tout le traitement est effectué dans la boucle principale.
 * L'attribut IRAM_ATTR est spécifique à l'ESP32 pour placer la fonction en RAM pour plus de rapidité.
 */
void IRAM_ATTR onInterrupt() {
  interruptFlag = true;
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Exemple d'interruption pour PCF8574");

  // Option: broches I2C personnalisées (ESP32/ESP8266)
  // #if defined(ARDUINO_ARCH_ESP32) || defined(ESP8266)
  //   pcf.setPins(21, 22);
  // #endif

  pcf.begin();
  pcf.pinMode(BUTTON_PIN, INPUT);

  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), onInterrupt, FALLING);

  Serial.println("Prêt. Appuyez sur le bouton pour déclencher une interruption.");
}

void loop() {
  if (interruptFlag) {
    Serial.println("Interruption détectée !");

    // Important : Lire l'état du port du PCF8574.
    // Cette action réinitialise la broche INT du PCF8574 à l'état HAUT.
    uint8_t portValue = pcf.readPortValue();

    Serial.print("Nouvel état du port : 0b");
    Serial.println(portValue, BIN);

    // Action à effectuer suite à l'interruption, par exemple, inverser une LED.
    pcf.toggle(LED_PIN);

    // Réinitialise le drapeau pour la prochaine interruption.
    interruptFlag = false;
    Serial.println("Interruption traitée. En attente de la prochaine.");
  }
}