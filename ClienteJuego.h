#ifndef CLIENTE_JUEGO_H
#define CLIENTE_JUEGO_H

#include <string>
#include <vector>

class ClienteJuego {
public:
    ClienteJuego(const std::string& direccionIP, int puerto);
    void conectarConServidor();
    void iniciarJuego();

private:
    std::string direccionIP;
    int puerto;
    int socketCliente;
    void recibirEstadoDelTablero();
    void enviarMovimiento(int columna);
};

#endif
