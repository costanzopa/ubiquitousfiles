/*
 *      Name: ./server/AdmUsuarios.cpp
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include "./AdmUsuarios.h"
#include <cstdlib>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <time.h>
#include "./../common/AdmArchivos.h"
#include "./../common/DataFile.h"
#include "./../common/UserFile.h"
#include "./../common/Mutex.h"


using configuration::UserFile;

#include <iostream>

namespace ubicuos {

  AdmUsuarios::Usuario::Usuario(const std::string &contrasenya, const std::string &archivosRuta)
      : contrasenya(contrasenya), archivosRuta(archivosRuta) {
  }

  const std::string &AdmUsuarios::Usuario::getContrasenya() const throw () {
    return contrasenya;
  }

  const std::string &AdmUsuarios::Usuario::getArchivosRuta() const throw () {
    return archivosRuta;
  }

  void AdmUsuarios::Usuario::agregarAtendedor(Atendedor *pAtendedor) {
    listaAtendedores.insert(pAtendedor);
  }

  void AdmUsuarios::Usuario::desagregarAtendedor(Atendedor *pAtendedor) {
    listaAtendedores.erase(pAtendedor);
  }

  bool AdmUsuarios::Usuario::tieneAtendedores() const throw () {
    return (listaAtendedores.empty() == false);
  }

  const std::set< Atendedor * > &AdmUsuarios::Usuario::getAtendedoresRef() const {
    return listaAtendedores;
  }

  AdmUsuarios::AdmUsuarios(const std::string &directorioRuta) throw ()
      : directorioRuta(directorioRuta) {
  }

  AdmUsuarios::~AdmUsuarios() throw () {
  }

  bool AdmUsuarios::verificarUsuario(const std::string &nombre) {
    // Busco a ver si ya está en memoria una instancia de "Usuario" con este "nombre".
    mutex.tomar();
    std::map< const std::string, Usuario >::const_iterator iterador = mapaUsuarios.find(nombre);
    if (iterador != mapaUsuarios.end()) {
      // Ya esá en memoria.
      mutex.liberar();
      return true;
    }

    // Método provisorio de crear el usuario, la contraseña es el nombre concatenado a sí mismo y la ruta es la ruta fija al archivo de usuarios, más un directorio concatenado (con el nombre del usuario).
    std::string ruta = directorioRuta + nombre + "/";
    std::string contrasenya;
    UserFile users;
    // TODO (Pablo): Falta lo del fileLock
    bool usuarioExiste = users.find(nombre);
    if (usuarioExiste) {
      contrasenya = users.getPassword(nombre);
      mapaUsuarios.insert(std::pair< const std::string, Usuario >(nombre, Usuario(contrasenya, ruta)));
    }
    mutex.liberar();
    return usuarioExiste;
  }

  const std::string &AdmUsuarios::getContrasenya(const std::string &nombre) {
    mutex.tomar();
    std::map< const std::string, Usuario >::const_iterator iterador;
    iterador = mapaUsuarios.find(nombre);
    if (iterador == mapaUsuarios.end()) {
      mutex.liberar();
      throw AdmUsuariosExcepcion();
    }
    mutex.liberar();
    return iterador->second.getContrasenya();
  }

  const std::string &AdmUsuarios::getArchivosRuta(const std::string &nombre) {
    mutex.tomar();
    std::map< const std::string, Usuario >::const_iterator iterador = mapaUsuarios.find(nombre);
    if (iterador == mapaUsuarios.end()) {
      mutex.liberar();
      throw AdmUsuariosExcepcion();
    }
    mutex.liberar();
    return iterador->second.getArchivosRuta();
  }

  void AdmUsuarios::registrar(const std::string &nombre, Atendedor *pAtendedor) {
    mutex.tomar();
    std::map< const std::string, Usuario >::iterator iterador = mapaUsuarios.find(nombre);
    if (iterador == mapaUsuarios.end()) {
      mutex.liberar();
      throw AdmUsuariosExcepcion();
    }
    mutex.liberar();
    iterador->second.agregarAtendedor(pAtendedor);
  }

  void AdmUsuarios::desregistrar(const std::string &nombre, Atendedor *pAtendedor) {
    mutex.tomar();
    //std::cout << "AdmUsuarios::desregistrar():" << std::endl;
    ::syslog(LOG_USER | LOG_INFO, "Desregistrar atendedor de cliente.");
    std::map< const std::string, Usuario >::iterator iterador = mapaUsuarios.find(nombre);
    if (iterador == mapaUsuarios.end()) {
      mutex.liberar();
      throw AdmUsuariosExcepcion();
    }
    mutex.liberar();
    iterador->second.desagregarAtendedor(pAtendedor);
  }

  void AdmUsuarios::purgar(AdmArchivos &admArchivos) {
    mutex.tomar();
    std::map< const std::string, Usuario >::iterator iterador = mapaUsuarios.begin();
    while (iterador != mapaUsuarios.end()) {
      if (iterador->second.tieneAtendedores() == false) {
        admArchivos.eliminarUsuario(iterador->first);
        mapaUsuarios.erase(iterador++);
      } else {
        ++iterador;
      }
    }
    mutex.liberar();
  }

  const std::set< Atendedor * > &AdmUsuarios::getAtendedoresRef(const std::string nombre) const {
    std::map< const std::string, Usuario >::const_iterator iterador = mapaUsuarios.find(nombre);
    if (iterador == mapaUsuarios.end()) {
      throw AdmUsuariosExcepcion();
    }
    return iterador->second.getAtendedoresRef();
  }

  void AdmUsuarios::relevarTamanyoBD() {
    //std::cout << "AdmUsuarios::relevarTamanyoBD():" << std::endl;
    long int tamanyo = 0;
    std::string comando("du -sb ");
    comando += directorioRuta;
    comando += " | cut -f1 2>&1";
    FILE *stream = popen(comando.c_str(), "r");
    if (stream) {
      const int max_size = 256;
      char buffer[max_size];
      if (fgets(buffer, max_size, stream) != NULL) {
        tamanyo = ::atol(buffer);
      }
      pclose(stream);
    }
    configuration::DataFile archivo;
    archivo.end();
    archivo.write(::time(NULL), tamanyo);
    ::syslog(LOG_USER | LOG_INFO, "Relevado tamaño BD: %ldBytes", tamanyo);

    /*
    std::ofstream archivoHandler;
    std::string ruta(directorioRuta);
    ruta += "TamanyoBD.txt";
    archivoHandler.open(ruta.c_str(), std::ofstream::out | std::ofstream::app);
    archivoHandler << tamanyo << std::endl;
    archivoHandler.close();
    */
  }

//**************************************************EXCEPCIONES**************************************************//

#define TIPO_DE_EXCEPCION AdmUsuariosExcepcion
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return "AdmUsuariosExcepcion";
  }
#undef TIPO_DE_EXCEPCION

} /* namespace ubicuos */
