/*
 *      Name: ./server/Atendedor.cpp
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include "./Atendedor.h"
#include <cstddef>
#include <queue>
#include <string>
#include <syslog.h>
#include "./../common/AdmArchivos.h"
#include "./../common/Comandos.h"
#include "./../common/HMAC.h"
#include "./../common/MisChecksums.h"
#include "./../common/MiSocket.h"
#include "./../common/Mutex.h"
#include "./../common/Organizador.h"
#include "./../common/Pedido.h"
#include "./../common/Sincronizador.h"
#include "./../common/ValoresRetorno.h"

#include <iostream>

namespace ubicuos {

  void *Atendedor::RepetidorEnvios::procesar() {
    try {
      //std::cout << "Atendedor::RepetidorEnvios::procesar():" << std::endl;
      Organizador &organizador = admArchivos.getOrganizador(usuarioNombre);
      while (getDetener() == false) {
        sePuedeEnviar.tomar();
        while ((ahoraPuedeEnviar == false) && (getDetener() == false)) {
          sePuedeEnviar.esperar();
        }
        if (getDetener() == false) {
          ahoraPuedeEnviar = false;
          hayElementosParaEnviar.tomar();

          while ((colaDeColaDePedidos.empty() == true) && (getDetener() == false)) {
            hayElementosParaEnviar.esperar();
          }
          if (getDetener() == false) {
            std::queue< Pedido > colaPedidos(colaDeColaDePedidos.front());
            colaDeColaDePedidos.pop();
            hayElementosParaEnviar.liberar();
            organizador.servidorRepetirEnvios(colaPedidos, miSocket);
            sePuedeEnviar.liberar();
          }
        }
      }
    } catch (const exceptions::MiExcepcion &excepcion) {
      if (getDetener() == false) {
        return (void *) ERROR_EXCEPCION;
      }
    }
    return (void *) RESULTADO_OK;
  }

  Atendedor::RepetidorEnvios::RepetidorEnvios(const MiSocket &miSocket, AdmArchivos &admArchivos)
      : miSocket(miSocket), admArchivos(admArchivos), ahoraPuedeEnviar(true) {
  }

  Atendedor::RepetidorEnvios::~RepetidorEnvios() throw () {
    //std::cout << "Atendedor::RepetidorEnvios::~RepetidorEnvios():" << std::endl;
    Thread::pedirDetener();
    //std::cout << "Atendedor::RepetidorEnvios::~RepetidorEnvios(): pidio detener" << std::endl;
    sePuedeEnviar.senyalarATodos();
    //std::cout << "Atendedor::RepetidorEnvios::~RepetidorEnvios(): señalo sePuedeEnviar" << std::endl;
    hayElementosParaEnviar.senyalarATodos();
    //std::cout << "Atendedor::RepetidorEnvios::~RepetidorEnvios(): señalo hayElementosParaEnviar" << std::endl;
    try {
      Thread::close(NULL);
      //std::cout << "Atendedor::RepetidorEnvios::~RepetidorEnvios(): pidio cerrar" << std::endl;
    } catch (const exceptions::MiExcepcion &excepcion) {
      //std::cerr << "error en Atendedor::RepetidorEnvios::~RepetidorEnvios(): " << excepcion.what() << std::endl;
    }

  }

  void Atendedor::RepetidorEnvios::pedirDetener() {
    //std::cout << "Atendedor::RepetidorEnvios::pedirDetener():" << std::endl;
    Thread::pedirDetener();
  }

  void Atendedor::RepetidorEnvios::close(void ** const pPValorDevuelto) {
    //std::cout << "Atendedor::RepetidorEnvios::close():" << std::endl;
    Thread::pedirDetener();
    Thread::close(pPValorDevuelto);
  }

  void Atendedor::RepetidorEnvios::setUsuarioNombre(const std::string &usuarioNombre) {
    this->usuarioNombre = usuarioNombre;
  }

  void Atendedor::RepetidorEnvios::tomar() {
    sePuedeEnviar.tomar();
  }

  void Atendedor::RepetidorEnvios::liberar() {
    sePuedeEnviar.liberar();
  }

  void Atendedor::RepetidorEnvios::senyalar() {
    ahoraPuedeEnviar = true;
    sePuedeEnviar.senyalar();
  }

  void Atendedor::RepetidorEnvios::agregarPedidos(const std::queue< Pedido > &colaPedidos) {
    hayElementosParaEnviar.tomar();
    colaDeColaDePedidos.push(colaPedidos);
    hayElementosParaEnviar.senyalar();
    hayElementosParaEnviar.liberar();
  }

  /**************************************************Atendedor**************************************************/

  bool Atendedor::registrarCliente() {
    //std::cout << "Atendedor::registrarCliente():" << std::endl;
    ::syslog(LOG_USER | LOG_INFO, "Registrar cliente");
    std::string comando;
    MiHMACResultadoBin hmacRecibida = MiHMACResultadoBin();
    pMiSocket->recibirCompleto(comando, hmacRecibida);
    //std::cout << "  Recibido comando: " << comando << std::endl;
    std::string comandoAux(REGISTRACION);
    size_t largoRegistroAux = comandoAux.length();
    if ((comando.length() <= largoRegistroAux) || (comando.compare(0, largoRegistroAux, comandoAux) != 0)) {
      //std::cout << "  ***PEDIDO DE REGISTRACION MAL FORMADO.\n";
      pMiSocket->enviarCompleto(std::string(COMANDO_ERROR));
      //std::cout << "  Enviado: " << COMANDO_ERROR << std::endl;
      socketMutex.tomar();
      if (seInterrumpioSocket == false) {
        pMiSocket->interrumpir();
      }
      pMiSocket->close();
      seInterrumpioSocket = true;
      socketMutex.liberar();
      return false;
    }
    //std::cout << "  ***PEDIDO DE REGISTRACION BIEN FORMADO.\n";
    std::string usuarioNombre(comando.substr(largoRegistroAux));
    if (admUsuarios.verificarUsuario(usuarioNombre) == true) {
      std::string contrasenya(admUsuarios.getContrasenya(usuarioNombre));
      //std::cout << "  ***LA CONTRASEÑA DEL USUARIO ES: " << contrasenya << std::endl;
      hashes::HMAC hmac(contrasenya);
      hmac.agregar(comando);
      hmac.finalizar();
      //std::cout << "  ***EL PEDIDO DE REGISTRACION RECIBIDO GENERA LA HMAC: " << hmac.getResultadoHexa() << std::endl;
      if (hmac.esIgual(hmacRecibida.getDatosPtr()) == true) {
        //std::cout << "  ***PEDIDO DE REGISTRACION CON DATOS CORRECTOS.\n";
        pMiSocket->setClave(contrasenya);
        //std::cout << "  Se configuró HMAC." << std::endl;
        pMiSocket->enviarCompleto(std::string(SI_REGISTRADO));
        //std::cout << "  Enviado: " << SI_REGISTRADO << std::endl;
        ::syslog(LOG_USER | LOG_INFO, "  Enviado: %s", SI_REGISTRADO);
        this->usuarioNombre = usuarioNombre;
        return true;
      }
    }
    //std::cout << "  ***COMANDO DE REGISTRACION CORRUPTO O CON DATOS INCORRECTOS.\n";
    pMiSocket->enviarCompleto(std::string(NO_REGISTRADO));
    //std::cout << "  Enviado: " << NO_REGISTRADO << std::endl;
    ::syslog(LOG_USER | LOG_INFO, "  Enviado: %s", NO_REGISTRADO);
    socketMutex.tomar();
    pMiSocket->interrumpir();
    seInterrumpioSocket = true;
    socketMutex.liberar();
    return false;
  }

  void *Atendedor::procesar() {
    //std::cout << "Atendedor::procesar():" << std::endl;
    ::syslog(LOG_USER | LOG_INFO, "Ejecutando Atendedor::procesar().");
    try {
      while (registrarCliente() == false)
        ;
    } catch (const exceptions::MiExcepcion &excepcion) {
      if (getDetener() == false) {
        //std::cout << "error en Atendedor::procesar(): " << excepcion.what() << std::endl;
        atendedoresARecolectar.push(this);
        return (void *) ERROR_EXCEPCION;
      }
    }
    try {
      admUsuarios.registrar(usuarioNombre, this);
    } catch (const exceptions::MiExcepcion &excepcion) {
      if (getDetener() == false) {
        //std::cout << "error en Atendedor::procesar(): " << excepcion.what() << std::endl;
        atendedoresARecolectar.push(this);
        return (void *) ERROR_EXCEPCION;
      }
    }
    try {
      std::string ruta(admUsuarios.getArchivosRuta(usuarioNombre));
      admArchivos.sincronizacionInicial(usuarioNombre, ruta, *pMiSocket);
      Organizador &organizador = admArchivos.getOrganizador(usuarioNombre);

      repetidorEnvios.setUsuarioNombre(usuarioNombre);
      repetidorEnvios.start();

      while (getDetener() == false) {
        organizador.servidorRecibirActualizaciones(*pMiSocket, admUsuarios, usuarioNombre, this);
      }
    } catch (const exceptions::MiExcepcion &excepcion) {
      if (getDetener() == false) {
        //std::cout << "error en Atendedor::procesar(): " << excepcion.what() << std::endl;
        atendedoresARecolectar.push(this);
        admUsuarios.desregistrar(usuarioNombre, this);
        return (void *) ERROR_EXCEPCION;
      }
    }
    return (void *) RESULTADO_OK;
  }

  Atendedor::Atendedor(MiSocket * const pMiSocket, AdmArchivos &admArchivos, AdmUsuarios &admUsuarios, std::queue< Atendedor * > &atendedoresARecolectar)
      : seInterrumpioSocket(false), pMiSocket(pMiSocket), admArchivos(admArchivos), admUsuarios(admUsuarios), atendedoresARecolectar(atendedoresARecolectar), repetidorEnvios(*pMiSocket, admArchivos) {
  }

  Atendedor::~Atendedor() throw () {
    delete pMiSocket;
  }

  void Atendedor::pedirDetener() {
    Thread::pedirDetener();
    socketMutex.tomar();
    if (seInterrumpioSocket == false) {
      pMiSocket->interrumpir();
      seInterrumpioSocket = true;
    }
    socketMutex.liberar();
  }

  void Atendedor::close(void ** const pPValorDevuelto) {
    Thread::pedirDetener();
    socketMutex.tomar();
    if (seInterrumpioSocket == false) {
      pMiSocket->interrumpir();
      seInterrumpioSocket = true;
    }
    socketMutex.liberar();
    Thread::close(pPValorDevuelto);
  }

  void Atendedor::tomar() {
    repetidorEnvios.tomar();
  }

  void Atendedor::liberar() {
    repetidorEnvios.liberar();
  }

  void Atendedor::senyalar() {
    repetidorEnvios.senyalar();
  }

  void Atendedor::agregarPedidos(const std::queue< Pedido > &colaPedidos) {
    repetidorEnvios.agregarPedidos(colaPedidos);
  }

//**************************************************EXCEPCIONES**************************************************//

#define TIPO_DE_EXCEPCION AtendedorExcepcion
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return "AtendedorExcepcion";
  }
#undef TIPO_DE_EXCEPCION

} /* namespace ubicuos */
