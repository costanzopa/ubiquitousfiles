/*
 * GraphicWindow.cpp
 *
 *  Created on: 14/06/2013
 *      Author: Pablito
 */

#include "./GraphicWindow.h"

namespace visual {

GraphicWindow::GraphicWindow() {
	this->set_title("Bytes vs Time");
	this->curve = new Curve();
	this->add(*curve);

}

GraphicWindow::~GraphicWindow() {
	delete curve;
}

void GraphicWindow::show(std::vector<double> &bytes, std::vector<double>& time) {
	curve->makeData(bytes,time);
	curve->show();
}

} /* namespace visual */
