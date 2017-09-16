#ifndef THREAD_H_
#define THREAD_H_

/*
 *      Name: ./server/Thread.h
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include <pthread.h>
#include "./../common/MiExcepcion.h"

namespace threads {

  class Thread {
    private:
      // Necesario para hacer RAII, y para evitar intentar cerrar el "Thread" sin haber iniciado su ejecución.
      bool sePidioStart;

      // Necesario para hacer RAII, y para evitar intentar cerrar varias veces el "Thread".
      bool sePidioClose;

      bool detener;
      pthread_t thread;

      // Constructor por copia declarado privado pero no definido (no se deben copiar las instancias de "Thread").
      Thread(const Thread &thread);

      // Operador asignación de copia declarado privado pero no definido (no se deben copiar las instancias de "Thread").
      Thread& operator=(const Thread &thread);

      static void *lanzamiento(void *pThread);

      virtual void *procesar() = 0;

    public:
      // Intenta crear y abrir el thread.
      // Pre: El thread no está creado ni abierto.
      // Post: 1) El thread fue creado y abierto y está ejecutando en estado "no detener" la función "procesar()" asociada a él.
      //       2) Se lanzó una excepción "ThreadException" si el sistema no tenía los recursos necesarios para crear y abrir el thread, o el invocador no tenía los privilegios necesarios para ejecutar la función.
      Thread();

      // Intenta destruír el thread.
      // Pre:
      // Post: El thread fue destruído.
      virtual ~Thread() throw ();

      void start();

      // Intenta cambiar el estado del thread a "detener".
      // Pre: El thread está abierto.
      // Post: 1) El estado del thread fue cambiado a "detener".
      //       2) Se lanzó una excepción "ThreadException" si no se pudo cambiar el estado del thread.
      //       3) Se lanzó una excepción "ThreadException" si se pudo cambiar el estado del thread, pero la integridad del mismo fue comprometida.
      virtual void pedirDetener();

      // Intenta cerrar el thread y almacenar el valor devuelto por su función "procesar()" asociada, en la dirección apuntada por *pPValorDevuelto. Si el mismo aún no terminó de ejecutar su función "procesar()" asociada, espera hasta que lo haga, y entonces lo cierra.
      // Pre: El thread está abierto.
      // Post: 1) El thread fue cerrado y (si no se pasó NULL como argumento) se almacenó el valor devuelto por su función "procesar()" asociada, en la dirección apuntada por *pPValorDevuelto.
      //       2) Se lanzó una excepción "ThreadException" si no se pudo cerrar el thread porque este quedó trabado en una espera infinita (por ejemplo estaba tratando de cerrar el thread que invocó "close()" sobre él), otro thread ya estaba esperando para cerrarlo, o ya estaba cerrado.
      virtual void close(void ** const pPValorDevuelto);

      // Intenta devolver el estado del thread.
      // Pre: El thread está abierto.
      // Post: Devolvió el estado del thread.
      virtual bool getDetener();

      virtual void cancelar();
  };

//**************************************************EXCEPCIONES**************************************************//

  class ThreadExcepcion: public exceptions::MiExcepcion {
    public:
      virtual ~ThreadExcepcion() throw ();
      virtual const char *what() const throw ();
  };

} /* namespace threads */
#endif /* THREAD_H_ */
