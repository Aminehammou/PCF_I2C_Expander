# Notes spécifiques à PCF8574

Ce document complète le README principal en détaillant les aspects propres au PCF8574 (8 bits).

## Mode des broches

- **INPUT (pull-up activé)**
  - `pinMode(pin, INPUT)` écrit `1` sur le bit correspondant (le PCF8574 utilise un "1" pour configurer la broche en entrée avec pull-up interne activé).
  - Lecture via `read(pin)` retourne `HIGH` si la broche est tirée au niveau haut (non activée), `LOW` si reliée à la masse (bouton pressé, etc.).

- **OUTPUT (anti-glitch)**
  - `pinMode(pin, OUTPUT)` ne modifie pas l’état actuel de la broche; utilisez `write(pin, HIGH/LOW)` pour fixer explicitement le niveau.
  - Cela évite tout changement de niveau involontaire au moment du passage en sortie.

## Séquence d’initialisation recommandée

```cpp
#include <Wire.h>
#include "PCF_I2C_Expander.h"

PCF8574_Expander io(0x20);

void setup() {
  // Option: broches I2C personnalisées (ESP32/ESP8266)
  // io.setPins(21, 22);

  io.begin();
  // Sortie sans glitch, puis fixation du niveau
  io.pinMode(3, OUTPUT);
  io.write(3, HIGH);
}
```

## Remarques d’implémentation

- L’écriture de port (`writePort(uint8_t)`) met à jour le cache interne et effectue une transaction I2C unique.
- La lecture de port (`readPort(uint8_t*)`) lit un octet et renvoie un statut (succès/erreur).
- `readPortValue()` retourne `0` en cas d’erreur bus (convenance). Pour une gestion robuste, préférez `readPort()`.
- `isConnected()` envoie une transmission vide et vérifie l’ACK à l’adresse cible.

## Dépannage rapide

- **Rien ne répond à l’adresse**: vérifier câblage SDA/SCL, pull-ups, alimentation, et adresse (A0-A2).
- **Lecture toujours HIGH en entrée**: s’assurer que `pinMode(pin, INPUT)` a été appelé; le PCF8574 nécessite un `1` écrit pour activer le pull-up.
- **Comportement inattendu lors du passage en OUTPUT**: vérifier que `write()` est appelé après `pinMode(OUTPUT)` (niveau explicite).