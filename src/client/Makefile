client.exe: ./../common/Mutex.o ./../common/Sincronizador.o ./../common/Thread.o ./../common/Adler32.o ./../common/MD5.o ./../common/HMAC.o ./../common/MisChecksums.o ./../common/Socket.o ./../common/MiSocket.o ./../common/RegistroArchivo.o ./../common/Pedido.o ./../common/Organizador.o ./../common/ConfigFile.o ./../common/UserFile.o ./../common/DataFile.o ./../common/PreferenceBase.o ./../common/Preferences.o Supervisor.o Cliente.o MenuClient.o LogIn.o Graphics.o Menu.o
	g++ -O0 -g3 -o client.exe ./../common/Mutex.o ./../common/Sincronizador.o ./../common/Thread.o ./../common/Adler32.o ./../common/MD5.o ./../common/HMAC.o ./../common/MisChecksums.o ./../common/Socket.o ./../common/MiSocket.o ./../common/RegistroArchivo.o ./../common/Pedido.o ./../common/Organizador.o ./../common/ConfigFile.o ./../common/UserFile.o ./../common/DataFile.o ./../common/PreferenceBase.o ./../common/Preferences.o Supervisor.o Cliente.o MenuClient.o LogIn.o Graphics.o Menu.o -lpthread `pkg-config --libs gtkmm-3.0`

Supervisor.o: ./../common/ValoresRetorno.h ./../common/Sincronizador.h ./../common/Thread.h ./../common/MiSocket.h ./../common/Organizador.h Cliente.h Supervisor.h Supervisor.cpp
	g++ -c Supervisor.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98

Cliente.o: ./../common/ValoresRetorno.h ./../common/Comandos.h ./../common/MiExcepcion.h ./../common/Sincronizador.h ./../common/Thread.h ./../common/MiSocket.h ./../common/Organizador.h  Supervisor.h Cliente.h Cliente.cpp
	g++ -c Cliente.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98

MenuClient.o: ./../common/Preferences.h MenuClient.h MenuClient.cpp
	g++ -c MenuClient.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98 `pkg-config --cflags gtkmm-3.0`

LogIn.o: Cliente.h MenuClient.h ./../common/ValoresRetorno.h ../common/ConfigFile.h LogIn.h LogIn.cpp
	g++ -c LogIn.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98 `pkg-config --cflags gtkmm-3.0`

Graphics.o: MenuClient.h LogIn.h Graphics.h Graphics.cpp
	 g++ -c Graphics.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98 `pkg-config --cflags gtkmm-3.0`

Menu.o: Graphics.h Menu.cpp
	g++ -c Menu.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98 `pkg-config --cflags gtkmm-3.0`
	
clean:
	rm -f client.exe *.o
