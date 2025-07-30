/**
 * @file DigitalRead.ino
 * @brief Lit l'état d'un bouton sur le PCF8574.
 *
 * Cet exemple montre comment lire l'état d'une entrée numérique (un bouton)
 * connectée à une broche du PCF8574. La broche est configurée en entrée,
 * ce qui active la résistance de rappel (pull-up) interne.
 *
 * Matériel requis :
 * - Une carte Arduino ou compatible
 * - Un expandeur de port PCF8574
 * - Un bouton poussoir
 * - Des câbles de connexion
 *
 * Connexions :
 * - VCC du PCF8574 -> 5V ou 3.3V de la carte
 * - GND du PCF8574 -> GND de la carte
 * - SDA du PCF8574 -> Broche SDA de la carte
 * - SCL du PCF8574 -> Broche SCL de la carte
 * - Broche P7 du PCF8574 -> Une patte du bouton
 * - L'autre patte du bouton -> GND
 */

#include <Arduino.h>
#include <Wire.h>
#include "PCF8574.h"

// Adresse I2C du PCF8574.
const uint8_t PCF8574_ADDRESS = 0x20;

// Broche du PCF8574 à laquelle le bouton est connecté.
const uint8_t BUTTON_PIN = 7;

// Crée une instance de la bibliothèque.
PCF8574 pcf(PCF8574_ADDRESS);

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Exemple DigitalRead pour PCF8574");

  pcf.begin();

  // Configure la broche du bouton en entrée.
  // Cela active la résistance de rappel (pull-up) interne du PCF8574.
  pcf.pinMode(BUTTON_PIN, INPUT);

  Serial.println("Appuyez sur le bouton connecté à P7...");
}

void loop() {
  uint8_t buttonState;
  pcf.read(BUTTON_PIN, &buttonState);

  Serial.print("État du bouton : ");
  Serial.println(buttonState == LOW ? "APPUYE" : "Relâché");

  delay(100);
}