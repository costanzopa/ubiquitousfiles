/*
 *      Name: ./common/Pedido.cpp
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include "./Pedido.h"
#include <string>
#include "./Comandos.h"
#include "./MiSocket.h"
#include "./RegistroArchivo.h"

#include <iostream>

namespace ubicuos {

  Pedido::Pedido(const std::string &comando, const std::string &nombreArchivo, const RegistroArchivo &registroArchivoNuevo)
      : comando(comando), nombreArchivo(nombreArchivo), registroArchivoNuevo(registroArchivoNuevo) {
    if (comando == std::string(MODIFICAR_ARCHIVO)) {
      throw PedidoExcepcion();
    }
  }

  Pedido::Pedido(const std::string &comando, const std::string &nombreArchivo, const RegistroArchivo &registroArchivoNuevo, const RegistroArchivo &registroArchivoViejo)
      : comando(comando), nombreArchivo(nombreArchivo), registroArchivoNuevo(registroArchivoNuevo), registroArchivoViejo(registroArchivoViejo) {
  }

  Pedido::Pedido(const MiSocket &miSocket) {
    miSocket.recibirCompleto(comando);
    miSocket.recibirCompleto(nombreArchivo);
    //std::cout << "          RECIBIDO COMANDO: " << comando << std::endl;
    //std::cout << "RECIBIDO NOMBRE DE ARCHIVO: " << nombreArchivo << std::endl;
    registroArchivoNuevo = RegistroArchivo(miSocket);
    if (comando == std::string(MODIFICAR_ARCHIVO)) {
      registroArchivoViejo = RegistroArchivo(miSocket);
    }
  }

  Pedido::~Pedido() throw () {
  }

  const std::string &Pedido::getComando() const throw () {
    return comando;
  }

  const std::string &Pedido::getNombreArchivo() const throw () {
    return nombreArchivo;
  }

  const RegistroArchivo &Pedido::getRegistroArchivoNuevo() const throw () {
    return registroArchivoNuevo;
  }

  const RegistroArchivo &Pedido::getRegistroArchivoViejo() const throw () {
    return registroArchivoViejo;
  }

  void Pedido::serializar(const MiSocket &miSocket) const {
    miSocket.enviarCompleto(comando);
    miSocket.enviarCompleto(nombreArchivo);
    //std::cout << "           ENVIADO COMANDO: " << comando << std::endl;
    //std::cout << " ENVIADO NOMBRE DE ARCHIVO: " << nombreArchivo << std::endl;
    registroArchivoNuevo.serializar(miSocket);
    if (comando == std::string(MODIFICAR_ARCHIVO)) {
      registroArchivoViejo.serializar(miSocket);
    }
  }

//**************************************************EXCEPCIONES**************************************************//

#define TIPO_DE_EXCEPCION PedidoExcepcion
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return "PedidoExcepcion";
  }
#undef TIPO_DE_EXCEPCION

} /* namespace ubicuos */
