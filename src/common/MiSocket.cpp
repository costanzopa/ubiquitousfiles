/*
 *      Name: ./common/MiSocket.cpp
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include "./MiSocket.h"
#include <cstring>  /* Funciones "memset", y "strlen". */
#include <netinet/in.h>   /* Funciones "htonl", y "ntohl". */
#include <stdint.h>  /* Tipo "uint32_t". */
#include <string>
#include <sys/socket.h>  /* Tipo "socklen_t", structs "sockaddr", y "sockaddr_storage", y funcion "accept".*/
#include "./HMAC.h"
#include "./MisChecksums.h"
#include "./Socket.h"

using sockets::Socket;

namespace ubicuos {

  void MiSocket::enviarHMAC(const hashes::HMAC &hmac) const {
    size_t falta = hmac.getResultadoBin().getLargo();
    size_t enviado = 0;
    int error;
    do {
      error = enviar(hmac.getResultadoBin().getDatosPtr() + enviado, falta);
      enviado += error;
      falta -= error;
    } while (falta > 0);
  }

  bool MiSocket::recibirHMACYVerificar(const hashes::HMAC &hmac) const {
    unsigned char hmacRecibido[MD5_LARGO];
    size_t falta = MD5_LARGO;
    size_t recibido = 0;
    int error;
    do {
      error = recibir(hmacRecibido + recibido, falta);
      if (error == 0) {
        throw MiSocketExcepcionRecibirEnSocketDesconectadoDelOtroLado();
      } else {
        recibido += error;
        falta -= error;
      }
    } while (falta > 0);
    return hmac.esIgual(hmacRecibido);
  }

  MiSocket::MiSocket(const std::string &clave) throw ()
      : Socket(), clave(clave) {
  }

  MiSocket::MiSocket(const std::string &puertoNro, const unsigned int colaLargo, const std::string &clave)
      : Socket(puertoNro, colaLargo), clave(clave) {
  }

  MiSocket::MiSocket(const std::string &hostNombre, const std::string &puertoNro, const std::string &clave)
      : Socket(hostNombre, puertoNro), clave(clave) {
  }

  MiSocket::~MiSocket() throw () {
  }

  void MiSocket::setClave(const std::string &nuevaClave) {
    clave = nuevaClave;
  }

  void MiSocket::enviarCompleto(const std::string &cadena) const {
    const uint32_t largo = cadena.length();
    const char *datos = cadena.c_str();
    uint32_t mensajeLargoNet = htonl(largo);
    size_t falta = sizeof mensajeLargoNet;
    size_t enviado = 0;
    int error;
    while (falta > 0) {
      error = enviar((char *) &mensajeLargoNet + enviado, falta);
      enviado += error;
      falta -= error;
    }
    falta = largo;
    enviado = 0;
    while (falta > 0) {
      error = enviar((char *) datos + enviado, falta);
      enviado += error;
      falta -= error;
    }
    hashes::HMAC hmac(clave);
    hmac.agregar(datos, largo);
    hmac.finalizar();
    enviarHMAC(hmac);
  }

  void MiSocket::enviarCompleto(const void * const datos, const uint32_t largo) const {
    uint32_t mensajeLargoNet = htonl(largo);
    size_t falta = sizeof mensajeLargoNet;
    size_t enviado = 0;
    int error;
    while (falta > 0) {
      error = enviar((char *) &mensajeLargoNet + enviado, falta);
      enviado += error;
      falta -= error;
    }
    falta = largo;
    enviado = 0;
    while (falta > 0) {
      error = enviar((char *) datos + enviado, falta);
      enviado += error;
      falta -= error;
    }
    hashes::HMAC hmac(clave);
    hmac.agregar(datos, largo);
    hmac.finalizar();
    enviarHMAC(hmac);
  }

