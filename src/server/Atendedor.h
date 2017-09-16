/*
 *      Name: ./server/Atendedor.h
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#ifndef ATENDEDOR_H_
#define ATENDEDOR_H_

#include <queue>
#include "./../common/AdmArchivos.h"
#include "./../common/MiExcepcion.h"
#include "./../common/MiSocket.h"
#include "./../common/Mutex.h"
#include "./../common/Pedido.h"
#include "./../common/Sincronizador.h"
#include "./../common/Thread.h"
#include "./AdmUsuarios.h"

#include <iostream>

namespace ubicuos {

  using threads::Mutex;
  using threads::Sincronizador;

  class AdmArchivos;

  class Atendedor: public threads::Thread {
    private:

      class RepetidorEnvios: public threads::Thread {
        private:
          const MiSocket &miSocket;
          AdmArchivos &admArchivos;
          std::string usuarioNombre;
          bool ahoraPuedeEnviar;
          Sincronizador sePuedeEnviar;
          Sincronizador hayElementosParaEnviar;
          std::queue< std::queue< Pedido > > colaDeColaDePedidos;

          // Constructor por copia declarado privado pero no definido (no se deben copiar las instancias de "RepetidorEnvios").
          RepetidorEnvios(const RepetidorEnvios &RepetidorEnvios);

          // Operador asignación de copia declarado privado pero no definido (no se deben copiar las instancias de "RepetidorEnvios").
          RepetidorEnvios& operator=(const RepetidorEnvios &RepetidorEnvios);

          static void sincronizacionOK() throw ();

          virtual void *procesar();

        public:
          RepetidorEnvios(const MiSocket &miSocket, AdmArchivos &admArchivos);

          virtual ~RepetidorEnvios() throw ();

          virtual void pedirDetener();

          virtual void close(void ** const pPValorDevuelto);

          virtual void setUsuarioNombre(const std::string &usuarioNombre);

          virtual void tomar();

          virtual void liberar();

          virtual void senyalar();

          virtual void agregarPedidos(const std::queue< Pedido > &colaPedidos);
      };

      bool seInterrumpioSocket;
      Mutex socketMutex;
      MiSocket * const pMiSocket;
      std::string usuarioNombre;
      AdmArchivos &admArchivos;
      AdmUsuarios &admUsuarios;
      std::queue< Atendedor * > &atendedoresARecolectar;
      RepetidorEnvios repetidorEnvios;

      // Constructor por copia declarado privado pero no definido (no se deben copiar las instancias de "Atendedor").
      Atendedor(const Atendedor &atendedor);

      // Operador asignación de copia declarado privado pero no definido (no se deben copiar las instancias de "Atendedor").
      Atendedor& operator=(const Atendedor &atendedor);

      virtual bool registrarCliente();

      virtual void *procesar();

    public:
      Atendedor(MiSocket * const pMiSocket, AdmArchivos &admArchivos, AdmUsuarios &admUsuarios, std::queue< Atendedor * > &atendedoresARecolectar);

      virtual ~Atendedor() throw ();

      virtual void pedirDetener();

      virtual void close(void ** const pPValorDevuelto);

      virtual void tomar();

      virtual void liberar();

      virtual void senyalar();

      virtual void agregarPedidos(const std::queue< Pedido > &colaPedidos);
  };

//**************************************************EXCEPCIONES**************************************************//

  class AtendedorExcepcion: public exceptions::MiExcepcion {
    public:
      virtual ~AtendedorExcepcion() throw ();
      virtual const char *what() const throw ();
  };

} /* namespace ubicuos */
#endif /* ATENDEDOR_H_ */
