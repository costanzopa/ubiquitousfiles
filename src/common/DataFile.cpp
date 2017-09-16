/*
 * DataFile.cpp
 *
 *  Created on: 19/06/2013
 *      Author: Pablito
 */

#include "./DataFile.h"

namespace configuration {

DataFile::DataFile() {
	this->path = "../data.txt";
	this->counter = 0;
	file.open(path.c_str(), std::fstream::in | std::fstream::out);

	if (!file.is_open()) {
		file.clear();

		file.open(path.c_str(), std::fstream::out);
		file.close();

		file.open(path.c_str(), std::fstream::in | std::fstream::out);

		if (!file.is_open())
			throw std::ios_base::failure("El archivo data no se pudo abrir");
		}
}

DataFile::~DataFile() {
	file.close();
}

void DataFile::write(double time, double bytes) {
	file << time << " " << bytes << std::endl;
}

void DataFile::getData() {
	double trash1, trash2;
	double nbytes, ntime;
	this->begin();
	while (!file.eof()) {
		file >> trash1 >> trash2;
		counter++;
	}
	counter--;
	//std::cout<< counter << std::endl;
	this->begin();
	if (counter > 10) {
		int route = this->counter -10;
		for(int i = 0; i < route; i++) {
			file >> trash1>>trash2;
		}
	}
	while(!file.eof()) {
		file >> ntime >> nbytes;
		this->bytes.push_back(nbytes);
		this->time.push_back(ntime);
	}
	this->bytes.pop_back();
	this->time.pop_back();
}

void DataFile::begin() {
	file.tellg();
	file.clear();
	file.seekg(0);
	file.seekp(0);
	file.tellg();
}

void DataFile::end() {
	file.tellg();
	file.clear();
	file.seekg(0, std::ios::end);
	file.seekp(0, std::ios::end);
	file.tellg();
}

std::vector<double> DataFile::getBytes() const {
	return bytes;
}

std::vector<double> DataFile::getTime() const {
	return time;
}

void DataFile::print() {
	for(unsigned int i = 0; i < this->bytes.size(); i++) {
		//std::cout << bytes[i]<<" ; " <<time[i] << std::endl;
	}
}

} /* namespace configuration */