  void MiSocket::enviarCompleto(std::ifstream &archivo) const {
    const std::streampos inicio = archivo.tellg();
    archivo.seekg(0, std::ios::end);
    const uint32_t largo = (uint32_t) (archivo.tellg() - inicio);
    archivo.seekg(inicio);
    uint32_t mensajeLargoNet = htonl(largo);
    size_t falta = sizeof mensajeLargoNet;
    size_t enviado = 0;
    int error;
    while (falta > 0) {
      error = enviar((char *) &mensajeLargoNet + enviado, falta);
      enviado += error;
      falta -= error;
    }
    char buffer[BUFFER_TAMANYO];
    falta = largo;
    hashes::HMAC hmac(clave);
    while (falta > 0) {
      enviado = 0;
      archivo.read(buffer, BUFFER_TAMANYO);
      std::streamsize leido = archivo.gcount();
      falta -= leido;
      hmac.agregar(buffer, leido);
      while (leido > 0) {
        error = enviar(buffer + enviado, leido);
        enviado += error;
        leido -= error;
      }
    }
    hmac.finalizar();
    enviarHMAC(hmac);
  }

  void MiSocket::enviarCompletoInseguro(const std::string &cadena) const {
    const uint32_t largo = cadena.length();
    const char *datos = cadena.c_str();
    uint32_t mensajeLargoNet = htonl(largo);
    size_t falta = sizeof mensajeLargoNet;
    size_t enviado = 0;
    int error;
    while (falta > 0) {
      error = enviar((char *) &mensajeLargoNet + enviado, falta);
      enviado += error;
      falta -= error;
    }
    falta = largo;
    enviado = 0;
    while (falta > 0) {
      error = enviar((char *) datos + enviado, falta);
      enviado += error;
      falta -= error;
    }
  }

  void MiSocket::recibirCompleto(std::string &mensaje) const {
    uint32_t mensajeLargoNet;
    size_t falta = sizeof mensajeLargoNet;
    size_t recibido = 0;
    int error;
    while (falta > 0) {
      error = recibir((char *) &mensajeLargoNet + recibido, falta);
      if (error == 0) {
        throw MiSocketExcepcionRecibirEnSocketDesconectadoDelOtroLado();
      } else {
        recibido += error;
        falta -= error;
      }
    }
    size_t mensajeLargoHost = ntohl(mensajeLargoNet);
    char *mensajeAux = new char[mensajeLargoHost + 1];
    memset(mensajeAux, 0, mensajeLargoHost + 1);
    falta = mensajeLargoHost;
    recibido = 0;
    while (falta > 0) {
      error = recibir(mensajeAux + recibido, falta);
      if (error == 0) {
        delete[] mensajeAux;
        throw MiSocketExcepcionRecibirEnSocketDesconectadoDelOtroLado();
      } else {
        recibido += error;
        falta -= error;
      }
    }
    hashes::HMAC hmac(clave);
    hmac.agregar(mensajeAux, recibido);
    hmac.finalizar();
    if (recibirHMACYVerificar(hmac) == true) {
      mensaje = mensajeAux;
      delete[] mensajeAux;
    } else {
      delete[] mensajeAux;
      throw MiSocketExcepcion();
    }
  }

