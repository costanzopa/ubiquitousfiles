monitor.exe: ../common/UserFile.o ./../common/ConfigFile.o ./../common/DataFile.o ./../common/PreferenceBase.o Pair.o BaseAccount.o ChangeAccount.o Curve.o  NewAccount.o DeleteAccount.o GraphicWindow.o Accounts.o MenuMonitor.o Monitor.o
	g++ -O0 -g3 -o monitor.exe ../common/UserFile.o ../common/ConfigFile.o ./../common/DataFile.o  ./../common/PreferenceBase.o Pair.o BaseAccount.o ChangeAccount.o Curve.o DeleteAccount.o NewAccount.o GraphicWindow.o Accounts.o MenuMonitor.o Monitor.o `pkg-config --libs gtkmm-3.0`

Pair.o : Pair.h Pair.cpp
	g++ -c Pair.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98

BaseAccount.o: ../common/UserFile.h BaseAccount.h BaseAccount.cpp
	g++ -c BaseAccount.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98 `pkg-config --cflags gtkmm-3.0`

ChangeAccount.o: ChangeAccount.h BaseAccount.h ChangeAccount.cpp
	g++ -c ChangeAccount.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98 `pkg-config --cflags gtkmm-3.0`

Curve.o: Curve.h Pair.h Curve.cpp
	g++ -c Curve.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98 `pkg-config --cflags gtkmm-3.0`

NewAccount.o: ./BaseAccount.h ./../common/UserFile.h  ./NewAccount.h NewAccount.cpp
	g++ -c NewAccount.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98 `pkg-config --cflags gtkmm-3.0`

DeleteAccount.o: ./DeleteAccount.h ../common/UserFile.h DeleteAccount.cpp
	g++ -c DeleteAccount.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98 `pkg-config --cflags gtkmm-3.0`

GraphicWindow.o: ./GraphicWindow.h Curve.h GraphicWindow.cpp
	g++ -c GraphicWindow.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98 `pkg-config --cflags gtkmm-3.0`

Accounts.o: Accounts.h Accounts.cpp NewAccount.h ChangeAccount.h DeleteAccount.h ./../common/UserFile.h ./../common/ConfigFile.h
	g++ -c Accounts.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98 `pkg-config --cflags gtkmm-3.0`

MenuMonitor.o: MenuMonitor.h  ./../common/PreferenceBase.h ./../common/DataFile.h Accounts.h MenuMonitor.cpp
	g++ -c MenuMonitor.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98 `pkg-config --cflags gtkmm-3.0`

Monitor.o: MenuMonitor.h Monitor.cpp
	g++ -c Monitor.cpp -O0 -g3 -pedantic -Wall -Wextra -Wconversion -std=c++98 `pkg-config --cflags gtkmm-3.0`

clean:
	rm -f monitor.exe *.o
