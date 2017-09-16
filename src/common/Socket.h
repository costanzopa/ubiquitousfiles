#ifndef SOCKET_H_
#define SOCKET_H_

/*
 *      Name: ./common/Socket.h
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include <cstddef>  /* Tipo "size_t". */
#include <netdb.h>  /* Struct "addrinfo". */
#include <string>
#include "./MiExcepcion.h"

namespace sockets {

  class Socket {
    public:
      bool interrumpido;
      struct addrinfo *socketInfo;
      struct addrinfo *socketInfoUsado;
      int socketHandler;

      // Constructor por copia declarado privado pero no definido (no se deben copiar las instancias de "Socket").
      Socket(const Socket &socket);

      // Operador asignación de copia declarado privado pero no definido (no se deben copiar las instancias de "Socket").
      Socket& operator=(const Socket &socket);

      //
      // Pre:
      // Post:
      virtual void bindear();

      //
      // Pre:
      // Post:
      virtual void conectar();

      //
      // Pre:
      // Post:
      virtual void crearFileDescriptor();

      //
      // Pre:
      // Post:
      virtual void crearFileDescriptorBindearYPonerAEscuchar(const unsigned int colaLargo);

      //
      // Pre:
      // Post:
      virtual void crearFileDescriptorYConectar();

      //
      // Pre:
      // Post:
      virtual void ponerAEscuchar(const unsigned int colaLargo);

      // Carga la información de pre-configuración del socket (flags, familia de direcciones, tipo de socket, y protocolo). Es virtual para que se pueda elegir cuáles clases derivadas la redefinen y cuáles no.
      // Pre: Nada.
      // Post: Cargó la información de pre-configuración del socket.
      virtual void setearTCP_IPvX_FLAGS(struct addrinfo * const datos) throw ();

    public:
      //
      //
      //
      explicit Socket() throw ();

      // Intenta crear y abrir un socket en modo "server".
      // Pre: El socket no está creado ni abierto.
      // Post: 1) El socket fue creado y abierto en modo "server".
      //       2) Se lanzó una excepción "SocketException" si no se pudo crear y abrir el socket en modo "server".
      explicit Socket(const std::string &puertoNro, const unsigned int colaLargo);

      // Intenta crear y abrir un socket.
      // Pre: El socket no está creado ni abierto.
      // Post: 1) El socket fue creado y abierto.
      //       2) Se lanzó una excepción "SocketException" si no se pudo crear y abrir el socket.
      explicit Socket(const std::string &hostID, const std::string &puertoNro);

      //
      // Pre:
      // Post:
      virtual ~Socket() throw ();

      //
      // Pre:
      // Post:
      virtual void aceptar(Socket * const socketNuevo);

      // Intenta interrumpir el envío y la recepción de datos a través del socket.
      // Pre:
      // Post:
      virtual void interrumpir();

      // Intenta cerrar el socket.
      // Pre:
      // Post:
      virtual void close();

      //
      // Pre:
      // Post:
      virtual int enviar(const void * const buffer, const size_t bufferLargo) const;

      //
      // Pre:
      // Post:
      virtual int recibir(void * const buffer, const size_t bufferLargo) const;

  };

//**************************************************EXCEPCIONES**************************************************//

  class SocketExcepcion: public exceptions::MiExcepcion {
    public:
      virtual ~SocketExcepcion() throw ();
      virtual const char *what() const throw ();
  };

  class SocketExcepcionBindear: public SocketExcepcion {
    private:
      const int bindearError;
    public:
      SocketExcepcionBindear(const int bindearError) throw ();
      virtual ~SocketExcepcionBindear() throw ();
      virtual const char *what() const throw ();
  };

  class SocketExcepcionConectar: public SocketExcepcion {
      const int conectarError;
    public:
      SocketExcepcionConectar(const int conectarError) throw ();
      virtual ~SocketExcepcionConectar() throw ();
      virtual const char *what() const throw ();
  };