  void MiSocket::recibirCompleto(std::string &mensaje, MiHMACResultadoBin &miHMACResultadoBin) const {
    uint32_t mensajeLargoNet;
    size_t falta = sizeof mensajeLargoNet;
    size_t recibido = 0;
    int error;
    while (falta > 0) {
      error = recibir((char *) &mensajeLargoNet + recibido, falta);
      if (error == 0) {
        throw MiSocketExcepcionRecibirEnSocketDesconectadoDelOtroLado();
      } else {
        recibido += error;
        falta -= error;
      }
    }
    size_t mensajeLargoHost = ntohl(mensajeLargoNet);
    char *mensajeAux = new char[mensajeLargoHost + 1];
    memset(mensajeAux, 0, mensajeLargoHost + 1);
    falta = mensajeLargoHost;
    recibido = 0;
    while (falta > 0) {
      error = recibir(mensajeAux + recibido, falta);
      if (error == 0) {
        delete[] mensajeAux;
        throw MiSocketExcepcionRecibirEnSocketDesconectadoDelOtroLado();
      } else {
        recibido += error;
        falta -= error;
      }
    }
    unsigned char hmacRecibido[MD5_LARGO];
    falta = MD5_LARGO;
    recibido = 0;
    while (falta > 0) {
      error = recibir(hmacRecibido + recibido, falta);
      if (error == 0) {
        delete[] mensajeAux;
        throw MiSocketExcepcionRecibirEnSocketDesconectadoDelOtroLado();
      } else {
        recibido += error;
        falta -= error;
      }
    }
    mensaje = mensajeAux;
    miHMACResultadoBin = MiHMACResultadoBin(hmacRecibido);
    delete[] mensajeAux;
  }

  void MiSocket::recibirCompleto(std::ofstream &archivo) const {
    uint32_t mensajeLargoNet;
    size_t falta = sizeof mensajeLargoNet;
    size_t recibido = 0;
    int error;
    while (falta > 0) {
      error = recibir((char *) &mensajeLargoNet + recibido, falta);
      if (error == 0) {
        throw MiSocketExcepcionRecibirEnSocketDesconectadoDelOtroLado();
      } else {
        recibido += error;
        falta -= error;
      }
    }
    size_t mensajeLargoHost = ntohl(mensajeLargoNet);
    char buffer[BUFFER_TAMANYO];
    falta = mensajeLargoHost;
    hashes::HMAC hmac(clave);
    while (falta > 0) {
      error = recibir(buffer, falta < BUFFER_TAMANYO ? falta : BUFFER_TAMANYO);
      if (error == 0) {
        throw MiSocketExcepcionRecibirEnSocketDesconectadoDelOtroLado();
      } else {
        archivo.write(buffer, error);
        hmac.agregar(buffer, error);
        falta -= error;
      }
    }
    hmac.finalizar();
    if (recibirHMACYVerificar(hmac) != true) {
      throw MiSocketExcepcion();
    }
  }

  void MiSocket::recibirCompleto() const {
    uint32_t mensajeLargoNet;
    size_t falta = sizeof mensajeLargoNet;
    size_t recibido = 0;
    int error;
    while (falta > 0) {
      error = recibir((char *) &mensajeLargoNet + recibido, falta);
      if (error == 0) {
        throw MiSocketExcepcionRecibirEnSocketDesconectadoDelOtroLado();
      } else {
        recibido += error;
        falta -= error;
      }
    }
    size_t mensajeLargoHost = ntohl(mensajeLargoNet);
    char buffer[BUFFER_TAMANYO];
    falta = mensajeLargoHost;
    hashes::HMAC hmac(clave);
    while (falta > 0) {
      error = recibir(buffer, falta < BUFFER_TAMANYO ? falta : BUFFER_TAMANYO);
      if (error == 0) {
        throw MiSocketExcepcionRecibirEnSocketDesconectadoDelOtroLado();
      } else {
        hmac.agregar(buffer, error);
        falta -= error;
      }
    }
    hmac.finalizar();
    if (recibirHMACYVerificar(hmac) != true) {
      throw MiSocketExcepcion();
    }
  }

