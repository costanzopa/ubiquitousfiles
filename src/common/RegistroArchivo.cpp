/*
 *      Name: ./common/RegistroArchivo.cpp
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include "./RegistroArchivo.h"
#include <cstring>
#include <ctime>
#include <unistd.h>
#include "./MD5.h"
#include "./MiSocket.h"

#include <iostream>

namespace ubicuos {

  RegistroArchivo::RegistroArchivo() throw ()
      : tamanyo(tamanyo), fecha(fecha) {
    memset(pHash, 0, MD5_LARGO);
  }

  RegistroArchivo::RegistroArchivo(const off_t tamanyo, const time_t fecha, const unsigned char *pHash) throw ()
      : tamanyo(tamanyo), fecha(fecha) {
    memcpy(this->pHash, pHash, MD5_LARGO);
  }

  RegistroArchivo::RegistroArchivo(const MiSocket &miSocket)
      : tamanyo(0), fecha(0) {
    miSocket.recibirEspecifico(&tamanyo, sizeof tamanyo);
    miSocket.recibirEspecifico(&fecha, sizeof fecha);
    miSocket.recibirEspecifico(pHash, MD5_LARGO);
    //std::cout << "RECIBIDO TAMAÑO DE ARCHIVO: " << tamanyo << "Bytes." << std::endl;
    //std::cout << " RECIBIDO FECHA DE ARCHIVO: " << fecha << std::endl;
  }

  RegistroArchivo::~RegistroArchivo() throw () {
  }

  off_t RegistroArchivo::getTamanyo() const throw () {
    return tamanyo;
  }

  time_t RegistroArchivo::getFecha() const throw () {
    return fecha;
  }

  const unsigned char *RegistroArchivo::getHashPtr() const throw () {
    return pHash;
  }

  unsigned int RegistroArchivo::getHashLargo() const throw () {
    return MD5_LARGO;
  }

  void RegistroArchivo::serializar(const MiSocket &miSocket) const {
    miSocket.enviarCompleto(&tamanyo, sizeof tamanyo);
    miSocket.enviarCompleto(&fecha, sizeof fecha);
    miSocket.enviarCompleto(pHash, MD5_LARGO);
    //std::cout << " ENVIADO TAMAÑO DE ARCHIVO: " << tamanyo << "Bytes." << std::endl;
    //std::cout << "  ENVIADO FECHA DE ARCHIVO: " << fecha << std::endl;
  }

  bool RegistroArchivo::corresponde(const RegistroArchivo &otro) const throw () {
    if ((tamanyo == otro.tamanyo) && (fecha == otro.fecha) && (memcmp(pHash, otro.pHash, MD5_LARGO) == 0)) {
      return true;
    } else {
      return false;
    }
  }

}
/* namespace ubicuos */
