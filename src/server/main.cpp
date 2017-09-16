//============================================================================
//      Name: ./server/main.cpp
// Copyright: Eclipse Public License - v 1.0
//   Version: 1.0
//    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
//============================================================================

#include <exception>
#include <iostream>
#include <string>
#include "./../common/ValoresRetorno.h"
#include "./Servidor.h"
#include "./../common/ConfigFile.h"
#include <sys/types.h>
#include <sys/stat.h>
#define COLA_LARGO 10
#define NRO_ARGUMENTOS 3

/*
 * argv[1] = Número de puerto del servidor (por ej.: 12345).
 * argv[2] = Ruta absoluta al directorio de archivos donde estará el archivo de usuarios.
 */

using configuration::ConfigFile;

int main() {
  std::string route = "../configserver.cfg";
  std::string newRoute = "../Users/";
  ConfigFile config(route);
  mkdir(newRoute.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  config.read();
  std::string puerto = config.getStringPort();
  std::string directorioRuta = config.getDirectory();
  int returnValue;
  try {
    ubicuos::Servidor servidor(puerto, COLA_LARGO, directorioRuta);
    servidor.start();
    char letra;
    do {
      std::cin >> letra;
    } while ((letra != 'q') && (letra != 'Q'));
    servidor.pedirDetener();
    servidor.close((void **) &returnValue);
    //std::cout << "main(): Servidor::close() devolvió: " << returnValue << std::endl;
  } catch (const std::exception &excepcion) {
    //std::cout << "error en main(): " << excepcion.what() << std::endl;
    returnValue = ERROR_EXCEPCION;
    //std::cout << "Fin controlado de programa con errores. ECHO $? = " << returnValue << std::endl;
    return returnValue;
  }
  //std::cout << "Fin controlado de programa sin errores. ECHO $? = " << returnValue << std::endl;
  return returnValue;
}
