#ifndef CONECTA_CUATRO_H
#define CONECTA_CUATRO_H

#include <vector>
#include <string>

class ConectaCuatro {
public:
    ConectaCuatro();
    void iniciarJuego();
    bool realizarMovimiento(int columna, char jugador);
    std::vector<std::vector<char>> obtenerTablero();
    bool juegoTerminado();
    char obtenerGanador();

private:
    std::vector<std::vector<char>> tablero;
    void verificarEstadoDelJuego();
    bool verificarGanador(char jugador);
};

#endif
