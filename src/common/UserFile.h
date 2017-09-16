/*
 * userFile.h
 *
 *  Created on: 19/06/2013
 *      Author: Pablito
 */

#ifndef USERFILE_H_
#define USERFILE_H_
#include <fstream>
#include <iostream>

namespace configuration {

class UserFile {
public:
	UserFile();
	virtual ~UserFile();
	bool create(std::string& user, std::string& pass);
	bool deleteUser(const std::string& user);
	bool change(std::string& user, std::string& oldPass, std::string& newPass);
	void begin();
	void end();
	void write(std::fstream& nFile, std::string& buffer, std::string& buffer2);
	void createAux(std::string& path);
	bool find(const std::string& user);
	std::string getPassword(const std::string& user);
private:
	std::string path;
	std::fstream file;
	std::fstream aux;
    std::string user;
	std::string pass;
	int bytes;
};

} /* namespace configuration */
#endif /* USERFILE_H_ */
