#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;

const int FILAS = 6;
const int COLUMNAS = 7;

class Juego {
public:
    Juego(int socket_cliente, struct sockaddr_in direccionCliente) {
        this->socket_cliente = socket_cliente;
        this->direccionCliente = direccionCliente;
        inicializarTablero();
        jugadorActual = 'C'; // El cliente juega como 'C'
        otroJugador = 'S';   // La CPU juega como 'S'
    }

    void iniciar() {
        char buffer[1024];
        memset(buffer, '\0', sizeof(buffer));
        int n_bytes = 0;

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(direccionCliente.sin_addr), ip, INET_ADDRSTRLEN);

        cout << "[" << ip << "] Bienvenido al juego 4 en línea, juegas como 'C'\n";

        while (true) {
            imprimirTablero();

            if (jugadorActual == 'C') {
                n_bytes = recv(socket_cliente, buffer, sizeof(buffer), 0);
                if (n_bytes <= 0) {
                    cout << "[" << ip << "] Error de conexión o cliente desconectado. Saliendo del juego." << endl;
                    break;
                }
                int columna = atoi(buffer) - 1;

                if (!validarEntrada(columna) || !colocarFicha(columna)) {
                    const char* msg = "Entrada inválida o columna llena. Intenta nuevamente.\n";
                    send(socket_cliente, msg, strlen(msg), 0);
                    continue;
                }
            } else {
                int columna = rand() % COLUMNAS;
                while (!colocarFicha(columna)) {
                    columna = rand() % COLUMNAS;
                }
            }

            if (verificarVictoria()) {
                imprimirTablero();
                const char* msg = (jugadorActual == 'C') ? "Felicidades, has ganado!\n" : "La CPU ha ganado.\n";
                send(socket_cliente, msg, strlen(msg), 0);
                break;
            }

            if (tableroLleno()) {
                imprimirTablero();
                const char* msg = "El juego terminó en empate.\n";
                send(socket_cliente, msg, strlen(msg), 0);
                break;
            }

            swap(jugadorActual, otroJugador);
        }

        close(socket_cliente);
    }

private:
    int socket_cliente;
    struct sockaddr_in direccionCliente;
    char tablero[FILAS][COLUMNAS];
    char jugadorActual;
    char otroJugador;

    void inicializarTablero() {
        for (int i = 0; i < FILAS; ++i) {
            for (int j = 0; j < COLUMNAS; ++j) {
                tablero[i][j] = ' ';
            }
        }
    }

    void imprimirTablero() {
        string output = " ---------------\n";
        for (int i = 0; i < FILAS; ++i) {
            output += to_string(i + 1) + " |";
            for (int j = 0; j < COLUMNAS; ++j) {
                output += tablero[i][j];
                output += "|";
            }
            output += "\n";
        }
        output += " ---------------\n";
        output += "   1 2 3 4 5 6 7\n";
        send(socket_cliente, output.c_str(), output.size(), 0);
    }

    bool tableroLleno() {
        for (int i = 0; i < FILAS; ++i) {
            for (int j = 0; j < COLUMNAS; ++j) {
                if (tablero[i][j] == ' ') {
                    return false;
                }
            }
        }
        return true;
    }

    bool verificarVictoria() {
        for (int i = 0; i < FILAS; ++i) {
            for (int j = 0; j < COLUMNAS - 3; ++j) {
                if (tablero[i][j] == jugadorActual && tablero[i][j + 1] == jugadorActual && tablero[i][j + 2] == jugadorActual && tablero[i][j + 3] == jugadorActual) {
                    return true;
                }
            }
        }
        for (int i = 0; i < FILAS - 3; ++i) {
            for (int j = 0; j < COLUMNAS; ++j) {
                if (tablero[i][j] == jugadorActual && tablero[i + 1][j] == jugadorActual && tablero[i + 2][j] == jugadorActual && tablero[i + 3][j] == jugadorActual) {
                    return true;
                }
            }
        }
        for (int i = 0; i < FILAS - 3; ++i) {
            for (int j = 0; j < COLUMNAS - 3; ++j) {
                if (tablero[i][j] == jugadorActual && tablero[i + 1][j + 1] == jugadorActual && tablero[i + 2][j + 2] == jugadorActual && tablero[i + 3][j + 3] == jugadorActual) {
                    return true;
                }
            }
        }
        for (int i = 3; i < FILAS; ++i) {
            for (int j = 0; j < COLUMNAS - 3; ++j) {
                if (tablero[i][j] == jugadorActual && tablero[i - 1][j + 1] == jugadorActual && tablero[i - 2][j + 2] == jugadorActual && tablero[i - 3][j + 3] == jugadorActual) {
                    return true;
                }
            }
        }
        return false;
    }

    bool colocarFicha(int columna) {
        for (int i = FILAS - 1; i >= 0; --i) {
            if (tablero[i][columna] == ' ') {
                tablero[i][columna] = jugadorActual;
                return true;
            }
        }
        return false;
    }

    bool validarEntrada(int columna) {
        return columna >= 0 && columna < COLUMNAS;
    }
};

class Servidor {
public:
    Servidor(int puerto) {
        this->puerto = puerto;
        inicializarSocket();
    }

    void iniciar() {
        cout << "Esperando la solicitud del cliente..." << endl;
        while (true) {
            int socket_cliente;
            socklen_t tam_dir_cliente = sizeof(struct sockaddr_in);

            if ((socket_cliente = accept(socket_servidor, (struct sockaddr*)&direccionCliente, &tam_dir_cliente)) < 0) {
                cerr << "Error al llamar a accept()." << endl;
                exit(EXIT_FAILURE);
            }
            thread t(&Servidor::manejarConexion, this, socket_cliente, direccionCliente);
            t.detach();
        }
    }

private:
    int puerto;
    int socket_servidor;
    struct sockaddr_in direccionServidor, direccionCliente;

    void inicializarSocket() {
        cout << "Creando socket de escucha..." << endl;
        if ((socket_servidor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            cerr << "Error al crear el socket de escucha." << endl;
            exit(EXIT_FAILURE);
        }

        cout << "Configurando estructura de dirección del socket..." << endl;
        memset(&direccionServidor, 0, sizeof(direccionServidor));
        direccionServidor.sin_family = AF_INET;
        direccionServidor.sin_addr.s_addr = htonl(INADDR_ANY);
        direccionServidor.sin_port = htons(puerto);

        cout << "Asociando el socket..." << endl;
        if (bind(socket_servidor, (struct sockaddr*)&direccionServidor, sizeof(direccionServidor)) < 0) {
            cerr << "Error al llamar a bind()." << endl;
            exit(EXIT_FAILURE);
        }

        cout << "Llamando a listen()..." << endl;
        if (listen(socket_servidor, 1024) < 0) {
            cerr << "Error al llamar a listen()." << endl;
            exit(EXIT_FAILURE);
        }
    }

    void manejarConexion(int socket_cliente, struct sockaddr_in direccionCliente) {
        cout << "Nueva conexión aceptada." << endl;
        Juego juego(socket_cliente, direccionCliente);
        juego.iniciar();
    }
};

int main(int argc, char** argv) {
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <Puerto>" << endl;
        return 1;
    }

    int puerto = atoi(argv[1]);

    Servidor servidor(puerto);
    servidor.iniciar();

    return 0;
}