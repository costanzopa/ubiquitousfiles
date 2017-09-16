/*
 * MenuMonitor.cpp
 *
 *  Created on: 11/06/2013
 *      Author: Pablito
 */

#include "./MenuMonitor.h"
#include "./../common/DataFile.h"
using configuration::DataFile;
namespace visual {

MenuMonitor::MenuMonitor(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade)
:Gtk::Window(cobject),refBuilder(refGlade) {
	this->account = 0;
	this->exit = 0;
	this->config = 0;
	this->graphic = 0;
	std::string path = "../configserver.cfg";
	this->preferences = new PreferenceBase(path);
	this->graphicWin = new GraphicWindow();
	this->accounts = new Accounts();
	this->assign();
	this->getData();
}

MenuMonitor::~MenuMonitor() {
		delete preferences;
		delete graphicWin;
		delete accounts;
		delete exit;
		delete config;
		delete account;
		delete graphic;
}

void MenuMonitor::assign() {
	refBuilder->get_widget("exit", exit);
	refBuilder->get_widget("preferences", config);
	refBuilder->get_widget("graphic", graphic);
	refBuilder->get_widget("accounts", account);
	exit->signal_clicked().connect( sigc::mem_fun(*this, &MenuMonitor::exit_button_clicked));
	graphic->signal_clicked().connect( sigc::mem_fun(*this, &MenuMonitor::graph_button_clicked));
	config->signal_clicked().connect( sigc::mem_fun(*this, &MenuMonitor::config_button_clicked));
	account->signal_clicked().connect( sigc::mem_fun(*this, &MenuMonitor::account_button_clicked));
}


void MenuMonitor::config_button_clicked() {
		this->preferences->show();
}

void MenuMonitor::exit_button_clicked() {
		this->hide();
}

void MenuMonitor::account_button_clicked() {
	this->accounts->show();
}

void MenuMonitor::graph_button_clicked() {
	this->graphicWin->show_all();
}

void MenuMonitor::getData() {
	std::vector<double> bytes, time;
	DataFile data;
	data.getData();
	bytes = data.getBytes();
	time = data.getTime();
	this->graphicWin->show(bytes, time);
}

} /* namespace visual */
