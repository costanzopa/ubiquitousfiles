/*
 *      Name: ./common/Socket.cpp
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include "./Socket.h"
#include <cerrno>  /* Variable "errno". */
#include <cstddef>  /* Tipo "size_t". */
#include <cstdio>  /* Funcion "fprintf". */
#include <cstring>  /* Funciones "memset", y "strerror". */
#include <netdb.h>  /* Struct "addrinfo" y funciones "freeaddrinfo", "gai_strerror", y "getaddrinfo". */
#include <string>
#include <sys/socket.h>  /* Tipo "socklen_t", structs "sockaddr", y "sockaddr_storage", y funciones "accept", "bind", "listen", "recv", "send", "shutdown", y "socket". */
#include <unistd.h>  /* Funcion "close". */
#include "./MiExcepcion.h"

namespace sockets {

  void Socket::bindear() {
    int yes = 1;
    if (::setsockopt(socketHandler, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt");
    }
    // http://www.manpagez.com/man/2/bind/
    // http://linux.die.net/man/2/bind
    // http://linux.die.net/man/3/bind
    // bind() devuelve '0' si termina correctamente, o '-1' si ocurre un error (y modifica "errno").
    const int bindError = ::bind(socketHandler, socketInfoUsado->ai_addr, socketInfoUsado->ai_addrlen);
    const int bindErrorErrno = errno;
    if (bindError != 0) {
      //::fprintf(stderr, "en Socket::bindear(), bind() error: %s.\n", ::strerror(bindErrorErrno));
      throw SocketExcepcionBindear(bindErrorErrno);
    }
  }

  void Socket::conectar() {
    // http://www.manpagez.com/man/2/connect/
    // http://linux.die.net/man/2/connect
    // http://linux.die.net/man/3/connect
    // connect() devuelve '0' si termina correctamente, o '-1' si ocurre un error (y modifica "errno").
    const int connectError = ::connect(socketHandler, socketInfoUsado->ai_addr, socketInfoUsado->ai_addrlen);
    const int connectErrorErrno = errno;
    if (connectError != 0) {
      //::fprintf(stderr, "en Socket::conectar(), connect() error: %s.\n", ::strerror(connectErrorErrno));
      throw SocketExcepcionConectar(connectErrorErrno);
    }
  }

  void Socket::crearFileDescriptor() {
    // http://www.manpagez.com/man/2/socket/
    // http://linux.die.net/man/2/socket
    // http://linux.die.net/man/3/socket
    // http://linux.die.net/man/7/socket
    // socket() devuelve un nuevo FD para el socket (un entero no negativo) si termina correctamente, o '-1' si ocurre un error (y modifica "errno").
    socketHandler = ::socket(socketInfoUsado->ai_family, socketInfoUsado->ai_socktype, socketInfoUsado->ai_protocol);
    const int socketErrorErrno = errno;
    if (socketHandler < 0) {
      //::fprintf(stderr, "en Socket::crearFileDescriptor(), socket() error: %s.\n", ::strerror(socketErrorErrno));
      socketHandler = -1;
      throw SocketExcepcionCrearFileDescriptor(socketErrorErrno);
    }
  }

  void Socket::crearFileDescriptorBindearYPonerAEscuchar(const unsigned int colaLargo) {
    try {
      crearFileDescriptor();
    } catch (const SocketExcepcionCrearFileDescriptor &excepcion) {
      // El socket no pudo ser creado con la "addrinfo" actual.
      throw;
    }
    try {
      bindear();
    } catch (const SocketExcepcionBindear &excepcion) {
      // http://www.manpagez.com/man/2/close/
      // http://linux.die.net/man/2/close
      // http://linux.die.net/man/3/close
      // close() devuelve '0' si termina correctamente, o '-1' si ocurre un error (y modifica "errno").
      const int closeError = ::close(socketHandler);
      const int closeErrorErrno = errno;
      if (closeError != 0) {
        // O lo saco por pantalla o lo meto dentro de una nueva excepción y la lanzo, o lo pierdo.
        ::fprintf(stderr, "en Socket::crearFileDescriptorBindearYPonerAEscuchar(), close() error: %s.\n", ::strerror(closeErrorErrno));
      }
      socketHandler = -1;
      // El socket creado con la "addrinfo" actual no pudo ser bindeado.
      throw;
    }
    try {
      ponerAEscuchar(colaLargo);
    } catch (const SocketExcepcionPonerAEscuchar &excepcion) {
      const int closeError = ::close(socketHandler);
      const int closeErrorErrno = errno;
      if (closeError != 0) {
        // O lo saco por pantalla o lo meto dentro de una nueva excepción y la lanzo, o lo pierdo.
        ::fprintf(stderr, "en Socket::crearFileDescriptorBindearYPonerAEscuchar(), close() error: %s.\n", ::strerror(closeErrorErrno));
      }
      socketHandler = -1;
      throw;
      // El socket creado y bindeado con la "addrinfo" actual no pudo ser puesto a escuchar".
    }
  }

  void Socket::crearFileDescriptorYConectar() {
    try {
      crearFileDescriptor();
    } catch (const SocketExcepcionCrearFileDescriptor &excepcion) {
      // El socket no pudo ser creado con la "addrinfo" actual.
      throw;
    }
    try {
      conectar();
    } catch (const SocketExcepcionConectar &excepcion) {
      const int closeError = ::close(socketHandler);
      const int closeErrorErrno = errno;
      if (closeError != 0) {
        // O lo saco por pantalla o lo meto dentro de una nueva excepción y la lanzo, o lo pierdo.
        ::fprintf(stderr, "en Socket::crearFileDescriptorYConectar(), close() error: %s.\n", ::strerror(closeErrorErrno));
      }
      socketHandler = -1;
      // El socket creado con la "addrinfo" actual no pudo ser conectado.
      throw;
    }
  }

  void Socket::ponerAEscuchar(const unsigned int colaLargo) {
    // http://www.manpagez.com/man/2/listen/
    // http://linux.die.net/man/2/listen
    // http://linux.die.net/man/3/listen
    // listen() devuelve '0' si termina correctamente, o '-1' si ocurre un error (y modifica "errno").
    const int listenError = ::listen(socketHandler, colaLargo);
    const int listenErrorErrno = errno;
    if (listenError != 0) {
      //::fprintf(stderr, "en Socket::ponerAEscuchar(), listen() error: %s.\n", ::strerror(listenErrorErrno));
      throw SocketExcepcionPonerAEscuchar(listenErrorErrno);
    }
  }

  void Socket::setearTCP_IPvX_FLAGS(struct addrinfo * const datos) throw () {
    // Se deja que el sistema determine el protocolo correcto de acuerdo al tipo de socket elegido, será TCP.
    ::memset(datos, 0, sizeof *datos);
    // Mejor que lo estándar, agrega mapeos de IPv4 a IPv6 y solo muestra direcciones en el/los formato/s que la máquina esté usando.
    datos->ai_flags = (AI_V4MAPPED | AI_ADDRCONFIG);
    // Familia de direcciones IPv4 o IPv6.
    datos->ai_family = AF_UNSPEC;
    // Tipo de socket STREAM.
    datos->ai_socktype = SOCK_STREAM;
  }

  Socket::Socket() throw ()
      : interrumpido(false), socketInfo(NULL), socketInfoUsado(NULL), socketHandler(-1) {
  }

  Socket::Socket(const std::string &puertoNro, const unsigned int colaLargo)
      : interrumpido(false), socketInfo(NULL), socketInfoUsado(NULL), socketHandler(-1) {
    // Aca voy a cargar datos manualmente.
    struct addrinfo datos;
    setearTCP_IPvX_FLAGS(&datos);
    // Este socket debe conectarse con cualquier dirección desde la que se haga un connect. Activar el flag AI_PASSIVE y pasar NULL como primer argumento a "getaddrinfo()" hace que se cargue INADDR_ANY como dirección entrante al socket.
    datos.ai_flags |= AI_PASSIVE;
    // http://www.manpagez.com/info/guile/guile-2.0.2/guile_448.php
    // http://www.manpagez.com/man/3/getaddrinfo/
    // http://linux.die.net/man/3/getaddrinfo
    // getaddrinfo() devuelve '0' si termina correctamente, u otro valor si ocurre un error.
    // getaddrinfo() asigna memoria si termina correctamente. Se libera con freeaddrinfo().
    // 'NULL' porque ya activé "AI_PASSIVE".
    const int getAddrInfoError = ::getaddrinfo(NULL, puertoNro.c_str(), &datos, &socketInfo);
    if (getAddrInfoError != 0) {
      throw SocketExcepcionConstructorGetAddrInfo(getAddrInfoError);
    }
    // Se hace con un ciclo para aprovechar toda la informacion en caso de que "getaddrinfo()" devuelva mas de un "struct addrinfo".
    for (socketInfoUsado = socketInfo; socketInfoUsado != NULL; socketInfoUsado = socketInfoUsado->ai_next) {
      try {
        crearFileDescriptorBindearYPonerAEscuchar(colaLargo);
      } catch (const SocketExcepcionCrearFileDescriptor &excepcion) {
        // Loguear la "addrinfo" actual (que no funcionó).
        // El socket no pudo ser creado, bindeado y puesto a escuchar, con la "addrinfo" actual, pruebo con la siguiente "addrinfo".
        continue;
      } catch (const SocketExcepcionBindear &excepcion) {
        // Loguear la "addrinfo" actual (que no funcionó).
        // El socket no pudo ser creado, bindeado y puesto a escuchar, con la "addrinfo" actual, pruebo con la siguiente "addrinfo".
        continue;
      } catch (const SocketExcepcionPonerAEscuchar &excepcion) {
        // Loguear la "addrinfo" actual (que no funcionó).
        // El socket no pudo ser creado, bindeado y puesto a escuchar, con la "addrinfo" actual, pruebo con la siguiente "addrinfo".
        continue;
      }
      // Funcionó para alguno, me quedo con él y dejo de probar.
      break;
    }
    // Me fijo si se pudo crear, bindear y poner a escuchar el socket, o si salió del ciclo porque se acabaron las posibilidades.
    if (socketInfoUsado == NULL) {
      // http://www.manpagez.com/man/3/freeaddrinfo/
      // http://linux.die.net/man/3/freeaddrinfo
      ::freeaddrinfo(socketInfo);
      socketInfo = NULL;
      throw SocketExcepcionConstructorNoPudoCrearFileDescriptorBindearYPonerAEscuchar();
    }
  }

  Socket::Socket(const std::string &hostID, const std::string &puertoNro)
      : interrumpido(false), socketInfo(NULL), socketInfoUsado(NULL), socketHandler(-1) {
    // Aca voy a cargar datos manualmente.
    struct addrinfo datos;
    setearTCP_IPvX_FLAGS(&datos);
    // Este socket debe conectarse con la dirección "hostID", que puede estar expresada como una dirección IPv4 numérica (con dígitos decimales y puntos), una dirección IPv6 numérica (con dígitos hexadecimales y dos puntos (':')), o como un nombre de dominio, cuya dirección es resuelta por la función.
    const int getAddrInfoError = ::getaddrinfo(hostID.c_str(), puertoNro.c_str(), &datos, &socketInfo);
    if (getAddrInfoError != 0) {
      throw SocketExcepcionConstructorGetAddrInfo(getAddrInfoError);
    }
    // Se hace con un ciclo para aprovechar toda la informacion en caso de que "getaddrinfo()" devuelva mas de un "struct addrinfo".
    for (socketInfoUsado = socketInfo; socketInfoUsado != NULL; socketInfoUsado = socketInfoUsado->ai_next) {
      try {
        crearFileDescriptorYConectar();
      } catch (const SocketExcepcionCrearFileDescriptor &excepcion) {
        // Loguear la "addrinfo" actual (que no funcionó).
        // El socket no pudo ser creado y conectado, con la "addrinfo" actual, pruebo con la siguiente "addrinfo".
        continue;
      } catch (const SocketExcepcionConectar &excepcion) {
        // Loguear la "addrinfo" actual (que no funcionó).
        // El socket no pudo ser creado y conectado, con la "addrinfo" actual, pruebo con la siguiente "addrinfo".
        continue;
      }
      // Funcionó para alguno, me quedo con él y dejo de probar.
      break;
    }
    // Me fijo si se pudo crear y conectar el socket, o si salió del ciclo porque se acabaron las posibilidades.
    if (socketInfoUsado == NULL) {
      ::freeaddrinfo(socketInfo);
      socketInfo = NULL;
      throw SocketExcepcionConstructorNoPudoCrearFileDescriptorYConectar();
    }
  }

  Socket::~Socket() throw () {
    if (socketInfo != NULL) {
      ::freeaddrinfo(socketInfo);
      socketInfo = NULL;
    }
    socketInfoUsado = NULL;
    if (socketHandler != -1) {
      // Desagoto el buffer.
      // http://www.manpagez.com/man/2/shutdown/
      // http://linux.die.net/man/2/shutdown
      // http://linux.die.net/man/3/shutdown
      // shutdown() devuelve '0' si termina correctamente, o '-1' si ocurre un error (y modifica "errno").
      const int shutdownError = ::shutdown(socketHandler, ::SHUT_RDWR);
      const int shutdownErrorErrno = errno;
      if (shutdownError != 0) {
        // O lo saco por pantalla o lo meto dentro de una nueva excepción y la lanzo, o lo pierdo.
        ::fprintf(stderr, "en Socket::~Socket(), shutdown() error: %s.\n", ::strerror(shutdownErrorErrno));
      }
      const int closeError = ::close(socketHandler);
      const int closeErrorErrno = errno;
      if (closeError != 0) {
        // O lo saco por pantalla o lo meto dentro de una nueva excepción y la lanzo, o lo pierdo.
        ::fprintf(stderr, "en Socket::~Socket(), close() error: %s.\n", ::strerror(closeErrorErrno));
      }
      socketHandler = -1;
    }
  }

  void Socket::aceptar(Socket * const socketNuevo) {
    struct sockaddr_storage cliSockAddr;
    socklen_t cliSockAddrLargo = sizeof cliSockAddr;
    // http://www.manpagez.com/man/2/accept/
    // http://linux.die.net/man/2/accept
    // http://linux.die.net/man/3/accept
    // accept() devuelve un nuevo FD (un entero no negativo) si termina correctamente, o '-1' si ocurre un error (y modifica "errno").
    const int acceptError = ::accept(socketHandler, (struct sockaddr *) &cliSockAddr, &cliSockAddrLargo);
    const int acceptErrorErrno = errno;
    if (acceptError >= 0) {
      // TODO(Ivan): hacer algo con la informacion que devuelve accept() ("cliSockAddr").
      socketNuevo->socketHandler = acceptError;
    } else {
      //::fprintf(stderr, "en Socket::aceptar(), accept() error: %s.\n", ::strerror(acceptErrorErrno));
      throw SocketExcepcionAceptar(acceptErrorErrno);
    }
  }

  void Socket::interrumpir() {
    interrumpido = true;
    const int shutdownError = ::shutdown(socketHandler, SHUT_RDWR);
    const int shutdownErrorErrno = errno;
    if (shutdownError != 0) {
      throw SocketExcepcionInterrumpir(shutdownErrorErrno);
    }
  }

  void Socket::close() {
    if (socketInfo != NULL) {
      ::freeaddrinfo(socketInfo);
      socketInfo = NULL;
    }
    socketInfoUsado = NULL;
    if (socketHandler != -1) {
      if (interrumpido == false) {
        // Desagoto el buffer.
        const int shutdownError = ::shutdown(socketHandler, ::SHUT_RDWR);
        const int shutdownErrorErrno = errno;
        if (shutdownError != 0) {
          const int closeError = ::close(socketHandler);
          const int closeErrorErrno = errno;
          if (closeError != 0) {
            // O lo saco por pantalla o lo meto dentro de una nueva excepción y la lanzo, o lo pierdo.
            ::fprintf(stderr, "en Socket::close(), close() error: %s.\n", ::strerror(closeErrorErrno));
          }
          socketHandler = -1;
          throw SocketExcepcionCloseInterrumpir(shutdownErrorErrno);
        }
      }
      const int closeError = ::close(socketHandler);
      const int closeErrorErrno = errno;
      socketHandler = -1;
      if (closeError != 0) {
        throw SocketExcepcionCloseClose(closeErrorErrno);
      }
    }
  }

  int Socket::enviar(const void * const buffer, const size_t bufferLargo) const {
    // http://www.manpagez.com/man/2/send/
    // http://linux.die.net/man/2/send
    // http://linux.die.net/man/3/send
    // send() devuelve '0' si termina correctamente, o '-1' si ocurre un error (y modifica "errno").
    const int sendError = ::send(socketHandler, buffer, bufferLargo, ::MSG_NOSIGNAL);
    const int sendErrorErrno = errno;
    if (sendError != -1) {
      return sendError;
    } else {
      throw SocketExcepcionEnviar(sendErrorErrno);
    }
  }

  int Socket::recibir(void * const buffer, const size_t bufferLargo) const {
    // http://www.manpagez.com/man/2/recv/
    // http://linux.die.net/man/2/recv
    // http://linux.die.net/man/3/recv
    // recv() devuelve el numero de Bytes recibidos, '0' si el otro lado ha cerrado el socket, o '-1' si ocurre un error (y modifica "errno").
    const int recvError = ::recv(socketHandler, buffer, bufferLargo, 0);
    const int recvErrorErrno = errno;
    if (recvError >= 0) {
      return recvError;
    } else {
      throw SocketExcepcionRecibir(recvErrorErrno);
    }
  }

//**************************************************EXCEPCIONES**************************************************//

#define TIPO_DE_EXCEPCION SocketExcepcion
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return "SocketExcepcion";
  }
#undef TIPO_DE_EXCEPCION

//**************************************************

#define TIPO_DE_EXCEPCION SocketExcepcionBindear
  TIPO_DE_EXCEPCION ::TIPO_DE_EXCEPCION(const int bindearError) throw ()
      : bindearError(bindearError) {
  }
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return ::strerror(bindearError);
  }
