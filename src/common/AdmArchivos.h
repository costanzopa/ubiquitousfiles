#ifndef ADMARCHIVOS_H_
#define ADMARCHIVOS_H_

/*
 *      Name: ./common/AdmArchivos.h
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include <map>
#include <string>
#include "./MiSocket.h"
#include "./Mutex.h"
//#include "./Organizador.h"

namespace ubicuos {

  class Organizador;

  class AdmArchivos {
    private:
      std::map< const std::string, Organizador > mapaPorUsuario;
      threads::Mutex mutex;

    public:
      AdmArchivos();

      virtual ~AdmArchivos() throw ();

      // Usada por el "Atendedor".
      virtual void sincronizacionInicial(const std::string &nombreUsuario, const std::string &directorioRuta, const MiSocket &miSocket);

      virtual Organizador &getOrganizador(const std::string &nombreUsuario);

      virtual void relevarTamanyoBD();

      virtual void eliminarUsuario(const std::string &nombreUsuario);
  };

//**************************************************EXCEPCIONES**************************************************//

  class AdmArchivosExcepcion: public exceptions::MiExcepcion {
    public:
      virtual ~AdmArchivosExcepcion() throw ();
      virtual const char *what() const throw ();
  };

} /* namespace ubicuos */
#endif /* ADMARCHIVOS_H_ */
