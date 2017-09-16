/*
 * ConfigFile.h
 *
 *  Created on: 15/06/2013
 *      Author: Pablito
 */
/*
 * ConfigFile.h
 *
 *  Created on: 11/06/2013
 *      Author: Pablito
 */

#ifndef CONFIGFILE_H_
#define CONFIGFILE_H_
#include <iostream>
#include <fstream>
#include <sstream>
namespace configuration {

class ConfigFile {
public:
	ConfigFile(std::string &path);
	virtual ~ConfigFile();
	void create(std::string &dir, std::string &nip, int nport, int poll);
	void read();
	void update(std::string &dir, std::string &nip, int nport, int poll);
	void update(std::string &dir, int nport);
	void begin();
	std::string getDirectory() const;
	std::string getIp() const;
	int getPort() const;
	int getPolling() const;
	std::string getStringPort() const;

private:
	std::fstream file;
	std::string directory;
	std::string ip;
	int port;
	int polling;
	std::string sport;
	std::string path;
};

} /* namespace configuration */

#endif /* CONFIGFILE_H_ */
