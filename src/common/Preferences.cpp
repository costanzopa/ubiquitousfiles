/*
 * Preferences.cpp
 *
 *  Created on: 07/06/2013
 *      Author: Pablito
 */

#include "./Preferences.h"

namespace visual {

Preferences::Preferences(std::string& path): PreferenceBase(path) {
	this->polling = 0;
	this->ip = 0;
}

Preferences::~Preferences() {
	delete polling;
}

void Preferences::load() {
	try {
			refBuilder->add_from_file("../client/glade/Preferences.glade");
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

void Preferences::assign() {
	refBuilder->get_widget("save", save);
	refBuilder->get_widget("cancel", cancel);
	refBuilder->get_widget("directory", directory);
	refBuilder->get_widget("IpServer", ip);
	refBuilder->get_widget("polling", polling);
	refBuilder->get_widget("port", port);
	this->fill();
	cancel->signal_clicked().connect( sigc::mem_fun(*this, &Preferences::cancel_button_clicked));
	save->signal_clicked().connect( sigc::mem_fun(*this, &Preferences::save_button_clicked));

}


void Preferences::fill() {
	PreferenceBase::fill();
	this->ip->set_text(file->getIp());
	std::ostringstream sPoll;
	sPoll <<std::dec<< file->getPolling();
	std::string ssPolling;
	ssPolling = sPoll.str();
	this->polling->set_text(ssPolling);
}


void Preferences::save_button_clicked() {
	Glib::ustring udir = this->directory->get_text();
	Glib::ustring uip = this->ip->get_text();
	Glib::ustring uport = this->port->get_text();
	Glib::ustring upolling = this->polling->get_text();
	std::string ndir = udir.raw();
	std::string nip = uip.raw();
	std::string nport = uport.raw();
	std::string npolling = upolling.raw();
	int iport, ipolling;
	std::istringstream(nport) >> std::dec >> iport;
	std::istringstream(npolling) >> std::dec >> ipolling;
	this->file->update(ndir, nip, iport, ipolling);
	this->preferences->hide();
}


} /* namespace visual */
