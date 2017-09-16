/*
 * LogIn.cpp
 *
 *  Created on: 01/06/2013
 *      Author: Pablito
 */

#include "./LogIn.h"
#include <iostream>

using namespace configuration;

namespace visual {

  LogIn::LogIn(BaseObjectType* cobject, const Glib::RefPtr< Gtk::Builder >& refGlade)
      : Gtk::Dialog(cobject), refBuilder(refGlade) {
    refBuilder->get_widget("exit", cancel);
    refBuilder->get_widget("ok", ok);
    refBuilder->get_widget("ClientLogIn", label);
    refBuilder->get_widget("user", user);
    refBuilder->get_widget("pass", pass);
    refBuilder->get_widget("See", visible);
    visible->signal_toggled().connect(sigc::mem_fun(*this, &LogIn::on_checkbox_visibility_toggled));
    visible->set_active(false);
    pass->set_visibility(visible->get_active());
    cancel->signal_clicked().connect(sigc::mem_fun(*this, &LogIn::exit_button_clicked));
    ok->signal_clicked().connect(sigc::mem_fun(*this, &LogIn::ok_button_clicked));
    this->pAccept = false;
    this->pCancel = false;
    this->cliente = 0;
  }

  LogIn::~LogIn() {
    if (this->cliente != 0) {
      delete cliente;
    }
  }

  void LogIn::ok_button_clicked() {
    int returnValue;
    Glib::ustring aUser = this->user->get_text();
    Glib::ustring aPass = this->pass->get_text();
    std::string nUser = aUser.raw();
    std::string nPass = aPass.raw();
    std::string path = "ConfigFile.cfg";
    ConfigFile config(path);
    config.read();
    std::string hostDireccion = config.getIp();
    std::string hostPuerto = config.getStringPort();
    std::string directorioRuta = config.getDirectory();
    const unsigned int intervalo = config.getPolling();
    try {
      cliente = new ubicuos::Cliente(hostDireccion, hostPuerto, directorioRuta, intervalo, nUser, nPass);
      if (this->isUser()) {
        label->set_text("Bienvenido " + nUser);
        cliente->start();
      } else {
        label->set_text("Incorrect User or password");
        try {
          cliente->pedirDetener();
          cliente->close((void **) &returnValue);
          //std::cout << "Fin controlado de programa. ECHO $? = " << returnValue << std::endl;
        } catch (const std::exception &excepcion) {
          //std::cerr << "error en main() al cerrar: " << excepcion.what() << std::endl;
          returnValue = ERROR_EXCEPCION;
          //std::cout << "Fin controlado de programa. ECHO $? = " << returnValue << std::endl;
        }
      }
    } catch (const std::exception &excepcion) {
      //std::cerr << "error en main() al iniciar: " << excepcion.what() << std::endl;
      returnValue = ERROR_EXCEPCION;
      //std::cout << "Fin controlado de programa. ECHO $? = " << returnValue << std::endl;
    }
    this->hide();
  }

  void LogIn::exit_button_clicked() {
    int returnValue;
    if (cliente != 0) {
      try {
        cliente->pedirDetener();
        cliente->close((void **) &returnValue);
        delete cliente;
        //std::cout << "Fin controlado de programa. ECHO $? = " << returnValue << std::endl;
      } catch (const std::exception &excepcion) {
        //std::cerr << "error en main() al cerrar: " << excepcion.what() << std::endl;
        returnValue = ERROR_EXCEPCION;
        //std::cout << "Fin controlado de programa. ECHO $? = " << returnValue << std::endl;
      }
    }
    hide();
    pCancel = true;
  }

  void LogIn::on_checkbox_visibility_toggled() {
    pass->set_visibility(visible->get_active());
  }

  bool LogIn::getAccept() {
    return pAccept;
  }

  bool LogIn::getCancel() {
    return pCancel;
  }

  void LogIn::exit() {
    hide();
    this->pAccept = false;
  }

  bool LogIn::isUser() {
    this->pAccept = this->cliente->registrar();
    return this->pAccept;
  }

}

