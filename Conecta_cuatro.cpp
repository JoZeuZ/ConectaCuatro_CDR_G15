#include "ConectaCuatro.h"

ConectaCuatro::ConectaCuatro() {
    tablero.resize(6, std::vector<char>(7, ' ')); // 6 filas y 7 columnas inicializadas con espacios vacíos.
}

void ConectaCuatro::iniciarJuego() {
    // Reinicia el tablero
    for (auto &fila : tablero) {
        std::fill(fila.begin(), fila.end(), ' ');
    }
}

bool ConectaCuatro::realizarMovimiento(int columna, char jugador) {
    // Asumimos que el movimiento es válido.
    for (auto &fila : tablero) {
        if (fila[columna] == ' ') {
            fila[columna] = jugador;
            return verificarGanador(jugador);
        }
    }
    return false;
}

bool ConectaCuatro::verificarGanador(char jugador) {
    // Implementar la lógica para verificar si hay cuatro en línea.
    return false; // Este método debería implementar la lógica de verificación real.
}

bool ConectaCuatro::juegoTerminado() {
    // Verificar si el tablero está completamente lleno o si alguien ha ganado.
    return false;
}

char ConectaCuatro::obtenerGanador() {
    // Devuelve el ganador del juego o un carácter vacío si aún no hay ganador.
    return ' ';
}
