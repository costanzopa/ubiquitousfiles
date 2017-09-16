/*
 * GraphicWindow.h
 *
 *  Created on: 14/06/2013
 *      Author: Pablito
 */

#ifndef GRAPHICWINDOW_H_
#define GRAPHICWINDOW_H_
#include "./Curve.h"
#include "gtkmm/window.h"
namespace visual {

class GraphicWindow : public Gtk::Window {
public:
	GraphicWindow();
	virtual ~GraphicWindow();
	void show(std::vector<double> &bytes, std::vector<double>& time);
private:
	Curve* curve;
};

} /* namespace visual */
#endif /* GRAPHICWINDOW_H_ */
