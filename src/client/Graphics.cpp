/*
 * Graphics.cpp
 *
 *  Created on: 06/06/2013
 *      Author: Pablito
 */

#include "./Graphics.h"
#include <iostream>

namespace visual {

Graphics::Graphics(int argc, char** argv) {
	this->logIn = 0;
	this->menu=0;
	this->app = Gtk::Application::create(argc, argv, "client.exe");
	refBuilder = Gtk::Builder::create();
}

Graphics::~Graphics() {
	delete logIn;
	delete menu;
}

void Graphics::loadLog() {
    try {
	    refBuilder->add_from_file("glade/LogIn.glade");
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
	refBuilder->get_widget_derived("LogIn",logIn);

}

void Graphics::loadMenu() {
	try {
	  refBuilder->add_from_file("glade/VisualClient.glade");
	}
	catch(const Glib::FileError& ex)
	{
	  std::cerr << "FileError: " << ex.what() << std::endl;
	}
	catch(const Glib::MarkupError& ex)
	{
	  std::cerr << "MarkupError: " << ex.what() << std::endl;
	}
	catch(const Gtk::BuilderError& ex)
	{
	  std::cerr << "BuilderError: " << ex.what() << std::endl;
	}
	refBuilder->get_widget_derived("nClient",this->menu);
}

void Graphics::show() {
	 while(!logIn->getCancel()) {	
	  app->run(*logIn);
	  if (logIn->getAccept()) {
	        menu->show_all();
	        logIn->exit();
	  }
  }
}

} /* namespace visual */
