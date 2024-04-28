CXX=g++
CXXFLAGS=-std=c++11 -Wall -pthread
LDFLAGS=-pthread

all: servidor cliente

servidor: main_servidor.o ServidorJuego.o ConectaCuatro.o
	$(CXX) $(LDFLAGS) -o servidor main_servidor.o ServidorJuego.o ConectaCuatro.o

cliente: main_cliente.o ClienteJuego.o ConectaCuatro.o
	$(CXX) $(LDFLAGS) -o cliente main_cliente.o ClienteJuego.o ConectaCuatro.o

main_servidor.o: main_servidor.cpp
	$(CXX) $(CXXFLAGS) -c main_servidor.cpp

Servidor_juego.o: Servidor_juego.cpp ServidorJuego.h
	$(CXX) $(CXXFLAGS) -c ServidorJuego.cpp

Conecta_cuatro.o: Conecta_cuatro.cpp ConectaCuatro.h
	$(CXX) $(CXXFLAGS) -c ConectaCuatro.cpp

main_cliente.o: main_cliente.cpp
	$(CXX) $(CXXFLAGS) -c main_cliente.cpp

Cliente_juego.o: Cliente_juego.cpp ClienteJuego.h
	$(CXX) $(CXXFLAGS) -c Cliente_juego.cpp

clean:
	rm -f *.o servidor cliente
