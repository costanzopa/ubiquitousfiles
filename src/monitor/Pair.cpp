/*
 * Pair.cpp
 *
 *  Created on: 14/06/2013
 *      Author: Pablito
 */

#include "./Pair.h"

namespace visual {

Pair::Pair(double x, double y, int posx, int posy) {
	this->x = x;
	this->y = y;
	this->posx = posx;
	this->posy = posy;
}

Pair::~Pair() {
}

int Pair::getPosX() const {
	return this->posx;
}

int Pair::getPosY() const {
	return this->posy;
}

double Pair::getX() const {
	return this->x;
}

double Pair::getY() const {
	return this->y;
}

void Pair::setPosY(int y) {
	this->posy = y;
}

} /* namespace visual */
