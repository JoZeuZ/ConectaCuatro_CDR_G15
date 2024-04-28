#ifndef SERVIDOR_JUEGO_H
#define SERVIDOR_JUEGO_H

#include <vector>
#include <string>
#include <thread>

class ServidorJuego {
public:
    explicit ServidorJuego(int puerto);
    void iniciar();

private:
    int puerto;
    void manejarCliente(int socketCliente);
};

#endif
