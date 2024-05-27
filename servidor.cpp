#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <cstdlib>
#include <ctime>

using namespace std;

const int FILAS = 6;
const int COLUMNAS = 7;

char tablero[FILAS][COLUMNAS];

void inicializarTablero()
{
    for (int i = 0; i < FILAS; ++i)
    {
        for (int j = 0; j < COLUMNAS; ++j)
        {
            tablero[i][j] = ' ';
        }
    }
}

void imprimirTablero(int socket_cliente)
{
    string output = " ---------------\n";
    for (int i = 0; i < FILAS; ++i)
    {
        output += to_string(i + 1) + " |";
        for (int j = 0; j < COLUMNAS; ++j)
        {
            output += tablero[i][j];
            output += "|";
        }
        output += "\n";
    }
    output += " ---------------\n";
    output += "   1 2 3 4 5 6 7\n";
    send(socket_cliente, output.c_str(), output.size(), 0);
}

bool tableroLleno()
{
    for (int i = 0; i < FILAS; ++i)
    {
        for (int j = 0; j < COLUMNAS; ++j)
        {
            if (tablero[i][j] == ' ')
            {
                return false;
            }
        }
    }
    return true;
}

bool verificarVictoria(char jugador)
{
    for (int i = 0; i < FILAS; ++i)
    {
        for (int j = 0; j < COLUMNAS - 3; ++j)
        {
            if (tablero[i][j] == jugador && tablero[i][j + 1] == jugador && tablero[i][j + 2] == jugador && tablero[i][j + 3] == jugador)
            {
                return true;
            }
        }
    }
    for (int i = 0; i < FILAS - 3; ++i)
    {
        for (int j = 0; j < COLUMNAS; ++j)
        {
            if (tablero[i][j] == jugador && tablero[i + 1][j] == jugador && tablero[i + 2][j] == jugador && tablero[i + 3][j] == jugador)
            {
                return true;
            }
        }
    }
    for (int i = 0; i < FILAS - 3; ++i)
    {
        for (int j = 0; j < COLUMNAS - 3; ++j)
        {
            if (tablero[i][j] == jugador && tablero[i + 1][j + 1] == jugador && tablero[i + 2][j + 2] == jugador && tablero[i + 3][j + 3] == jugador)
            {
                return true;
            }
        }
    }
    for (int i = 3; i < FILAS; ++i)
    {
        for (int j = 0; j < COLUMNAS - 3; ++j)
        {
            if (tablero[i][j] == jugador && tablero[i - 1][j + 1] == jugador && tablero[i - 2][j + 2] == jugador && tablero[i - 3][j + 3] == jugador)
            {
                return true;
            }
        }
    }
    return false;
}

bool colocarFicha(int columna, char jugador, int socket_cliente)
{
    for (int i = FILAS - 1; i >= 0; --i)
    {
        if (tablero[i][columna] == ' ')
        {
            tablero[i][columna] = jugador;
            return true;
        }
    }
    return false;
}

bool validarEntrada(int columna)
{
    return columna >= 0 && columna < COLUMNAS;
}

void jugarPartida(int socket_cliente, struct sockaddr_in direccionCliente)
{
    char buffer[1024];
    memset(buffer, '\0', sizeof(buffer));
    int n_bytes = 0;

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(direccionCliente.sin_addr), ip, INET_ADDRSTRLEN);

    inicializarTablero();
    char jugadorActual = 'S';
    char otroJugador = 'C';

    cout << "[" << ip << "] Bienvenido al juego 4 en línea, juegas como 'C'\n";

    while (true)
    {
        imprimirTablero(socket_cliente);

        int columna = -1;
        n_bytes = recv(socket_cliente, buffer, sizeof(buffer), 0);
        if (n_bytes <= 0)
        {
            cout << "[" << ip << "] Error de conexión o cliente desconectado. Saliendo del juego." << endl;
            break;
        }
        columna = atoi(buffer) - 1;

        if (!validarEntrada(columna) || !colocarFicha(columna, jugadorActual, socket_cliente))
        {
            const char *msg = "Entrada inválida o columna llena. Intenta nuevamente.\n";
            send(socket_cliente, msg, strlen(msg), 0);
            continue;
        }

        if (verificarVictoria(jugadorActual))
        {
            imprimirTablero(socket_cliente);
            const char *msg = (jugadorActual == 'C') ? "Felicidades, has ganado!\n" : "La CPU ha ganado.\n";
            send(socket_cliente, msg, strlen(msg), 0);
            break;
        }

        if (tableroLleno())
        {
            imprimirTablero(socket_cliente);
            const char *msg = "El juego terminó en empate.\n";
            send(socket_cliente, msg, strlen(msg), 0);
            break;
        }

        swap(jugadorActual, otroJugador);

        if (jugadorActual == 'S')
        {
            columna = rand() % COLUMNAS;
            while (!colocarFicha(columna, jugadorActual, socket_cliente))
            {
                columna = rand() % COLUMNAS;
            }
            if (verificarVictoria(jugadorActual))
            {
                imprimirTablero(socket_cliente);
                const char *msg = "La CPU ha ganado.\n";
                send(socket_cliente, msg, strlen(msg), 0);
                break;
            }
            if (tableroLleno())
            {
                imprimirTablero(socket_cliente);
                const char *msg = "El juego terminó en empate.\n";
                send(socket_cliente, msg, strlen(msg), 0);
                break;
            }
        }
    }
}

void manejarConexion(int socket_cliente, struct sockaddr_in direccionCliente)
{
    cout << "Nueva conexión aceptada." << endl;
    jugarPartida(socket_cliente, direccionCliente);
    close(socket_cliente);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cerr << "Uso: " << argv[0] << " <Puerto>" << endl;
        return 1;
    }

    int puerto = atoi(argv[1]);
    int socket_servidor = 0;
    struct sockaddr_in direccionServidor, direccionCliente;

    cout << "Creando socket de escucha..." << endl;
    if ((socket_servidor = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cerr << "Error al crear el socket de escucha." << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Configurando estructura de dirección del socket..." << endl;
    memset(&direccionServidor, 0, sizeof(direccionServidor));
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_addr.s_addr = htonl(INADDR_ANY);
    direccionServidor.sin_port = htons(puerto);

    cout << "Asociando el socket..." << endl;
    if (bind(socket_servidor, (struct sockaddr *)&direccionServidor, sizeof(direccionServidor)) < 0)
    {
        cerr << "Error al llamar a bind()." << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Llamando a listen()..." << endl;
    if (listen(socket_servidor, 1024) < 0)
    {
        cerr << "Error al llamar a listen()." << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Esperando la solicitud del cliente..." << endl;
    while (true)
    {
        int socket_cliente;
        socklen_t tam_dir_cliente = sizeof(struct sockaddr_in);

        if ((socket_cliente = accept(socket_servidor, (struct sockaddr *)&direccionCliente, &tam_dir_cliente)) < 0)
        {
            cerr << "Error al llamar a accept()." << endl;
            exit(EXIT_FAILURE);
        }
        thread t(manejarConexion, socket_cliente, direccionCliente);
        t.detach();
    }

    return 0;
}
