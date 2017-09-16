/*
 *      Name: ./client/Supervisor.cpp
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include "./Supervisor.h"
#include <csignal>
#include <iostream>
#include <syslog.h>
#include <unistd.h>
#include "./Cliente.h"
#include "./../common/MiSocket.h"
#include "./../common/Organizador.h"
#include "./../common/Sincronizador.h"
#include "./../common/ValoresRetorno.h"

namespace ubicuos {

  Supervisor::Temporizador *Supervisor::pTemporizador = NULL;

  void Supervisor::signalHandler(const int entero) {
    // El parámetro "entero" es requerido por la especificación de firma del miembro "sa_handler" del "struct sigaction".
    Supervisor::pTemporizador->signalHandler();
  }

  void Supervisor::Temporizador::signalHandler() {
    Thread::cancelar();
  }

  Supervisor::Temporizador::Temporizador(const unsigned int intervalo, threads::Sincronizador &sincronizador)
      : intervalo(intervalo), sincronizador(sincronizador) {
    struct sigaction miStructSigaction;
    miStructSigaction.sa_handler = &Supervisor::signalHandler;
    ::sigemptyset(&miStructSigaction.sa_mask);
    miStructSigaction.sa_flags = 0;
    ::sigaction(SIGUSR1, &miStructSigaction, NULL);
  }

  Supervisor::Temporizador::~Temporizador() throw () {
    //std::cout << "Supervisor::Temporizador::~Temporizador():" << std::endl;
    pedirDetener();
    ::raise(SIGUSR1);
    try {
      Thread::close(NULL);
    } catch (const exceptions::MiExcepcion &excepcion) {
      //std::cerr << "error en Supervisor::Temporizador::~Temporizador(): " << excepcion.what() << std::endl;
    }
  }

  void *Supervisor::Temporizador::procesar() {
    while (getDetener() == false) {
      ::sleep(intervalo);
      sincronizador.tomar();
      sincronizador.senyalar();
      sincronizador.liberar();
    }
    return (void *) RESULTADO_OK;
  }

  void Supervisor::Temporizador::pedirDetener() {
    //std::cout << "Supervisor::Temporizador::pedirDetener():" << std::endl;
    Thread::pedirDetener();
  }

  void Supervisor::Temporizador::close(void ** const pPValorDevuelto) {
    //std::cout << "Supervisor::Temporizador::close():" << std::endl;
    Thread::pedirDetener();
    Thread::close(pPValorDevuelto);
  }

  Supervisor::Supervisor(const unsigned int intervalo, Organizador &organizador, const MiSocket &miSocket, void (* const informarSincronizacionOK)() throw (), Cliente &cliente)
      : intervalo(intervalo), organizador(organizador), miSocket(miSocket), informarSincronizacionOK(informarSincronizacionOK), cliente(cliente) {
    //std::cout << "Supervisor::Supervisor():" << std::endl;
  }

  Supervisor::~Supervisor() throw () {
    //std::cout << "Supervisor::~Supervisor():" << std::endl;
    Thread::pedirDetener();
    sincronizador.senyalar();
    try {
      Thread::close(NULL);
    } catch (const exceptions::MiExcepcion &excepcion) {
      //std::cerr << "error en Supervisor::~Supervisor(): " << excepcion.what() << std::endl;
    }
  }

  void *Supervisor::procesar() {
    //std::cout << "Supervisor::procesar():" << std::endl;
    ::syslog(LOG_USER | LOG_INFO, "Ejecutando Supervisor::procesar().");
    Supervisor::Temporizador temporizador(intervalo, sincronizador);
    Supervisor::pTemporizador = &temporizador;
    temporizador.start();
    while (getDetener() == false) {
      sincronizador.tomar();
      sincronizador.esperar();
      if (getDetener() == false) {
        try {
          organizador.clienteEnviarSincronizacion(miSocket, informarSincronizacionOK);
        } catch (const exceptions::MiExcepcion &excepcion) {
          if (getDetener() == false) {
            ::syslog(LOG_USER | LOG_INFO, "Error en Cliente::procesar() al enviar sincronizaciones: %s", excepcion.what());
            return (void *) ERROR_EXCEPCION;
          }
        }
      }
      cliente.informarSincronizado();
      sincronizador.liberar();
    }
    return (void *) RESULTADO_OK;
  }

  void Supervisor::pedirDetener() {
    //std::cout << "Supervisor::pedirDetener():" << std::endl;
    Thread::pedirDetener();
    sincronizador.senyalar();
  }

  void Supervisor::close(void ** const pPValorDevuelto) {
    //std::cout << "Supervisor::close():" << std::endl;
    Thread::pedirDetener();
    sincronizador.senyalar();
    Thread::close(pPValorDevuelto);
  }

  void Supervisor::sincronizar() {
    sincronizador.tomar();
    sincronizador.senyalar();
    sincronizador.liberar();
  }

} /* namespace ubicuos */

