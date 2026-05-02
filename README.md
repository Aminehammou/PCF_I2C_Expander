# Bibliothèque d'Extension d'E/S I2C pour PCF8574 / PCF8575

<p align="left">
  <!-- Badge de la dernière version (Release). Pensez à créer une "Release" sur GitHub ! -->
  <a href="https://github.com/Aminehammou/PCF_I2C_Expander/releases/latest"><img alt="Release" src="https://img.shields.io/github/v/release/Aminehammou/PCF_I2C_Expander?style=for-the-badge&logo=github"></a>
  <!-- Badge de la licence. Il pointe déjà correctement vers votre fichier LICENSE. -->
  <a href="./LICENSE"><img alt="Licence" src="https://img.shields.io/badge/licence-MIT-green.svg?style=for-the-badge"></a>
  <!-- Badge de compatibilité Arduino. Il pointe vers la page de votre projet. -->
  <a href="https://github.com/Aminehammou/PCF_I2C_Expander"><img alt="Arduino" src="https://img.shields.io/badge/Arduino-Compatible-00979D.svg?style=for-the-badge&logo=arduino"></a>
</p>

Une bibliothèque C++ pour Arduino, générique et performante, pour contrôler les extenseurs de port I2C **PCF8574 (8 bits)** et **PCF8575 (16 bits)**.

## 📖 Description

Cette bibliothèque fournit une interface simple et efficace pour étendre le nombre d'entrées/sorties de votre microcontrôleur (Arduino, ESP8266, ESP32, etc.) en utilisant les populaires puces PCF8574 ou PCF8575.

Grâce à l'utilisation de templates C++, la même base de code gère de manière transparente les deux versions du composant, rendant votre code plus propre et facilement adaptable.

## ✨ Fonctionnalités Clés

- **Support unifié :** Une seule bibliothèque pour le PCF8574 (8 broches) et le PCF8575 (16 broches).
- **Interface familière :** Utilise des fonctions de style Arduino comme `pinMode()`, `digitalWrite()` et `digitalRead()`.
- **Approche Orientée Objet :** Obtenez des objets `DigitalPin` pour manipuler chaque broche individuellement de manière claire et lisible.
- **Performances optimisées :** Un cache interne de l'état des sorties minimise les transactions sur le bus I2C.
- **Détection de Fronts :** Fonctions `risingEdge()` et `fallingEdge()` pour détecter les changements d'état sans effort.
- **Gestion de port complète :** Lisez ou écrivez l'état de toutes les broches en une seule opération.
- **Détection de composant :** Une fonction `isConnected()` permet de vérifier si le composant répond sur le bus I2C.

## 🚀 Installation

1. Ouvrez l'IDE Arduino.
2. Allez dans `Croquis` > `Inclure une bibliothèque` > `Gérer les bibliothèques...`.
3. Recherchez `PCF_I2C_Expander` (une fois publié) et cliquez sur "Installer".

**Installation manuelle :**

Téléchargez ce dépôt sous forme de fichier ZIP. Dans l'IDE Arduino, allez dans `Croquis` > `Inclure une bibliothèque` > `Ajouter la bibliothèque .ZIP...` et sélectionnez le fichier que vous venez de télécharger.

## 💡 Utilisation

L'utilisation est conçue pour être simple. Incluez l'en-tête principal, instanciez l'objet correspondant à votre puce, et utilisez-le !

### Exemple 1 : Faire clignoter une LED avec un PCF8574 (8 bits)

```cpp
#include <Arduino.h>
#include <Wire.h>
#include "PCF_I2C_Expander.h" // Inclure la bibliothèque générique

// Créez une instance pour un PCF8574 à l'adresse I2C 0x20
PCF8574_Expander pcf(0x20);

const int LED_PIN = 4; // Utiliser la broche P4 du PCF8574

void setup() {
  Serial.begin(115200);

  // Option: broches I2C personnalisées (ESP32/ESP8266)
  // pcf.setPins(21, 22); // SDA=21, SCL=22 sur ESP32

  pcf.begin(); // Initialise la lib et écrit l'état initial

  // Vérifiez si le composant est bien connecté
  if (!pcf.isConnected()) {
    Serial.println("Erreur: PCF8574 non trouvé à l'adresse 0x20 !");
    while (1);
  }

  Serial.println("PCF8574 trouvé !");

  // Configurez la broche P4 en sortie
  pcf.pinMode(LED_PIN, OUTPUT);
}

void loop() {
  Serial.println("LED ON");
  pcf.write(LED_PIN, HIGH); // Allume la LED
  delay(1000);

  Serial.println("LED OFF");
  pcf.write(LED_PIN, LOW);  // Éteint la LED
  delay(1000);
}
```

### Exemple 2 : Lire un bouton avec un PCF8575 (16 bits)

Le passage à un PCF8575 est trivial. Il suffit d'utiliser `PCF8575_Expander`.

