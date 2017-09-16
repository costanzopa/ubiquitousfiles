/*
 *      Name: ./common/HMAC.cpp
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include "./HMAC.h"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <stdint.h>
#include <string>
#include "./../common/MD5.h"

#define HMAC_BLOQUE_LARGO 64

namespace hashes {

  HMAC::HMAC(const std::string &clave)
      : finalizado(false) {
    size_t claveLargo = clave.length();
    unsigned char entradaClave[HMAC_BLOQUE_LARGO];
    memset(entradaClave, 0, HMAC_BLOQUE_LARGO);
    if (claveLargo > HMAC_BLOQUE_LARGO) {
      MD5 claveHash;
      claveHash.update(clave.c_str(), claveLargo);
      claveHash.finalize();
      MD5RawDigest claveDigest(claveHash.raw_digest());
      memcpy(entradaClave, claveDigest.getDataPtr(), claveDigest.getLength());
    } else {
      memcpy(entradaClave, clave.c_str(), claveLargo);
    }
    unsigned char entradaClaveMascara[HMAC_BLOQUE_LARGO];
    for (int contador = 0; contador < HMAC_BLOQUE_LARGO; ++contador) {
      entradaClaveMascara[contador] = (entradaClave[contador] ^ 0x36);
    }
    primeraPasada.update(entradaClaveMascara, HMAC_BLOQUE_LARGO);
    unsigned char salidaClaveMascara[HMAC_BLOQUE_LARGO];
    for (int contador = 0; contador < HMAC_BLOQUE_LARGO; ++contador) {
      salidaClaveMascara[contador] = (entradaClave[contador] ^ 0x5C);
    }
    segundaPasada.update(salidaClaveMascara, HMAC_BLOQUE_LARGO);
  }

  HMAC::~HMAC() throw () {
  }

  void HMAC::agregar(const void * const datos, const size_t largo) {
    if (finalizado == false) {
      primeraPasada.update(datos, largo);
    } else {
      throw HMACExcepcion();
    }
  }

  void HMAC::agregar(const std::string &datos) {
    if (finalizado == false) {
      primeraPasada.update(datos.c_str(), datos.length());
    } else {
      throw HMACExcepcion();
    }
  }

  void HMAC::finalizar() {
    if (finalizado == false) {
      primeraPasada.finalize();
      MD5RawDigest entradaDigest(primeraPasada.raw_digest());
      segundaPasada.update(entradaDigest.getDataPtr(), entradaDigest.getLength());
      segundaPasada.finalize();
      finalizado = true;
    } else {
      throw HMACExcepcion();
    }
  }

  HMACResultadoBin HMAC::getResultadoBin() const {
    if (finalizado == false) {
      throw HMACExcepcion();
    } else {
      MD5RawDigest salidaDigest(segundaPasada.raw_digest());
      return HMACResultadoBin(salidaDigest.getDataPtr());
    }
  }

  std::string HMAC::getResultadoHexa() const {
    if (finalizado == false) {
      throw HMACExcepcion();
    } else {
      return std::string(segundaPasada.hex_digest());
    }
  }

  bool HMAC::esIgual(const HMAC &otra) const {
    if (finalizado == false) {
      throw HMACExcepcion();
    } else {
      return (memcmp(segundaPasada.raw_digest().getDataPtr(), otra.segundaPasada.raw_digest().getDataPtr(), MD5_LARGO) == 0);
    }
  }

  bool HMAC::esIgual(const HMACResultadoBin &otra) const {
    if (finalizado == false) {
      throw HMACExcepcion();
    } else {
      return (memcmp(segundaPasada.raw_digest().getDataPtr(), otra.getDatosPtr(), MD5_LARGO) == 0);
    }
  }

  bool HMAC::esIgual(const void * const pDatos) const {
    if (finalizado == false) {
      throw HMACExcepcion();
    } else {
      return (memcmp(segundaPasada.raw_digest().getDataPtr(), pDatos, MD5_LARGO) == 0);
    }
  }

  bool HMAC::esIgual(std::string &otra) const {
    if (finalizado == false) {
      throw HMACExcepcion();
    } else {
      return (memcmp(segundaPasada.hex_digest().c_str(), otra.c_str(), MD5_LARGO) == 0);
    }
  }

  HMACResultadoBin::HMACResultadoBin(const unsigned char * const hmac) throw () {
    memcpy(this->hmac, hmac, MD5_LARGO);
  }

  HMACResultadoBin::~HMACResultadoBin() throw () {
  }

  const unsigned char *HMACResultadoBin::getDatosPtr() const throw () {
    return hmac;
  }

  unsigned int HMACResultadoBin::getLargo() const throw () {
    return MD5_LARGO;
  }

  bool HMACResultadoBin::esIgual(const void * const pDatos) const {
    return (memcmp(hmac, pDatos, MD5_LARGO) == 0);
  }

  bool HMACResultadoBin::esIgual(const HMAC &otra) const {
    return (memcmp(hmac, otra.getResultadoBin().getDatosPtr(), MD5_LARGO) == 0);
  }

  bool HMACResultadoBin::esIgual(const HMACResultadoBin &otra) const {
    return (memcmp(hmac, otra.getDatosPtr(), MD5_LARGO) == 0);
  }

  bool HMACResultadoBin::esIgual(std::string &otra) const {
    char string[MD5_LARGO * 2 + 1];

    for (int i = 0; i < MD5_LARGO; ++i)
      sprintf(string + i * 2, "%02x", otra.c_str()[i]);

    string[MD5_LARGO * 2] = '\0';

    return (memcmp(hmac, string, MD5_LARGO * 2 + 1) == 0);
  }

//**************************************************EXCEPCIONES**************************************************//

#define TIPO_DE_EXCEPCION HMACExcepcion
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return "HMACExcepcion";
  }
#undef TIPO_DE_EXCEPCION

} /* namespace hashes */
