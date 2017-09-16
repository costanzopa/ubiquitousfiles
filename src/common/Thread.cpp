/*
 *      Name: ./server/Thread.cpp
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include "./Thread.h"
#include <pthread.h>

namespace threads {

  void *Thread::lanzamiento(void *pThread) {
    return ((Thread *) pThread)->procesar();
  }

  Thread::Thread()
      : sePidioStart(false), sePidioClose(false), detener(false), thread(0) {
  }

  Thread::~Thread() throw () {
    if (sePidioStart == true) {
      detener = true;
      if (sePidioClose == false) {
        // http://www.manpagez.com/man/3/pthread_join/
        // http://linux.die.net/man/3/pthread_join
        ::pthread_join(thread, NULL);
        //sePidioClose = true;
      }
    }
  }

  void Thread::start() {
    if (sePidioStart == false) {
      // http://www.manpagez.com/man/3/pthread_create/
      // http://linux.die.net/man/3/pthread_create
      if (::pthread_create(&thread, NULL, &lanzamiento, this) != 0) {
        throw ThreadExcepcion();
      }
      sePidioStart = true;
    } else {
      throw ThreadExcepcion();
    }
  }

  void Thread::pedirDetener() {
    detener = true;
  }

  void Thread::close(void ** const pPValorDevuelto) {
    if ((sePidioStart == true) && (sePidioClose == false)) {
      detener = true;
      if (::pthread_join(thread, pPValorDevuelto) != 0) {
        throw ThreadExcepcion();
      } else {
        sePidioClose = true;
      }
    } else {
      throw ThreadExcepcion();
    }
  }

  bool Thread::getDetener() {
    return detener;
  }

  void Thread::cancelar() {
    ::pthread_cancel(thread);
  }

//**************************************************EXCEPCIONES**************************************************//

#define TIPO_DE_EXCEPCION ThreadExcepcion
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return "ThreadExcepcion";
  }
#undef TIPO_DE_EXCEPCION

} /* namespace threads */
