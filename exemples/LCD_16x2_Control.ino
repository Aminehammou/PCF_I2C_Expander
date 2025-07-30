/**
 * @file LCD_16x2_Control.ino
 * @brief Exemple avancé : Contrôle d'un afficheur LCD 16x2 avec un adaptateur I2C (PCF8574).
 *
 * Cet exemple montre comment piloter un afficheur LCD en mode 4 bits en utilisant
 * uniquement les fonctions de bas niveau de la bibliothèque PCF_I2C_Expander.
 * C'est une démonstration de la flexibilité de la bibliothèque. Pour un usage
 * courant, une bibliothèque dédiée comme LiquidCrystal_I2C est souvent plus simple.
 */
#include <Arduino.h>
#include "PCF_I2C_Expander.h"

// --- Configuration ---
// Adresse I2C de l'adaptateur LCD.
// Généralement 0x27 ou 0x3F. Vérifiez avec un scanner I2C si besoin.
const uint8_t LCD_ADDRESS = 0x27;

// Instance de notre bibliothèque pour le PCF8574
PCF8574_Expander pcf(LCD_ADDRESS);

// --- Mappage des broches du PCF8574 vers l'afficheur LCD ---
// C'est le mappage le plus courant pour les adaptateurs I2C.
const uint8_t PIN_RS = 0; // Register Select (0: Commande, 1: Donnée)
const uint8_t PIN_RW = 1; // Read/Write (toujours à 0 pour l'écriture)
const uint8_t PIN_E  = 2; // Enable (front descendant pour valider les données)
const uint8_t PIN_BL = 3; // Backlight (Rétroéclairage)
const uint8_t PIN_D4 = 4;
const uint8_t PIN_D5 = 5;
const uint8_t PIN_D6 = 6;
const uint8_t PIN_D7 = 7;

// --- Commandes LCD ---
const uint8_t LCD_CLEARDISPLAY = 0x01;
const uint8_t LCD_RETURNHOME = 0x02;
const uint8_t LCD_ENTRYMODESET = 0x04;
const uint8_t LCD_DISPLAYCONTROL = 0x08;
const uint8_t LCD_CURSORSHIFT = 0x10;
const uint8_t LCD_FUNCTIONSET = 0x20;
const uint8_t LCD_SETDDRAMADDR = 0x80;

// --- Drapeaux pour les commandes ---
const uint8_t LCD_ENTRYLEFT = 0x02;
const uint8_t LCD_DISPLAYON = 0x04;
const uint8_t LCD_4BITMODE = 0x00;
const uint8_t LCD_2LINE = 0x08;
const uint8_t LCD_5x8DOTS = 0x00;
const uint8_t LCD_SHIFTLEFT = 0x08;
const uint8_t LCD_SHIFTRIGHT = 0x0C;
const uint8_t LCD_DISPLAYSHIFT = 0x08;


// --- Fonctions bas niveau ---

/**
 * @brief Envoie une impulsion sur la broche Enable pour que le LCD lise les données.
 * @param data L'état actuel du port à maintenir pendant l'impulsion.
 */
void lcd_pulse_enable(uint8_t data) {
  pcf.writePort(data | (1 << PIN_E)); // E = HIGH
  delayMicroseconds(1);
  pcf.writePort(data & ~(1 << PIN_E)); // E = LOW
  delayMicroseconds(50); // Temps d'exécution de la commande
}

/**
 * @brief Écrit 4 bits de données sur les broches D4-D7.
 * @param value Octet contenant les 4 bits à envoyer dans son quartet de poids fort.
 */
void lcd_write_4bits(uint8_t value) {
  // Lit l'état des broches de contrôle depuis le cache pour ne pas les altérer
  uint8_t portValue = pcf.readOutputCache() & 0x0F; 
  // Combine avec les nouvelles données
  portValue |= (value & 0xF0);
  pcf.writePort(portValue);
  lcd_pulse_enable(portValue);
}

/**
 * @brief Envoie un octet complet (commande ou donnée) en deux fois 4 bits.
 * @param value L'octet à envoyer.
 * @param mode L'état de la broche RS (0 pour commande, 1 pour donnée).
 */
void lcd_send(uint8_t value, uint8_t mode) {
  uint8_t high_nibble = value & 0xF0;
  uint8_t low_nibble = (value << 4) & 0xF0;
  
  // Prépare l'état du port avec le mode (RS) et le rétroéclairage
  uint8_t portState = mode | (1 << PIN_BL);
  pcf.writePort(portState);

  lcd_write_4bits(high_nibble | portState);
  lcd_write_4bits(low_nibble | portState);
}

void lcd_command(uint8_t value) {
  lcd_send(value, 0); // RS=0 pour une commande
}

void lcd_write_char(char value) {
  lcd_send(value, (1 << PIN_RS)); // RS=1 pour des données
}

// --- Fonctions haut niveau ---

void lcd_init() {
  pcf.begin();
  pcf.writePort(0x00); // Toutes les broches en sortie, rétroéclairage éteint
  delay(50);

  // Séquence d'initialisation pour le mode 4 bits
  lcd_write_4bits(0x30); delayMicroseconds(4500);
  lcd_write_4bits(0x30); delayMicroseconds(4500);
  lcd_write_4bits(0x30); delayMicroseconds(150);
  lcd_write_4bits(0x20); // Passage en mode 4 bits

  // Configuration finale
  lcd_command(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS);
  lcd_command(LCD_DISPLAYCONTROL | LCD_DISPLAYON);
  lcd_command(LCD_CLEARDISPLAY); delay(2);
  lcd_command(LCD_ENTRYMODESET | LCD_ENTRYLEFT);
}

void lcd_print(const char* str) {
  while (*str) {
    lcd_write_char(*str++);
  }
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
  int row_offsets[] = { 0x00, 0x40 };
  lcd_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void setup() {
  Serial.begin(115200);
  Serial.println("Exemple de contrôle d'un LCD 16x2 via PCF8574");

  lcd_init();
  
  lcd_set_cursor(0, 0);
  lcd_print("PCF_I2C_Expander");
  lcd_set_cursor(0, 1);
  lcd_print("Test LCD OK!");
}

void loop() {
  // Fait défiler le texte vers la gauche toutes les 400ms
  lcd_command(LCD_CURSORSHIFT | LCD_DISPLAYSHIFT | LCD_SHIFTLEFT);
  delay(400);
}