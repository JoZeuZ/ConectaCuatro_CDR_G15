#include "ClienteJuego.h"
#include <iostream> // std::cerr

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: ./cliente <dirección IP> <puerto>" << std::endl;
        return 1;
    }
    std::string direccionIP = argv[1];
    int puerto = std::stoi(argv[2]);
    ClienteJuego cliente(direccionIP, puerto);
    cliente.conectarConServidor();
    cliente.iniciarJuego();
    return 0;
}
