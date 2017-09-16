#ifndef SINCRONIZADOR_H_
#define SINCRONIZADOR_H_

/*
 *      Name: ./common/Sincronizador.h
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include <pthread.h>
#include "./../common/MiExcepcion.h"

namespace threads {

  enum SincronizadorInicializarModo {
    SINCRONIZADOR_DEFAULT = 0, SINCRONIZADOR_ERRORCHECK = 1
  };

  class Sincronizador {
    private:
      pthread_mutex_t mutex;
      pthread_cond_t condVar;

    public:
      // Intenta crear y abrir un sincronizador en modo "ERRORCHECK", que brinda mayor ayuda para depurar errores que el modo "DEFAULT".
      // Pre: El sincronizador no está creado ni abierto.
      // Post: 1) El sincronizador fue creado y abierto en modo "ERRORCHECK" y se encuentra en estado "liberado".
      //       2) Se lanzó una excepción "SincronizadorException" si el sistema no tenía los recursos necesarios para crear y abrir el sincronizador, el invocador no tenía los privilegios necesarios para ejecutar la función, o se intentó abrir un sincronizador que ya estaba abierto.
      Sincronizador();

      // Intenta crear y abrir un sincronizador en modo "DEFAULT".
      // Pre: El sincronizador no está creado ni abierto.
      // Post: 1) El sincronizador fue creado y abierto en modo "ERRORCHECK" y se encuentra en estado "liberado".
      //       2) Se lanzó una excepción "SincronizadorException" si el sistema no tenía los recursos necesarios para crear y abrir el sincronizador, el invocador no tenía los privilegios necesarios para ejecutar la función, o se intentó abrir un sincronizador que ya estaba abierto.
      explicit Sincronizador(const SincronizadorInicializarModo modo);

      // Intenta destruír el sincronizador.
      // Pre:
      // Post: El sincronizador fue destruído.
      virtual ~Sincronizador() throw ();

      // Intenta tomar el sincronizador. Si el sincronizador ya está tomado por otro hilo, el hilo que invocó la función será bloqueado hasta que el sincronizador sea liberado, y una vez que esto ocurra, lo intentará tomar nuevamente.
      // Pre: El sincronizador está abierto y no está tomado por el hilo que invoca la función.
      // Post: 1) El sincronizador fue tomado.
      //       2) Se lanzó una excepción "SincronizadorException" si el sincronizador no estaba abierto, o si ya estaba tomado por el hilo que invocó la función (solo para sincronizador en modo "ERROCHECK").
      virtual void tomar();

      // Intenta liberar el sincronizador.
      // Pre: El sincronizador está tomado por el hilo que invoca la función.
      // Post: 1) El sincronizador fue liberado.
      //       2) Se lanzó una excepción "SincronizadorException" si el sincronizador no estaba abierto, o si no estaba tomado por el hilo que invocó la función (solo para sincronizador en modo "ERROCHECK").
      virtual void liberar();

      // Intenta liberar el sincronizador y bloquea el hilo que invocó la función hasta que el mismo reciba, a través del sincronizador, una señalización. Una vez recibida la señalización, continúa bloqueando hasta tomar nuevamente el sincronizador, momento en el que desbloquea el hilo que invocó la función.
      // Pre: El sincronizador está tomado por el hilo que invoca la función.
      // Post: 1) El sincronizador fue liberado y el hilo que invocó la función está bloqueado esperando recibir una "señalización".
      //       2) Se lanzó una excepción "SincronizadorException" si el sincronizador no estaba abierto, o si no estaba tomado por el hilo que invocó la función.
      virtual void esperar();

      // Intenta enviar una señalización a al menos un hilo bloqueado esperando recibir una "señalización" a través del sincronizador.
      // Pre: El sincronizador está abierto.
      // Post: 1) El sincronizador fue liberado y el hilo que invocó la función está bloqueado esperando recibir una "señalización".
      //       2) Se lanzó una excepción "SincronizadorException" si el sincronizador no estaba abierto.
      virtual void senyalar();

      // Intenta enviar una señalización a todos los hilos bloqueados esperando recibir una "señalización" a través del sincronizador.
      // Pre: El sincronizador está abierto.
      // Post: 1) El sincronizador fue liberado y el hilo que invocó la función está bloqueado esperando recibir una "señalización".
      //       2) Se lanzó una excepción "SincronizadorException" si el sincronizador no estaba abierto.
      virtual void senyalarATodos();

      // Intenta cerrar el sincronizador.
      // Pre: El sincronizador está abierto y no está tomado.
      // Post: 1) El sincronizador fue cerrado.
      //       2) Se lanzó una excepción "SincronizadorException" si el sincronizador no estaba abierto, estaba tomado, o si algún hilo estaba esperando recibir una "señalización" a través de él.
      virtual void close();
  };

//**************************************************EXCEPCIONES**************************************************//

  class SincronizadorExcepcion: public exceptions::MiExcepcion {
    public:
      virtual ~SincronizadorExcepcion() throw ();
      virtual const char *what() const throw ();
  };

} /* namespace threads */
#endif /* SINCRONIZADOR_H_ */
