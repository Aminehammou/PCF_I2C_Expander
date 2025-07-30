/**
 * @file ReadOutputCache.ino
 * @brief Démontre comment lire le cache de sortie interne.
 *
 * Cet exemple montre comment utiliser la fonction readOutputCache() pour
 * obtenir l'état que la bibliothèque pense être celui des broches de sortie,
 * sans effectuer de transaction I2C.
 *
 * Scénario :
 * 1. On écrit une valeur sur le port.
 * 2. On lit le cache interne pour vérifier que la bibliothèque a bien enregistré cet état.
 * 3. On lit la valeur réelle sur le port pour confirmer que les deux correspondent.
 *
 * Cas d'usage :
 * Cela peut être utile si vous avez besoin de connaître l'état de sortie
 * actuel dans votre logique de programme sans vouloir interroger le bus I2C,
 * ce qui est plus rapide et évite du trafic inutile.
 */

#include <Arduino.h>
#include <Wire.h>
#include "PCF8574.h"

const uint8_t PCF8574_ADDRESS = 0x20;
PCF8574 pcf(PCF8574_ADDRESS);

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Exemple ReadOutputCache pour PCF8574");

  pcf.begin();
  if (!pcf.isConnected()) {
    Serial.println("Erreur: PCF8574 non trouvé !");
    while(1);
  }

  // 1. Définir un état de sortie connu.
  uint8_t testValue = 0b11001010;
  Serial.print("Écriture de la valeur sur le port : 0b");
  Serial.println(testValue, BIN);
  pcf.writePort(testValue);
  delay(100);

  // 2. Lire la valeur depuis le cache interne (sans I2C).
  uint8_t cachedValue = pcf.readOutputCache();
  Serial.print("Valeur lue depuis le cache interne : 0b");
  Serial.println(cachedValue, BIN);

  // 3. Lire la valeur réelle depuis le composant (avec I2C).
  uint8_t actualValue = pcf.readPortValue();
  Serial.print("Valeur réelle lue depuis le port : 0b");
  Serial.println(actualValue, BIN);

  if (cachedValue == actualValue) {
    Serial.println("Succès : La valeur du cache correspond à la valeur réelle du port.");
  } else {
    Serial.println("Erreur : La valeur du cache ne correspond pas à la valeur réelle du port.");
  }
}

void loop() {
  // La boucle principale est vide.
}