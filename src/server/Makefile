server.exe: ./../common/Mutex.o ./../common/Sincronizador.o ./../common/Thread.o ./../common/Pedido.o ./../common/Adler32.o ./../common/MD5.o ./../common/HMAC.o ./../common/Socket.o ./../common/MiSocket.o ./../common/MisChecksums.o ./../common/RegistroArchivo.o ./../common/Organizador.o ./../common/AdmArchivos.o ./../common/ConfigFile.o ./../common/UserFile.o ./../common/DataFile.o AdmUsuarios.o Atendedor.o Servidor.o main.o
	g++ -O0 -g3 -o server.exe ./../common/Mutex.o ./../common/Sincronizador.o ./../common/Thread.o ./../common/Pedido.o ./../common/Adler32.o ./../common/MD5.o ./../common/HMAC.o ./../common/Socket.o ./../common/MiSocket.o ./../common/MisChecksums.o ./../common/RegistroArchivo.o ./../common/Organizador.o ./../common/AdmArchivos.o ./../common/ConfigFile.o ./../common/UserFile.o ./../common/DataFile.o AdmUsuarios.o Atendedor.o Servidor.o main.o -lpthread #`pkg-config --libs gtkmm-3.0`
	
AdmUsuarios.o: ./../common/MiExcepcion.h ./../common/Mutex.h ./../common/AdmArchivos.h ./../common/UserFile.h ./../common/DataFile.h AdmUsuarios.h AdmUsuarios.cpp
	g++ -c AdmUsuarios.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98

Atendedor.o: ./../common/ValoresRetorno.h ./../common/Comandos.h ./../common/MiExcepcion.h ./../common/Mutex.h ./../common/Sincronizador.h ./../common/Thread.h ./../common/HMAC.h ./../common/MisChecksums.h ./../common/MiSocket.h ./../common/Pedido.h ./../common/Organizador.h ./../common/AdmArchivos.h AdmUsuarios.h Atendedor.h Atendedor.cpp
	g++ -c Atendedor.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98

Servidor.o: ./../common/ValoresRetorno.h ./../common/MiExcepcion.h ./../common/MiSocket.h ./../common/AdmArchivos.h AdmUsuarios.h ./../common/Thread.h Atendedor.h Servidor.h Servidor.cpp 
	g++ -c Servidor.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98

main.o: ./../common/ValoresRetorno.h Servidor.h ./../common/ConfigFile.h main.cpp
	g++ -c main.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98 #`pkg-config --cflags gtkmm-3.0`

clean:
	rm -f server.exe *.o
