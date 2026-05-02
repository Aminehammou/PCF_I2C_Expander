/**
 * @file PCF_I2C_Expander.h
 * @author Hammou Mohammed Amine
 * @brief Bibliothèque C++ générique à base de templates pour les extenseurs d'E/S PCF8574 (8 bits) et PCF8575 (16 bits).
 * @version 1.0.1
 * @date 2024-05-16
 */

#ifndef PCF_I2C_EXPANDER_H
#define PCF_I2C_EXPANDER_H

#include <Arduino.h>
#include <Wire.h>

/**
 * @brief Codes de statut pour les opérations de l'extenseur PCF.
 */
typedef enum {
  PCF_OK,     /**< L'opération a réussi. */
  PCF_ERROR   /**< Une erreur est survenue pendant l'opération. */
} PCF_Status;

// Forward declaration
template <typename PortType>
class PCF_I2C_Expander;

/**
 * @class PCF_Expander_DigitalPin
 * @brief Représente une seule broche de l'extenseur sous forme d'objet.
 */
template <typename PortType>
class PCF_Expander_DigitalPin {
public:
    PCF_Expander_DigitalPin(PCF_I2C_Expander<PortType>& parent, uint8_t pin)
        : _parent(parent), _pin(pin) {}

    void pinMode(uint8_t mode) { _parent.pinMode(_pin, mode); }
    void digitalWrite(uint8_t value) { _parent.write(_pin, value); }
    int digitalRead() { return _parent.read(_pin); }
    void toggle() { _parent.toggle(_pin); }

private:
    PCF_I2C_Expander<PortType>& _parent;
    uint8_t _pin;
};

/**
 * @class PCF_I2C_Expander
 * @brief Bibliothèque générique à base de templates pour les extenseurs d'E/S I2C PCF8574 (8 bits) et PCF8575 (16 bits).
 * @tparam PortType Le type de données pour le port (uint8_t pour PCF8574, uint16_t pour PCF8575).
 */
template <typename PortType>
class PCF_I2C_Expander {
public:
    /**
     * @brief Constructeur pour un nouvel objet extenseur d'E/S PCF.
     * @param address L'adresse I2C du composant.
     * @param initialState L'état initial du port (par défaut, toutes les broches en entrée).
     */
    PCF_I2C_Expander(uint8_t address, PortType initialState = (PortType)-1)
        : _address(address), _portState(initialState), _wire(&Wire) {}

    // Doit être appelée dans setup(). Si des broches personnalisées ont été définies via setPins(),
    // elles seront utilisées sur ESP32/ESP8266; ailleurs, on ignore et utilise begin() par défaut.
    void begin() {
    #if defined(ARDUINO_ARCH_ESP32) || defined(ESP8266)
        if (_use_custom_pins) {
            _wire->begin(_sda_pin, _scl_pin); // utilise SDA/SCL personnalisées
        } else {
            _wire->begin();
        }
    #else
        _wire->begin();
    #endif
        writePort(_portState);
    }

    void setClock(uint32_t clockSpeed) {
        _wire->setClock(clockSpeed);
    }

    // Permet d'utiliser un autre bus I2C (ex: Wire1 sur ESP32). A appeler avant begin().
    void setWire(TwoWire& wire) {
        _wire = &wire;
    }

    // Définit des broches I2C personnalisées (ex: SDA=21, SCL=22 sur ESP32).
    // A appeler avant begin(). Sans effet sur plateformes où begin(sda,scl) n'existe pas.
    void setPins(int sda_pin, int scl_pin) {
        _sda_pin = sda_pin;
        _scl_pin = scl_pin;
        _use_custom_pins = true;
    }

    bool isConnected() {
        _wire->beginTransmission(_address);
        return (_wire->endTransmission() == 0);
    }

    PCF_Status pinMode(uint8_t pin, uint8_t mode) {
        const uint8_t pinCount = sizeof(PortType) * 8;
        if (pin >= pinCount) return PCF_ERROR;
        if (mode == INPUT) {
            // INPUT: écrire 1 pour activer le pull-up interne
            _portState |= ((PortType)1 << pin);
            return writePort(_portState);
        } else {
            // OUTPUT: ne change pas le niveau ici pour éviter un glitch; l'utilisateur appellera write()
            return PCF_OK;
        }
    }

