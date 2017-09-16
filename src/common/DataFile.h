/*
 * DataFile.h
 *
 *  Created on: 19/06/2013
 *      Author: Pablito
 */

#ifndef DATAFILE_H_
#define DATAFILE_H_
#include <iostream>
#include <fstream>
#include <vector>
namespace configuration {

class DataFile {
public:
	DataFile();
	virtual ~DataFile();
	void write(double time, double bytes);
	void getData();
	std::vector<double> getBytes() const;
	std::vector<double> getTime() const;
	void begin();
	void end();
	void print();
private:
	int counter;
	std::fstream file;
	std::vector<double> bytes;
	std::vector<double> time;
	std::string path;
};

} /* namespace configuration */
#endif /* DATAFILE_H_ */
