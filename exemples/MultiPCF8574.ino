/**
 * @file MultiPCF8574.ino
 * @brief Démontre l'utilisation de plusieurs expandeurs PCF8574 sur le même bus I2C.
 *
 * Cet exemple montre comment contrôler deux expandeurs PCF8574 indépendamment.
 * Pour que cela fonctionne, chaque expandeur DOIT avoir une adresse I2C unique.
 *
 * Comment définir les adresses :
 * Le PCF8574 a 3 broches d'adresse (A0, A1, A2). En les connectant à VCC (HIGH)
 * ou à GND (LOW), vous pouvez définir 8 adresses différentes.
 *
 * PCF8574 : plage de 0x20 (A2=L, A1=L, A0=L) à 0x27 (A2=H, A1=H, A0=H).
 * PCF8574A : plage de 0x38 (A2=L, A1=L, A0=L) à 0x3F (A2=H, A1=H, A0=H).
 *
 * Matériel requis :
 * - Une carte Arduino ou compatible.
 * - Deux expandeurs PCF8574 (ou un PCF8574 et un PCF8574A).
 * - (Optionnel) Des LEDs pour visualiser les sorties.
 *
 * Connexions pour cet exemple :
 * - Les broches SDA, SCL, VCC, et GND de chaque PCF8574 sont connectées en parallèle.
 * - PCF8574 #1 : A0, A1, A2 connectées à GND (Adresse = 0x20).
 * - PCF8574 #2 : A0 connectée à VCC, A1 et A2 à GND (Adresse = 0x21).
 *   OU, si vous utilisez un PCF8574A : A0, A1, A2 connectées à GND (Adresse = 0x38).
 */

#include <Arduino.h>
#include <Wire.h>
#include "PCF8574.h"

// Définir les adresses uniques pour chaque expandeur.
const uint8_t PCF_ADDRESS_1 = 0x20;
const uint8_t PCF_ADDRESS_2 = 0x21; // Ou 0x38 pour un PCF8574A

// Créer une instance de la bibliothèque pour chaque expandeur.
PCF8574 pcf1(PCF_ADDRESS_1);
PCF8574 pcf2(PCF_ADDRESS_2);

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Exemple Multi-PCF8574");

  // Initialiser les deux expandeurs.
  pcf1.begin();
  pcf2.begin();

  // Vérifier la connexion pour chaque expandeur.
  if (!pcf1.isConnected()) {
    Serial.println("Erreur: PCF8574 #1 non trouvé !");
    while(1);
  }
  if (!pcf2.isConnected()) {
    Serial.println("Erreur: PCF8574 #2 non trouvé !");
    while(1);
  }

  Serial.println("Les deux expandeurs sont connectés. Démarrage de la démo.");
}

void loop() {
  // Action sur le premier expandeur : faire un chenillard.
  static int led_index = 0;
  pcf1.writePort(1 << led_index);
  led_index = (led_index + 1) % 8;

  // Action sur le deuxième expandeur : faire clignoter la broche P0.
  pcf2.toggle(0);

  delay(200);
}