/*
 * BaseAccount.h
 *
 *  Created on: 09/06/2013
 *      Author: Pablito
 */

#ifndef BASEACCOUNT_H_
#define BASEACCOUNT_H_

#include <gtkmm.h>
#include <glibmm.h>
#include <glibmm/exception.h>
#include <iostream>
#include "./../common/UserFile.h"

using configuration::UserFile;

namespace visual {

class BaseAccount {
public:
	BaseAccount();
	virtual ~BaseAccount();
	void load();
	virtual void assign();
	void show();
	void cancel_button_clicked();
	void on_pass_visibility_toggled();
	void on_newpass_visibility_toggled();

protected:
	Glib::RefPtr<Gtk::Builder> refBuilder;
	Gtk::Entry* user;
	Gtk::Entry* pass;
	Gtk::Entry* newPass;
	Gtk::Button* ok;
	Gtk::Button* cancel;
	Gtk::CheckButton* checkPass;
	Gtk::CheckButton* checkNew;
	Gtk::Window* account;
	std::string path;
};

} /* namespace visual */
#endif /* BASEACCOUNT_H_ */