```cpp
#include <Arduino.h>
#include <Wire.h>
#include "PCF_I2C_Expander.h"

// Créez une instance pour un PCF8575 à l'adresse I2C 0x21
PCF8575_Expander pcf(0x21);

const int BUTTON_PIN = 10; // Utiliser la broche P10 (11ème broche)
const int LED_PIN = 1;     // Utiliser la broche P1

void setup() {
  Serial.begin(115200);

  // Option: broches I2C personnalisées (ESP32/ESP8266)
  // pcf.setPins(21, 22); // SDA=21, SCL=22

  pcf.begin();

  if (!pcf.isConnected()) {
    Serial.println("Erreur: PCF8575 non trouvé !");
    while (1);
  }
  
  // Configurer la broche du bouton en entrée (avec pull-up interne activé)
  pcf.pinMode(BUTTON_PIN, INPUT);
  // Configurer la broche de la LED en sortie
  pcf.pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // Lit l'état du bouton. La logique est inversée à cause du pull-up.
  int buttonState = pcf.read(BUTTON_PIN);

  // Vérifiez si la lecture a réussi
  if (buttonState < 0) {
    Serial.println("Erreur de lecture I2C !");
    return;
  }

  if (buttonState == LOW) { // Si le bouton est pressé
    pcf.write(LED_PIN, HIGH); // Allume la LED
  } else {
    pcf.write(LED_PIN, LOW);  // Éteint la LED
  }
  
  delay(50); // Anti-rebond simple
}
```

### Exemple 3 : Utilisation Orientée Objet des Broches

Pour un code plus structuré, vous pouvez obtenir un objet pour chaque broche.

```cpp
#include "PCF_I2C_Expander.h"

PCF8574_Expander expander(0x20);

// Obtenez des objets pour les broches que vous voulez utiliser
auto led = expander.getPin(7);
auto relay = expander.getPin(3);

void setup() {
  Wire.begin();
  
  led.pinMode(OUTPUT);
  relay.pinMode(OUTPUT);
}

void loop() {
  led.toggle(); // Fait clignoter la LED
  relay.digitalWrite(HIGH);
  delay(500);
  relay.digitalWrite(LOW);
  delay(500);
}
```

## ⚙️ Documentation de l'API

### Classe Principale

- `PCF8574_Expander(address)` / `PCF8575_Expander(address)`: Constructeur.
- `void begin()`: Initialise la communication. Si des broches personnalisées ont été définies via `setPins(sda, scl)`, elles sont utilisées sur ESP32/ESP8266.
- `bool isConnected()`: Vérifie la présence du composant.
- `PCF_Status pinMode(pin, mode)`: Définit une broche en `INPUT` ou `OUTPUT`. En `OUTPUT`, ne modifie pas le niveau courant (évite tout glitch); utilisez `write()` pour définir HIGH/LOW.
- `PCF_Status write(pin, value)`: Écrit `HIGH` ou `LOW` sur une broche.
- `int read(pin)`: Lit l'état d'une broche. Retourne `HIGH` (1), `LOW` (0), ou `-1` en cas d'erreur.
- `PCF_Status toggle(pin)`: Inverse l'état d'une broche.
- `bool risingEdge(pin)`: Détecte un front montant. Retourne `true` si un front montant est détecté, `false` sinon.
- `bool fallingEdge(pin)`: Détecte un front descendant. Retourne `true` si un front descendant est détecté, `false` sinon.
- `PCF_Status writePort(value)`: Écrit une valeur sur toutes les broches (8 ou 16 bits).
- `PortType readPortValue()`: Lit la valeur de toutes les broches. En cas d’erreur bus, retourne 0.
- `void setWire(TwoWire&)`: Sélectionne un autre bus I2C (ex: `Wire1`). À appeler avant `begin()`.
- `void setPins(int sda, int scl)`: Définit des broches SDA/SCL personnalisées (ESP32/ESP8266). À appeler avant `begin()`.
- `PCF_Expander_DigitalPin getPin(pin)`: Retourne un objet pour manipuler une broche.

## 📝 Notes spécifiques PCF8574/PCF8575

- **Mode OUTPUT (anti-glitch)**: `pinMode(pin, OUTPUT)` ne modifie plus le niveau de la broche; définissez explicitement l’état avec `write(pin, HIGH/LOW)`.
- **Mode INPUT (pull-up)**: `pinMode(pin, INPUT)` écrit `1` sur le bit afin d’activer la résistance de rappel interne du PCF.
- **Ordre des octets PCF8575**: lecture/écriture sur 16 bits envoie/reçoit d’abord le LSB (P00–P07), puis le MSB (P10–P17).
- **Choix du bus I2C**: utilisez `setWire(TwoWire&)` (ex: `Wire1` sur ESP32) avant `begin()` si vous n’utilisez pas le bus par défaut.
- **Vérification de bornes**: si `pin` est hors plage, `pinMode/write/toggle` retournent `PCF_ERROR`, et `read(pin)` retourne `-1`.
- **Gestion d’erreur de lecture**: 
  - `read(pin)` et `getPin(pin).digitalRead()` retournent `-1` en cas d'erreur de communication.
  - `readPortValue()` retourne `0` en cas d’erreur bus; pour une gestion robuste, préférez `readPort(&value)` qui retourne un statut `PCF_Status`.

### Exemple anti-glitch (recommandé)
```cpp
PCF8574_Expander io(0x20);
Wire.begin();
io.begin();
// Configurer en sortie sans changer l’état, puis fixer le niveau voulu
io.pinMode(3, OUTPUT);
io.write(3, HIGH);
```

## 🤝 Contribuer

Les contributions sont les bienvenues ! Si vous souhaitez améliorer cette bibliothèque, n'hésitez pas à ouvrir une "issue" pour discuter de vos idées ou à soumettre une "pull request".

## 📜 Licence

Ce projet est distribué sous la licence MIT. Voir le fichier `LICENSE` pour plus de détails.
