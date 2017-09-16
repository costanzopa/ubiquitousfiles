/*
 * ConfigFile.cpp
 *
 *  Created on: 11/06/2013
 *      Author: Pablito
 */

#include "./ConfigFile.h"

namespace configuration {

ConfigFile::ConfigFile(std::string &npath) {
	this->path = npath;
	this->polling = 0;
	this->port = 0;
	file.open(path.c_str(), std::fstream::in | std::fstream::out);

	if (!file.is_open()) {
		file.clear();

		file.open(path.c_str(), std::fstream::out);
		file.close();

		file.open(path.c_str(), std::fstream::in | std::fstream::out);

		if (!file.is_open())
			throw std::ios_base::failure("El archivo no se pudo abrir");
	}
}

ConfigFile::~ConfigFile() {
	file.close();
}

void ConfigFile::create(std::string &dir, std::string &nip, int nport,
	int poll) {
	this->directory.clear();
	this->ip.clear();
	this->directory = dir;
	this->ip = nip;
	this->port = nport;
	this->polling = poll;
	file << "Configuration File" << std::endl;
	file << "Directory " << this->directory << " " << ";" << std::endl;
	file << "Ip " << this->ip << " " << ";" << std::endl;
	file << "Port " << this->port << " " << ";" << std::endl;
	file << "Polling " << this->polling << " " << ";" << std::endl;

	if (file.fail())
		throw std::ios_base::failure(
				"No se pudo crear correctamente el archivo");
}

void ConfigFile::read() {
	this->begin();
	std::string trash;
	char sep;
	this->directory.clear();
	this->ip.clear();
	file >> std::skipws >> trash;
	trash.clear();
	file >> std::skipws >> trash;
	trash.clear();
	file >> std::skipws >> trash;
	trash.clear();
	file >> std::skipws >> this->directory;
	file >> std::skipws >> sep;
	file >> std::skipws >> trash;
	trash.clear();
	file >> std::skipws >> ip;
	file >> std::skipws >> sep;
	file >> std::skipws >> trash;
	trash.clear();
	file >> std::skipws >> this->port;
	std::ostringstream sPort;
	sPort <<std::dec<< this->port;
	this->sport = sPort.str();
	file >> std::skipws >> sep;
	file >> std::skipws >> trash;
	trash.clear();
	file >> std::skipws >> this->polling;
	file >> std::skipws >> sep;

	if (file.fail())
		throw std::ios_base::failure(
				"No se pudo parsear correctamente el archivo");
}

void ConfigFile::update(std::string &dir, std::string &nip, int nport,
		int poll) {
	file.close();
	file.clear();
	remove(this->path.c_str());
	file.open(path.c_str(), std::fstream::in | std::fstream::out);

	if (!file.is_open()) {
		file.clear();

		file.open(path.c_str(), std::fstream::out);
		file.close();

		file.open(path.c_str(), std::fstream::in | std::fstream::out);

		if (!file.is_open())
			throw std::ios_base::failure("El archivo no se pudo actualizar");
	}
	this->create(dir, nip, nport, poll);
}

void ConfigFile::update(std::string &dir, int nport) {
	int poll = this->getPolling();
	std::string nip = this->getIp();
	file.close();
	file.clear();
	remove(this->path.c_str());
	file.open(path.c_str(), std::fstream::in | std::fstream::out);

	if (!file.is_open()) {
		file.clear();

		file.open(path.c_str(), std::fstream::out);
		file.close();

		file.open(path.c_str(), std::fstream::in | std::fstream::out);

		if (!file.is_open())
			throw std::ios_base::failure("El archivo no se pudo actualizar");
		}
	this->create(dir, nip, nport, poll);
}

void ConfigFile::begin() {
	file.tellg();
	file.clear();
	file.seekg(0);
	file.seekp(0);
	file.tellg();
}

std::string ConfigFile::getDirectory() const {
	return this->directory;
}

std::string ConfigFile::getIp() const {
	return this->ip;
}

int ConfigFile::getPort() const {
	return this->port;
}

int ConfigFile::getPolling() const {
	return this->polling;
}

std::string ConfigFile::getStringPort() const {
	return this->sport;
}

} /* namespace configuration */