  void MiSocket::recibirCompletoInseguro(std::string &mensaje) const {
    uint32_t mensajeLargoNet;
    size_t falta = sizeof mensajeLargoNet;
    size_t recibido = 0;
    int error;
    while (falta > 0) {
      error = recibir((char *) &mensajeLargoNet + recibido, falta);
      if (error == 0) {
        throw MiSocketExcepcionRecibirEnSocketDesconectadoDelOtroLado();
      } else {
        recibido += error;
        falta -= error;
      }
    }
    size_t mensajeLargoHost = ntohl(mensajeLargoNet);
    char *mensajeAux = new char[mensajeLargoHost + 1];
    memset(mensajeAux, 0, mensajeLargoHost + 1);
    falta = mensajeLargoHost;
    recibido = 0;
    while (falta > 0) {
      error = recibir(mensajeAux + recibido, falta);
      if (error == 0) {
        delete[] mensajeAux;
        throw MiSocketExcepcionRecibirEnSocketDesconectadoDelOtroLado();
      } else {
        recibido += error;
        falta -= error;
      }
    }
    mensaje = mensajeAux;
    delete[] mensajeAux;
  }

  void MiSocket::recibirCompleto(void ** const datos, uint32_t * const largo) const {
    uint32_t mensajeLargoNet;
    size_t falta = sizeof mensajeLargoNet;
    size_t recibido = 0;
    int error;
    while (falta > 0) {
      error = recibir((char *) &mensajeLargoNet + recibido, falta);
      if (error == 0) {
        throw MiSocketExcepcionRecibirEnSocketDesconectadoDelOtroLado();
      } else {
        recibido += error;
        falta -= error;
      }
    }
    size_t mensajeLargoHost = ntohl(mensajeLargoNet);
    *datos = new char[mensajeLargoHost];
    falta = mensajeLargoHost;
    recibido = 0;
    while (falta > 0) {
      error = recibir(((char *) (*datos)) + recibido, falta);
      if (error == 0) {
        delete[] (char *) (*datos);
        *datos = NULL;
        throw MiSocketExcepcionRecibirEnSocketDesconectadoDelOtroLado();
      } else {
        recibido += error;
        falta -= error;
      }
    }
    hashes::HMAC hmac(clave);
    hmac.agregar(*datos, recibido);
    hmac.finalizar();
    if (recibirHMACYVerificar(hmac) == true) {
      *largo = mensajeLargoHost;
    } else {
      delete[] (char *) *datos;
      *datos = NULL;
      throw MiSocketExcepcion();
    }
  }

  void MiSocket::recibirEspecifico(void * const datos, const uint32_t largo) const {
    uint32_t mensajeLargoNet;
    size_t falta = sizeof mensajeLargoNet;
    size_t recibido = 0;
    int error;
    while (falta > 0) {
      error = recibir((char *) &mensajeLargoNet + recibido, falta);
      if (error == 0) {
        throw MiSocketExcepcionRecibirEnSocketDesconectadoDelOtroLado();
      } else {
        recibido += error;
        falta -= error;
      }
    }
    size_t mensajeLargoHost = ntohl(mensajeLargoNet);
    if (mensajeLargoHost != largo) {
      throw MiSocketExcepcion();  // Agregar "mensajeLargoHost" a la excepción.
    }
    falta = mensajeLargoHost;
    recibido = 0;
    while (falta > 0) {
      error = recibir((char *) datos + recibido, falta);
      if (error == 0) {
        throw MiSocketExcepcionRecibirEnSocketDesconectadoDelOtroLado();
      } else {
        recibido += error;
        falta -= error;
      }
    }
    hashes::HMAC hmac(clave);
    hmac.agregar(datos, recibido);
    hmac.finalizar();
    if (recibirHMACYVerificar(hmac) == true) {
      return;
    } else {
      throw MiSocketExcepcion();
    }
  }

//**************************************************EXCEPCIONES**************************************************//

#define TIPO_DE_EXCEPCION MiSocketExcepcion
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return "MiSocketExcepcion";
  }
#undef TIPO_DE_EXCEPCION

//**************************************************

#define TIPO_DE_EXCEPCION MiSocketExcepcionRecibirEnSocketDesconectadoDelOtroLado
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return "MiSocketExcepcionRecibirEnSocketDesconectadoDelOtroLado";
  }
#undef TIPO_DE_EXCEPCION

} /* namespace ubicuos */
