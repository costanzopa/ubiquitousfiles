/*
 *      Name: ./common/Organizador.cpp
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include "./Organizador.h"
#include <cstdio>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <dirent.h>
#include <utime.h>
#include <sys/stat.h>
//#include <sys/types.h>
#include <syslog.h>
#include "./Comandos.h"
#include "./MiSocket.h"
#include "./Mutex.h"
#include "./Pedido.h"
#include "./RegistroArchivo.h"
#include "./../client/Cliente.h"
#include "./../server/AdmUsuarios.h"
#include "./../server/Atendedor.h"

#include <iostream>

namespace ubicuos {

  bool Organizador::ordenadorDeMapa(const std::string &primero, const std::string &segundo) throw () {
    return (strverscmp(primero.c_str(), segundo.c_str()) < 0);
  }

  int Organizador::filtro(const struct dirent *pDirent) {
    return (strcmp(pDirent->d_name, ".") && strcmp(pDirent->d_name, ".."));
  }

  void Organizador::generarMapa(const std::string &directorioRuta, std::map< const std::string, RegistroArchivo, bool (*)(const std::string &primero, const std::string &segundo) throw () > &mapaPorArchivo) {
    //std::cout << "Organizador::generarMapa():" << std::endl;
    //std::cout << "***INICIO DE GENERACIÓN DE MAPA." << std::endl;
    struct dirent **vector;
    // http://linux.die.net/man/3/scandir
    // http://linux.die.net/man/3/versionsort
    const int cantidad = ::scandir(directorioRuta.c_str(), &vector, &filtro, versionsort);
    //std::cout << "  Cantidad de archios a indexar: " << cantidad << std::endl;
    if (cantidad < 0) {
      ::perror("scandir()");
      throw OrganizadorExcepcion();
    } else {
      for (int contador = 0; contador < cantidad; ++contador) {
        struct stat info;
        std::string nombre(vector[contador]->d_name);
        std::string archivoRuta(directorioRuta + nombre);
        // http://linux.die.net/man/1/stat
        // http://linux.die.net/man/2/stat
        // http://linux.die.net/man/3/stat
        if (::stat(archivoRuta.c_str(), &info) == -1) {
          ::perror("stat()");
          throw OrganizadorExcepcion();
        }
        off_t tamanyo = info.st_size;
        time_t fecha = info.st_mtime;
        //std::cout << "    Archivo indexado: " << archivoRuta << std::endl;
        std::ifstream archivoHandler(archivoRuta.c_str());
        const hashes::MD5 md5(archivoHandler);
        archivoHandler.close();
        RegistroArchivo registro(tamanyo, fecha, md5.raw_digest().getDataPtr());
        mapaPorArchivo.insert(std::pair< std::string, RegistroArchivo >(nombre, registro));
        ::free(vector[contador]);
      }
      ::free(vector);
    }
    //std::cout << "***FÍN DE GENERACIÓN DE MAPA." << std::endl;
  }

  Organizador::Organizador(const std::string &directorioRuta)
      : mapaPorArchivo(&ordenadorDeMapa), directorioRuta(directorioRuta) {
    //std::cout << "Organizador::Organizador():" << std::endl;
    mutex.tomar();
    generarMapa(directorioRuta, mapaPorArchivo);
    mutex.liberar();
  }

  Organizador::~Organizador() throw () {
    //std::cout << "Organizador::~Organizador():" << std::endl;
  }

  void Organizador::bloquearParaOtros() {
    // Tomo el mutex del "Organizador" (específico de esta cuenta de usuario).
    mutex.tomar();
  }

  void Organizador::desbloquearParaOtros() {
    // Libero el mutex del "Organizador" (específico de esta cuenta de usuario).
    mutex.liberar();
  }

  void Organizador::clienteAparear(std::queue< std::string > &colaPeticiones, const MiSocket &miSocket) {
    //std::cout << "  Organizador::clienteAparear():" << std::endl;
    std::map< std::string, RegistroArchivo >::iterator iterador = mapaPorArchivo.begin();
    unsigned int nroRegistros;
    //std::cout << "  ***INICIO DEL APAREO DE ARCHIVOS." << std::endl;
    miSocket.recibirEspecifico(&nroRegistros, sizeof nroRegistros);
    //std::cout << "  Recibido cantidad de archivos en el servidor: " << nroRegistros << std::endl;
    for (unsigned int contador = 0; contador < nroRegistros; ++contador) {
      std::string nombreArchivo;
      miSocket.recibirCompleto(nombreArchivo);
      RegistroArchivo registro(miSocket);
      //std::cout << "    Recibido registro de archivo: " << nombreArchivo << std::endl;
      while ((iterador != mapaPorArchivo.end()) && (iterador->first < nombreArchivo)) {
        //std::cout << "      ***Se borra el archivo local: " << iterador->first << std::endl;
        ::remove(std::string(directorioRuta + iterador->first).c_str());
        mapaPorArchivo.erase(iterador++);
      }
      if ((iterador == mapaPorArchivo.end()) || (iterador->first > nombreArchivo)) {
        //std::cout << "      ***Se peticionará una copia del archivo: " << nombreArchivo << std::endl;
        colaPeticiones.push(nombreArchivo);
      } else {
        if ((iterador->second.getTamanyo() != registro.getTamanyo()) || (iterador->second.getFecha() != registro.getFecha()) || (memcmp(iterador->second.getHashPtr(), registro.getHashPtr(), registro.getHashLargo()))) {
          //std::cout << "      ***Se borra la copia local y se peticionará una copia actualizada de: " << nombreArchivo << std::endl;
          ::remove(std::string(directorioRuta + nombreArchivo).c_str());
          mapaPorArchivo.erase(iterador++);
          colaPeticiones.push(nombreArchivo);
        } else {
          //std::cout << "      ***Se mantiene sin cambios el archivo: " << nombreArchivo << std::endl;
          ++iterador;
        }
      }
    }
    while (iterador != mapaPorArchivo.end()) {
      //std::cout << "      ***Se borra el archivo local: " << iterador->first << std::endl;
      ::remove(std::string(directorioRuta + iterador->first).c_str());
      mapaPorArchivo.erase(iterador++);
    }
    //std::cout << "  ***FIN DEL APAREO DE ARCHIVOS." << std::endl;
  }

  void Organizador::clienteProcesar(const std::string &nombre, const MiSocket &miSocket) {
    //std::cout << "  Organizador::clienteProcesar():" << std::endl;
    //std::cout << "  ***INICIO DE PROCESAR RESPUESTA A PETICIÓN." << std::endl;
    const RegistroArchivo registro(miSocket);
    const std::string archivoRuta(directorioRuta + nombre);
    std::ofstream archivoHandler(archivoRuta.c_str(), std::ofstream::out);
    //std::cout << "    ***INICIO DE RECIBIR ARCHIVO: " << archivoRuta << std::endl;
    miSocket.recibirCompleto(archivoHandler);
    //std::cout << "    ***FIN DE RECIBIR ARCHIVO." << std::endl;
    archivoHandler.close();
    struct stat info;
    if (::stat(archivoRuta.c_str(), &info) == -1) {
      ::perror("stat()");
      throw OrganizadorExcepcion();
    }
    if (info.st_size != registro.getTamanyo()) {
      //std::cout << "    ***EL TAMAÑO NO COINCIDE" << std::endl;
      throw OrganizadorExcepcion();
    }
    // TODO (Iván): verificar los hashes.
    const struct utimbuf times = { info.st_atime, registro.getFecha() };
    if (::utime(archivoRuta.c_str(), &times) == -1) {
      ::perror("utime()");
      throw OrganizadorExcepcion();
    }
    //std::cout << "    Crear el archivo: " << nombre << std::endl;
    mapaPorArchivo.insert(mapaPorArchivo.end(), std::pair< std::string, RegistroArchivo >(nombre, registro));
    //std::cout << "  ***FIN DE PROCESAR RESPUESTA A PETICIÓN." << std::endl;
  }

  void Organizador::clienteEstablecerCopiaDelServidor(const MiSocket &miSocket) {
    ::syslog(LOG_USER | LOG_INFO, "Ejecutando Organizador::clienteEstablecerCopiaDelServidor().");
    std::queue< std::string > colaPeticiones;
    //std::cout << "***COMIENZO DE ESTABLECIMIENTO DE COPIA DEL SERVIDOR." << std::endl;
    mutex.tomar();
    clienteAparear(colaPeticiones, miSocket);
    unsigned int cantidad = colaPeticiones.size();
    miSocket.enviarCompleto(&cantidad, sizeof cantidad);
    //std::cout << "Enviado cantidad de peticiones: " << cantidad << std::endl;
    while (colaPeticiones.empty() == false) {
      std::string nombre(colaPeticiones.front());
      miSocket.enviarCompleto(nombre);
      //std::cout << "  Enviado petición del archivo: " << nombre << std::endl;
      std::string nombreRecibido;
      miSocket.recibirCompleto(nombreRecibido);
      //std::cout << "  Recibido nombre de archivo peticionado: " << nombre << std::endl;
      if (nombreRecibido.compare(nombre) != 0) {
        //std::cout << "  ***EL ARCHIVO RECIBIDO NO ES EL PEDIDO." << std::endl;
        throw OrganizadorExcepcion();
      } else {
        clienteProcesar(nombreRecibido, miSocket);
      }
      colaPeticiones.pop();
    }
    mutex.liberar();
    //std::cout << "***FIN DE ESTABLECIMIENTO DE COPIA DEL SERVIDOR." << std::endl;
  }

  const std::string Organizador::obtenerDirectorioRutaPadre(const std::string &ruta) const {
    size_t posicion = ruta.find_last_of('/');
    std::string directorioRutaPadre(ruta.substr(0, posicion));
    posicion = directorioRutaPadre.find_last_of('/');
    directorioRutaPadre = directorioRutaPadre.substr(0, posicion);
    directorioRutaPadre += '/';
    return directorioRutaPadre;
  }

  void Organizador::clienteModificarArchivo(const Pedido &pedidoRecibido, std::queue< Pedido > &colaPedidos, const MiSocket &miSocket) {
    //std::cout << "      Organizador::clienteModificarArchivo():" << std::endl;
    const std::string &nombre = pedidoRecibido.getNombreArchivo();
    const RegistroArchivo &registro = pedidoRecibido.getRegistroArchivoNuevo();
    std::map< std::string, RegistroArchivo >::iterator iterador = mapaPorArchivo.find(nombre);
    if (iterador != mapaPorArchivo.end()) {
      if (iterador->second.corresponde(pedidoRecibido.getRegistroArchivoViejo())) {
        const std::string archivoRuta(directorioRuta + nombre);
        {
          //std::cout << "        Borrar el archivo: " << nombre << std::endl;
          ::remove(archivoRuta.c_str());
          mapaPorArchivo.erase(iterador);
          std::ofstream archivoHandler(archivoRuta.c_str(), std::ofstream::out);
          //std::cout << "        ***INICIO DE RECIBIR ARCHIVO: " << archivoRuta << std::endl;
          miSocket.recibirCompleto(archivoHandler);
          //std::cout << "        ***FIN DE RECIBIR ARCHIVO." << std::endl;
          archivoHandler.close();
        }
        struct stat info;
        if (::stat(archivoRuta.c_str(), &info) == -1) {
          ::perror("stat()");
          throw OrganizadorExcepcion();
        }
        if (info.st_size != registro.getTamanyo()) {
          //std::cout << "        ***EL TAMAÑO NO COINCIDE." << std::endl;
          throw OrganizadorExcepcion();
        }
        const struct utimbuf times = { info.st_atime, registro.getFecha() };
        if (::utime(archivoRuta.c_str(), &times) == -1) {
          ::perror("utime()");
          throw OrganizadorExcepcion();
        }
        // TODO (Iván): verificar los hashes.
        //std::cout << "        Crear el archivo: " << nombre << std::endl;
        mapaPorArchivo.insert(std::pair< std::string, RegistroArchivo >(nombre, registro));
        return;
      }
    }
    // Hubo un conflicto de sincronización.
    // Específicamente, el cliente modificó o borró el archivo a partir del cual fue generado el pedido de modificación.
    miSocket.recibirCompleto();
    colaPedidos.push(Pedido(std::string(ENVIAR_ARCHIVO_MODIFICAR), nombre, registro));
    //std::cout << "        ***Se agregó pedido de archivo completo a cola de pedidos." << std::endl;

  }

  void Organizador::clienteCrearArchivo(const Pedido &pedidoRecibido, const MiSocket &miSocket, void (* const informarSincronizacionConflicto)(const std::string &) throw ()) {
    //std::cout << "      Organizador::clienteCrearArchivo():" << std::endl;
    const std::string &nombre = pedidoRecibido.getNombreArchivo();
    const RegistroArchivo &registro = pedidoRecibido.getRegistroArchivoNuevo();
    const std::string archivoRuta(directorioRuta + nombre);
    bool conflicto = false;
    std::map< std::string, RegistroArchivo >::iterator iterador = mapaPorArchivo.find(nombre);
    if (iterador != mapaPorArchivo.end()) {
      // Hubo un conflicto de sincronización.
      // Específicamente, el cliente creó un archivo con el mismo nombre que el archivo que se pidió crear.
      std::string archivoViejoRuta(obtenerDirectorioRutaPadre(archivoRuta));
      archivoViejoRuta += nombre;
      //std::cout << "        Mover al directorio superior el archivo: " << nombre << std::endl;
      ::rename(archivoRuta.c_str(), archivoViejoRuta.c_str());
      mapaPorArchivo.erase(iterador);
      conflicto = true;
    }
    std::ofstream archivoHandler(archivoRuta.c_str(), std::ofstream::out);
    //std::cout << "        ***INICIO DE RECIBIR ARCHIVO: " << archivoRuta << std::endl;
    miSocket.recibirCompleto(archivoHandler);
    //std::cout << "        ***FIN DE RECIBIR ARCHIVO." << std::endl;
    archivoHandler.close();
    struct stat info;
    if (::stat(archivoRuta.c_str(), &info) == -1) {
      ::perror("stat()");
      throw OrganizadorExcepcion();
    }
    if (info.st_size != registro.getTamanyo()) {
      //std::cout << "        ***EL TAMAÑO NO COINCIDE" << std::endl;
      throw OrganizadorExcepcion();
    }
    const struct utimbuf times = { info.st_atime, registro.getFecha() };
    if (::utime(archivoRuta.c_str(), &times) == -1) {
      ::perror("utime()");
      throw OrganizadorExcepcion();
    }
    // TODO (Iván): verificar los hashes.
    //std::cout << "        Crear el archivo: " << nombre << std::endl;
    mapaPorArchivo.insert(std::pair< std::string, RegistroArchivo >(nombre, registro));
    if (conflicto == true) {
      std::string mensajeConflicto("El archivo " + nombre + " fue creado por otro cliente a pesar de que Ud. ya lo había creado, se restituyó una copia actualizada al directorio, y se movió su copia al directorio superior.");
      informarSincronizacionConflicto(mensajeConflicto);
    }
  }

  void Organizador::clienteBorrarArchivo(const Pedido &pedidoRecibido, void (* const informarSincronizacionConflicto)(const std::string &) throw ()) {
    //std::cout << "      Organizador::clienteBorrarArchivo():" << std::endl;
    const std::string &nombre = pedidoRecibido.getNombreArchivo();
    const RegistroArchivo &registro = pedidoRecibido.getRegistroArchivoNuevo();
    const std::string archivoRuta(directorioRuta + nombre);
    std::map< std::string, RegistroArchivo >::iterator iterador = mapaPorArchivo.find(nombre);
    if (iterador != mapaPorArchivo.end()) {
      if (iterador->second.corresponde(registro)) {
        //std::cout << "        Borrar el archivo: " << nombre << std::endl;
        ::remove(archivoRuta.c_str());
        mapaPorArchivo.erase(iterador);
        return;
      }
    }
    // Hubo un conflicto de sincronización.
    // Específicamente, el cliente modificó o borró el archivo a partir del cual fue generado el pedido de borrado.
    // Si me piden que lo borre y yo ya lo había borrado, no hay problema porque no hay información de la que no me enteré.
    // Si me piden que lo borre y yo lo había modificado, hay problema porque puedo perder mis cambios.
    // No se puede enviar pedido de archivo completo porque en el servidor, a esta altura, ya está borrado este archivo.
    if (iterador != mapaPorArchivo.end()) {
      mapaPorArchivo.erase(iterador);
      std::string archivoViejoRuta(obtenerDirectorioRutaPadre(archivoRuta));
      archivoViejoRuta += nombre;
      //std::cout << "        Mover al directorio superior el archivo: " << nombre << std::endl;
      ::rename(archivoRuta.c_str(), archivoViejoRuta.c_str());
      std::string mensajeConflicto("El archivo " + nombre + " fue borrado por otro cliente sin que se tuvieran en cuenta los cambios aquí realizados, se movió su copia al directorio superior.");
      informarSincronizacionConflicto(mensajeConflicto);
    } else {
      // Otro cliente lo borró, pero este cliente ya lo había borrado --> no hay problema.
    }

  }

  void Organizador::clienteModificarArchivoConflictivo(const Pedido &pedidoRecibido, const MiSocket &miSocket, void (* const informarSincronizacionConflicto)(const std::string &) throw ()) {
    //std::cout << "      Organizador::clienteModificarArchivoConflictivo():" << std::endl;
    const std::string &nombre = pedidoRecibido.getNombreArchivo();
    const RegistroArchivo &registro = pedidoRecibido.getRegistroArchivoNuevo();
    const std::string archivoRuta(directorioRuta + nombre);
    std::map< std::string, RegistroArchivo >::iterator iterador = mapaPorArchivo.find(nombre);
    std::string mensajeConflicto;
    if (iterador != mapaPorArchivo.end()) {
      mapaPorArchivo.erase(iterador);
      std::string archivoViejoRuta(obtenerDirectorioRutaPadre(archivoRuta));
      archivoViejoRuta += nombre;
      //std::cout << "        Mover al directorio superior el archivo: " << nombre << std::endl;
      ::rename(archivoRuta.c_str(), archivoViejoRuta.c_str());
      mensajeConflicto = "El archivo " + pedidoRecibido.getNombreArchivo() + " fue modificado por otro cliente a pesar de que Ud. lo había modificado, se restituyó una copia actualizada al directorio, y se movió su copia al directorio superior.";
    } else {
      mensajeConflicto = "El archivo " + pedidoRecibido.getNombreArchivo() + " fue modificado por otro cliente a pesar de que Ud. lo había borrado, se restituyó una copia actualizada al directorio.";
    }
    std::ofstream archivoHandler(archivoRuta.c_str(), std::ofstream::out);
    //std::cout << "        ***INICIO DE RECIBIR ARCHIVO: " << archivoRuta << std::endl;
    miSocket.recibirCompleto(archivoHandler);
    //std::cout << "        ***FIN DE RECIBIR ARCHIVO." << std::endl;
    archivoHandler.close();
    struct stat info;
    if (::stat(archivoRuta.c_str(), &info) == -1) {
      ::perror("stat()");
      throw OrganizadorExcepcion();
    }
    if (info.st_size != registro.getTamanyo()) {
      //std::cout << "        ***EL TAMAÑO NO COINCIDE" << std::endl;
      throw OrganizadorExcepcion();
    }
    const struct utimbuf times = { info.st_atime, registro.getFecha() };
    if (::utime(archivoRuta.c_str(), &times) == -1) {
      ::perror("utime()");
      throw OrganizadorExcepcion();
    }
    // TODO (Iván): verificar los hashes.
    //std::cout << "        Crear el archivo: " << nombre << std::endl;
    mapaPorArchivo.insert(std::pair< std::string, RegistroArchivo >(nombre, registro));
    informarSincronizacionConflicto(mensajeConflicto);
  }

  void Organizador::serializarColaPedidos(std::queue< Pedido > &colaPedidos, const MiSocket &miSocket) const {
    //std::cout << "    Organizador::serializarColaPedidos():" << std::endl;
    // Manda la cantidad de archivos completos que necesitamos, y luego los pedidos uno por uno.
    unsigned int cantidad = colaPedidos.size();
    //std::cout << "    ***INICIO DE ENVIO DE COLA DE PEDIDOS." << std::endl;
    miSocket.enviarCompleto(&cantidad, sizeof cantidad);
    //std::cout << "    Enviado cantidad de pedidos: " << cantidad << std::endl;
    while (colaPedidos.empty() == false) {
      Pedido pedido(colaPedidos.front());
      //std::cout << "      ***Inicio de envío de pedido." << std::endl;
      pedido.serializar(miSocket);
      //std::cout << "      ***Fín de envío de pedido." << std::endl;
      colaPedidos.pop();
    }
    //std::cout << "    ***FIN DE ENVIO DE COLA DE PEDIDOS." << std::endl;
  }

  // Toma el mutex del "Organizador" al momento de empezar a hacer cambios.
  void Organizador::clienteRecibirActualizaciones(const MiSocket &miSocket, void (* const informarSincronizacionOK)() throw (), void (* const informarSincronizacionConflicto)(const std::string &) throw (), Cliente &cliente, Supervisor &supervisor) {
    ::syslog(LOG_USER | LOG_INFO, "Ejecutando Organizador::clienteRecibirActualizaciones().");
    unsigned int nroPedidos;
    //std::cout << "***INICIO DE RECEPCIÓN DE PEDIDOS DE ACTUALIZACIÓN." << std::endl;
    miSocket.recibirEspecifico(&nroPedidos, sizeof nroPedidos);
    //std::cout << "  Recibido cantidad de pedidos de actualización: " << nroPedidos << std::endl;
    if (nroPedidos == 0) {
      //std::cout << "***FÍN DE RECEPCIÓN DE PEDIDOS DE ACTUALIZACIÓN." << std::endl;
      informarSincronizacionOK();
    } else {
      cliente.sincronizarYBloquear(supervisor);
      mutex.tomar();
      std::queue< Pedido > colaPedidos;
      while (nroPedidos > 0) {
        //std::cout << "    ***Inicio de recepción de pedido de actualización." << std::endl;
        const Pedido pedidoRecibido(miSocket);
        const std::string &comando = pedidoRecibido.getComando();
        //std::cout << "      Comando de pedido de actualización recibido es: " << comando << std::endl;
        if (comando == std::string(MODIFICAR_ARCHIVO)) {
          clienteModificarArchivo(pedidoRecibido, colaPedidos, miSocket);
        } else {
          if (comando == std::string(CREAR_ARCHIVO)) {
            clienteCrearArchivo(pedidoRecibido, miSocket, informarSincronizacionConflicto);
          } else {
            if (comando == std::string(BORRAR_ARCHIVO)) {
              clienteBorrarArchivo(pedidoRecibido, informarSincronizacionConflicto);
            } else {
              if (comando == std::string(ENVIAR_ARCHIVO_MODIFICAR)) {
                clienteModificarArchivoConflictivo(pedidoRecibido, miSocket, informarSincronizacionConflicto);
              } else {
                //std::cout << "      ***Comando de pedido de actualización recibido es: OPERACIÓN NO VALIDA." << std::endl;
                mutex.liberar();
                throw OrganizadorExcepcion();
              }
            }
          }
        }
        //std::cout << "    ***Fin de recepción de pedido de actualización." << std::endl;
        --nroPedidos;
      }
      serializarColaPedidos(colaPedidos, miSocket);
      //std::cout << "***FÍN DE RECEPCIÓN DE PEDIDOS DE ACTUALIZACIÓN." << std::endl;
      mutex.liberar();
      cliente.desbloquearSincronizaciones();
    }
  }

  bool Organizador::serializarColaPedidosYArchivos(std::queue< Pedido > &colaPedidos, const MiSocket &miSocket) const {
    //std::cout << "    Organizador::serializarColaPedidosYArchivos():" << std::endl;
    unsigned int cantidad = colaPedidos.size();
    //std::cout << "    ***COMIENZO DE ENVÍO DE PEDIDOS PARA SINCRONIZACIÓN." << std::endl;
    if (cantidad == 0) {
      //std::cout << "    ***FÍN DE ENVÍO DE PEDIDOS PARA SINCRONIZACIÓN." << std::endl;
      return true;
    } else {
      miSocket.enviarCompleto(&cantidad, sizeof cantidad);
      //std::cout << "    Enviado cantidad de pedidos: " << cantidad << std::endl;
      while (colaPedidos.empty() == false) {
        Pedido &pedidoEnviado = colaPedidos.front();
        //std::cout << "      ***Comienzo de envío de pedido para sincronización." << std::endl;
        pedidoEnviado.serializar(miSocket);
        std::string comando(pedidoEnviado.getComando());
        if ((comando.compare(MODIFICAR_ARCHIVO) == 0) || (comando.compare(CREAR_ARCHIVO) == 0)) {
          std::string archivoRuta(directorioRuta + pedidoEnviado.getNombreArchivo());
          std::ifstream archivoHandler(archivoRuta.c_str(), std::ifstream::in);
          //std::cout << "        ***Inicio envío archivo correspondiente a pedido para sincronización: " << pedidoEnviado.getNombreArchivo() << std::endl;
          miSocket.enviarCompleto(archivoHandler);
          //std::cout << "        ***Fín envío archivo correspondiente a pedido para sincronización: " << pedidoEnviado.getNombreArchivo() << std::endl;
          archivoHandler.close();
        }
        //std::cout << "      ***Fín de envío de pedido para sincronización." << std::endl;
        colaPedidos.pop();
      }
      //std::cout << "    ***FÍN DE ENVÍO DE PEDIDOS PARA SINCRONIZACIÓN." << std::endl;
      return false;
    }
  }

  void Organizador::clienteEnviarSincronizacion(const MiSocket &miSocket, void (* const informarSincronizacionOK)() throw ()) {
    ::syslog(LOG_USER | LOG_INFO, "Ejecutando Organizador::clienteEnviarSincronizacion().");
    mutex.tomar();
    std::queue< Pedido > colaPedidos;
    //std::cout << "  ***INICIO DE LA SINCRONIZACIÓN." << std::endl;
    // Un nuevo "mapaPorArchivo" con los contenidos actuales.
    std::map< const std::string, RegistroArchivo, bool (*)(const std::string &primero, const std::string &segundo) throw () > mapaPorArchivoNuevo(&ordenadorDeMapa);
    generarMapa(directorioRuta, mapaPorArchivoNuevo);
    std::map< std::string, RegistroArchivo >::iterator iterador = mapaPorArchivo.begin();
    std::map< std::string, RegistroArchivo >::iterator iteradorNuevo = mapaPorArchivoNuevo.begin();
    // Recorro el "mapaPorArchivoNuevo" colocando en la "colaPedidos" los pedidos para borrar, crear o modificar archivos.
    //std::cout << "    ***INICIO DEL APAREO." << std::endl;
    while (iteradorNuevo != mapaPorArchivoNuevo.end()) {
      std::string nombreArchivo(iteradorNuevo->first);
      RegistroArchivo registro(iteradorNuevo->second);
      while ((iterador != mapaPorArchivo.end()) && (iterador->first < nombreArchivo)) {
        // Mientras no haya llegado al final de la lista de archivos viejos, y el nombre del archivo nuevo que estoy procesando sea mayor que el del archivo viejo que leí de la lista, el archivo viejo que leí de la lista fue borrado recientemente.
        //std::cout << "    Archivo borrado: " << iterador->first << std::endl;
        Pedido pedido(std::string(BORRAR_ARCHIVO), iterador->first, iterador->second);
        colaPedidos.push(pedido);
        mapaPorArchivo.erase(iterador++);
      }
      if ((iterador == mapaPorArchivo.end()) || (iterador->first > nombreArchivo)) {
        // Si llegué al final de la lista de archivos viejos, o el nombre del archivo nuevo que estoy procesando es menor que el del archivo viejo que leí de la lista, entonces el archivo nuevo que estoy procesando es un archivo que fue creado recientemente.
        //std::cout << "    Archivo creado: " << nombreArchivo << std::endl;
        Pedido pedido(std::string(CREAR_ARCHIVO), nombreArchivo, registro);
        colaPedidos.push(pedido);
        mapaPorArchivo.insert(std::pair< std::string, RegistroArchivo >(iteradorNuevo->first, iteradorNuevo->second));
      } else {
        // Ambos archivos tienen el mismo nombre.
        if (iterador->second.corresponde(registro) == false) {
          // Si alguno de sus parámetros ha cambiado, entonces el archivo ha sido modificado recientemente.
          //std::cout << "    Archivo modificado: " << nombreArchivo << std::endl;
          Pedido pedido(std::string(MODIFICAR_ARCHIVO), nombreArchivo, registro, iterador->second);
          colaPedidos.push(pedido);
          mapaPorArchivo.erase(iterador++);
          mapaPorArchivo.insert(std::pair< std::string, RegistroArchivo >(nombreArchivo, registro));
        } else {
          // Ambos archivos son copias exactas.
          //std::cout << "    Archivo sin cambios: " << nombreArchivo << std::endl;
          ++iterador;
        }
      }
      ++iteradorNuevo;
    }
    while (iterador != mapaPorArchivo.end()) {
      // Termino de procesar los archivos viejos con nombres de archivo mayores al último nuevo nombre de archivo hallado.
      //std::cout << "    Archivo borrado: " << iterador->first << std::endl;
      Pedido pedido(std::string(BORRAR_ARCHIVO), iterador->first, iterador->second);
      colaPedidos.push(pedido);
      mapaPorArchivo.erase(iterador++);
    }
    //std::cout << "    ***FIN DEL APAREO." << std::endl;
    if (serializarColaPedidosYArchivos(colaPedidos, miSocket) == true) {
      // El servidor responderá a este cliente con un mensaje de sincronizacion con 0 tareas que realizar, indicando que no ha habido conflictos para actualizar, o el cliente recibirá los paquetes demorados que provocarán los conflictos a solucionar.
      informarSincronizacionOK();
    }
    mutex.liberar();
    //std::cout << "  ***FIN DE LA SINCRONIZACIÓN." << std::endl;
  }

  // El hilo que la llama debe antes tomar el mutex del "Organizador".
  void Organizador::servidorEnviarRegistrosIniciales(const MiSocket &miSocket) {
    //std::cout << "Organizador::servidorEnviarRegistrosIniciales():" << std::endl;
    ::syslog(LOG_USER | LOG_INFO, "Ejecutando Organizador::servidorEnviarRegistrosIniciales().");
    unsigned int cantidad = mapaPorArchivo.size();
    //std::cout << "  ***Comienzo de envío de registros." << std::endl;
    miSocket.enviarCompleto(&cantidad, sizeof cantidad);  // TODO Punto donde se puede enviar un paquete de datos más largo (para menos HMACs).
    //std::cout << "  Enviado cantidad de archivos en el servidor: " << cantidad << std::endl;
    std::map< std::string, RegistroArchivo >::const_iterator iterador = mapaPorArchivo.begin();
    while (iterador != mapaPorArchivo.end()) {
      miSocket.enviarCompleto(iterador->first);
      //std::cout << " ENVIADO NOMBRE DE ARCHIVO: " << iterador->first << std::endl;
      iterador->second.serializar(miSocket);
      ++iterador;
    }
    //std::cout << "  ***Fín de envío de registros." << std::endl;
  }

  // El hilo que la llama debe antes tomar el mutex del "Organizador".
  void Organizador::servidorRecibirConfirmacionOPeticiones(const MiSocket &miSocket) {
    //std::cout << "Organizador::servidorRecibirConfirmacionOPeticiones():" << std::endl;
    ::syslog(LOG_USER | LOG_INFO, "Ejecutando Organizador::servidorRecibirConfirmacionOPeticiones().");
    // La confirmación es un "unsigned int" inicial que vale cero.
    // Las peticiones son el nombre del archivo requerido.
    unsigned int nroPeticiones;
    //std::cout << "  ***INICIO DE RECEPCIÓN DE PETICIONES DE ARCHIVO." << std::endl;
    miSocket.recibirEspecifico(&nroPeticiones, sizeof nroPeticiones);
    //std::cout << "  Recibido cantidad de peticiones de archivo: " << nroPeticiones << std::endl;
    while (nroPeticiones > 0) {
      //std::cout << "    ***Inicio de recepción petición de archivo." << std::endl;
      std::string nombre;
      miSocket.recibirCompleto(nombre);
      //std::cout << "      Recibido nombre de archivo peticionado: " << nombre << std::endl;
      std::map< std::string, RegistroArchivo >::const_iterator iterador = mapaPorArchivo.find(nombre);
      if (iterador == mapaPorArchivo.end()) {
        //std::cout << "     ***EL ARCHIVO PETICIONADO NO EXISTE." << std::endl;
        throw OrganizadorExcepcion();
      }
      RegistroArchivo registroPedido(iterador->second);
      miSocket.enviarCompleto(std::string(iterador->first));
      //std::cout << "      Enviado nombre de archivo: " << std::string(iterador->first) << std::endl;
      registroPedido.serializar(miSocket);
      std::string archivoRuta(directorioRuta + iterador->first);
      std::ifstream archivoHandler(archivoRuta.c_str(), std::ifstream::in);
      //std::cout << "      ***INICIO DE ENVIAR ARCHIVO: " << archivoRuta << std::endl;
      miSocket.enviarCompleto(archivoHandler);
      //std::cout << "      ***FÍN DE ENVIAR ARCHIVO: " << archivoRuta << std::endl;
      archivoHandler.close();
      //std::cout << "    ***Fín de recepción petición de archivo." << std::endl;
      --nroPeticiones;
    }
    //std::cout << "  ***FÍN DE RECEPCIÓN DE PETICIONES DE ARCHIVO." << std::endl;
  }

  bool Organizador::servidorModificarArchivo(const Pedido &pedidoRecibido, std::queue< Pedido > &colaPedidosSincronizacion, const MiSocket &miSocket) {
    //std::cout << "    Organizador::servidorModificarArchivo():" << std::endl;
    const std::string &nombre = pedidoRecibido.getNombreArchivo();
    const RegistroArchivo &registro = pedidoRecibido.getRegistroArchivoNuevo();
    std::map< std::string, RegistroArchivo >::iterator iterador = mapaPorArchivo.find(nombre);
    if (iterador != mapaPorArchivo.end()) {
      if (iterador->second.corresponde(pedidoRecibido.getRegistroArchivoViejo())) {
        const std::string archivoRuta(directorioRuta + nombre);
        {
          //std::cout << "      Borrar el archivo: " << nombre << std::endl;
          ::remove(archivoRuta.c_str());
          mapaPorArchivo.erase(iterador);
          std::ofstream archivoHandler(archivoRuta.c_str(), std::ofstream::out);
          //std::cout << "      ***INICIO DE RECIBIR ARCHIVO: " << archivoRuta << std::endl;
          miSocket.recibirCompleto(archivoHandler);
          //std::cout << "      ***FIN DE RECIBIR ARCHIVO." << std::endl;
          archivoHandler.close();
        }
        struct stat info;
        if (::stat(archivoRuta.c_str(), &info) == -1) {
          ::perror("stat()");
          throw OrganizadorExcepcion();
        }
        if (info.st_size != registro.getTamanyo()) {
          //std::cout << "      ***EL TAMAÑO NO COINCIDE." << std::endl;
          throw OrganizadorExcepcion();
        }
        const struct utimbuf times = { info.st_atime, registro.getFecha() };
        if (::utime(archivoRuta.c_str(), &times) == -1) {
          ::perror("utime()");
          throw OrganizadorExcepcion();
        }
        // TODO (Iván): verificar los hashes.
        //std::cout << "      Crear el archivo: " << nombre << std::endl;
        mapaPorArchivo.insert(std::pair< std::string, RegistroArchivo >(nombre, registro));
        colaPedidosSincronizacion.push(pedidoRecibido);
        return true;
      }
    }
    miSocket.recibirCompleto();
    //std::cout << "      ***CONFLICTO DE ACTUALIZACIÓN." << std::endl;
    // Hubo un conflicto de sincronización.
    // Específicamente, el archivo a partir del cual fue generado el pedido de modificación fue modificado o borrado en el servidor (se supone que por algún otro cliente).
    // Al cliente ya le va a llegar el pedido de sincronización correspondiente que le va a provocar un conflicto, no hace falta hacer nada acá.
    return false;
  }

  bool Organizador::servidorCrearArchivo(const Pedido &pedidoRecibido, std::queue< Pedido > &colaPedidosSincronizacion, const MiSocket &miSocket) {
    //std::cout << "    Organizador::servidorCrearArchivo():" << std::endl;
    const std::string &nombre = pedidoRecibido.getNombreArchivo();
    const RegistroArchivo &registro = pedidoRecibido.getRegistroArchivoNuevo();
    std::map< std::string, RegistroArchivo >::const_iterator iterador = mapaPorArchivo.find(nombre);
    if (iterador == mapaPorArchivo.end()) {
      const std::string archivoRuta(directorioRuta + nombre);
      std::ofstream archivoHandler(archivoRuta.c_str(), std::ofstream::out);
      //std::cout << "      ***INICIO DE RECIBIR ARCHIVO: " << archivoRuta << std::endl;
      miSocket.recibirCompleto(archivoHandler);
      //std::cout << "      ***FIN DE RECIBIR ARCHIVO." << std::endl;
      archivoHandler.close();
      struct stat info;
      if (::stat(archivoRuta.c_str(), &info) == -1) {
        ::perror("stat()");
        throw OrganizadorExcepcion();
      }
      if (info.st_size != registro.getTamanyo()) {
        //std::cout << "      ***EL TAMAÑO NO COINCIDE" << std::endl;
        throw OrganizadorExcepcion();
      }
      const struct utimbuf times = { info.st_atime, registro.getFecha() };
      if (::utime(archivoRuta.c_str(), &times) == -1) {
        ::perror("utime()");
        throw OrganizadorExcepcion();
      }
      // TODO (Iván): verificar los hashes.
      //std::cout << "      Crear el archivo: " << nombre << std::endl;
      mapaPorArchivo.insert(std::pair< std::string, RegistroArchivo >(nombre, registro));
      colaPedidosSincronizacion.push(pedidoRecibido);
      return true;
    } else {
      miSocket.recibirCompleto();
      //std::cout << "      ***CONFLICTO DE ACTUALIZACIÓN." << std::endl;
      // Hubo un conflicto de sincronización.
      // Específicamente, en el servidor ya existe un archivo con el nombre del archivo que el cliente pidió crear (se supone que fue creado por algún otro cliente).
      // Al cliente ya le va a llegar el pedido de sincronización correspondiente que le va a provocar un conflicto, no hace falta hacer nada acá.
      return false;
    }
  }

  bool Organizador::servidorBorrarArchivo(const Pedido &pedidoRecibido, std::queue< Pedido > &colaPedidosSincronizacion) {
    //std::cout << "    Organizador::servidorBorrarArchivo():" << std::endl;
    const std::string &nombre = pedidoRecibido.getNombreArchivo();
    const RegistroArchivo &registro = pedidoRecibido.getRegistroArchivoNuevo();
    const std::string archivoRuta(directorioRuta + nombre);
    std::map< std::string, RegistroArchivo >::iterator iterador = mapaPorArchivo.find(nombre);
    if (iterador != mapaPorArchivo.end()) {
      if (iterador->second.corresponde(registro)) {
        //std::cout << "      Borrar el archivo: " << nombre << std::endl;
        ::remove(archivoRuta.c_str());
        mapaPorArchivo.erase(iterador);
        colaPedidosSincronizacion.push(pedidoRecibido);
        return true;
      }
    }
    //std::cout << "      ***CONFLICTO DE ACTUALIZACIÓN." << std::endl;
    // Hubo un conflicto de sincronización.
    // Específicamente, el archivo a partir del cual fue generado el pedido de envío fue borrado en el servidor (se supone que por algún otro cliente).
    // Al cliente ya le va a llegar el pedido de sincronización correspondiente que le va a provocar un conflicto, no hace falta hacer nada acá.
    return false;
  }

  void Organizador::servidorEnviarArchivo(const Pedido &pedidoRecibido, std::queue< Pedido > &colaPedidosArchivosCompletos) {
    //std::cout << "Organizador::servidorEnviarArchivo():" << std::endl;
    const std::string &nombre = pedidoRecibido.getNombreArchivo();
    std::map< std::string, RegistroArchivo >::const_iterator iterador = mapaPorArchivo.find(nombre);
    if (iterador != mapaPorArchivo.end()) {
      // No es necesaria la comprobación de información suplementaria.
      colaPedidosArchivosCompletos.push(Pedido(pedidoRecibido.getComando(), nombre, iterador->second));
    } else {
      //std::cout << "      ***CONFLICTO DE ACTUALIZACIÓN." << std::endl;
      // Hubo un conflicto de sincronización.
      // Específicamente, el archivo fue borrado en el servidor (se supone que por algún otro cliente).
      // Al cliente ya le va a llegar el pedido de sincronización correspondiente que le va a provocar un conflicto, no hace falta hacer nada acá.
    }
  }

  void Organizador::serializarColaPedidosArchivosCompletos(std::queue< Pedido > &colaPedidosArchivosCompletos, const MiSocket &miSocket) const {
    //std::cout << "  Organizador::serializarColaPedidosArchivosCompletos():" << std::endl;
    // Manda la cantidad de archivos completos que va a enviar, y luego los pedidos y archivos, uno por uno.
    unsigned int cantidad = colaPedidosArchivosCompletos.size();
    //std::cout << "  ***INICIO DE ENVÍO DE PEDIDOS DE ARCHIVO COMPLETO." << std::endl;
    miSocket.enviarCompleto(&cantidad, sizeof cantidad);
    //std::cout << "  Enviado cantidad de pedidos de archivo completo: " << cantidad << std::endl;
    while (colaPedidosArchivosCompletos.empty() == false) {
      Pedido pedido(colaPedidosArchivosCompletos.front());
      pedido.serializar(miSocket);
      //std::cout << "    ***Enviado pedido de archivo completo." << std::endl;
      std::string archivoRuta(directorioRuta + pedido.getNombreArchivo());
      std::ifstream archivoHandler(archivoRuta.c_str(), std::ifstream::in);
      //std::cout << "    ***Inicio envío archivo correspondiente a pedido para sincronización: " << pedido.getNombreArchivo() << std::endl;
      miSocket.enviarCompleto(archivoHandler);
      //std::cout << "    ***Fín envío archivo correspondiente a pedido para sincronización: " << pedido.getNombreArchivo() << std::endl;
      archivoHandler.close();
      colaPedidosArchivosCompletos.pop();
    }
    //std::cout << "  ***FÍN DE ENVÍO DE PEDIDOS DE ARCHIVO COMPLETO." << std::endl;
  }

  // Toma el mutex del "Organizador" al momento de empezar a hacer cambios.
  void Organizador::servidorRecibirActualizaciones(const MiSocket &miSocket, const AdmUsuarios &admUsuarios, const std::string &nombre, Atendedor * const atendedor) {
    //std::cout << "Organizador::servidorRecibirActualizaciones():" << std::endl;
    ::syslog(LOG_USER | LOG_INFO, "Ejecutando Organizador::servidorRecibirActualizaciones().");
    unsigned int nroPedidos;
    //std::cout << "***INICIO DE RECEPCIÓN DE PEDIDOS DE ACTUALIZACIÓN." << std::endl;
    miSocket.recibirEspecifico(&nroPedidos, sizeof nroPedidos);
    //std::cout << "  Recibido cantidad de pedidos de actualización: " << nroPedidos << std::endl;
    if (nroPedidos == 0) {
      // Si se reciben cero pedidos, quiere decir que el pedido de sincronización que se envió a este cliente en el paso anterior se pudo procesar correctamente, con lo cual se responde con un mensaje de cero pedidos.
      miSocket.enviarCompleto(&nroPedidos, sizeof nroPedidos);
      //std::cout << "  Enviado cantidad de pedidos de actualización: " << nroPedidos << std::endl;
      // La idea es: paquete de sincronización - resolución de conflictos - paquete de sincronización - resolución de conflictos - paquete de sincronización - resolución de conflictos - ... y así, alternadamente siempre uno y uno. Así, cada vez que sale un conflicto, este se resuelve en la etapa siguiente, y para la próxima sincronización, ya no existen más esos conflictos. Las instancias de "Atendedor" tienen un sincronizador, sobre el que el "enviador" de paquetes de sincronización hace un "esperar()", cuando lo logra conseguir, envía un paquete y vuelve a hacer un "esperar()". Este método, cuando recibe un "nroPedidos" = 0, o cuando recibe una serie de pedidos, todos de archivos completos (indicio de una resolución de conflictos en el lado del cliente), hace un "senyalar()" a la instancia de "Atendedor".
      atendedor->senyalar();
      //std::cout << "***FÍN DE RECEPCIÓN DE PEDIDOS DE ACTUALIZACIÓN." << std::endl;
      return;
    }
    // Toma el mutex del "Organizador" para que si otro cliente de este mismo usuario manda un paquete de sincronización, el "Atendedor" de ese cliente no lo procese hasta que este paquete se haya terminado de procesar.
    mutex.tomar();
    bool sinConflictos = true;
    bool pedidoParaResolverConflictos = true;
    std::queue< Pedido > colaPedidosSincronizacion;
    std::queue< Pedido > colaPedidosArchivosCompletos;
    while (nroPedidos > 0) {
      //std::cout << "  ***Inicio de recepción de pedido de actualización." << std::endl;
      const Pedido pedido(miSocket);
      const std::string &comando = pedido.getComando();
      //std::cout << "    Comando de pedido de actualización recibido es: " << comando << std::endl;
      if (comando == std::string(MODIFICAR_ARCHIVO)) {
        pedidoParaResolverConflictos = false;
        sinConflictos = servidorModificarArchivo(pedido, colaPedidosSincronizacion, miSocket);
      } else {
        if (comando == std::string(CREAR_ARCHIVO)) {
          pedidoParaResolverConflictos = false;
          sinConflictos = servidorCrearArchivo(pedido, colaPedidosSincronizacion, miSocket);
        } else {
          if (comando == std::string(BORRAR_ARCHIVO)) {
            pedidoParaResolverConflictos = false;
            sinConflictos = servidorBorrarArchivo(pedido, colaPedidosSincronizacion);
          } else {
            if (comando == std::string(ENVIAR_ARCHIVO_MODIFICAR)) {
              servidorEnviarArchivo(pedido, colaPedidosArchivosCompletos);
            } else {
              //std::cout << "    ***Comando de pedido de actualización recibido es: OPERACIÓN NO VÁLIDA." << comando << std::endl;
              mutex.tomar();
              throw OrganizadorExcepcion();
            }
          }
        }
      }
      --nroPedidos;
      //std::cout << "  ***Fín de recepción de pedido de actualización." << std::endl;
    }
    if (sinConflictos == true) {
      unsigned int cantidad = 0;
      miSocket.enviarCompleto(&cantidad, sizeof cantidad);
      //std::cout << "  Enviado cantidad de pedidos de actualización: " << nroPedidos << std::endl;
    }
    if (pedidoParaResolverConflictos == true) {
      serializarColaPedidosArchivosCompletos(colaPedidosArchivosCompletos, miSocket);
      atendedor->senyalar();
    } else {
      if (colaPedidosSincronizacion.empty() == false) {
        const std::set< Atendedor * > &setAtendedores = admUsuarios.getAtendedoresRef(nombre);
        std::set< Atendedor * >::const_iterator iterador = setAtendedores.begin();
        while (iterador != setAtendedores.end()) {
          if (*iterador != atendedor) {
            //std::cout << "  Informar atendedor: " << *iterador << std::endl;
            (*iterador)->agregarPedidos(colaPedidosSincronizacion);
          }
          ++iterador;
        }
      }
    }
    //std::cout << "***FÍN DE RECEPCIÓN DE PEDIDOS DE ACTUALIZACIÓN." << std::endl;
    mutex.liberar();
  }

  void Organizador::servidorRepetirEnvios(std::queue< Pedido > &colaPedidos, const MiSocket &miSocket) {
    //std::cout << "Organizador::servidorRepetirEnvios():" << std::endl;
    mutex.tomar();
    serializarColaPedidosYArchivos(colaPedidos, miSocket);
    mutex.liberar();
  }

  unsigned int Organizador::getTamanyo() {
    //std::cout << "Organizador::getTamanyo():" << std::endl;
    mutex.tomar();
    unsigned int tamanyo = 0;
    std::map< std::string, RegistroArchivo >::const_iterator iterador = mapaPorArchivo.begin();
    while (iterador != mapaPorArchivo.end()) {
      tamanyo += iterador->second.getTamanyo();
      ++iterador;
    }
    mutex.liberar();
    return tamanyo;
  }

//**************************************************EXCEPCIONES**************************************************//

#define TIPO_DE_EXCEPCION OrganizadorExcepcion
  TIPO_DE_EXCEPCION ::~TIPO_DE_EXCEPCION() throw () {
  }
  const char *TIPO_DE_EXCEPCION::what() const throw () {
    return "OrganizadorExcepcion";
  }
#undef TIPO_DE_EXCEPCION

} /* namespace ubicuos */
