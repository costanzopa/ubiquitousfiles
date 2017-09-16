/*
 * NewAccount.h
 *
 *  Created on: 09/06/2013
 *      Author: Pablito
 */

#ifndef NEWACCOUNT_H_
#define NEWACCOUNT_H_
#include "./BaseAccount.h"
#include "./../common/UserFile.h"
#include <sys/types.h>
#include <sys/stat.h>

namespace visual {

class NewAccount : public BaseAccount {
public:
	NewAccount(std::string& path);
	virtual ~NewAccount();
	virtual void assign();
	virtual void create_button_clicked();
private:
	Gtk::Label* label;
	Gtk::Button* create;
	std::string route;
};

} /* namespace visual */
#endif /* NEWACCOUNT_H_ */

