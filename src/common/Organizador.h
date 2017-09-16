#ifndef ORGANIZADOR_H_
#define ORGANIZADOR_H_

/*
 *      Name: ./common/Organizador.h
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 * Las instancias de "Organizador" son las encargadas de realizar las actualizaciones de archivos tanto en el lado cliente, como en el lado servidor.
 */

#include <map>
#include <queue>
#include <string>
#include <dirent.h>
#include "./MiExcepcion.h"
#include "./MiSocket.h"
#include "./Mutex.h"
#include "./Pedido.h"
#include "./RegistroArchivo.h"
#include "./../server/AdmUsuarios.h"

namespace ubicuos {

  using threads::Mutex;

  // Declaración de clase "Atendedor" para evitar problemas de referencia circular.
  class Atendedor;

  class Cliente;
  class Supervisor;

  class Organizador {
    private:
      std::map< const std::string, RegistroArchivo, bool (*)(const std::string &primero, const std::string &segundo) throw () > mapaPorArchivo;
      Mutex mutex;
      const std::string directorioRuta;

      // Se utiliza para ordenar el "mapaPorArchivo" con las mismas reglas que se usan en los programas exploradores de archivos.
      static bool ordenadorDeMapa(const std::string &primero, const std::string &segundo) throw ();

      // Se utiliza para filtrar los "struct dirent *" correspondientes a los archivos llamados "." y ".." del listado generado por "::scandir()" en "Organizador::generarMapa()".
      static int filtro(const struct dirent *pDirent);

      // Genera un mapa de los archivos presentes en "directorioRuta", usando la función "filtro()" para filtrarlos, y "ordenadorDeMapa()" para ordenarlos. Los elementos del mapa tienen como clave el nombre del archivo, y como dato el "RegistroArchivo" que le corresponde.
      static void generarMapa(const std::string &directorioRuta, std::map< const std::string, RegistroArchivo, bool (*)(const std::string &primero, const std::string &segundo) throw () > &mapaPorArchivo);

      // Compara los archivos del cliente con los registros que envía el servidor, generando una "colaPeticiones" de los que necesita y borrando los que no existen en el servidor.
      virtual void clienteAparear(std::queue< std::string > &colaPeticiones, const MiSocket &miSocket);

      // Procesa el archivo recibido del servidor en respuesta a la petición para establecer la copia del servidor inicial.
      virtual void clienteProcesar(const std::string &nombreArchivo, const MiSocket &miSocket);

      virtual const std::string obtenerDirectorioRutaPadre(const std::string &ruta) const;

      virtual void clienteModificarArchivo(const Pedido &pedidoRecibido, std::queue< Pedido > &colaPedidos, const MiSocket &miSocket);

      virtual void clienteCrearArchivo(const Pedido &pedidoRecibido, const MiSocket &miSocket, void (* const informarSincronizacionConflicto)(const std::string &) throw ());

      virtual void clienteBorrarArchivo(const Pedido &pedidoRecibido, void (* const informarSincronizacionConflicto)(const std::string &) throw ());

      virtual void clienteModificarArchivoConflictivo(const Pedido &pedidoRecibido, const MiSocket &miSocket, void (* const informarSincronizacionConflicto)(const std::string &) throw ());

      virtual void serializarColaPedidos(std::queue< Pedido > &colaPedidos, const MiSocket &miSocket) const;

      virtual bool serializarColaPedidosYArchivos(std::queue< Pedido > &colaPedidos, const MiSocket &miSocket) const;

      virtual void serializarColaPedidosArchivosCompletos(std::queue< Pedido > &colaPedidos, const MiSocket &miSocket) const;

      virtual bool servidorModificarArchivo(const Pedido &pedidoRecibido, std::queue< Pedido > &colaPedidosSincronizacion, const MiSocket &miSocket);

      virtual bool servidorCrearArchivo(const Pedido &pedidoRecibido, std::queue< Pedido > &colaPedidosSincronizacion, const MiSocket &miSocket);

      virtual bool servidorBorrarArchivo(const Pedido &pedidoRecibido, std::queue< Pedido > &colaPedidosSincronizacion);

      virtual void servidorEnviarArchivo(const Pedido &pedidoRecibido, std::queue< Pedido > &colaPedidosSincronizacion);

    public:
      // Constructor utilizado por las instancias de "Cliente", y por las instancias de "AdmArchivos".
      Organizador(const std::string &directorioRuta);

      virtual ~Organizador() throw ();

      // Crea en el directorio del cliente una copia exacta del conetenido del servidor para dicho cliente.
      virtual void clienteEstablecerCopiaDelServidor(const MiSocket &miSocket);

      // Recibe pedidos del servidor (enviados por otros clientes registrados bajo el mismo nombre de usuario) y los procesa.
      virtual void clienteRecibirActualizaciones(const MiSocket &miSocket, void (* const informarSincronizacionOK)() throw (), void (* const informarSincronizacionConflicto)(const std::string &) throw (), Cliente &cliente, Supervisor &supervisor);

      // Usada por el "Supervisor".
      virtual void clienteEnviarSincronizacion(const MiSocket &miSocket, void (* const informarSincronizacionOK)() throw ());

      // Bloquea el acceso al "mapaPorArchivo".
      virtual void bloquearParaOtros();

      // Desbloquea el acceso al "mapaPorArchivo".
      virtual void desbloquearParaOtros();

      // Usada por el "AdmArchivos" para hacer la "sincronizacionInicial()".
      virtual void servidorEnviarRegistrosIniciales(const MiSocket &miSocket);

      // Usada por el "AdmArchivos" para hacer la "sincronizacionInicial()".
      virtual void servidorRecibirConfirmacionOPeticiones(const MiSocket &miSocket);

      // Este método se encarga de enviar un mensaje de sincronización correcta al cliente en caso de que no haya conflictos en la sincronización.
      virtual void servidorRecibirActualizaciones(const MiSocket &miSocket, const AdmUsuarios &admUsuarios, const std::string &nombre, Atendedor * const atendedor);

      virtual void servidorRepetirEnvios(std::queue< Pedido > &colaPedidos, const MiSocket &miSocket);

      virtual unsigned int getTamanyo();
  };

//**************************************************EXCEPCIONES**************************************************//

  class OrganizadorExcepcion: public exceptions::MiExcepcion {
    public:
      virtual ~OrganizadorExcepcion() throw ();
      virtual const char *what() const throw ();
  };

} /* namespace ubicuos */
#endif /* ORGANIZADOR_H_ */
