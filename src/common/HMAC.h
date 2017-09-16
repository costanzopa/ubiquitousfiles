#ifndef HMAC_H_
#define HMAC_H_

/*
 *      Name: ./common/HMAC.h
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include <cstddef>
#include <string>
#include "./../common/MD5.h"
#include "./../common/MiExcepcion.h"

namespace hashes {

  class HMACResultadoBin;

  class HMAC {
    private:
      bool finalizado;
      MD5 primeraPasada;
      MD5 segundaPasada;

    public:
      HMAC(const std::string &clave);

      virtual ~HMAC() throw ();

      virtual void agregar(const void * const datos, const size_t largo);

      virtual void agregar(const std::string &datos);

      virtual void finalizar();

      virtual HMACResultadoBin getResultadoBin() const;

      virtual std::string getResultadoHexa() const;

      virtual bool esIgual(const HMAC &otra) const;

      virtual bool esIgual(const HMACResultadoBin &otra) const;

      virtual bool esIgual(const void * const pOtra) const;

      virtual bool esIgual(std::string &otra) const;
  };

  class HMACResultadoBin {
      friend HMACResultadoBin HMAC::getResultadoBin() const throw ();

    private:
      unsigned char hmac[MD5_LARGO];

      HMACResultadoBin(const unsigned char * const hmac) throw ();

    public:
      virtual ~HMACResultadoBin() throw ();

      virtual const unsigned char *getDatosPtr() const throw ();

      virtual unsigned int getLargo() const throw ();

      virtual bool esIgual(const void * const pOtra) const;

      virtual bool esIgual(const HMAC &otra) const;

      virtual bool esIgual(const HMACResultadoBin &otra) const;

      virtual bool esIgual(std::string &otra) const;
  };

//**************************************************EXCEPCIONES**************************************************//

  class HMACExcepcion: public exceptions::MiExcepcion {
    public:
      virtual ~HMACExcepcion() throw ();
      virtual const char *what() const throw ();
  };

} /* namespace hashes */
#endif /* HMAC_H_ */