  class SocketExcepcionCrearFileDescriptor: public SocketExcepcion {
      const int crearFileDescriptorError;
    public:
      SocketExcepcionCrearFileDescriptor(const int crearFileDescriptorError) throw ();
      virtual ~SocketExcepcionCrearFileDescriptor() throw ();
      virtual const char *what() const throw ();
  };

  class SocketExcepcionPonerAEscuchar: public SocketExcepcion {
      const int ponerAEscucharError;
    public:
      SocketExcepcionPonerAEscuchar(const int ponerAEscucharError) throw ();
      virtual ~SocketExcepcionPonerAEscuchar() throw ();
      virtual const char *what() const throw ();
  };

  class SocketExcepcionConstructor: public SocketExcepcion {
    public:
      virtual ~SocketExcepcionConstructor() throw ();
      virtual const char *what() const throw ();
  };

  class SocketExcepcionConstructorGetAddrInfo: public SocketExcepcionConstructor {
    private:
      const int getAddrInfoError;
    public:
      SocketExcepcionConstructorGetAddrInfo(const int getAddrInfoError) throw ();
      virtual ~SocketExcepcionConstructorGetAddrInfo() throw ();
      virtual const char *what() const throw ();
  };

  class SocketExcepcionConstructorNoPudoCrearFileDescriptorBindearYPonerAEscuchar: public SocketExcepcionConstructor {
    public:
      virtual ~SocketExcepcionConstructorNoPudoCrearFileDescriptorBindearYPonerAEscuchar() throw ();
      virtual const char *what() const throw ();
  };

  class SocketExcepcionConstructorNoPudoCrearFileDescriptorYConectar: public SocketExcepcionConstructor {
    public:
      virtual ~SocketExcepcionConstructorNoPudoCrearFileDescriptorYConectar() throw ();
      virtual const char *what() const throw ();
  };

  class SocketExcepcionAceptar: public SocketExcepcion {
    private:
      const int aceptarError;
    public:
      SocketExcepcionAceptar(const int aceptarError) throw ();
      virtual ~SocketExcepcionAceptar() throw ();
      virtual const char *what() const throw ();
  };

  class SocketExcepcionInterrumpir: public SocketExcepcion {
    private:
      const int interrumpirError;
    public:
      SocketExcepcionInterrumpir(const int interrumpirError) throw ();
      virtual ~SocketExcepcionInterrumpir() throw ();
      virtual const char *what() const throw ();
  };

  class SocketExcepcionClose: public SocketExcepcion {
    public:
      virtual ~SocketExcepcionClose() throw ();
      virtual const char *what() const throw ();
  };

  class SocketExcepcionCloseInterrumpir: public SocketExcepcionClose {
    private:
      const int interrumpirError;
    public:
      SocketExcepcionCloseInterrumpir(const int interrumpirError) throw ();
      virtual ~SocketExcepcionCloseInterrumpir() throw ();
      virtual const char *what() const throw ();
  };

  class SocketExcepcionCloseClose: public SocketExcepcionClose {
    private:
      const int closeError;
    public:
      SocketExcepcionCloseClose(const int closeError) throw ();
      virtual ~SocketExcepcionCloseClose() throw ();
      virtual const char *what() const throw ();
  };

  class SocketExcepcionEnviar: public SocketExcepcion {
    private:
      const int enviarError;
    public:
      SocketExcepcionEnviar(const int enviarError) throw ();
      virtual ~SocketExcepcionEnviar() throw ();
      virtual const char *what() const throw ();
  };

  class SocketExcepcionRecibir: public SocketExcepcion {
    private:
      const int recibirError;
    public:
      SocketExcepcionRecibir(const int recibirError) throw ();
      virtual ~SocketExcepcionRecibir() throw ();
      virtual const char *what() const throw ();
  };

} /* namespace sockets */
#endif /* SOCKET_H_ */
