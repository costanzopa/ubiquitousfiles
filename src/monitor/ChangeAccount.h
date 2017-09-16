/*
 * ChangeAccount.h
 *
 *  Created on: 19/06/2013
 *      Author: Pablito
 */

#ifndef CHANGEACCOUNT_H_
#define CHANGEACCOUNT_H_
#include "./BaseAccount.h"
namespace visual {
class ChangeAccount : public BaseAccount {
public:
	ChangeAccount();
	virtual ~ChangeAccount();
	virtual void assign();
	void ok_button_clicked();
private:
	Gtk::Label* label;
};
} /* namespace visual */
#endif /* CHANGEACCOUNT_H_ */
