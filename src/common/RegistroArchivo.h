/*
 *      Name: ./common/RegistroArchivo.h
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#ifndef REGISTROARCHIVO_H_
#define REGISTROARCHIVO_H_

#include <ctime>
#include <unistd.h>
#include "./MD5.h"
#include "./MiSocket.h"

namespace ubicuos {

  class RegistroArchivo {
    private:
      off_t tamanyo;
      time_t fecha;
      unsigned char pHash[MD5_LARGO];

    public:
      RegistroArchivo() throw ();

      RegistroArchivo(const off_t tamanyo, const time_t fecha, const unsigned char *pHash) throw ();

      RegistroArchivo(const MiSocket &miSocket);

      virtual ~RegistroArchivo() throw ();

      virtual off_t getTamanyo() const throw ();

      virtual time_t getFecha() const throw ();

      virtual const unsigned char *getHashPtr() const throw ();

      virtual unsigned int getHashLargo() const throw ();

      virtual void serializar(const MiSocket &miSocket) const;

      virtual bool corresponde(const RegistroArchivo &otro) const throw ();
  };

} /* namespace ubicuos */
#endif /* REGISTROARCHIVO_H_ */
