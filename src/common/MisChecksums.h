#ifndef CHECKSUM_H_
#define CHECKSUM_H_

/*
 *      Name: ./common/Checksum.h
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include <stdint.h>
#include "./Adler32.h"
#include "./HMAC.h"
#include "./MD5.h"
//#include "./MiSocket.h"

namespace ubicuos {

  class Checksum {
    public:
      Checksum();
      virtual ~Checksum();
      uint32_t Adler32(const char *data);
      /*char *MD5(const char *data);*/
  };

  class MiHMACResultadoBin {
    private:
      unsigned char hmac[MD5_LARGO];

    public:
      MiHMACResultadoBin() throw ();

      MiHMACResultadoBin(const hashes::HMACResultadoBin &hmacResultadoBin) throw ();

      MiHMACResultadoBin(const unsigned char * const hmac) throw ();

      //MiHMACResultadoBin(const MiSocket &miSocket);

      virtual ~MiHMACResultadoBin() throw ();

      virtual const unsigned char *getDatosPtr() const throw ();

      virtual unsigned int getLargo() const throw ();

      //virtual void serializar(const MiSocket &miSocket) const;

      virtual bool esIgual(const MiHMACResultadoBin &otra) const throw ();

      virtual bool esIgual(const hashes::HMAC &otra) const;

      virtual bool esIgual(const hashes::HMACResultadoBin &otra) const throw ();

      virtual bool esIgual(const void * const pOtra) const throw ();

      virtual bool esIgual(std::string &otra) const throw ();
  };

  class MiMD5ResultadoBin {
    private:
      unsigned char md5[MD5_LARGO];

    public:
      MiMD5ResultadoBin(const hashes::MD5RawDigest &md5RawDigest) throw ();

      MiMD5ResultadoBin(const unsigned char * const md5) throw ();

      //MiMD5ResultadoBin(const MiSocket &miSocket);

      virtual ~MiMD5ResultadoBin() throw ();

      virtual const unsigned char *getDatosPtr() const throw ();

      virtual unsigned int getLargo() const throw ();

      //virtual void serializar(const MiSocket &miSocket) const;

      virtual bool esIgual(const MiMD5ResultadoBin &otra) const throw ();

      virtual bool esIgual(const hashes::MD5 &otra) const throw ();

      virtual bool esIgual(const hashes::MD5RawDigest &otra) const throw ();

      virtual bool esIgual(const void * const pOtra) const throw ();

      //virtual bool esIgual(std::string &otra) const throw ();
  };

} /* namespace ubicuos */
#endif /* CHECKSUM_H_ */
