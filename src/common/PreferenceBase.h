/*
 * PreferenceBase.h
 *
 *  Created on: 17/06/2013
 *      Author: Pablito
 */

#ifndef PREFERENCEBASE_H_
#define PREFERENCEBASE_H_
#include <gtkmm.h>
#include <iostream>
#include "./ConfigFile.h"
using namespace configuration;

namespace visual {

class PreferenceBase {
public:
	PreferenceBase(std::string& path);
	virtual ~PreferenceBase();
	void load();
	virtual void assign();
	void show();
	virtual void save_button_clicked();
	void cancel_button_clicked();
	virtual void fill();

protected:
	Glib::RefPtr<Gtk::Builder> refBuilder;
	Gtk::Entry* directory;
	Gtk::Entry* port;
	Gtk::Button* save;
	Gtk::Button* cancel;
	Gtk::Window* preferences;
	ConfigFile* file;
	std::string path;
};

} /* namespace visual */
#endif /* PREFERENCEBASE_H_ */
