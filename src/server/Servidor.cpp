/*
 *      Name: ./server/Servidor.cpp
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include "./Servidor.h"
#include <cerrno>  /* Variable "errno". */
#include <cstdio>  /* Funcion "fprintf". */
#include <cstring>  /* Funcion "strerror". */
#include <queue>
#include <string>
#include "./../common/AdmArchivos.h"
#include "./../common/MiSocket.h"
#include "./../common/Thread.h"
#include "./../common/ValoresRetorno.h"
#include "./AdmUsuarios.h"
#include "./Atendedor.h"

#define PURGA_ATENDEDORES 5
#define PURGA_ADM_USUARIOS 50
#define RELEVO_TAMANYO_BD 3

#include <iostream>

namespace ubicuos {

  Servidor::Servidor(const std::string &puerto, const unsigned int colaLargo, const std::string &directorioRuta)
      : seInterrumpioSocket(false), miSocket(puerto, colaLargo), admUsuarios(directorioRuta) {
  }

  Servidor::~Servidor() throw () {
    //std::cout << "Servidor::~Servidor():" << std::endl;
    Thread::pedirDetener();
    if (seInterrumpioSocket == false) {
      try {
        miSocket.interrumpir();
      } catch (const exceptions::MiExcepcion &excepcion) {
        //std::cerr << "error en Servidor::~Servidor(): " << excepcion.what() << std::endl;
      }
    }
    std::set< Atendedor * >::iterator iterador = atendedores.begin();
    while (iterador != atendedores.end()) {
      try {
        (*iterador)->pedirDetener();
      } catch (const exceptions::MiExcepcion &excepcion) {
        //std::cerr << "error en Servidor::~Servidor(): " << excepcion.what() << std::endl;
      }
      try {
        (*iterador)->close(NULL);
      } catch (const exceptions::MiExcepcion &excepcion) {
        //std::cerr << "error en Servidor::~Servidor(): " << excepcion.what() << std::endl;
      }
      delete (*iterador);
      atendedores.erase(iterador++);
    }
    try {
      Thread::close(NULL);
    } catch (const exceptions::MiExcepcion &excepcion) {
      //std::cerr << "error en Servidor::~Servidor(): " << excepcion.what() << std::endl;
    }
  }

  void *Servidor::procesar() {
    int returnValue = RESULTADO_OK;
    try {
      MiSocket *pSocketAtendedor = new MiSocket();
      unsigned int contadorPurgaAtendedores = 0;
      unsigned int contadorPurgaAdmUsuarios = 0;
      unsigned int contadorRelevarTamanyoBD = RELEVO_TAMANYO_BD;
      while (getDetener() == false) {
        try {
          miSocket.aceptar(pSocketAtendedor);
        } catch (const sockets::SocketExcepcion &excepcion) {
          if (getDetener() == false) {
            delete pSocketAtendedor;
            throw;
          }
        }
        if (getDetener() == false) {
          // Ownership del socket pasa al atendedor.
          Atendedor *pAtendedor = new Atendedor(pSocketAtendedor, admArchivos, admUsuarios, atendedoresARecolectar);
          pAtendedor->start();
          atendedores.insert(pAtendedor);
          pSocketAtendedor = new MiSocket();
          ++contadorPurgaAtendedores;
          // Cada "PURGA_ATENDEDOR" número de "accepts", hacer una purga de atendedores cuyas conexiones están caídas.
          // Cada "PURGA_ADM_USUARIOS" número de purgas de atendedores, hacer una purga de datos de usuarios que ya no están más online.
          if (contadorPurgaAtendedores >= PURGA_ATENDEDORES) {
            while (atendedoresARecolectar.empty() == false) {
              //std::cout << "***RECOLECTANDO ATENDEDOR." << std::endl;
              Atendedor *pAtendedor = atendedoresARecolectar.front();
              pAtendedor->pedirDetener();
              pAtendedor->close(NULL);  // TODO (Ivan): verificar valor de retorno.
              delete pAtendedor;
              atendedores.erase(pAtendedor);
              atendedoresARecolectar.pop();
            }
            contadorPurgaAtendedores = 0;
            ++contadorPurgaAdmUsuarios;
            if (contadorPurgaAdmUsuarios >= PURGA_ADM_USUARIOS) {
              admUsuarios.purgar(admArchivos);
              contadorPurgaAdmUsuarios = 0;
            }
          }
          ++contadorRelevarTamanyoBD;
          if (contadorRelevarTamanyoBD >= RELEVO_TAMANYO_BD) {
            admUsuarios.relevarTamanyoBD();
            contadorRelevarTamanyoBD = 0;
          }
        }
      }
      delete pSocketAtendedor;
      try {
        miSocket.close();
      } catch (const sockets::SocketExcepcion &excepcion) {
        //std::cout << "en Servidor::procesar() error: " << excepcion.what() << std::endl;
      }
      std::set< Atendedor * >::iterator iterador = atendedores.begin();
      while (iterador != atendedores.end()) {
        try {
          (*iterador)->pedirDetener();
        } catch (const exceptions::MiExcepcion &excepcion) {
          //std::cerr << "error en Servidor::procesar(): " << excepcion.what() << std::endl;
        }
        try {
          (*iterador)->close((void **) &returnValue);
        } catch (const exceptions::MiExcepcion &excepcion) {
          //std::cerr << "error en Servidor::procesar(): " << excepcion.what() << std::endl;
        }
        delete (*iterador);
        atendedores.erase(iterador++);
      }
    } catch (const exceptions::MiExcepcion &excepcion) {
      if (getDetener() == false) {
        //std::cerr << "error en Servidor::procesar(): " << excepcion.what() << std::endl;
        return (void *) ERROR_EXCEPCION;
      }
    }
    return (void *) returnValue;
  }

  void Servidor::pedirDetener() {
    //std::cout << "Servidor::pedirDetener():" << std::endl;
    Thread::pedirDetener();
    miSocket.interrumpir();
    seInterrumpioSocket = true;
  }

  void Servidor::close(void ** const pPValorDevuelto) {
    //std::cout << "Servidor::close():" << std::endl;
    Thread::pedirDetener();
    if (seInterrumpioSocket == false) {
      miSocket.interrumpir();
      seInterrumpioSocket = true;
    }
    Thread::close(pPValorDevuelto);
  }

//**************************************************EXCEPCIONES**************************************************//

#define TIPO_DE_EXCEPCION ServidorExcepcion
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return "ServidorExcepcion";
  }
#undef TIPO_DE_EXCEPCION

} /* namespace ubicuos */
