#ifndef MISOCKET_H_
#define MISOCKET_H_

/*
 *      Name: ./common/MiSocket.h
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include <string>
#include <stdint.h>
#include <fstream>
#include "./HMAC.h"
#include "./MisChecksums.h"
#include "./Socket.h"

#define BUFFER_TAMANYO 1024

namespace ubicuos {

  class MiSocket: public sockets::Socket {
    private:
      std::string clave;

      // Constructor por copia declarado privado pero no definido (no se deben copiar las instancias de "MiSocket").
      MiSocket(const MiSocket &miSocket);

      // Operador asignación de copia declarado privado pero no definido (no se deben copiar las instancias de "MiSocket").
      MiSocket& operator=(const MiSocket &miSocket);

      virtual void enviarHMAC(const hashes::HMAC &hmac) const;

      virtual bool recibirHMACYVerificar(const hashes::HMAC &hmac) const;

    public:
      MiSocket(const std::string &clave = std::string()) throw ();

      explicit MiSocket(const std::string &puertoNro, const unsigned int colaLargo, const std::string &clave = std::string());

      explicit MiSocket(const std::string &hostNombre, const std::string &puertoNro, const std::string &clave = std::string());

      virtual ~MiSocket() throw ();

      virtual void setClave(const std::string &nuevaClave);

      virtual void enviarCompleto(const std::string &cadena) const;

      virtual void enviarCompleto(std::ifstream &archivo) const;

      virtual void enviarCompletoInseguro(const std::string &cadena) const;

      virtual void enviarCompleto(const void * const datos, const uint32_t largo) const;

      virtual void recibirCompleto(std::string &mensaje) const;

      virtual void recibirCompleto(std::string &mensaje, MiHMACResultadoBin &miHMACResultadoBin) const;

      virtual void recibirCompleto(void ** const datos, uint32_t * const largo) const;

      virtual void recibirCompleto(std::ofstream &archivo) const;

      virtual void recibirCompleto() const;

      virtual void recibirCompletoInseguro(std::string &mensaje) const;

      virtual void recibirEspecifico(void * const datos, const uint32_t largo) const;
  };

//**************************************************EXCEPCIONES**************************************************//

  class MiSocketExcepcion: public sockets::SocketExcepcion {
    public:
      virtual ~MiSocketExcepcion() throw ();
      virtual const char *what() const throw ();
  };

  class MiSocketExcepcionRecibirEnSocketDesconectadoDelOtroLado: public MiSocketExcepcion {
    public:
      virtual ~MiSocketExcepcionRecibirEnSocketDesconectadoDelOtroLado() throw ();
      virtual const char *what() const throw ();
  };

} /* namespace ubicuos */
#endif /* MISOCKET_H_ */