#undef TIPO_DE_EXCEPCION

//**************************************************

#define TIPO_DE_EXCEPCION SocketExcepcionConectar
  TIPO_DE_EXCEPCION ::TIPO_DE_EXCEPCION(const int conectarError) throw ()
      : conectarError(conectarError) {
  }
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return ::strerror(conectarError);
  }
#undef TIPO_DE_EXCEPCION

//**************************************************

#define TIPO_DE_EXCEPCION SocketExcepcionCrearFileDescriptor
  TIPO_DE_EXCEPCION ::TIPO_DE_EXCEPCION(const int crearFileDescriptorError) throw ()
      : crearFileDescriptorError(crearFileDescriptorError) {
  }
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return ::strerror(crearFileDescriptorError);
  }
#undef TIPO_DE_EXCEPCION

//**************************************************

#define TIPO_DE_EXCEPCION SocketExcepcionPonerAEscuchar
  TIPO_DE_EXCEPCION ::TIPO_DE_EXCEPCION(const int ponerAEscucharError) throw ()
      : ponerAEscucharError(ponerAEscucharError) {
  }
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return ::strerror(ponerAEscucharError);
  }
#undef TIPO_DE_EXCEPCION

//**************************************************

#define TIPO_DE_EXCEPCION SocketExcepcionConstructor
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return "SocketExcepcionConstructor";
  }
