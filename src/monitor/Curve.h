/*
 * Curve.h
 *
 *  Created on: 13/06/2013
 *      Author: Pablito
 */

#ifndef CURVE_H_
#define CURVE_H_

#include <gtkmm/checkbutton.h>
#include <gtkmm/button.h>
#include <gtkmm/box.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/label.h>
#include <glibmm.h>
#include <gtkmm/window.h>
#include <gtkmm/main.h>
#include <gtkmm/drawingarea.h>
#include <cairomm/context.h>
#include <pangomm/fontdescription.h>
#include <gtkmm.h>
#include <pangomm/layout.h>
#include "./Pair.h"
#include <iostream>

namespace visual {

class Curve :public Gtk::DrawingArea {
public:
	Curve();
	virtual ~Curve();
	void drawAxes(const Cairo::RefPtr<Cairo::Context>& cr);
	void drawCurve(const Cairo::RefPtr<Cairo::Context>& cr);
	void drawText(const Cairo::RefPtr<Cairo::Context>& cr);
	void drawDash(const Cairo::RefPtr<Cairo::Context>& cr);
	void drawLimits(const Cairo::RefPtr<Cairo::Context>& cr);
	void print();
	void makeData(std::vector<double>& bytes, std::vector<double>& time);
	void vectors(std::vector<double>& bytes, std::vector<double>& time);
private:
	void complete();
	void findPosition();
	void sort();
	void makePosition();

protected:
	virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

private:
	int x0;
	int y0;
	double axesWidth;
	double curveWidth;
	int height;
	int width;
	std::vector<double> time;
	std::vector<double>  bytes;
	std::vector<Pair*> pairs;
	std::vector<double> position;
};

} /* namespace visual */
#endif /* CURVE_H_ */