    PCF_Status write(uint8_t pin, uint8_t value) {
        const uint8_t pinCount = sizeof(PortType) * 8;
        if (pin >= pinCount) return PCF_ERROR;
        if (value == HIGH) {
            _portState |= ((PortType)1 << pin);
        } else {
            _portState &= ~((PortType)1 << pin);
        }
        return writePort(_portState);
    }

    int read(uint8_t pin) {
        const uint8_t pinCount = sizeof(PortType) * 8;
        if (pin >= pinCount) return -1; // Erreur: broche invalide

        PortType portValue;
        if (readPort(&portValue) != PCF_OK) {
            return -1; // Erreur: lecture I2C a échoué
        }

        return (portValue >> pin) & 1;
    }

    PCF_Status toggle(uint8_t pin) {
        const uint8_t pinCount = sizeof(PortType) * 8;
        if (pin >= pinCount) return PCF_ERROR;
        _portState ^= ((PortType)1 << pin);
        return writePort(_portState);
    }

    PCF_Status writePort(PortType value) {
        _portState = value;
        _wire->beginTransmission(_address);
        if (sizeof(PortType) == 2) {
            _wire->write(value & 0xFF); // LSB (P00-P07)
            _wire->write(value >> 8);   // MSB (P10-P17)
        } else {
            _wire->write(value);
        }
        if (_wire->endTransmission() != 0) {
            return PCF_ERROR;
        }
        return PCF_OK;
    }

    PCF_Status readPort(PortType* value) {
        if (_wire->requestFrom(_address, (uint8_t)sizeof(PortType)) != sizeof(PortType)) {
            return PCF_ERROR;
        }
        if (sizeof(PortType) == 2) {
            uint8_t lsb = _wire->read();
            uint8_t msb = _wire->read();
            *value = ((PortType)msb << 8) | lsb;
        } else {
            *value = _wire->read();
        }
        return PCF_OK;
    }

    PortType readPortValue() {
        PortType value = 0;
        if (readPort(&value) == PCF_ERROR) {
            return 0; // Return 0 on error
        }
        return value;
    }

    PortType readOutputCache() {
        return _portState;
    }

    PCF_Expander_DigitalPin<PortType> getPin(uint8_t pin) {
        return PCF_Expander_DigitalPin<PortType>(*this, pin);
    }

    bool risingEdge(uint8_t pin) {
        return checkEdge(pin, PCF_RISING);
    }

    bool fallingEdge(uint8_t pin) {
        return checkEdge(pin, PCF_FALLING);
    }

private:
    enum EdgeType { PCF_RISING, PCF_FALLING };

    bool checkEdge(uint8_t pin, EdgeType edge) {
        const uint8_t pinCount = sizeof(PortType) * 8;
        if (pin >= pinCount) return false;

        PortType currentState;
        if (readPort(&currentState) != PCF_OK) {
            return false;
        }

        bool previousBit = (_previousState >> pin) & 1;
        bool currentBit = (currentState >> pin) & 1;
        _previousState = currentState;

        if (edge == PCF_RISING) {
            return !previousBit && currentBit;
        } else {
            return previousBit && !currentBit;
        }
    }

    uint8_t _address;
    PortType _portState;
    PortType _previousState; // Pour la détection de front
    TwoWire* _wire;
    // Gestion des broches personnalisées (utilisées uniquement si setPins() est appelé)
    int _sda_pin = -1;
    int _scl_pin = -1;
    bool _use_custom_pins = false;
};

/**
 * @brief Alias pour le PCF8574 (extenseur d'E/S 8 bits).
 *
 * Utilisez ceci pour créer une instance pour un composant PCF8574.
 * Exemple : `PCF8574_Expander pcf(0x20);`
 */
using PCF8574_Expander = PCF_I2C_Expander<uint8_t>;

/**
 * @brief Alias pour le PCF8575 (extenseur d'E/S 16 bits).
 *
 * Utilisez ceci pour créer une instance pour un composant PCF8575.
 * Exemple : `PCF8575_Expander pcf(0x20);`
 */
using PCF8575_Expander = PCF_I2C_Expander<uint16_t>;

#endif // PCF_I2C_EXPANDER_H