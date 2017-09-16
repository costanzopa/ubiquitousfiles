/*
 * Accounts.cpp
 *
 *  Created on: 18/06/2013
 *      Author: Pablito
 */

#include "./Accounts.h"

namespace visual {

Accounts::Accounts() {
	this->create = 0;
	this->del = 0;
	this->change = 0;
	this->accounts = 0;
	std::string path ="../configserver.cfg";
	ConfigFile* config = new ConfigFile(path);
	config->read();
	std::string route = config->getDirectory();
	this->newAccount = new NewAccount(route);
	this->changeAccount = new ChangeAccount();
	this->deleteAccount = new DeleteAccount(route);
	refBuilder = Gtk::Builder::create();
	delete config;
}

Accounts::~Accounts() {
	delete this->newAccount;
	delete this->changeAccount;
}

void Accounts::load() {
	try {
		refBuilder->add_from_file("glade/Accounts.glade");
	}
	catch(const Glib::FileError& ex) {
		     std::cerr << "FileError: " << ex.what() << std::endl;
	}
	catch(const Glib::MarkupError& ex) {
		    std::cerr << "MarkupError: " << ex.what() << std::endl;
	}
	catch(const Gtk::BuilderError& ex) {
	        std::cerr << "BuilderError: " << ex.what() << std::endl;
	}
	refBuilder->get_widget("wAccounts", this->accounts);
}

void Accounts::assign() {
	refBuilder->get_widget("create", create);
	refBuilder->get_widget("delete", del);
	refBuilder->get_widget("change", change);
	create->signal_clicked().connect( sigc::mem_fun(*this, &Accounts::create_button_clicked));
	del->signal_clicked().connect( sigc::mem_fun(*this, &Accounts::delete_button_clicked));
	change->signal_clicked().connect( sigc::mem_fun(*this, &Accounts::change_button_clicked));
}

void Accounts::show() {
	this->load();
	this->assign();
	this->accounts->show_all();
}

void Accounts::create_button_clicked() {
	this->newAccount->show();
}

void Accounts::delete_button_clicked() {
	this->deleteAccount->show();
}

void Accounts::change_button_clicked() {
	this->changeAccount->show();
}

} /* namespace visual */
