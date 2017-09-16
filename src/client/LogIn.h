/* LogIn.h
 *
 *  Created on: 01/06/2013
 *      Author: Pablito
 */

#ifndef LOGIN_H_
#define LOGIN_H_

#include <gtkmm.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <glibmm.h>
#include <glibmm/exception.h>
#include <cstdlib>  /* Función atoi. */
#include <exception>
#include <iostream>
#include <string>
#include "./Cliente.h"
#include "./MenuClient.h"
#include "./../common/ValoresRetorno.h"
#include "./../common/ConfigFile.h"

namespace visual {
class LogIn : public Gtk::Dialog {
public:
	LogIn(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade);
	virtual ~LogIn();
	void load();
	void assign();
	void show();
	void ok_button_clicked();
	void exit_button_clicked();
	void account_button_clicked();
	void on_checkbox_visibility_toggled();
	bool isUser();
	bool getAccept();
	void exit();
	bool getCancel();

private:
	Gtk::Button* ok;
	Gtk::Button* cancel;
	Gtk::Entry*  user;
	Gtk::Entry*  pass;
	Gtk::Label* label;
	Gtk::CheckButton* visible;
	Glib::RefPtr<Gtk::Builder> refBuilder;
	ubicuos::Cliente* cliente;
	bool pAccept;
	bool pCancel;
};

} /* namespace visual */
#endif /* LOGIN_H_ */
