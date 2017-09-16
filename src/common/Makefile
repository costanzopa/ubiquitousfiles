common: Mutex.o Sincronizador.o Thread.o Adler32.o MD5.o HMAC.o MisChecksums.o Socket.o MiSocket.o RegistroArchivo.o Pedido.o Organizador.o AdmArchivos.o ConfigFile.o UserFile.o DataFile.o PreferenceBase.o Preferences.o

Mutex.o: MiExcepcion.h Mutex.h Mutex.cpp
	g++ -c Mutex.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98

Sincronizador.o: MiExcepcion.h Sincronizador.h Sincronizador.cpp
	g++ -c Sincronizador.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98

Thread.o: MiExcepcion.h Thread.h Thread.cpp
	g++ -c Thread.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98

Adler32.o: Adler32.h Adler32.cpp
	g++ -c Adler32.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98

MD5.o: MiExcepcion.h MD5.h MD5.cpp
	g++ -c MD5.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98

HMAC.o: MiExcepcion.h MD5.h HMAC.h HMAC.cpp
	g++ -c HMAC.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98

MisChecksums.o: Adler32.h MD5.h HMAC.h MisChecksums.h MisChecksums.cpp
	g++ -c MisChecksums.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98

Socket.o: MiExcepcion.h Socket.h Socket.cpp
	g++ -c Socket.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98

MiSocket.o: Socket.h HMAC.h MisChecksums.h MiSocket.h MiSocket.cpp
	g++ -c MiSocket.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98

RegistroArchivo.o: MD5.h MiSocket.h RegistroArchivo.h RegistroArchivo.cpp
	g++ -c RegistroArchivo.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98

Pedido.o: Comandos.h MiSocket.h RegistroArchivo.h Pedido.h Pedido.cpp
	g++ -c Pedido.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98

Organizador.o: Comandos.h MiExcepcion.h Mutex.h MiSocket.h RegistroArchivo.h Pedido.h ./../client/Cliente.h ./../server/AdmUsuarios.h ./../server/Atendedor.h Organizador.h Organizador.cpp
	g++ -c Organizador.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98

AdmArchivos.o: Mutex.h MiSocket.h Organizador.h AdmArchivos.h AdmArchivos.cpp
	g++ -c AdmArchivos.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98

ConfigFile.o: ConfigFile.h ConfigFile.cpp
	g++ -c ConfigFile.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98

UserFile.o: UserFile.h UserFile.cpp 
	g++ -c UserFile.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98

DataFile.o: DataFile.h DataFile.cpp
	g++ -c DataFile.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98

PreferenceBase.o: ConfigFile.h PreferenceBase.h PreferenceBase.cpp
	g++ -c PreferenceBase.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98 `pkg-config --cflags gtkmm-3.0`

Preferences.o: ConfigFile.h PreferenceBase.h Preferences.h Preferences.cpp
	g++ -c Preferences.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98 `pkg-config --cflags gtkmm-3.0`

clean:
	rm -f *.o
