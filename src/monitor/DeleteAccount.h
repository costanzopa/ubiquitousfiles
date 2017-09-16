/*
 * DeleteAccount.h
 *
 *  Created on: 19/06/2013
 *      Author: Pablito
 */

#ifndef DELETEACCOUNT_H_
#define DELETEACCOUNT_H_
#include <gtkmm.h>
#include <iostream>
#include "./../common/UserFile.h"
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

using configuration::UserFile;

namespace visual {

class DeleteAccount {
public:
	DeleteAccount(std::string& path);
	virtual ~DeleteAccount();
	void load();
    void assign();
    void show();
    bool isUser(const std::string& user);
    void ok_button_clicked();
    void exit_button_clicked();
    bool deleteDirectory(const std::string& directory);
private:
	Gtk::Entry* user;
	Gtk::Button* ok;
	Gtk::Button* exit;
	Gtk::Window* deleteAccount;
	Glib::RefPtr<Gtk::Builder> refBuilder;
	Gtk::Label* find;
	std::string route;
};

} /* namespace visual */
#endif /* DELETEACCOUNT_H_ */
