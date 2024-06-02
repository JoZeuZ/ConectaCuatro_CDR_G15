#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <limits>

using namespace std;

class Cliente {
public:
    Cliente(const char* ip, int puerto) {
        this->ip = ip;
        this->puerto = puerto;
        inicializarSocket();
    }

    void iniciar() {
        if (connect(socket_cliente, (struct sockaddr*)&direccionServidor, sizeof(direccionServidor)) < 0) {
            cerr << "Conexión fallida." << endl;
            return;
        }

        cout << "Conectado al servidor de juego." << endl;

        while (true) {
            imprimirMensaje();

            cout << "Ingresa el número de columna (1-7): ";
            int columna;
            cin >> columna;
            if (cin.fail() || columna < 1 || columna > 7) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cerr << "Entrada inválida. Intenta de nuevo." << endl;
                continue;
            }

            string input = to_string(columna);
            ssize_t bytes_sent = send(socket_cliente, input.c_str(), input.size(), 0);
            if (bytes_sent < 0) {
                cerr << "Error al enviar datos." << endl;
                close(socket_cliente);
                return;
            }
        }

        close(socket_cliente);
    }

private:
    const char* ip;
    int puerto;
    int socket_cliente;
    struct sockaddr_in direccionServidor;

    void inicializarSocket() {
        if ((socket_cliente = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            cerr << "Error al crear el socket." << endl;
            exit(1);
        }

        direccionServidor.sin_family = AF_INET;
        direccionServidor.sin_port = htons(puerto);
        if (inet_pton(AF_INET, ip, &direccionServidor.sin_addr) <= 0) {
            cerr << "Dirección IP inválida o no soportada." << endl;
            exit(1);
        }
    }

    void imprimirMensaje() {
        char buffer[1024];
        int n_bytes = recv(socket_cliente, buffer, sizeof(buffer) - 1, 0);
        if (n_bytes > 0) {
            buffer[n_bytes] = '\0';
            cout << buffer << endl;
        } else if (n_bytes == 0) {
            cout << "Conexión cerrada por el servidor." << endl;
            close(socket_cliente);
            exit(0);
        } else {
            cerr << "Error en la recepción de datos." << endl;
            close(socket_cliente);
            exit(1);
        }
    }
};

int main(int argc, char** argv) {
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " <IP del servidor> <Puerto>" << endl;
        return 1;
    }

    const char* ip = argv[1];
    int puerto = atoi(argv[2]);

    Cliente cliente(ip, puerto);
    cliente.iniciar();

    return 0;
}