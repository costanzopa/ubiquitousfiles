#ifndef CLIENTE_H_
#define CLIENTE_H_

/*
 *      Name: ./client/Cliente.h
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 * Las instancias de "Cliente" son las encargadas de establecer la comunicación con el servidor, coordinar la sincronización inicial de archivos con el servidor, el lanzamiento del hilo "Supervisor", el manejo de los paquetes de sincronización de archivos que llegan del servidor, y el cierre de la conexión y del hilo "Supervisor".
 */

#include <string>
#include "./../common/MiExcepcion.h"
#include "./../common/MiSocket.h"
#include "./../common/Organizador.h"
#include "./../common/Sincronizador.h"
#include "./../common/Thread.h"

namespace ubicuos {

  class Supervisor;

  class Cliente: public threads::Thread {
    private:
      bool seInterrumpioSocket;
      MiSocket miSocket;
      const unsigned int intervalo;
      const std::string usuarioNombre;
      const std::string contrasenya;
      Organizador organizador;
      bool registrado;
      threads::Sincronizador sincronizador;
      bool sincronizado;

      // Constructor por copia declarado privado pero no definido (no se deben copiar las instancias de "Cliente").
      Cliente(const Cliente &cliente);

      // Operador asignación de copia declarado privado pero no definido (no se deben copiar las instancias de "Cliente").
      Cliente& operator=(const Cliente &cliente);

      // Es una función que se ejecuta cuando la sincronización temporizada de los archivos se completa sin conflictos. Si no se ejecutó, es porque se intentó actualizar archivos del servidor a partir de versiones desactualizadas, con lo cual en algún momento cercano llegarán notificaciones de actualización del servidor (emitidas originalmente por el cliente que efectivamente modificó los archivos en el servidor) que generarán un conflicto (en el lado del cliente) a ser resuelto por el usuario.
      static void sincronizacionOK() throw ();

      // Es una función que se ejecuta cuando hay conflictos por resolver. Si se ejecutó, es porque se intentó actualizar archivos del servidor a partir de versiones desactualizadas, con lo cual en algún momento cercano llegarán notificaciones de actualización del servidor (emitidas originalmente por el cliente que efectivamente modificó los archivos en el servidor) que generarán un conflicto (en el lado del cliente) a ser resuelto por el usuario, o porque se recibieron modificaciones que intentan actualizar archivos locales que fueron modificado por el cliente.
      static void sincronizacionConflicto(const std::string &explicacion) throw ();

      // Es la función que se ejecuta en un hilo nuevo en un ciclo infinito hasta que se ejecute "pedirDetener()", "close()", o "~Cliente()".
      // Intenta registrar al cliente ante el servidor, bajar una copia de la estructura y contenidos de los archivos que el servidor tiene almacenados para este cliente, lanzar el hilo "Supervisor" que realiza el "polling" y envía los reportes de sincronización temporizada; luego de esto entra en un ciclo infinito donde recibe actualizaciones que otros clientes hacen al servidor estando registrados bajo este mismo nombre de usuario.
      virtual void *procesar();

    public:
      Cliente(const std::string &hostDireccion, const std::string &hostPuerto, const std::string &directorioRuta, const unsigned int intervalo, const std::string &usuarioNombre, const std::string &contrasenya);

      virtual ~Cliente() throw ();

      // Intenta registrarse ante el servidor.
      virtual bool registrar();

      virtual void pedirDetener();

      virtual void close(void ** const pPValorDevuelto);

      virtual bool getRegistrado() const throw ();

      virtual void sincronizarYBloquear(Supervisor &supervisor);

      virtual void informarSincronizado();

      virtual void desbloquearSincronizaciones();
  };

//**************************************************EXCEPCIONES**************************************************//

  class ClienteExcepcion: public exceptions::MiExcepcion {
    public:
      virtual ~ClienteExcepcion() throw ();
      virtual const char *what() const throw ();
  };

} /* namespace ubicuos */
#endif /* CLIENTE_H_ */
