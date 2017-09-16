/*
 *      Name: ./common/Checksum.cpp
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include "./MisChecksums.h"
#include <cstring>
#include <stdint.h>
#include "./Adler32.h"
#include "./HMAC.h"
#include "./MD5.h"
//#include "./MiSocket.h"

namespace ubicuos {

  Checksum::Checksum() {
  }

  Checksum::~Checksum() {
  }

  uint32_t Checksum::Adler32(const char *data) {
    hashes::Adler32 adler;
    uint32_t length = strlen((char *) data);
    uint32_t hash = adler.checksum(data, length);
    return hash;
  }

  /*
   char *Checksum::MD5(const char *data) {
   hashes::MD5 context;
   unsigned int len = strlen((char *) data);

   context.update((unsigned char *) data, len);
   context.finalize();
   char *hash = context.hex_digest();
   return hash;
   }
   */

  MiHMACResultadoBin::MiHMACResultadoBin() throw () {
  }

  MiHMACResultadoBin::MiHMACResultadoBin(const hashes::HMACResultadoBin &hmacResultadoBin) throw () {
    memcpy(this->hmac, hmacResultadoBin.getDatosPtr(), MD5_LARGO);
  }

  MiHMACResultadoBin::MiHMACResultadoBin(const unsigned char * const hmac) throw () {
    memcpy(this->hmac, hmac, MD5_LARGO);
  }

  /*MiHMACResultadoBin::MiHMACResultadoBin(const MiSocket &miSocket) {
   miSocket.recibirEspecifico(hmac, MD5_LARGO);
   }*/

  MiHMACResultadoBin::~MiHMACResultadoBin() throw () {
  }

  const unsigned char *MiHMACResultadoBin::getDatosPtr() const throw () {
    return hmac;
  }

  unsigned int MiHMACResultadoBin::getLargo() const throw () {
    return MD5_LARGO;
  }

  /*void MiHMACResultadoBin::serializar(const MiSocket& miSocket) const {
   miSocket.enviarCompleto(hmac, MD5_LARGO);
   }*/

  bool MiHMACResultadoBin::esIgual(const MiHMACResultadoBin &otra) const throw () {
    return (memcmp(hmac, otra.hmac, MD5_LARGO) == 0);
  }

  bool MiHMACResultadoBin::esIgual(const hashes::HMAC &otra) const {
    return (memcmp(hmac, otra.getResultadoBin().getDatosPtr(), MD5_LARGO) == 0);
  }

  bool MiHMACResultadoBin::esIgual(const hashes::HMACResultadoBin &otra) const throw () {
    return (memcmp(hmac, otra.getDatosPtr(), MD5_LARGO) == 0);
  }

  bool MiHMACResultadoBin::esIgual(const void * const pOtra) const throw () {
    return (memcmp(hmac, pOtra, MD5_LARGO) == 0);
  }

  bool MiHMACResultadoBin::esIgual(std::string &otra) const throw () {
    return (memcmp(hmac, otra.c_str(), MD5_LARGO) == 0);
  }

  MiMD5ResultadoBin::MiMD5ResultadoBin(const hashes::MD5RawDigest &md5RawDigest) throw () {
    memcpy(this->md5, md5RawDigest.getDataPtr(), MD5_LARGO);
  }

  MiMD5ResultadoBin::MiMD5ResultadoBin(const unsigned char * const md5) throw () {
    memcpy(this->md5, md5, MD5_LARGO);
  }

  /*MiMD5ResultadoBin::MiMD5ResultadoBin(const MiSocket &miSocket) {
   miSocket.recibirEspecifico(md5, MD5_LARGO);
   }*/

  MiMD5ResultadoBin::~MiMD5ResultadoBin() throw () {
  }

  const unsigned char *MiMD5ResultadoBin::getDatosPtr() const throw () {
    return md5;
  }

  unsigned int MiMD5ResultadoBin::getLargo() const throw () {
    return MD5_LARGO;
  }

  /*void MiMD5ResultadoBin::serializar(const MiSocket& miSocket) const {
   miSocket.enviarCompleto(md5, MD5_LARGO);
   }*/

  bool MiMD5ResultadoBin::esIgual(const MiMD5ResultadoBin &otra) const throw () {
    return (memcmp(md5, otra.md5, MD5_LARGO) == 0);
  }

  bool MiMD5ResultadoBin::esIgual(const hashes::MD5 &otra) const throw () {
    return (memcmp(md5, otra.raw_digest().getDataPtr(), MD5_LARGO) == 0);
  }

  bool MiMD5ResultadoBin::esIgual(const hashes::MD5RawDigest &otra) const throw () {
    return (memcmp(md5, otra.getDataPtr(), MD5_LARGO) == 0);
  }

  bool MiMD5ResultadoBin::esIgual(const void * const pOtra) const throw () {
    return (memcmp(md5, pOtra, MD5_LARGO) == 0);
  }

/*bool MiMD5ResultadoBin::esIgual(std::string &otra) const throw () {
 return (memcmp(md5, otra.c_str(), MD5_LARGO) == 0);
 }*/

} /* namespace ubicuos */
