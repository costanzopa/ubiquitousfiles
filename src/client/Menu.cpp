//============================================================================
// Name        : VisualClient.cpp
// Author      : Pablito
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "./Graphics.h"
#include <iostream>
#include <gtkmm/main.h>
#include <syslog.h>

using visual::Graphics;

int main(int argc, char** argv) {
  ::openlog("client.exe", LOG_CONS, LOG_USER);
  char aplicacion[] = "archivos ubicuos - cliente";
  ::syslog(LOG_USER | LOG_INFO, "INICIO DE: %s", aplicacion);
  ::closelog();
  Graphics* menu = new Graphics(argc, argv);
  menu->loadLog();
  menu->loadMenu();
  menu->show();
  ::syslog(LOG_USER | LOG_INFO, "FIN DE: %s", aplicacion);
  return 0;
}
