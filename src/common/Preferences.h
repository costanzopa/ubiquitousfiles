#ifndef PREFERENCES_H_
#define PREFERENCES_H_
#include <gtkmm.h>
#include <iostream>
#include "./PreferenceBase.h"
#include "./ConfigFile.h"
using namespace configuration;
namespace visual {

class Preferences : public PreferenceBase  {
public:
	Preferences(std::string& route);
	virtual ~Preferences();
	virtual void load();
	virtual void assign();
	virtual void save_button_clicked();
	virtual void fill();

private:
	Gtk::Entry* polling;
	Gtk::Entry* ip;
};

} /* namespace visual */
#endif /* PREFERENCES_H_ */

