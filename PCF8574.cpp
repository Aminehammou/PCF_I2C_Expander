/**
 * @file PCF8574.cpp
 * @brief Fichier d'implémentation pour la bibliothèque du PCF8574.
 */

#include "PCF8574.h"
#include <Wire.h>

/**
 * @brief Constructeur pour la classe PCF8574.
 * @param address L'adresse I2C du composant.
 * @param initialState L'état initial du port.
 *
 * Initialise l'objet avec l'adresse I2C fournie et définit l'état initial
 * du port. La valeur par défaut est 0xFF, ce qui configure toutes les broches
 * comme des entrées avec leurs résistances de rappel (pull-up) internes activées.
 */
PCF8574::PCF8574(uint8_t address, uint8_t initialState) : _address(address), _portState(initialState) {}

/**
 * @brief Initialise le bus I2C et définit l'état initial de l'expandeur.
 * Doit être appelée dans la fonction `setup()` de votre sketch.
 */
void PCF8574::begin() {
  Wire.begin();
  // Écrit l'état par défaut sur le composant pour s'assurer que toutes les broches sont à l'état haut (entrées)
  writePort(_portState);
}

/**
 * @brief Définit la vitesse d'horloge du bus I2C.
 * @param clockSpeed La vitesse d'horloge en Hz (par exemple, 100000 pour 100kHz, 400000 pour 400kHz).
 */
void PCF8574::setClock(uint32_t clockSpeed) {
  Wire.setClock(clockSpeed);
}

/**
 * @brief Simule une réinitialisation logicielle en écrivant 0xFF sur le port.
 * Cela configure toutes les broches en entrée, ce qui est l'état par défaut au démarrage.
 */
PCF8574_Status PCF8574::reset() {
  return writePort(0xFF);
}

/**
 * @brief Configure une broche en tant qu'entrée ou sortie.
 * @param pin La broche à configurer (0-7).
 * @param mode Le mode (INPUT ou OUTPUT).
 * @return PCF8574_OK en cas de succès, PCF8574_ERROR en cas d'échec.
 *
 * Pour le PCF8574, il n'y a pas de registre de direction dédié. Pour utiliser une broche
 * comme entrée, vous devez lui écrire un '1'. Cette bibliothèque gère cela en
 * définissant le bit correspondant dans le cache interne (`_portState`) lorsque
 * le mode `INPUT` est sélectionné.
 */
PCF8574_Status PCF8574::pinMode(uint8_t pin, uint8_t mode) {
  if (pin > 7) return PCF8574_ERROR;

  if (mode == INPUT) {
    _portState |= (1 << pin);
  } else {
    // Pour le mode OUTPUT, aucun changement n'est nécessaire ici dans le cache d'état.
    // Le niveau de la broche sera défini par le prochain appel à write().
  }
  return writePort(_portState);
}

/**
 * @brief Écrit une valeur sur une broche spécifique.
 * @param pin La broche sur laquelle écrire (0-7).
 * @param value La valeur à écrire (HIGH ou LOW).
 * @return PCF8574_OK en cas de succès, PCF8574_ERROR en cas d'échec.
 *
 * Cette fonction modifie le cache d'état interne puis écrit
 * l'intégralité du cache de 8 bits sur le PCF8574.
 */
PCF8574_Status PCF8574::write(uint8_t pin, uint8_t value) {
  if (pin > 7) return PCF8574_ERROR;

  if (value == HIGH) {
    _portState |= (1 << pin); // Met le bit de la broche spécifiée à 1
  } else {
    _portState &= ~(1 << pin); // Met le bit de la broche spécifiée à 0
  }
  return writePort(_portState);
}

/**
 * @brief Inverse l'état d'une seule broche.
 * @param pin La broche à inverser (0-7).
 * @return PCF8574_OK en cas de succès, PCF8574_ERROR en cas d'échec.
 */
PCF8574_Status PCF8574::toggle(uint8_t pin) {
  if (pin > 7) return PCF8574_ERROR;

  _portState ^= (1 << pin); // Utilise XOR pour inverser le bit
  return writePort(_portState);
}

/**
 * @brief Inverse l'état de toutes les 8 broches en même temps.
 * @return PCF8574_OK en cas de succès, PCF8574_ERROR en cas d'échec.
 */
PCF8574_Status PCF8574::togglePort() {
  _portState ^= 0xFF; // Utilise XOR avec 0xFF pour inverser tous les 8 bits
  return writePort(_portState);
}

/**
 * @brief Lit la valeur d'une seule broche.
 * @param pin La broche à lire (0-7).
 * @param value Pointeur vers une variable où la valeur sera stockée.
 * @return PCF8574_OK en cas de succès, PCF8574_ERROR en cas d'échec.
 */
