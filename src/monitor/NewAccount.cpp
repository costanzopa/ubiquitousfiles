
#include "./NewAccount.h"

namespace visual {

NewAccount::NewAccount(std::string& npath):BaseAccount() {
	this->label = 0;
	this->create = 0;
	this->path = "glade/NewAccount.glade";
	this->route = npath;
}

NewAccount::~NewAccount() {
}

void NewAccount::assign() {
	BaseAccount::assign();
	refBuilder->get_widget("create", create);
	refBuilder->get_widget("label", this->label);
	create->signal_clicked().connect( sigc::mem_fun(*this, &NewAccount::create_button_clicked));
}

void NewAccount::create_button_clicked() {
	bool flag = false;
	Glib::ustring nuser;
	Glib::ustring npass;
	Glib::ustring nepass;
	std::string suser;
	std::string spass;
	std::string snewpass;
	UserFile* fuser = new UserFile();
	nuser = this->user->get_text();
	npass = this->pass->get_text();
	nepass= this->newPass->get_text();
	suser = nuser.raw();
	spass = npass.raw();
	snewpass = nepass.raw();
		if ((!suser.empty()) && (!spass.empty())&&(!snewpass.empty())) {
			if (spass==snewpass) {
				flag = fuser->create(suser, spass);
				if(flag == false)  {
					label->set_text("This user is already in use.");
					suser.clear();
					spass.clear();
				}
				else {
					label->set_text("Succesfully user create, press cancel to exit");
					std::string newRoute;
					newRoute = this->route;
					newRoute.append(suser.c_str());
					mkdir(newRoute.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
				}
			}
			else {
				label->set_text("Differents passwords.");
			}
		}
	delete fuser;
}

} /* namespace visual */
