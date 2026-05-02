# Notes spécifiques à PCF8575 (16 bits)

Ce document complète le README principal en détaillant les aspects propres au **PCF8575** (port 16 bits).

## Mode des broches

- **INPUT (pull-up activé)**
  - `pinMode(pin, INPUT)` écrit `1` sur le bit correspondant (chaque bit à `1` configure la broche en entrée avec pull-up interne activé).
  - `read(pin)` retourne `HIGH` si la broche est tirée au niveau haut (inactif), `LOW` si reliée à la masse (bouton pressé, etc.).

- **OUTPUT (anti-glitch)**
  - `pinMode(pin, OUTPUT)` ne modifie pas l’état courant; utilisez `write(pin, HIGH/LOW)` pour fixer explicitement le niveau.
  - Évite tout changement de niveau involontaire lors du passage en sortie.

## Mapping des broches et ordre des octets

- Le port est sur **16 bits**: index `0..15`.
- **Ordre des octets en I2C (lecture/écriture)**:
  - LSB en premier (bits `0..7`) → correspondent à `P00–P07`.
  - MSB ensuite (bits `8..15`) → correspondent à `P10–P17`.
- Exemples:
  - `writePort(0x00FF)` → LSB = `0xFF` (P00–P07 = 1), MSB = `0x00` (P10–P17 = 0).
  - `writePort(0xA5A5)` → LSB = `0xA5`, MSB = `0xA5`.

## Séquence d’initialisation recommandée (anti-glitch)

```cpp
#include <Wire.h>
#include "PCF_I2C_Expander.h"

PCF8575_Expander io(0x21);

void setup() {
  // Option: broches I2C personnalisées (ESP32/ESP8266)
  // io.setPins(21, 22);

  io.begin();
  // Configurer des sorties sans changer d’abord l’état, puis définir explicitement le niveau
  io.pinMode(2, OUTPUT);
  io.write(2, HIGH);
  io.pinMode(10, OUTPUT);
  io.write(10, LOW);
}
```

## Opérations de port 16 bits

- **Écriture complète**:
  ```cpp
  io.writePort(0x0F0F); // P00–P03 = 1, P04–P07 = 0, P10–P13 = 1, P14–P17 = 0
  ```
- **Lecture robuste**:
  ```cpp
  uint16_t v = 0;
  if (io.readPort(&v) == PCF_OK) {
    // Utiliser v (16 bits)
  } else {
    // Gérer l’erreur bus
  }
  ```

## Conseils pratiques

- **Sélection du bus I2C**: `io.setWire(Wire1);` avant `begin()` si vous utilisez un bus alternatif (ESP32, etc.).
- **Vérifs de bornes**: index de broche valide `0..15`; hors plage → `PCF_ERROR` (ou `LOW` pour `read`).
- **Pull-ups externes**: comme pour toute liaison I2C, SDA/SCL nécessitent des résistances de tirage adaptées.

## Dépannage rapide

- **Rien ne répond à l’adresse**: vérifier SDA/SCL, pull-ups, alimentation, adresse (A0–A2), vitesse I2C (`setClock`).
- **Lecture incohérente**: confirmer l’ordre LSB→MSB, et que les broches en entrée ont bien été mises à `1`.
- **Glitch lors du passage en sortie**: assurez-vous de définir le niveau après `pinMode(OUTPUT)` via `write()`.