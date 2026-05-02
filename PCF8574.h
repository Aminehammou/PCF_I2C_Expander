/**
 * @file PCF8574.h
 * @brief Fichier d'en-tête pour la bibliothèque du PCF8574 I2C I/O Expander.
 */

#ifndef PCF8574_H
#define PCF8574_H

#include <Arduino.h>
#include <Wire.h>

/**
 * @brief Codes de statut pour les opérations du PCF8574.
 */
typedef enum {
  PCF8574_OK,     /**< L'opération a réussi. */
  PCF8574_ERROR   /**< Une erreur est survenue pendant l'opération. */
} PCF8574_Status;

// Déclaration anticipée pour permettre à PCF8574_DigitalPin de référencer PCF8574.
class PCF8574;

/**
 * @class PCF8574_DigitalPin
 * @brief Représente une seule broche de l'expandeur PCF8574 comme un objet.
 */
class PCF8574_DigitalPin {
public:
  PCF8574_DigitalPin(PCF8574& parent, uint8_t pin);
  void pinMode(uint8_t mode);
  void digitalWrite(uint8_t value);
  int digitalRead();
  void toggle();

private:
  PCF8574& _parent;
  uint8_t _pin;
};

/**
 * @class PCF8574
 * @brief Bibliothèque Arduino pour l'expandeur d'E/S I2C PCF8574.
 */
class PCF8574 {
public:
  // Permet à PCF8574_DigitalPin d'accéder aux membres privés de PCF8574 si nécessaire.
  friend class PCF8574_DigitalPin;

  /**
   * @brief Constructeur pour un nouvel objet PCF8574.
   * @param address L'adresse I2C du composant.
   * @param initialState L'état initial de 8 bits pour le port (par défaut 0xFF, toutes les broches en entrée).
   */
  PCF8574(uint8_t address, uint8_t initialState = 0xFF);

  /**
   * @brief Initialise la communication I2C.
   * Doit être appelée dans la fonction setup().
   */
  void begin();

  /**
   * @brief Définit la vitesse d'horloge du bus I2C.
   * Pour une vitesse maximale, utilisez 400000 (Fast Mode). La vitesse par défaut est 100000 (Standard Mode).
   * @param clockSpeed La vitesse d'horloge souhaitée en Hz.
   * @warning Cette fonction modifie la vitesse pour l'ensemble du bus I2C, ce qui affecte tous les périphériques connectés.
   * Assurez-vous que tous les appareils sur le bus supportent la vitesse sélectionnée.
   */
  void setClock(uint32_t clockSpeed);

  /**
   * @brief Définit des broches I2C personnalisées (ex: SDA=21, SCL=22 sur ESP32).
   * A appeler avant begin(). Ignoré sur plateformes sans begin(sda,scl).
   */
  void setPins(int sda_pin, int scl_pin);

  /**
   * @brief Réinitialise le PCF8574 à son état par défaut (toutes les broches en entrée).
   * @return PCF8574_OK en cas de succès, PCF8574_ERROR en cas d'échec.
   */
  PCF8574_Status reset();

  /**
   * @brief Configure une broche spécifique pour qu'elle se comporte comme une entrée ou une sortie.
   * Pour le PCF8574, configurer une broche en INPUT écrit un HIGH dessus,
   * activant ainsi la résistance de rappel (pull-up) interne.
   * @param pin Le numéro de la broche à configurer (0-7).
   * @param mode Le mode à définir : INPUT ou OUTPUT.
   * @return PCF8574_OK en cas de succès, PCF8574_ERROR en cas d'échec.
   */
  PCF8574_Status pinMode(uint8_t pin, uint8_t mode);

  /**
   * @brief Écrit une valeur numérique (HIGH ou LOW) sur une broche spécifique.
   * @param pin Le numéro de la broche sur laquelle écrire (0-7).
   * @param value La valeur à écrire : HIGH ou LOW.
   * @return PCF8574_OK en cas de succès, PCF8574_ERROR en cas d'échec.
   */
  PCF8574_Status write(uint8_t pin, uint8_t value);

  /**
   * @brief Inverse l'état d'une broche spécifique (de HIGH à LOW, ou de LOW à HIGH).
   * @param pin Le numéro de la broche à inverser (0-7).
   * @return PCF8574_OK en cas de succès, PCF8574_ERROR en cas d'échec.
   */
  PCF8574_Status toggle(uint8_t pin);

