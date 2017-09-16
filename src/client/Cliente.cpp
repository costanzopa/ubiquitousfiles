/*
 *      Name: ./client/Cliente.cpp
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include "./Cliente.h"
#include <iostream>
#include <string>
#include <syslog.h>
#include "./Supervisor.h"
#include "./../common/Comandos.h"
#include "./../common/MiExcepcion.h"
#include "./../common/MiSocket.h"
#include "./../common/Organizador.h"
#include "./../common/Sincronizador.h"
#include "./../common/ValoresRetorno.h"

namespace ubicuos {

  Cliente::Cliente(const std::string &hostDireccion, const std::string &hostPuerto, const std::string &directorioRuta, const unsigned int intervalo, const std::string &usuarioNombre, const std::string &contrasenya)
  try
      : seInterrumpioSocket(false), miSocket(hostDireccion, hostPuerto, contrasenya), intervalo(intervalo), usuarioNombre(usuarioNombre), contrasenya(contrasenya), organizador(directorioRuta), registrado(false), sincronizado(false) {
    //std::cout << "Cliente::Cliente():" << std::endl;
  }
  catch (const exceptions::MiExcepcion &excepcion) {
    //std::cerr << "error en Cliente::Cliente(): " << excepcion.what() << std::endl;
    // La excepción atrapada es inevitablemente re-lanzada.
  }

  Cliente::~Cliente() throw () {
    //std::cout << "Cliente::~Cliente():" << std::endl;
    Thread::pedirDetener();
    if (seInterrumpioSocket == false) {
      try {
        miSocket.interrumpir();
      } catch (const exceptions::MiExcepcion &excepcion) {
        //std::cerr << "error en Cliente::~Cliente(): " << excepcion.what() << std::endl;
      }
    }
    sincronizador.senyalar();
    try {
      Thread::close(NULL);
    } catch (const exceptions::MiExcepcion &excepcion) {
      //std::cerr << "error en Cliente::~Cliente(): " << excepcion.what() << std::endl;
    }
  }

  void Cliente::sincronizacionOK() throw () {
    // TODO Informa al usuario que se ejecutó una sincronización de forma satisfactoria.
    ::syslog(LOG_USER | LOG_INFO, "***INFORME: SE SINCRONIZÓ CORRECTAMENTE.");
  }

  void Cliente::sincronizacionConflicto(const std::string &explicacion) throw () {
    // TODO Informa al usuario que existen conflictos por resolver.
    ::syslog(LOG_USER | LOG_INFO, "***INFORME: CONFLICTO: %s", explicacion.c_str());
  }

  void *Cliente::procesar() {
    ::syslog(LOG_USER | LOG_INFO, "Ejecutando Cliente::procesar().");
    int returnValue;
    try {
      if (getDetener() == false) {
        organizador.clienteEstablecerCopiaDelServidor(miSocket);
      }
      if (getDetener() == false) {
        Supervisor supervisor(intervalo, organizador, miSocket, &sincronizacionOK, *this);
        supervisor.start();
        try {
          while (getDetener() == false) {
            organizador.clienteRecibirActualizaciones(miSocket, &sincronizacionOK, &sincronizacionConflicto, *this, supervisor);
          }
        } catch (const exceptions::MiExcepcion &excepcion) {
          if (getDetener() == false) {
            ::syslog(LOG_USER | LOG_INFO, "Error en Cliente::procesar() al recibir actualizaciones: %s", excepcion.what());
            return (void *) ERROR_EXCEPCION;
          }
        }
        supervisor.pedirDetener();
        supervisor.close((void **) &returnValue);
      }
    } catch (const exceptions::MiExcepcion &excepcion) {
      if (getDetener() == false) {
        ::syslog(LOG_USER | LOG_INFO, "Error en Cliente::procesar(): %s", excepcion.what());
        return (void *) ERROR_EXCEPCION;
      }
    }
    return (void *) returnValue;
  }

  bool Cliente::registrar() {
    ::syslog(LOG_USER | LOG_INFO, "Ejecutando Cliente::registrar().");
    std::string comando(REGISTRACION);
    comando += usuarioNombre;
    try {
      miSocket.enviarCompleto(comando);
    } catch (const MiSocketExcepcion &excepcion) {
      throw ClienteExcepcion();
    }
    //std::cout << "Enviado: " << comando << std::endl;
    std::string recibido;
    try {
      miSocket.recibirCompleto(recibido);
    } catch (const MiSocketExcepcion &excepcion) {
      return false;
    }
    if (recibido.compare(std::string(SI_REGISTRADO)) == 0) {
      ::syslog(LOG_USER | LOG_INFO, "Recibido: %s", SI_REGISTRADO);
      registrado = true;
      return true;
    }
    if (recibido.compare(std::string(NO_REGISTRADO)) == 0) {
      ::syslog(LOG_USER | LOG_INFO, "Recibido: %s", NO_REGISTRADO);
      return false;
    }
    if (recibido.compare(std::string(COMANDO_ERROR)) == 0) {
      ::syslog(LOG_USER | LOG_INFO, "Recibido: %s", COMANDO_ERROR);
      return false;
    }
    ::syslog(LOG_USER | LOG_INFO, "Recibido mensaje desconocido: %s", recibido.c_str());
    return false;
  }

  void Cliente::pedirDetener() {
    //std::cout << "Cliente::pedirDetener():" << std::endl;
    Thread::pedirDetener();
    miSocket.interrumpir();
    seInterrumpioSocket = true;
    sincronizador.senyalar();
  }

  void Cliente::close(void ** const pPValorDevuelto) {
    //std::cout << "Cliente::close():" << std::endl;
    Thread::pedirDetener();
    if (seInterrumpioSocket == false) {
      miSocket.interrumpir();
      seInterrumpioSocket = true;
    }
    sincronizador.senyalar();
    Thread::close(pPValorDevuelto);
  }

  bool Cliente::getRegistrado() const throw () {
    return registrado;
  }

  void Cliente::sincronizarYBloquear(Supervisor &supervisor) {
    sincronizado = false;
    supervisor.sincronizar();
    sincronizador.tomar();
    while (sincronizado == false) {
      sincronizador.esperar();
    }
  }

  void Cliente::informarSincronizado() {
    sincronizador.tomar();
    sincronizado = true;
    sincronizador.senyalar();
    sincronizador.liberar();
  }

  void Cliente::desbloquearSincronizaciones() {
    sincronizador.liberar();
  }

//**************************************************EXCEPCIONES**************************************************//

#define TIPO_DE_EXCEPCION ClienteExcepcion
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return "ClienteExcepcion";
  }
#undef TIPO_DE_EXCEPCION

} /* namespace ubicuos */
