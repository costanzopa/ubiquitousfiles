/*
 * MenuMonitor.h
 *
 *  Created on: 11/06/2013
 *      Author: Pablito
 */

#ifndef MENUMONITOR_H_
#define MENUMONITOR_H_

#include <gtkmm.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <glibmm.h>
#include <glibmm/exception.h>
#include <sigc++/connection.h>
#include "./GraphicWindow.h"
#include "./../common/PreferenceBase.h"
#include "./Accounts.h"
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
namespace visual {

class MenuMonitor : public Gtk::Window {
public:
	MenuMonitor(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade);
	virtual ~MenuMonitor();
	void assign();
	void config_button_clicked();
	void graph_button_clicked();
	void exit_button_clicked();
	void account_button_clicked();
	void getData();

private:
	Gtk::Button* exit;
	Gtk::Button* config;
	Gtk::Button* account;
	Gtk::Button* graphic;
	PreferenceBase* preferences;
	GraphicWindow* graphicWin;
	Accounts* accounts;
	Glib::RefPtr<Gtk::Builder> refBuilder;
};

} /* namespace visual */
#endif /* MENUMONITOR_H_ */
