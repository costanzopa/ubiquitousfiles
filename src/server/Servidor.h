#ifndef SERVIDOR_H_
#define SERVIDOR_H_

/*
 *      Name: ./server/Servidor.h
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 * "Las instancias de "Servidor" se encargan de establecer la comunicación con los clientes, crear nuevas instancias de "Atendedor" que continúen la atención de los mismos, purgar estas instancias de "Atendedor" cuando los clientes se desconectan, destruir el resto de las instancias de "Atendedor" al finalizar la ejecución del programa, contener e inicializar la instancia de "AdmUsuarios" que contendrá la información de los usuarios, y contener la instancia de "AdmArchivos" que manejará la disposición (en el servidor) de los archivos de los clientes.
 */

#include <queue>
#include <set>
#include <string>
#include "./../common/AdmArchivos.h"
#include "./../common/MiExcepcion.h"
#include "./../common/MiSocket.h"
#include "./../common/Thread.h"
#include "./AdmUsuarios.h"
#include "./Atendedor.h"

namespace ubicuos {

  class Servidor: public threads::Thread {
    private:
      bool seInterrumpioSocket;
      MiSocket miSocket;
      AdmArchivos admArchivos;
      AdmUsuarios admUsuarios;
      std::set< Atendedor * > atendedores;
      std::queue< Atendedor * > atendedoresARecolectar;

      // Constructor por copia declarado privado pero no definido (no se deben copiar las instancias de "Servidor").
      Servidor(const Servidor &servidor);

      // Operador asignación de copia declarado privado pero no definido (no se deben copiar las instancias de "Servidor").
      Servidor& operator=(const Servidor &servidor);

      // Es la función que se ejecuta en un hilo nuevo en un ciclo infinito hasta que se ejecute "pedirDetener()", "close()", o "~Servidor()".
      // Intenta aceptar conexiones entrantes de clientes, crearles a cada uno, una instancia de "Atendedor" de dedicacion exclusiva, y manejar estos recursos tanto durante la ejecución de su ciclo infinito, como al concluir el mismo.
      virtual void *procesar();

    public:
      Servidor(const std::string &puerto, const unsigned int colaLargo, const std::string &directorioRuta);

      virtual ~Servidor() throw ();

      virtual void pedirDetener();

      virtual void close(void ** const pPValorDevuelto);
  };

  //**************************************************EXCEPCIONES**************************************************//

  class ServidorExcepcion: public exceptions::MiExcepcion {
    public:
      virtual ~ServidorExcepcion() throw ();
      virtual const char *what() const throw ();
  };

} /* namespace ubicuos */
#endif /* SERVIDOR_H_ */
