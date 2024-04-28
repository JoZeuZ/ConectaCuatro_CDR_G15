#include "ServidorJuego.h"
#include <iostream> // std::cerr

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: ./servidor <puerto>" << std::endl;
        return 1;
    }

    int puerto = std::stoi(argv[1]);
    ServidorJuego servidor(puerto);
    servidor.iniciar();

    return 0;
}
