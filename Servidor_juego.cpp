#include "ServidorJuego.h"
#include "ConectaCuatro.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

ServidorJuego::ServidorJuego(int puerto) : puerto(puerto) {}

void ServidorJuego::iniciar() {
    // Configuración inicial del socket y espera de conexiones.
    int socketServidor = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in direccionServidor{};
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_addr.s_addr = INADDR_ANY;
    direccionServidor.sin_port = htons(puerto);

    bind(socketServidor, (struct sockaddr *)&direccionServidor, sizeof(direccionServidor));
    listen(socketServidor, 5);

    while (true) {
        int socketCliente = accept(socketServidor, nullptr, nullptr);
        std::thread(&ServidorJuego::manejarCliente, this, socketCliente).detach();
    }
}

void ServidorJuego::manejarCliente(int socketCliente) {
    ConectaCuatro juego;
    juego.iniciarJuego();

    // Ciclo del juego para este cliente.
    while (!juego.juegoTerminado()) {
        char buffer[1024];
        int bytesRecibidos = recv(socketCliente, buffer, 1024, 0);
        if (bytesRecibidos <= 0) {
            break; // Cliente desconectado o error.
        }

        int columna = atoi(buffer);
        juego.realizarMovimiento(columna, 'X'); // 'X' podría representar al servidor.

        // Enviar el estado actual del tablero al cliente.
        std::string estadoTablero = "Estado del juego aquí"; // Construir el estado del tablero.
        send(socketCliente, estadoTablero.c_str(), estadoTablero.size(), 0);
    }

    close(socketCliente);
}
