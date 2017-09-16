/*
 * userFile.cpp
 *
 *  Created on: 19/06/2013
 *      Author: Pablito
 */

#include "./UserFile.h"

using configuration::UserFile;

namespace configuration {

UserFile::UserFile() {
	this->path = "../user.txt";
	this->bytes = 0;
	file.open(path.c_str(), std::fstream::in | std::fstream::out);

	if (!file.is_open()) {
		file.clear();

		file.open(path.c_str(), std::fstream::out);
		file.close();

		file.open(path.c_str(), std::fstream::in | std::fstream::out);

		if (!file.is_open())
			throw std::ios_base::failure("El archivo users no se pudo abrir");
	}
}

UserFile::~UserFile() {
	file.close();
}

bool UserFile::create(std::string& user, std::string& pass) {
	bool toReturn = false;
	std::string fileUser, filePass;
	this->begin();
	while(!file.eof()) {
		file >> fileUser;
		file >> filePass;
		if(fileUser == user) {
			return toReturn;
		}
		fileUser.clear();
	}
	this->end();
	this->write(this->file, user, pass);
	toReturn = true;
	return toReturn;
}

bool UserFile::deleteUser(const std::string& user) {
	bool toReturn = false;
	std::string auxiliar = "auxiliar.txt";
	std::string fileUser, filePass;
	this->createAux(auxiliar);
	this->begin();
	while(!file.eof()) {
		file >> fileUser;
		file >> filePass;
		if(fileUser == user) {
			toReturn = true;
		}
		else {
			this->write(this->aux, fileUser, filePass);
		}
		fileUser.clear();
		filePass.clear();
	}
	file.close();
	remove(this->path.c_str());
	rename(auxiliar.c_str(), this->path.c_str());
	return toReturn;
}

bool UserFile::change(std::string& user, std::string& oldPass, std::string& newPass) {
	bool toReturn = false;
	std::string auxiliar = "auxiliar.txt";
	std::string fileUser, filePass;
	this->createAux(auxiliar);
	this->begin();
	while(!file.eof()) {
		file >> fileUser;
		file >> filePass;
		if((fileUser == user) && (filePass == oldPass)) {
			toReturn = true;
			this->write(this->aux, fileUser, newPass);
		}
		else {
			this->write(this->aux, fileUser, filePass);
		}
		fileUser.clear();
		filePass.clear();
		}
		file.close();
		remove(this->path.c_str());
		rename(auxiliar.c_str(), this->path.c_str());
	return toReturn;
}

void UserFile::begin() {
	file.tellg();
	file.clear();
	file.seekg(0);
	file.seekp(0);
	file.tellg();
}

void UserFile::end() {
	file.tellg();
	file.clear();
	file.seekg(0, std::ios::end);
	file.seekp(0, std::ios::end);
	file.tellg();
}

void UserFile::write(std::fstream& nFile, std::string& user, std::string& pass) {
	nFile << user;
	nFile << " ";
	nFile << pass << std::endl;
	if (nFile.fail()) {
		throw std::ios_base::failure("No se pudo escribir el archivo Users");
	}
}

void UserFile::createAux(std::string& path) {
	aux.open(path.c_str(), std::fstream::in | std::fstream::out);
		if (!aux.is_open()) {
		aux.clear();
		aux.open(path.c_str(), std::fstream::out);
		aux.close();
	    aux.open(path.c_str(), std::fstream::in | std::fstream::out);
		if (!aux.is_open())
	        throw std::ios_base::failure("El archivo auxiliar no se pudo abrir");
		}
}

bool UserFile::find(const std::string& user) {
	bool toReturn = false;
	std::string fileUser, filePass;
	this->begin();
	while(!file.eof()) {
		file >> fileUser;
		file >> filePass;
		if(fileUser == user) {
			return true;
		}
    }
    return toReturn;
}

std::string UserFile::getPassword(const std::string& user) {
	std::string fileUser, filePass;
	this->begin();
	while(!file.eof()) {
		file >> fileUser;
		file >> filePass;
		if(fileUser == user) {
			return filePass;
		}
		filePass.clear();
	}
	return filePass;
}

} /* namespace configuration */

