/*
 *      Name: ./common/Sincronizador.cpp
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include "./Sincronizador.h"
#include <pthread.h>

namespace threads {

  Sincronizador::Sincronizador() {
    // Hay que repetir este código porque, para evitar exponer partes de la implementación y hacerla independiente de la biblioteca utilizada, no se puede hacer un "get" del atributo "pthread_mutex_t" del mutex asociado, para pasárselo como argumento a "pthread_cond_wait()". Así que tenemos que tener nuestro propio atributo "pthread_mutex_t" aquí y manejarlo independientemente de la clase "Mutex".
    pthread_mutexattr_t mutexAtrib;
    if (::pthread_mutexattr_init(&mutexAtrib) != 0) {
      throw SincronizadorExcepcion();
    }
    if (::pthread_mutexattr_settype(&mutexAtrib, PTHREAD_MUTEX_ERRORCHECK) != 0) {
      ::pthread_mutexattr_destroy(&mutexAtrib);
      throw SincronizadorExcepcion();
    }
    if (::pthread_mutex_init(&mutex, &mutexAtrib) != 0) {
      ::pthread_mutexattr_destroy(&mutexAtrib);
      throw SincronizadorExcepcion();
    }
    if (::pthread_mutexattr_destroy(&mutexAtrib) != 0) {
      ::pthread_mutex_destroy(&mutex);
      throw SincronizadorExcepcion();
    }
    // Acá es donde cambia respecto a Mutex.
    // http://www.manpagez.com/man/3/pthread_cond_init/
    // http://linux.die.net/man/3/pthread_cond_init
    if (::pthread_cond_init(&condVar, NULL) != 0) {
      ::pthread_mutex_destroy(&mutex);
      throw SincronizadorExcepcion();
    }
  }

  Sincronizador::Sincronizador(const SincronizadorInicializarModo modo) {
    switch (modo) {
      case SINCRONIZADOR_DEFAULT:
        if (::pthread_mutex_init(&mutex, NULL) != 0) {
          throw SincronizadorExcepcion();
        }
        break;
      case SINCRONIZADOR_ERRORCHECK:
        Sincronizador();
        break;
      default:
        throw SincronizadorExcepcion();
        break;
    }
    if (::pthread_cond_init(&condVar, NULL) != 0) {
      ::pthread_mutex_destroy(&mutex);
      throw SincronizadorExcepcion();
    }
  }

  Sincronizador::~Sincronizador() throw () {
    ::pthread_mutex_unlock(&mutex);
    ::pthread_mutex_destroy(&mutex);
    ::pthread_cond_destroy(&condVar);
  }

  void Sincronizador::tomar() {
    // http://www.manpagez.com/man/3/pthread_mutex_lock/
    // http://linux.die.net/man/3/pthread_mutex_lock
    if (::pthread_mutex_lock(&mutex) != 0) {
      throw SincronizadorExcepcion();
    }
  }

  void Sincronizador::liberar() {
    // http://www.manpagez.com/man/3/pthread_mutex_unlock/
    // http://linux.die.net/man/3/pthread_mutex_unlock
    if (::pthread_mutex_unlock(&mutex) != 0) {
      throw SincronizadorExcepcion();
    }
  }

  void Sincronizador::esperar() {
    // http://www.manpagez.com/man/3/pthread_cond_wait/
    // http://linux.die.net/man/3/pthread_cond_wait
    if (::pthread_cond_wait(&condVar, &mutex) != 0) {
      throw SincronizadorExcepcion();
    }
  }

  void Sincronizador::senyalar() {
    // http://www.manpagez.com/man/3/pthread_cond_signal/
    // http://linux.die.net/man/3/pthread_cond_signal
    if (::pthread_cond_signal(&condVar) != 0) {
      throw SincronizadorExcepcion();
    }
  }

  void Sincronizador::senyalarATodos() {
    // http://www.manpagez.com/man/3/pthread_cond_broadcast/
    // http://linux.die.net/man/3/pthread_cond_broadcast
    if (::pthread_cond_broadcast(&condVar) != 0) {
      throw SincronizadorExcepcion();
    }
  }

  void Sincronizador::close() {
    if (::pthread_mutex_destroy(&mutex) != 0) {
      // http://www.manpagez.com/man/3/pthread_cond_destroy/
      // http://linux.die.net/man/3/pthread_cond_destroy
      ::pthread_cond_destroy(&condVar);
      throw SincronizadorExcepcion();
    }
    if (::pthread_cond_destroy(&condVar) != 0) {
      throw SincronizadorExcepcion();
    }
  }

//**************************************************EXCEPCIONES**************************************************//

#define TIPO_DE_EXCEPCION SincronizadorExcepcion
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return "SincronizadorExcepcion";
  }
#undef TIPO_DE_EXCEPCION

} /* namespace threads */
