#include "arduino.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdbool.h> // Necesario para usar 'bool' en C

int serial_fd = -1;
int light_level = 0;

//#define ARDUINO_DEBUG

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

void arduinoDisconnect()
{
    if (serial_fd >= 0) {
        close(serial_fd);
        serial_fd = -1;
        printf("Conexión con Arduino cerrada.\n");
    }
}

void getLightLevel(int *light)
{
    if (serial_fd < 0) return;

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

#ifdef ARDUINO_DEBUG

int main()
{
    if(!arduinoConnect())
        return 1;
        
    // Pequeña espera para que el Arduino se reinicie al abrir el puerto (pasa en algunos modelos)
    usleep(2000000); // 2 segundos

    while(1)
    {
        getLightLevel(&light_level);
        printf("Nivel de luz: %d\n", light_level);
        
        // Un pequeño sleep para no saturar la consola de Linux, 
        // aunque el read ya bloquea un poco esperando datos.
        usleep(100000); // 0.1 segundos
    }

    close(serial_fd);
    return 0;
}

#endif