  /**
   * @brief Inverse l'état de toutes les 8 broches simultanément.
   * @return PCF8574_OK en cas de succès, PCF8574_ERROR en cas d'échec.
   */
  PCF8574_Status togglePort();

  /**
   * @brief Lit la valeur numérique d'une broche spécifique.
   * @param pin Le numéro de la broche à lire (0-7).
   * @param value Pointeur vers une variable uint8_t pour stocker la valeur lue (HIGH ou LOW).
   * @return PCF8574_OK en cas de succès, PCF8574_ERROR en cas d'échec.
   */
  PCF8574_Status read(uint8_t pin, uint8_t *value);

  /**
   * @brief Écrit une valeur de 8 bits sur l'ensemble du port.
   * @param value La valeur de 8 bits à écrire sur le port.
   * @return PCF8574_OK en cas de succès, PCF8574_ERROR en cas d'échec.
   */
  PCF8574_Status writePort(uint8_t value);

  /**
   * @brief Lit la valeur de 8 bits de l'ensemble du port.
   * @param value Pointeur vers une variable uint8_t pour stocker la valeur du port de 8 bits.
   * @return PCF8574_OK en cas de succès, PCF8574_ERROR en cas d'échec.
   */
  PCF8574_Status readPort(uint8_t *value);

  /**
   * @brief Lit la valeur de 8 bits de l'ensemble du port et la retourne directement.
   * @note Pour une gestion d'erreur robuste, utilisez plutôt readPort(uint8_t* value).
   * @return La valeur de 8 bits du port. Retourne 0 en cas d'erreur de communication.
   */
  uint8_t readPortValue();

  /**
   * @brief Lit la valeur du cache de sortie interne sans transaction I2C.
   * @return La valeur de 8 bits du cache de sortie.
   */
  uint8_t readOutputCache();

  /**
   * @brief Lit l'état de plusieurs broches en une seule fois en utilisant un masque.
   * @param mask Masque de 8 bits indiquant les broches à lire (ex: 0b00000101 pour P0 et P2).
   * @param value Pointeur vers une variable uint8_t pour stocker le résultat. Seuls les bits du masque seront valides.
   * @return PCF8574_OK en cas de succès, PCF8574_ERROR en cas d'échec.
   */
  PCF8574_Status readMultiple(uint8_t mask, uint8_t *value);

  /**
   * @brief Lit l'état de plusieurs broches et retourne le résultat directement.
   * @param mask Masque de 8 bits indiquant les broches à lire.
   * @note Pour une gestion d'erreur robuste, utilisez plutôt readMultiple(uint8_t mask, uint8_t* value).
   * @return La valeur de 8 bits des broches masquées. Retourne 0 en cas d'erreur.
   */
  uint8_t readMultiple(uint8_t mask);

  /**
   * @brief Vérifie si un composant PCF8574 est connecté à l'adresse spécifiée.
   * @return true si le composant est trouvé, false sinon.
   */
  bool isConnected();

  /**
   * @brief Détecte un front montant sur une broche spécifique.
   * @param pin Le numéro de la broche à surveiller (0-7).
   * @return true si un front montant est détecté, false sinon.
   */
  bool risingEdge(uint8_t pin);

  /**
   * @brief Détecte un front descendant sur une broche spécifique.
   * @param pin Le numéro de la broche à surveiller (0-7).
   * @return true si un front descendant est détecté, false sinon.
   */
  bool fallingEdge(uint8_t pin);

  /**
   * @brief Obtient un objet représentant une seule broche numérique.
   * @param pin Le numéro de la broche à obtenir (0-7).
   * @return Un objet PCF8574_DigitalPin.
   */
  PCF8574_DigitalPin getPin(uint8_t pin);

private:
  /// @brief Type de front à détecter.
  enum EdgeType { PCF_RISING, PCF_FALLING };

  /// @brief Fonction d'aide pour la détection de fronts.
  bool checkEdge(uint8_t pin, EdgeType edge);

  /// @brief L'adresse I2C du composant.
  uint8_t _address;

  /// @brief Un cache local de l'état de sortie du port pour minimiser le trafic I2C.
  uint8_t _portState;
  /// @brief État précédent du port pour la détection de fronts.
  uint8_t _previousState;
  // Broches I2C personnalisées (utilisées si setPins() est appelé sur ESP32/ESP8266)
  int _sda_pin = -1;
  int _scl_pin = -1;
  bool _use_custom_pins = false;
};

#endif
