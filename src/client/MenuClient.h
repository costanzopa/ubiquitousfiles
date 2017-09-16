/*
 * MenuClient.h
 *
 *  Created on: 05/06/2013
 *      Author: Pablito
 */

#ifndef MENUCLIENT_H_
#define MENUCLIENT_H_
#include <gtkmm.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <glibmm.h>
#include <glibmm/exception.h>
#include <sigc++/connection.h>
#include "./../common/Preferences.h"


namespace visual {

class MenuClient : public Gtk::Dialog {
public:
	MenuClient(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade);
	virtual ~MenuClient();
	void assign();
	void config_button_clicked();
	void ok_button_clicked();
	void cancel_button_clicked();


private:
	Gtk::Button* ok;
	Gtk::Button* cancel;
	Gtk::Button* config;
	Preferences* preferences;
	Glib::RefPtr<Gtk::Builder> refBuilder;
};
}
#endif /* MENUCLIENT_H_ */

