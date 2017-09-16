/*
 * Accounts.h
 *
 *  Created on: 18/06/2013
 *      Author: Pablito
 */

#ifndef ACCOUNTS_H_
#define ACCOUNTS_H_
#include <gtkmm.h>
#include "./NewAccount.h"
#include "./ChangeAccount.h"
#include "./DeleteAccount.h"
#include "./../common/UserFile.h"
#include "./../common/ConfigFile.h"
using namespace configuration;

namespace visual {
class Accounts : public Gtk::Window {
public:
	Accounts();
	virtual ~Accounts();
	void load();
	void assign();
	void show();
	void create_button_clicked();
	void delete_button_clicked();
	void change_button_clicked();
private:
	Gtk::Button* create;
	Gtk::Button* change;
	Gtk::Button* del;
	Gtk::Window* accounts;
	NewAccount* newAccount;
	ChangeAccount* changeAccount;
	Glib::RefPtr<Gtk::Builder> refBuilder;
	DeleteAccount* deleteAccount;
};

} /* namespace visual */
#endif /* ACCOUNTS_H_ */
