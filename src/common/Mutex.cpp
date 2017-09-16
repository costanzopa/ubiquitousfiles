/*
 *      Name: ./server/Mutex.cpp
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include "./Mutex.h"
#include <pthread.h>

namespace threads {

  Mutex::Mutex() {
    pthread_mutexattr_t mutexAtrib;
    // http://linux.die.net/man/3/pthread_mutexattr_init
    if (::pthread_mutexattr_init(&mutexAtrib) != 0) {
      throw MutexExcepcion();
    }
    // http://linux.die.net/man/3/pthread_mutexattr_settype
    if (::pthread_mutexattr_settype(&mutexAtrib, PTHREAD_MUTEX_ERRORCHECK) != 0) {
      // http://linux.die.net/man/3/pthread_mutexattr_destroy
      ::pthread_mutexattr_destroy(&mutexAtrib);
      throw MutexExcepcion();
    }
    // http://www.manpagez.com/man/3/pthread_mutex_init/
    // http://linux.die.net/man/3/pthread_mutex_init
    if (::pthread_mutex_init(&mutex, &mutexAtrib) != 0) {
      ::pthread_mutexattr_destroy(&mutexAtrib);
      throw MutexExcepcion();
    }
    if (::pthread_mutexattr_destroy(&mutexAtrib) != 0) {
      // http://www.manpagez.com/man/3/pthread_mutex_destroy/
      // http://linux.die.net/man/3/pthread_mutex_destroy
      ::pthread_mutex_destroy(&mutex);
      throw MutexExcepcion();
    }
  }

  Mutex::Mutex(const MutexInicializarModo modo) {
    switch (modo) {
      case MUTEX_DEFAULT:
        if (::pthread_mutex_init(&mutex, NULL) != 0) {
          throw MutexExcepcion();
        }
        break;
      case MUTEX_ERRORCHECK:
        Mutex();
        break;
      default:
        throw MutexExcepcion();
        break;
    }
  }

  Mutex::~Mutex() throw () {
    ::pthread_mutex_unlock(&mutex);
    ::pthread_mutex_destroy(&mutex);
  }

  void Mutex::tomar() {
    // http://www.manpagez.com/man/3/pthread_mutex_lock/
    // http://linux.die.net/man/3/pthread_mutex_lock
    if (::pthread_mutex_lock(&mutex) != 0) {
      throw MutexExcepcion();
    }
  }

  void Mutex::liberar() {
    // http://www.manpagez.com/man/3/pthread_mutex_unlock/
    // http://linux.die.net/man/3/pthread_mutex_unlock
    if (::pthread_mutex_unlock(&mutex) != 0) {
      throw MutexExcepcion();
    }
  }

  void Mutex::close() {
    if (::pthread_mutex_destroy(&mutex) != 0) {
      throw MutexExcepcion();
    }
  }

//**************************************************EXCEPCIONES**************************************************//

#define TIPO_DE_EXCEPCION MutexExcepcion
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return "MutexExcepcion";
  }
#undef TIPO_DE_EXCEPCION

}
/* namespace threads */
