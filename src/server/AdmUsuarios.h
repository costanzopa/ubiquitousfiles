#ifndef ADMUSUARIOS_H_
#define ADMUSUARIOS_H_

/*
 *      Name: ./server/AdmUsuarios.h
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include <map>
#include <set>
#include <string>
#include "./../common/AdmArchivos.h"
#include "./../common/MiExcepcion.h"
#include "./../common/Mutex.h"
#include "./../common/UserFile.h"
namespace ubicuos {

  // Declaración de clase "Atendedor" para evitar problemas de referencia circular.
  class Atendedor;

  class AdmUsuarios {
    private:

      class Usuario {
        private:
          const std::string contrasenya;
          const std::string archivosRuta;

          // Cada instancia de "Usuario" tiene una lista de atendedores, donde se colocan punteros a las instancias de "Atendedor" correspondientes a todos los clientes que se registraron con el nombre de usuario correspondiente, a fín de mandarles la actualización de archivos en forma grupal, cada vez que el servidor reciba un pedido de sincronización temporizada válido.
          std::set< Atendedor * > listaAtendedores;

        public:
          Usuario(const std::string &contrasenya, const std::string &archivosRuta);

          const std::string &getContrasenya() const throw ();

          const std::string &getArchivosRuta() const throw ();

          void agregarAtendedor(Atendedor *pAtendedor);

          void desagregarAtendedor(Atendedor *pAtendedor);

          bool tieneAtendedores() const throw ();

          const std::set< Atendedor * > &getAtendedoresRef() const;
      };

      // Ruta absoluta a la ubicación del archivo de usuarios.
      // También funciona como una ruta absoluta a partir de donde cada instancia de "Usuario" tiene su propia ruta.
      const std::string &directorioRuta;

      // Un mapa de instancias de "Usuario", donde la clave es el "nombre" del usuario.
      std::map< const std::string, Usuario > mapaUsuarios;
      // Los contenedores de la STL aceptan accesos para lectura concurrentes.

      threads::Mutex mutex;

    public:
      AdmUsuarios(const std::string &directorioRuta) throw ();

      virtual ~AdmUsuarios() throw ();

      virtual bool verificarUsuario(const std::string &nombre);

      virtual const std::string &getContrasenya(const std::string &nombre);

      virtual const std::string &getArchivosRuta(const std::string &nombre);

      virtual void registrar(const std::string &nombre, Atendedor *pAtendedor);

      virtual void desregistrar(const std::string &nombre, Atendedor *pAtendedor);

      virtual void purgar(AdmArchivos &admArchivos);

      virtual const std::set< Atendedor * > &getAtendedoresRef(const std::string nombre) const;

      virtual void relevarTamanyoBD();
  };

//**************************************************EXCEPCIONES**************************************************//

  class AdmUsuariosExcepcion: public exceptions::MiExcepcion {
    public:
      virtual ~AdmUsuariosExcepcion() throw ();
      virtual const char *what() const throw ();
  };

} /* namespace ubicuos */
#endif /* ADMUSUARIOS_H_ */