PCF8574_Status PCF8574::read(uint8_t pin, uint8_t *value) {
  if (pin > 7) return PCF8574_ERROR;

  uint8_t portValue;
  // D'abord, lire l'intégralité du port
  PCF8574_Status status = readPort(&portValue);

  if (status == PCF8574_OK) {
    // Si la lecture a réussi, isoler le bit pour la broche demandée
    *value = (portValue >> pin) & 1;
    return PCF8574_OK;
  } else {
    // Si la lecture a échoué, retourner le statut d'erreur
    return status;
  }
}

/**
 * @brief Écrit une valeur de 8 bits sur l'ensemble du port.
 * @param value La valeur de 8 bits à écrire.
 * @return PCF8574_OK en cas de succès, PCF8574_ERROR en cas d'échec.
 */
PCF8574_Status PCF8574::writePort(uint8_t value) {
  _portState = value; // Met à jour le cache interne
  Wire.beginTransmission(_address);
  Wire.write(value);
  uint8_t result = Wire.endTransmission();

  // endTransmission retourne 0 en cas de succès
  return (result == 0) ? PCF8574_OK : PCF8574_ERROR;
}

/**
 * @brief Lit la valeur de 8 bits de l'ensemble du port.
 * @param value Pointeur vers une variable où la valeur de 8 bits sera stockée.
 * @return PCF8574_OK en cas de succès, PCF8574_ERROR en cas d'échec.
 */
PCF8574_Status PCF8574::readPort(uint8_t *value) {
  // Demande 1 octet au périphérique I2C
  Wire.requestFrom(_address, (uint8_t)1);
  if (Wire.available()) {
    *value = Wire.read();
    return PCF8574_OK;
  }
  // Si aucun octet n'est disponible, il y a eu une erreur
  return PCF8574_ERROR;
}

/**
 * @brief Lit la valeur de 8 bits du port et la retourne directement.
 * @return La valeur de 8 bits du port. Retourne 0 en cas d'erreur de communication.
 * @note Ceci est une fonction de convenance. Pour une gestion d'erreur robuste,
 *       il est recommandé d'utiliser `readPort(uint8_t *value)`.
 */
uint8_t PCF8574::readPortValue() {
  uint8_t value = 0;
  // Cet appel tente de lire le port. S'il échoue, le statut est
  // ignoré, et la fonction retournera la valeur initiale de 0.
  readPort(&value);
  return value;
}

/**
 * @brief Lit la valeur du cache de sortie interne.
 * @return La valeur de 8 bits du cache de sortie.
 *
 * Cette fonction est utile pour connaître l'état que la bibliothèque
 * croit être celui des broches de sortie, sans effectuer de communication I2C.
 */
uint8_t PCF8574::readOutputCache() {
  return _portState;
}

/**
 * @brief Vérifie si un composant est présent et répond à l'adresse I2C spécifiée.
 * @return true si le composant a accusé réception, false sinon.
 */
bool PCF8574::isConnected() {
  Wire.beginTransmission(_address);
  // endTransmission retourne 0 en cas de succès (le composant a répondu).
  return (Wire.endTransmission() == 0);
}

/**
 * @brief Lit l'état de plusieurs broches en utilisant un masque.
 * @param mask Le masque des broches à lire.
 * @param value Pointeur pour stocker le résultat.
 * @return PCF8574_OK en cas de succès.
 */
PCF8574_Status PCF8574::readMultiple(uint8_t mask, uint8_t *value) {
  PCF8574_Status status = readPort(value);
  if (status == PCF8574_OK) {
    *value &= mask; // Applique le masque au résultat
  }
  return status;
}

/**
 * @brief Lit l'état de plusieurs broches et retourne le résultat.
 * @param mask Le masque des broches à lire.
 * @return La valeur des broches masquées.
 */
uint8_t PCF8574::readMultiple(uint8_t mask) {
  uint8_t value = 0;
  readMultiple(mask, &value);
  return value;
}

// =========================================================================
// Implémentation de la classe PCF8574_DigitalPin
// =========================================================================

/**
 * @brief Constructeur pour un objet PCF8574_DigitalPin.
 * @param parent Référence vers l'objet PCF8574 parent.
 * @param pin Le numéro de la broche (0-7).
 */
PCF8574_DigitalPin::PCF8574_DigitalPin(PCF8574& parent, uint8_t pin) : _parent(parent), _pin(pin) {}

/**
 * @brief Configure le mode de la broche (INPUT ou OUTPUT).
 */
void PCF8574_DigitalPin::pinMode(uint8_t mode) {
  _parent.pinMode(_pin, mode);
}

/**
 * @brief Écrit une valeur sur la broche.
 */
void PCF8574_DigitalPin::digitalWrite(uint8_t value) {
  _parent.write(_pin, value);
}

/**
 * @brief Lit la valeur de la broche.
 * @return HIGH ou LOW.
 */
uint8_t PCF8574_DigitalPin::digitalRead() {
  uint8_t value = LOW;
  _parent.read(_pin, &value);
  return value;
}

/**
 * @brief Inverse l'état de la broche.
 */
void PCF8574_DigitalPin::toggle() {
  _parent.toggle(_pin);
}
