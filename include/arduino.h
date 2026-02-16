/**
 * @file arduino.h
 * @brief Interfaz publica para la comunicacion serial con Arduino.
 *
 * Proporciona funciones para conectar, desconectar y leer datos
 * del sensor de luz a traves del puerto serial.
 */

#ifndef ARDUINO_H
#define ARDUINO_H

// ============================================================
// Includes
// ============================================================
#include <stdbool.h>

// ============================================================
// Definiciones
// ============================================================

/** @brief Ruta del dispositivo serial al que se conecta el Arduino. */
#define ARDUINO_PORT "/dev/ttyUSB0"

// ============================================================
// Variables globales
// ============================================================

/** @brief Descriptor de fichero del puerto serial (-1 si no esta conectado). */
extern int serial_fd;

/** @brief Ultimo nivel de luz leido desde el sensor del Arduino. */
extern int light_level;

// ============================================================
// Funciones publicas
// ============================================================

/**
 * @brief Abre y configura la conexion serial con el Arduino.
 *
 * Configura el puerto a 9600 baudios, 8N1, modo RAW sin bloqueo.
 *
 * @return true si la conexion fue exitosa, false en caso de error.
 */
bool arduinoConnect();

/**
 * @brief Cierra la conexion serial con el Arduino.
 *
 * Libera el descriptor de fichero y lo resetea a -1.
 */
void arduinoDisconnect();

/**
 * @brief Lee el nivel de luz actual desde el Arduino (no bloqueante).
 *
 * Lee bytes disponibles del puerto serial y, al recibir una linea
 * completa (terminada en '\\n'), actualiza el valor apuntado por @p light.
 *
 * @param light Puntero donde se almacena el nivel de luz leido.
 */
void getLightLevel(int *light);

#endif
