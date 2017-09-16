#ifndef PEDIDO_H_
#define PEDIDO_H_

/*
 *      Name: ./common/Pedido.h
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include <string>
#include "./MiSocket.h"
#include "./RegistroArchivo.h"

namespace ubicuos {

  class Pedido {
    private:
      std::string comando;
      std::string nombreArchivo;
      RegistroArchivo registroArchivoNuevo;
      RegistroArchivo registroArchivoViejo;

    public:
      Pedido(const std::string &comando, const std::string &nombreArchivo, const RegistroArchivo &registroArchivoNuevo);

      Pedido(const std::string &comando, const std::string &nombreArchivo, const RegistroArchivo &registroArchivoNuevo, const RegistroArchivo &registroArchivoViejo);

      Pedido(const MiSocket &miSocket);

      virtual ~Pedido() throw ();

      virtual const std::string &getComando() const throw ();

      virtual const std::string &getNombreArchivo() const throw ();

      virtual const RegistroArchivo &getRegistroArchivoNuevo() const throw ();

      virtual const RegistroArchivo &getRegistroArchivoViejo() const throw ();

      virtual void serializar(const MiSocket &miSocket) const;
  };

//**************************************************EXCEPCIONES**************************************************//

  class PedidoExcepcion: public exceptions::MiExcepcion {
    public:
      virtual ~PedidoExcepcion() throw ();
      virtual const char *what() const throw ();
  };

} /* namespace ubicuos */
#endif /* PEDIDO_H_ */