#undef TIPO_DE_EXCEPCION

//**************************************************

#define TIPO_DE_EXCEPCION SocketExcepcionConstructorGetAddrInfo
  TIPO_DE_EXCEPCION ::TIPO_DE_EXCEPCION(const int getAddrInfoError) throw ()
      : getAddrInfoError(getAddrInfoError) {
  }
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    // http://www.manpagez.com/man/3/gai_strerror/
    // "gai_strerror()" traduce los códigos de error devueltos por "getaddrinfo()" a mensajes de texto que se puedan mostrar por pantalla o escribir a disco.
    return ::gai_strerror(getAddrInfoError);
  }
#undef TIPO_DE_EXCEPCION

//**************************************************

#define TIPO_DE_EXCEPCION SocketExcepcionConstructorNoPudoCrearFileDescriptorBindearYPonerAEscuchar
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return "SocketExcepcionConstructorNoPudoCrearFileDescriptorBindearYPonerAEscuchar";
  }
#undef TIPO_DE_EXCEPCION

//**************************************************

#define TIPO_DE_EXCEPCION SocketExcepcionConstructorNoPudoCrearFileDescriptorYConectar
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return "SocketExcepcionConstructorNoPudoCrearFileDescriptorYConectar";
  }
#undef TIPO_DE_EXCEPCION

