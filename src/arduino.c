/**
 * @file arduino.c
 * @brief Implementacion de la comunicacion serial con Arduino.
 *
 * Gestiona la apertura, configuracion y lectura no bloqueante del
 * puerto serial para obtener datos del sensor de luz.
 */

// ============================================================
// Includes
// ============================================================
#include "arduino.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <stdbool.h> // Necesario para usar 'bool' en C

// ============================================================
// Variables globales
// ============================================================
int serial_fd = -1;
int light_level = 0;

//#define ARDUINO_DEBUG

// ============================================================
// Funciones publicas
// ============================================================

/// Abre el puerto serial y lo configura a 9600 baudios, 8N1, modo RAW.
bool arduinoConnect()
{
    // 1. Intentamos abrir el puerto (ajusta si es ttyUSB0)
    serial_fd = open(ARDUINO_PORT, O_RDWR | O_NOCTTY | O_SYNC);

    if (serial_fd < 0) {
        perror("Error abriendo puerto serial"); // Muestra el error exacto
        return false;
    }

    // 2. Configuración del puerto (Termios)
    struct termios tty;
    if (tcgetattr(serial_fd, &tty) != 0) {
        return false;
    }

    // Velocidad 9600 baudios
    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);

    // Configuración RAW (Cruda) - Para leer los datos tal cual vienen
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
    tty.c_iflag &= ~IGNBRK;                     // disable break processing
    tty.c_lflag = 0;                            // no signaling chars, no echo, no canonical processing
    tty.c_oflag = 0;                            // no remapping, no delays
    tty.c_cc[VMIN]  = 0;                        // read no bloquea
    tty.c_cc[VTIME] = 0;                        // sin timeout - retorna inmediatamente

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // apagar ctrl-s/ctrl-q
    tty.c_cflag |= (CLOCAL | CREAD);        // ignorar modem controls, habilitar lectura
    tty.c_cflag &= ~(PARENB | PARODD);      // apagar paridad
    tty.c_cflag &= ~CSTOPB;                 // un solo bit de parada

    // Aplicar configuración
    if (tcsetattr(serial_fd, TCSANOW, &tty) != 0) {
        return false;
    }

    printf("Conexión con Arduino exitosa.\n");
    return true;
}

/// Cierra el puerto serial y resetea el descriptor a -1.
void arduinoDisconnect()
{
    if (serial_fd >= 0) {
        close(serial_fd);
        serial_fd = -1;
        printf("Conexión con Arduino cerrada.\n");
    }
}

/// Lee bytes disponibles del serial y actualiza el nivel de luz al recibir '\n'.
void getLightLevel(int *light)
{
    if (serial_fd < 0)
        return;

    static char buffer[10];
    static int index = 0;
    char c;

    // Leer todos los bytes disponibles sin bloquear
    while (1) {
        int n = read(serial_fd, &c, 1);

        if (n <= 0) {
            // No hay más datos disponibles, salir sin bloquear
            return;
        }

        if (c == '\n') {
            buffer[index] = '\0';
            if (index > 0) {
                *light = atoi(buffer);
            }
            index = 0;
            return;
        }
        else if (c >= '0' && c <= '9') {
            if (index < 9) {
                buffer[index++] = c;
            }
        }
    }
}

void getArduinoStatus(char *statusBuffer, int maxLength)
{
    if (serial_fd < 0) return;

    static char internalBuffer[64];
    static int index = 0;
    char c;

    // Leemos byte a byte lo que llega del puerto serial
    while (read(serial_fd, &c, 1) > 0) {
        if (c == '\n' || c == '\r') {
            if (index > 0) {
                internalBuffer[index] = '\0';
                // Copiamos el mensaje al buffer externo
                strncpy(statusBuffer, internalBuffer, maxLength);
                index = 0;
                return; // Retornamos al recibir una línea completa
            }
        } else {
            if (index < (int)sizeof(internalBuffer) - 1) {
                internalBuffer[index++] = c;
            }
        }
    }
    // Si no hay línea completa, ponemos el primer caracter en nulo para indicar "nada nuevo"
    statusBuffer[0] = '\0';
}

#ifdef ARDUINO_DEBUG

int main()
{
    if(!arduinoConnect())
        return 1;

    // Espera a que Arduino se estabilice tras el reset de conexión
    printf("Esperando a Arduino...\n");
    //sleep(2); 

    char mensaje[64];

    printf("Escuchando eventos del botón (presiona el botón en el circuito)...\n");

    while(1)
    {
        getArduinoStatus(mensaje, 64);
        
        // Si el mensaje no está vacío, lo imprimimos
        if (mensaje[0] != '\0') {
            printf("[ARDUINO]: %s\n", mensaje);

            // Ejemplo de lógica basada en el texto recibido
            if (strstr(mensaje, "Fuera!")) {
                printf("--> ¡Detección de inicio de carrera en C!\n");
            }
        }

        //sleep(1); // 10ms para no consumir 100% de CPU
    }

    arduinoDisconnect();
    return 0;
}

#endif
