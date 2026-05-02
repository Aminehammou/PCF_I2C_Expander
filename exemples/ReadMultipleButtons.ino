#include <Arduino.h>
#include "PCF_I2C_Expander.h"

// --- Configuration ---
// Adresse I2C de l'extenseur.
// PCF8574: 0x20-0x27
// PCF8575: 0x20-0x27
const uint8_t PCF_ADDRESS = 0x20;

// Définir les broches connectées aux boutons.
// Pour cet exemple, nous utilisons les 4 premières broches.
const uint8_t BUTTON_PINS[] = {0, 1, 2, 3};
const int NUM_BUTTONS = sizeof(BUTTON_PINS) / sizeof(BUTTON_PINS[0]);

// --- Initialisation de la bibliothèque ---
// Utilisez PCF8574_Expander pour un composant 8 bits.
// Pour un PCF8575 (16 bits), changez la ligne suivante par :
// PCF8575_Expander pcf(PCF_ADDRESS);
PCF8574_Expander pcf(PCF_ADDRESS);

// Variable pour stocker l'état précédent des boutons et éviter les affichages répétés.
// Pour un PCF8575, changez le type en uint16_t.
uint8_t previousPortState = 0xFF;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // Attendre que le port série soit prêt
  }
  Serial.println("Exemple de lecture de plusieurs boutons avec PCF_I2C_Expander");

  // Option: broches I2C personnalisées (ESP32/ESP8266)
  // pcf.setPins(21, 22);

  pcf.begin();

  if (!pcf.isConnected()) {
    Serial.println("Erreur : Extenseur PCF non trouvé à l'adresse spécifiée !");
    while (1);
  }

  // Configurer toutes les broches des boutons en entrée.
  // Le PCF8574/5 a des résistances de pull-up internes.
  // Quand un bouton est pressé (connecté à la masse), la broche passe à LOW.
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pcf.pinMode(BUTTON_PINS[i], INPUT);
  }

  Serial.println("Configuration terminée. Appuyez sur les boutons connectés aux broches P0-P3.");
}

void loop() {
  // Lit l'état de toutes les broches en une seule transaction I2C.
  // C'est la méthode la plus efficace pour lire plusieurs entrées.
  uint8_t currentPortState = pcf.readPortValue(); // Pour un PCF8575, utilisez uint16_t

  // Vérifier si l'état du port a changé depuis la dernière lecture.
  if (currentPortState != previousPortState) {
    for (int i = 0; i < NUM_BUTTONS; i++) {
      uint8_t pin = BUTTON_PINS[i];
      
      bool isPressed = !(currentPortState & (1 << pin));
      bool wasPressed = !(previousPortState & (1 << pin));

      if (isPressed && !wasPressed) {
        Serial.print("Bouton sur la broche P");
        Serial.print(pin);
        Serial.println(" a été pressé !");
      }
    }
    previousPortState = currentPortState;
  }

  // Petite pause pour un "debouncing" simple.
  delay(50); 
}