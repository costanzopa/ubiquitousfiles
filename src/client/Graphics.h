/*
 * Graphics.h
 *
 *  Created on: 06/06/2013
 *      Author: Pablito
 */

#ifndef GRAPHICS_H_
#define GRAPHICS_H_
#include "./LogIn.h"
#include "./MenuClient.h"
namespace visual {

class Graphics {
public:
	Graphics(int argc, char** argv);
	virtual ~Graphics();
	void loadMenu();
	void loadLog();
	void show();
private:
	LogIn* logIn;
	MenuClient* menu;
	Glib::RefPtr<Gtk::Builder> refBuilder;
	Glib::RefPtr<Gtk::Application> app;

};

} /* namespace visual */
#endif /* GRAPHICS_H_ */
