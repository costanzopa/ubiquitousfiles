/*
 * MenuClient.cpp
 *
 *  Created on: 05/06/2013
 *      Author: Pablito
 */

#include "./MenuClient.h"
#include <gtkmm.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <glibmm.h>
#include <iostream>

using namespace std;

namespace visual {

MenuClient::MenuClient(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade)
      :Gtk::Dialog(cobject),refBuilder(refGlade) {
	this->cancel = 0;
	this->config = 0;
	this->ok = 0;
	std::string path = "ConfigFile.cfg";
	this->preferences = new Preferences(path);
	this->assign();
}

MenuClient::~MenuClient() {
	delete preferences;
}

void MenuClient::assign() {
	refBuilder->get_widget("ok", ok);
	refBuilder->get_widget("cancel", cancel);
	refBuilder->get_widget("preferences", config);
	cancel->signal_clicked().connect( sigc::mem_fun(*this, &MenuClient::cancel_button_clicked));
	ok->signal_clicked().connect( sigc::mem_fun(*this, &MenuClient::ok_button_clicked));
	config->signal_clicked().connect( sigc::mem_fun(*this, &MenuClient::config_button_clicked));
}

void MenuClient::config_button_clicked() {
	this->preferences->show();
}

void MenuClient::ok_button_clicked() {
	this->hide();
}

void MenuClient::cancel_button_clicked() {
	this->hide();
}

} /* namespace visual */
