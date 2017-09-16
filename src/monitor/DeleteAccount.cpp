/*
 * DeleteAccount.cpp
 *
 *  Created on: 19/06/2013
 *      Author: Pablito
 */

#include "./DeleteAccount.h"
#include "../common/UserFile.h"
#include <iostream>

using namespace configuration;
namespace visual {

DeleteAccount::DeleteAccount(std::string& npath) {
	this->deleteAccount = 0;
	this->find = 0;
	this->ok = 0;
	this->user = 0;
    refBuilder = Gtk::Builder::create();
	this->exit = 0;
	this->route = npath;
}

DeleteAccount::~DeleteAccount() {
}

void DeleteAccount::load() {
	try {
			refBuilder->add_from_file("glade/DeleteAccount.glade");
		}
		catch(const Glib::FileError& ex) {
		     std::cerr << "FileError: " << ex.what() << std::endl;
		}
		catch(const Glib::MarkupError& ex) {
		    std::cerr << "MarkupError: " << ex.what() << std::endl;
		}
		catch(const Gtk::BuilderError& ex) {
		    std::cerr << "BuilderError: " << ex.what() << std::endl;
		}
		refBuilder->get_widget("wDelete", this->deleteAccount);
}

void DeleteAccount::assign() {
	refBuilder->get_widget("ok", ok);
	refBuilder->get_widget("user", user);
	refBuilder->get_widget("find", find);
	refBuilder->get_widget("exit", exit);
	ok->signal_clicked().connect( sigc::mem_fun(*this, &DeleteAccount::ok_button_clicked));
	exit->signal_clicked().connect( sigc::mem_fun(*this, &DeleteAccount::exit_button_clicked));
}

void DeleteAccount::show() {
	this->load();
	this->assign();
	this->deleteAccount->show_all();
}

bool DeleteAccount::isUser(const std::string& user) {
	//UserFile fuser;
	bool flag = false;
	if (!user.empty()) {
		//flag = fuser.deleteUser(user);
		if (flag == true) {
			std::string auxiliar = this->route;
			//auxiliar.append(user);
			this->deleteDirectory(auxiliar);
		}
	}
	return flag;
}

bool DeleteAccount::deleteDirectory(const std::string& directory) {
	    DIR *folder; 
	    struct dirent *entry;
	    std::string nombre;
	    chdir(directory.c_str());
	    folder = opendir(directory.c_str());
	    entry = readdir(folder);
	    while (entry) {
	        if (strcmp(entry->d_name, ".") != 0 &&
	            strcmp(entry->d_name, "..") != 0)
	        {
	            nombre = entry->d_name;
	            if (remove(nombre.c_str()))
	            {
	                char dir[600];
	                sprintf(dir, "%s\\%s", directory.c_str(),
	                        entry->d_name);
	                std::string auxiliar(dir);
	                deleteDirectory(auxiliar);
	            }
	        }
	        entry = readdir(folder);
	    }
	    closedir(folder);
	    chdir("..");
	    rmdir(directory.c_str());
	    return true;
}

void DeleteAccount::ok_button_clicked() {
	Glib::ustring nuser = this->user->get_text();
	std::string sUser = nuser.raw();
	bool isUser = this->isUser(sUser);
	if(isUser) {
	  find->set_text(sUser + ": The user has been deleted.");
	}
	else
		find->set_text(sUser +" : The user hasn't been found.");
	}

void DeleteAccount::exit_button_clicked() {
	this->deleteAccount->hide();
}
} /* namespace visual */
