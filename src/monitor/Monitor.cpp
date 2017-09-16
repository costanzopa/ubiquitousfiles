//============================================================================
// Name        : Monitor.cpp
// Author      : Pablito
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "./MenuMonitor.h"

using namespace visual;

int main(int argc, char** argv) {
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "monitor.exe");

		  //Load the Glade file and instiate its widgets:
		  Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create();
		  try
		  {
		    refBuilder->add_from_file("glade/Monitor.glade");
		  }
		  catch(const Glib::FileError& ex)
		  {
		    std::cerr << "FileError: " << ex.what() << std::endl;
		    return 1;
		  }
		  catch(const Glib::MarkupError& ex)
		  {
		    std::cerr << "MarkupError: " << ex.what() << std::endl;
		    return 1;
		  }
		  catch(const Gtk::BuilderError& ex)
		  {
		    std::cerr << "BuilderError: " << ex.what() << std::endl;
		    return 1;
		  }

		  //Get the GtkBuilder-instantiated dialog::
		  MenuMonitor* pDialog = 0;
		  refBuilder->get_widget_derived("wMonitor", pDialog);
		  if(pDialog)
		  {
		    //Start:
		    app->run(*pDialog);
		  }

		  delete pDialog;
		return 0;
}
