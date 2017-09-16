/*
 * Pair.h
 *
 *  Created on: 14/06/2013
 *      Author: Pablito
 */

#ifndef PAIR_H_
#define PAIR_H_

namespace visual {

class Pair {
public:
	Pair(double x, double y, int posx, int posy);
	virtual ~Pair();
	int getPosY() const;
	int getPosX() const;
	double getX() const;
	double getY() const;
	void setPosY(int y);

private:
	double x;
	double y;
	int posx;
	int posy;
};

} /* namespace visual */
#endif /* PAIR_H_ */
