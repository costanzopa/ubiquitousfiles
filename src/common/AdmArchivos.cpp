/*
 *      Name: ./common/AdmArchivos.cpp
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include "./AdmArchivos.h"
#include <fstream>
#include <map>
#include <string>
#include "./MiSocket.h"
#include "./Mutex.h"
#include "./Organizador.h"

#include <iostream>

namespace ubicuos {

  AdmArchivos::AdmArchivos() {
    //std::cout << "AdmArchivos::AdmArchivos():" << std::endl;
  }

  AdmArchivos::~AdmArchivos() throw () {
    //std::cout << "AdmArchivos::~AdmArchivos():" << std::endl;
    // No hace falta liberar el mutex porque es RAII.
  }

  void AdmArchivos::sincronizacionInicial(const std::string &nombreUsuario, const std::string &directorioRuta, const MiSocket &miSocket) {
    //std::cout << "AdmArchivos::sincronizacionInicial():" << std::endl;
    // Tomo el mutex del "AdmArchivos" para que otros clientes no traten de acceder al "mapaPorUsuario", ya que voy a estar modificándolo. También, para que no me pase de que dos clientes se traten de conectar bajo el mismo nombre de usuario y traten de acceder a los mismos archivos al mismo tiempo.
    mutex.tomar();
    std::map< std::string, Organizador >::iterator iterador = mapaPorUsuario.find(nombreUsuario);
    if (iterador == mapaPorUsuario.end()) {
      // Es la primera instancia de este nombre de usuario que se registra con el "AdmArchivos", creo su instancia de "Organizador" y lo inserto en el "mapaPorUsuario", usando el nombre de usuario como clave.
      Organizador organizador(directorioRuta);
      iterador = mapaPorUsuario.insert(std::pair< std::string, Organizador >(nombreUsuario, organizador)).first;
    }
    // Tomo el mutex del Organizador de este usuario, para que otros usuarios no me cambien estos archivos en medio de la sincronización inicial.
    iterador->second.bloquearParaOtros();
    // Libero el mutex del "AdmArchivos" para que otros clientes que se registren bajo otros nombres de usuario puedan ir trabajando con el "AdmArchivos"..
    mutex.liberar();
    iterador->second.servidorEnviarRegistrosIniciales(miSocket);
    iterador->second.servidorRecibirConfirmacionOPeticiones(miSocket);
    iterador->second.desbloquearParaOtros();
  }

  Organizador &AdmArchivos::getOrganizador(const std::string &nombreUsuario) {
    //std::cout << "AdmArchivos::getOrganizador():" << std::endl;
    mutex.tomar();
    std::map< std::string, Organizador >::iterator iterador = mapaPorUsuario.find(nombreUsuario);
    mutex.liberar();
    if (iterador == mapaPorUsuario.end()) {
      throw AdmArchivosExcepcion();
    } else {
      return iterador->second;
    }
  }

  void AdmArchivos::relevarTamanyoBD() {
    mutex.tomar();
    unsigned int tamanyo = 0;
    std::map< std::string, Organizador >::iterator iterador = mapaPorUsuario.begin();
    while (iterador != mapaPorUsuario.end()) {
      tamanyo += iterador->second.getTamanyo();
      ++iterador;
    }
    std::ofstream archivoHandler;
    archivoHandler.open("TamanyoBD.txt", std::ofstream::out | std::ofstream::app);
    archivoHandler << tamanyo << std::endl;
    archivoHandler.close();
    mutex.liberar();
  }

  void AdmArchivos::eliminarUsuario(const std::string &nombreUsuario) {
    mutex.tomar();
    std::map< std::string, Organizador >::iterator iterador = mapaPorUsuario.find(nombreUsuario);
    if (iterador == mapaPorUsuario.end()) {
      throw AdmArchivosExcepcion();
    } else {
      mapaPorUsuario.erase(iterador);
    }
    mutex.liberar();
  }

//**************************************************EXCEPCIONES**************************************************//

#define TIPO_DE_EXCEPCION AdmArchivosExcepcion
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return "AdmArchivosExcepcion";
  }
#undef TIPO_DE_EXCEPCION

} /* namespace ubicuos */
