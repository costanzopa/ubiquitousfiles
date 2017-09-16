/*
 * ChangeAccount.cpp
 *
 *  Created on: 19/06/2013
 *      Author: Pablito
 */

#include "./ChangeAccount.h"

namespace visual {

ChangeAccount::ChangeAccount():BaseAccount() {
	this->path = "glade/Account.glade";
	this->label = 0;
}


ChangeAccount::~ChangeAccount() {
}


void ChangeAccount::assign() {
	BaseAccount::assign();
	refBuilder->get_widget("ok", ok);
	refBuilder->get_widget("label", label);
	ok->signal_clicked().connect( sigc::mem_fun(*this, &ChangeAccount::ok_button_clicked));
}


void ChangeAccount::ok_button_clicked() {
	bool flag = false;
	Glib::ustring aUser = this->user->get_text();
	Glib::ustring aPass = this->pass->get_text();
	Glib::ustring nePass = this->newPass->get_text();
	std::string nUser = aUser.raw();
	std::string nPass = aPass.raw();
	std::string newpass = nePass.raw();
	UserFile* fuser = new UserFile();
	if ((!nUser.empty()) && (!nPass.empty())&&(!newpass.empty())) {
				if (nPass!=newpass) {
					flag = fuser->change(nUser, nPass, newpass);
					if(flag == false)  {
						label->set_text("Wrong Password or User.");
						nUser.clear();
						nPass.clear();
					}
					else {
						label->set_text("Succesfully password changed, press cancel to exit");
					}
				}
				else {
					label->set_text("The same passwords.");
				}
			}
		delete fuser;
}


} /* namespace visual */
