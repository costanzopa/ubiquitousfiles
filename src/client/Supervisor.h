#ifndef SUPERVISOR_H_
#define SUPERVISOR_H_

/*
 *      Name: ./client/Supervisor.h
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 * Las instancias de "Supervisor" son las encargadas de llevar a cabo la sincronización temporizada de los archivos del cliente.
 */

#include "./../common/MiSocket.h"
#include "./../common/Sincronizador.h"
#include "./../common/Thread.h"

namespace ubicuos {

  class Cliente;
  class Organizador;

  class Supervisor: public threads::Thread {
    private:
      const unsigned int intervalo;
      Organizador &organizador;
      const MiSocket &miSocket;
      void (* const informarSincronizacionOK)() throw ();
      threads::Sincronizador sincronizador;
      Cliente &cliente;

      // Constructor por copia declarado privado pero no definido (no se deben copiar las instancias de "Supervisor").
      Supervisor(const Supervisor &supervisor);

      // Operador asignación de copia declarado privado pero no definido (no se deben copiar las instancias de "Supervisor").
      Supervisor& operator=(const Supervisor &supervisor);

      // Es la función que se ejecuta en un hilo nuevo en un ciclo infinito hasta que se ejecute "pedirDetener()", "close()", o "~Supervisor()".
      // Hace un ciclo infinito en el cual "duerme" durante (al menos) el tiempo especificado en "intervalo" y luego ejecuta la sincronización temporizada de archivos, llamando a la función correspondiente de la instancia de "Organizador". Una vez finalizada esta sincronización, informa si al misma se realizó sin conflictos, ejecutando una vez el puntero a función "(*informar)()".
      virtual void *procesar();

      class Temporizador: public threads::Thread {
        private:
          const unsigned int intervalo;
          threads::Sincronizador &sincronizador;
          virtual void *procesar();

        public:
          Temporizador(const unsigned int intervalo, threads::Sincronizador &sincronizador);
          ~Temporizador() throw ();
          virtual void pedirDetener();
          virtual void close(void ** const pPValorDevuelto);
          void signalHandler();
      };

      static Temporizador *pTemporizador;

      static void signalHandler(const int entero);

    public:
      Supervisor(const unsigned int intervalo, Organizador &organizador, const MiSocket &miSocket, void (* const informarSincronizacionOK)() throw (), Cliente &cliente);

      virtual ~Supervisor() throw ();

      virtual void pedirDetener();

      virtual void close(void ** const pPValorDevuelto);

      virtual void sincronizar();
  };

} /* namespace ubicuos */
#endif /* SUPERVISOR_H_ */

