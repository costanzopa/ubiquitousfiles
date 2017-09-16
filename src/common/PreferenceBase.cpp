/*
 * PreferenceBase.cpp
 *
 *  Created on: 17/06/2013
 *      Author: Pablito
 */

#include "./PreferenceBase.h"

namespace visual {

PreferenceBase::PreferenceBase(std::string& path) {
	this->path = path;
	this->save = 0;
	this->cancel = 0;
	this->directory = 0;
	this->port = 0;
	this->preferences = 0;
	this->refBuilder = Gtk::Builder::create();
	this->file = new ConfigFile(path);

}

PreferenceBase::~PreferenceBase() {
	delete file;
	delete preferences;
	delete directory;
	delete cancel;
	delete save;
	delete port;
}

void PreferenceBase::load() {
	try {
			refBuilder->add_from_file("glade/Preferences.glade");
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
		refBuilder->get_widget("wConfig", this->preferences);
}

void PreferenceBase::assign() {
	refBuilder->get_widget("save", save);
	refBuilder->get_widget("cancel", cancel);
	refBuilder->get_widget("directory", directory);
	refBuilder->get_widget("port", port);
	this->fill();
	cancel->signal_clicked().connect( sigc::mem_fun(*this, &PreferenceBase::cancel_button_clicked));
	save->signal_clicked().connect( sigc::mem_fun(*this, &PreferenceBase::save_button_clicked));
}

void PreferenceBase::fill() {
	file->read();
	directory->set_text(file->getDirectory());
	std::ostringstream sPort;
	sPort <<std::dec<< file->getPort();
	std::string ssPort;
	ssPort= sPort.str();
	this->port->set_text(ssPort);
}

void PreferenceBase::cancel_button_clicked() {
	this->preferences->hide();
}

void PreferenceBase::save_button_clicked() {
	Glib::ustring udir = this->directory->get_text();
	Glib::ustring uport = this->port->get_text();
	std::string ndir = udir.raw();
	std::string nport = uport.raw();
	int iport;
	std::istringstream(nport) >> std::dec >> iport;
	this->file->update(ndir, iport);
	this->preferences->hide();
}

void PreferenceBase::show() {
	this->load();
	this->assign();
	this->preferences->show_all();
}



} /* namespace visual */
