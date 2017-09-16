/*
 * BaseAccount.cpp
 *
 *  Created on: 09/06/2013
 *      Author: Pablito
 */

#include "./BaseAccount.h"

namespace visual {

BaseAccount::BaseAccount() {
	this->refBuilder = Gtk::Builder::create();
	this->account = 0;
	this->cancel = 0;
	this->newPass = 0;
	this->ok = 0;
	this->user = 0;
	this->pass = 0;
	this->checkNew = 0;
	this->checkPass = 0;
}

BaseAccount::~BaseAccount() {
	delete account;
}

void BaseAccount::load() {
	try {
		refBuilder->add_from_file(path);
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
	refBuilder->get_widget("wAccount", this->account);
}

void BaseAccount::assign() {
	refBuilder->get_widget("cancel", cancel);
	refBuilder->get_widget("user", user);
	refBuilder->get_widget("pass", pass);
	refBuilder->get_widget("newPass", newPass);
	refBuilder->get_widget("checkPass", checkPass);
	refBuilder->get_widget("checkNew", checkNew);
	checkPass->signal_toggled().connect( sigc::mem_fun(*this,
							              &BaseAccount::on_pass_visibility_toggled) );
	checkPass->set_active(false);
	pass->set_visibility(checkPass->get_active());
	checkNew->signal_toggled().connect( sigc::mem_fun(*this,
									              &BaseAccount::on_newpass_visibility_toggled) );
	checkNew->set_active(false);
	newPass->set_visibility(checkNew->get_active());
	cancel->signal_clicked().connect( sigc::mem_fun(*this, &BaseAccount::cancel_button_clicked));
}

void BaseAccount::show() {
	this->load();
	this->assign();
	this->account->show_all();
}

void BaseAccount::cancel_button_clicked() {
	this->account->hide();
}

void BaseAccount::on_pass_visibility_toggled() {
	pass->set_visibility(checkPass->get_active());
}

void BaseAccount::on_newpass_visibility_toggled() {
	newPass->set_visibility(checkNew->get_active());
}

} /* namespace visual */
