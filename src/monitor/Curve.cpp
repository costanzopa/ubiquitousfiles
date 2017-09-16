/*
 * Curve.cpp
 *
 *  Created on: 13/06/2013
 *      Author: Pablito
 */

#include "./Curve.h"

namespace visual {

Curve::Curve() {
	this->height = 600;
	this->width = 600;
	this->set_size_request(width, height);
	this->axesWidth = 2.0;
	this->curveWidth = 2.0;
	this->x0 = 50;
	this->y0 = 50;
	this->bytes = bytes;
	this->time = time;
}

Curve::~Curve() {
	for (unsigned int  i = 0; i< this->pairs.size(); i++) {
		delete this->pairs[i];
	}
}

bool Curve::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
	cr->rectangle(0,0,this->width,this->height);
	cr->set_source_rgb(1.0,1.0,1.0);
	cr->fill();
	this->drawAxes(cr);
	this->drawText(cr);
	this->drawCurve(cr);
	this->drawDash(cr);
	this->drawLimits(cr);
	return true;
}

void Curve::drawAxes(const Cairo::RefPtr<Cairo::Context>& cr) {
	cr->set_line_width(this->axesWidth);
	cr->set_source_rgb(0.0,0.0,0.0);
	cr->move_to(x0,y0);
	cr->line_to(x0, this->height-y0);
	cr->stroke();
	cr->move_to(x0, this->height-y0);
	cr->line_to(this->width-x0, this->height-y0);
	cr->stroke();
}

void Curve::drawCurve(const Cairo::RefPtr<Cairo::Context>& cr) {
	int x, y;
	x = x0;
	y = this->height - y0;
	cr->set_line_width(this->axesWidth*2);
	cr->set_source_rgb(1.0,0.0,0.0);
	for (unsigned int i = 0; i<this->bytes.size(); i++) {
		cr->move_to(x,y);
		if (pairs[i]->getY() == 0) {
			cr->line_to(this->pairs[i]->getPosX(), this->height -y0);
			cr->stroke();
			x = this->pairs[i]->getPosX();
			y = this->height - y0;
		}
		else {
			cr->line_to(this->pairs[i]->getPosX(), this->pairs[i]->getPosY());
			cr->stroke();
			x = this->pairs[i]->getPosX();
			y = this->pairs[i]->getPosY();
		}
	}

}

void Curve::drawText(const Cairo::RefPtr<Cairo::Context>& cr) {
	Pango::FontDescription font;
	font.set_family("Monospace");
	font.set_weight(Pango::WEIGHT_BOLD);
	Glib::RefPtr<Pango::Layout> layout = create_pango_layout("Grafico que muestra Bytes almacenados en "
			"funcion del tiempo.");
	Glib::RefPtr<Pango::Layout> layout2 = create_pango_layout("Time(Sec)");
	Glib::RefPtr<Pango::Layout> layout3 = create_pango_layout("Size(B)");
	Glib::RefPtr<Pango::Layout> layout4 = create_pango_layout("0");
	layout->set_font_description(font);
	int text_width;
	int text_heigth;

	layout->get_pixel_size(text_width, text_heigth);

	cr->move_to(x0,y0/2);

	layout->show_in_cairo_context(cr);
	cr->move_to(this->width/2 -20,this->height-45);
	layout2->show_in_cairo_context(cr);
	cr->move_to(0,this->height/2);
	layout3->show_in_cairo_context(cr);
	cr->move_to(x0,this->height -45);
	layout4->show_in_cairo_context(cr);
}

void Curve::drawDash(const Cairo::RefPtr<Cairo::Context>& cr) {
	cr->set_line_width(this->axesWidth-1.0);
	cr->set_source_rgb(0.0,0.0,0.0);
	std::valarray< double > dashes(2);
	dashes[0] = 2.0;
	dashes[1] = 2.0;

	cr->set_dash (dashes, 0.0);
	for(int i = y0; i < this->height - y0; i+=y0) {
		cr->move_to(x0, i);
		cr->line_to(this->width-x0, i);
		cr->stroke();
	}
	for(int j = x0; j < this->width - x0; j+=x0) {
			cr->move_to(j, y0);
			cr->line_to(j, this->height-y0);
			cr->stroke();
	}
}

void Curve::drawLimits(const Cairo::RefPtr<Cairo::Context>& cr) {
	std::ostringstream maxBytes, maxTime;
	if (!this->bytes.empty()) {
		maxBytes <<std::dec<< this->bytes[bytes.size()-1];
		maxTime << std::dec<< this->time[time.size()-1];
    }
	std::string stime = "Rango de Tiempo: 0..";
	stime += maxTime.str(); 
	std::string sbytes = "Rango de Bytes: 0..";
	sbytes += maxBytes.str(); 
	Glib::RefPtr<Pango::Layout> layout2 = create_pango_layout(stime);
	Glib::RefPtr<Pango::Layout> layout3 = create_pango_layout(sbytes);
	int text_width;
	int text_heigth;

	layout2->get_pixel_size(text_width, text_heigth);

	cr->move_to(this->width-210,50);

	layout2->show_in_cairo_context(cr);
	cr->move_to(this->width -210,70);
	layout3->show_in_cairo_context(cr);
}

void Curve::complete() {
	int j = x0*2;
	Pair* pair;
	for(unsigned int i = 0; i < this->bytes.size() ;i++) {
		pair= new Pair(this->time[i],this->bytes[i],j,0);
		this->pairs.push_back(pair);
		j +=x0;
	}

}

void Curve::findPosition() {
	int aux=this->height-y0;
	int aux2,aux1 = 0;
	this->makePosition();
	for (unsigned int i = 0; i < this->bytes.size(); i++) {
		for(unsigned int j = 0; j < this->position.size(); j++) {
			if(this->pairs[i]->getY()== this->position[j]) {
				aux1 = (y0*(j+1));
				aux2 = aux- aux1;
				this->pairs[i]->setPosY(aux2);
			}
		}
	}
}

void Curve::makePosition() {
	if (!this->bytes.empty()) {
		double data= this->bytes[0];
		position.push_back(data);
		for(unsigned int j = 1; j < this->bytes.size(); j++) {
			if (this->bytes[j] != data) {
				data = this->bytes[j];
				position.push_back(data);
			}
		}
	}

}

void Curve::makeData(std::vector<double>& bytes, std::vector<double>& time) {
	this->vectors(bytes, time);
	this->complete();
	std::sort(this->bytes.begin(),this->bytes.end());
	this->findPosition();
}

void Curve::vectors(std::vector<double>& bytes, std::vector<double>& time) {
	this->bytes = bytes;
	this->time = time;
}

} /* namespace visual */