//**************************************************

#define TIPO_DE_EXCEPCION SocketExcepcionAceptar
  TIPO_DE_EXCEPCION ::TIPO_DE_EXCEPCION(const int aceptarError) throw ()
      : aceptarError(aceptarError) {
  }
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return ::strerror(aceptarError);
  }
#undef TIPO_DE_EXCEPCION

//**************************************************

#define TIPO_DE_EXCEPCION SocketExcepcionInterrumpir
  TIPO_DE_EXCEPCION ::TIPO_DE_EXCEPCION(const int interrumpirError) throw ()
      : interrumpirError(interrumpirError) {
  }
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return ::strerror(interrumpirError);
  }
#undef TIPO_DE_EXCEPCION

//**************************************************

#define TIPO_DE_EXCEPCION SocketExcepcionClose
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return "SocketExcepcionClose";
  }
#undef TIPO_DE_EXCEPCION

//**************************************************

#define TIPO_DE_EXCEPCION SocketExcepcionCloseInterrumpir
  TIPO_DE_EXCEPCION ::TIPO_DE_EXCEPCION(const int interrumpirError) throw ()
      : interrumpirError(interrumpirError) {
  }
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return ::strerror(interrumpirError);
  }
#undef TIPO_DE_EXCEPCION

//**************************************************

#define TIPO_DE_EXCEPCION SocketExcepcionCloseClose
  TIPO_DE_EXCEPCION ::TIPO_DE_EXCEPCION(const int closeError) throw ()
      : closeError(closeError) {
  }
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return ::strerror(closeError);
  }
#undef TIPO_DE_EXCEPCION

//**************************************************

#define TIPO_DE_EXCEPCION SocketExcepcionEnviar
  TIPO_DE_EXCEPCION ::TIPO_DE_EXCEPCION(const int enviarError) throw ()
      : enviarError(enviarError) {
  }
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return ::strerror(enviarError);
  }
#undef TIPO_DE_EXCEPCION

//**************************************************

#define TIPO_DE_EXCEPCION SocketExcepcionRecibir
  TIPO_DE_EXCEPCION ::TIPO_DE_EXCEPCION(const int recibirError) throw ()
      : recibirError(recibirError) {
  }
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return ::strerror(recibirError);
  }
#undef TIPO_DE_EXCEPCION

}
/* namespace sockets */
