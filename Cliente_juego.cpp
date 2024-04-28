#include "ClienteJuego.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

ClienteJuego::ClienteJuego(const std::string &ip, int port) : direccionIP(ip), puerto(port) {}

void ClienteJuego::conectarConServidor()
{
    // Configura y conecta el socket al servidor.
    socketCliente = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in direccionServidor{};
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_port = htons(puerto);
    inet_pton(AF_INET, direccionIP.c_str(), &direccionServidor.sin_addr);

    connect(socketCliente, (struct sockaddr *)&direccionServidor, sizeof(direccionServidor));
    std::cout << "Conectado al servidor." << std::endl;
}

void ClienteJuego::iniciarJuego()
{
    // Bucle principal del juego en el cliente.
    while (true)
    {
        recibirEstadoDelTablero();
        int columna;
        std::cout << "Ingrese la columna para jugar: ";
        std::cin >> columna;
        enviarMovimiento(columna);
    }
}

void ClienteJuego::recibirEstadoDelTablero()
{
    char buffer[1024];
    recv(socketCliente, buffer, 1024, 0);
    std::cout << "Tablero actualizado: " << std::endl
              << buffer << std::endl;
}

void ClienteJuego::enviarMovimiento(int columna)
{
    std::string mensaje = std::to_string(columna);
    send(socketCliente, mensaje.c_str(), mensaje.size(), 0);
}
