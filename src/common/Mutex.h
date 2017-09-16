#ifndef MUTEX_H_
#define MUTEX_H_

/*
 *      Name: ./server/Mutex.h
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include <pthread.h>
#include "./../common/MiExcepcion.h"

namespace threads {

  enum MutexInicializarModo {
    MUTEX_DEFAULT = 0, MUTEX_ERRORCHECK = 1
  };

  class Mutex {
    private:
      pthread_mutex_t mutex;

    public:
      // Intenta crear y abrir un mutex en modo "ERRORCHECK", que brinda mayor ayuda para depurar errores que el modo "DEFAULT".
      // Pre: El mutex no está creado ni abierto.
      // Post: 1) El mutex fue creado y abierto en modo "ERRORCHECK" y se encuentra en estado "liberado".
      //       2) Se lanzó una excepción "MutexException" si el sistema no tenía los recursos necesarios para crear y abrir el mutex, el invocador no tenía los privilegios necesarios para ejecutar la función, o se intentó abrir un mutex que ya estaba abierto.
      Mutex();

      // Intenta crear y abrir un mutex en modo "DEFAULT".
      // Pre: El mutex no está creado ni abierto.
      // Post: 1) El mutex fue creado y abierto en modo "DEFAULT" y se encuentra en estado "liberado".
      //       2) Se lanzó una excepción "MutexException" si el sistema no tenía los recursos necesarios para crear y abrir el mutex, el invocador no tenía los privilegios necesarios para ejecutar la función, o se intentó abrir un mutex que ya estaba abierto.
      explicit Mutex(const MutexInicializarModo modo);

      // Intenta destruír el mutex.
      // Pre:
      // Post: El mutex fue destruído.
      virtual ~Mutex() throw ();

      // Intenta tomar el mutex. Si el mutex ya está tomado por otro hilo, el hilo que invocó la función será bloqueado hasta que el mutex sea liberado, y una vez que esto ocurra, lo intentará tomar nuevamente.
      // Pre: El mutex está abierto y no está tomado por el hilo que invoca la función.
      // Post: 1) El mutex fue tomado.
      //       2) Se lanzó una excepción "MutexException" si el mutex no estaba abierto, o si ya estaba tomado por el hilo que invocó la función (solo para mutex en modo "ERROCHECK").
      virtual void tomar();

      // Intenta liberar el mutex.
      // Pre: El mutex está tomado por el hilo que invoca la función.
      // Post: 1) El mutex fue liberado.
      //       2) Se lanzó una excepción "MutexException" si el mutex no estaba abierto, o si no estaba tomado por el hilo que invocó la función (solo para mutex en modo "ERROCHECK").
      virtual void liberar();

      // Intenta cerrar el mutex.
      // Pre: El mutex está abierto y no está tomado.
      // Post: 1) El mutex fue cerrado.
      //       2) Se lanzó una excepción "MutexException" si el mutex no estaba abierto, o si estaba tomado.
      virtual void close();
  };

//**************************************************EXCEPCIONES**************************************************//

  class MutexExcepcion: public exceptions::MiExcepcion {
    public:
      virtual ~MutexExcepcion() throw ();
      virtual const char *what() const throw ();
  };

} /* namespace threads */
#endif /* MUTEX_H_ */
