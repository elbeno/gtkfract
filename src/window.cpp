// ----------------------------------------------------------------------------

// Copyright (C) 2006  Ben Deane (ben@elbeno.com)

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

// ----------------------------------------------------------------------------

#include "window.h"

#include <gtkmm/stock.h>
#include <gtkmm/main.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/toggleaction.h>

// ----------------------------------------------------------------------------

Window::Window()
{
	Glib::RefPtr<Gtk::ActionGroup> mActionGroup = Gtk::ActionGroup::create();
	
	mActionGroup->add(Gtk::Action::create("MenuFile", "_File"));
	mActionGroup->add(Gtk::Action::create("New", Gtk::Stock::NEW),
						sigc::mem_fun(*this, &Window::on_action_file_new));
  	mActionGroup->add(Gtk::Action::create("Export", "_Export..."),
						sigc::mem_fun(*this, &Window::on_action_file_export));
	mActionGroup->add(Gtk::Action::create("Quit", Gtk::Stock::QUIT),
						sigc::mem_fun(*this, &Window::on_action_file_quit));
				
	mActionGroup->add(Gtk::Action::create("MenuView", "_View"));
	mActionGroup->add(Gtk::ToggleAction::create("Toolbar", "_Toolbar", "", true),
						sigc::mem_fun(*this, &Window::on_action_view_toolbar));
	mActionGroup->add(Gtk::ToggleAction::create("Status Bar", "Status _Bar", "", true),
						sigc::mem_fun(*this, &Window::on_action_view_statusbar));

	mUIManager = Gtk::UIManager::create();
	mUIManager->insert_action_group(mActionGroup);
	add_accel_group(mUIManager->get_accel_group());
	
	Glib::ustring ui_info =
        "<ui>"
        "  <menubar name='MenuBar'>"
        "    <menu action='MenuFile'>"
        "      <menuitem action='New'/>"
        "      <separator/>"
        "      <menuitem action='Export'/>"
        "      <separator/>"
        "      <menuitem action='Quit'/>"
        "    </menu>"
        "    <menu action='MenuView'>"
        "      <menuitem action='Toolbar'/>"
        "      <menuitem action='Status Bar'/>"
        "    </menu>"
        "  </menubar>"
        "  <toolbar  name='ToolBar'>"
        "    <toolitem action='New'/>"
        "    <separator/>"
        "    <toolitem action='Quit'/>"
        "  </toolbar>"
        "</ui>";

    mUIManager->add_ui_from_string(ui_info);	
    Gtk::Widget	*pMenuBar = mUIManager->get_widget("/MenuBar");
    mVBox.pack_start(*pMenuBar, Gtk::PACK_SHRINK);

    Gtk::Widget	*pToolBar = mUIManager->get_widget("/ToolBar");
    mVBox.pack_start(*pToolBar, Gtk::PACK_SHRINK);

	mVBox.pack_start(mView);
	mVBox.pack_start(mStatusBar, Gtk::PACK_SHRINK);
	add(mVBox);	
}

// ----------------------------------------------------------------------------

void Window::on_realize()
{
	Gtk::Window::on_realize();
}

// ----------------------------------------------------------------------------

extern double x_min;
extern double x_max;
extern double y_min;
extern double y_max;

void Window::on_action_file_new()
{
	x_min = -2.1;
	x_max = 0.7;
	y_max = 1.2;
	y_min = -1.2;
	
	mView.Draw();
}

// ----------------------------------------------------------------------------

Window::FileType Window::GetTypeFromFilename(const char *filename)
{
	const char *extension = strrchr(filename, '.');
	if (extension)
	{	
		if (!strcmp(extension, ".png"))
			return PNG;
		else if (!strcmp(extension, ".jpg") || !strcmp(extension, ".jpeg"))
			return JPEG;
		else if (!strcmp(extension, ".bmp"))
			return BMP;
	}
	return NONE;
}

// ----------------------------------------------------------------------------

void Window::on_action_file_export()
{
	Gtk::FileChooserDialog dialog("Export fractal...", Gtk::FILE_CHOOSER_ACTION_SAVE);
	dialog.set_transient_for(*this);

	//Add response buttons the the dialog:
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

	//Add filters, so that only certain file types can be selected:
	Gtk::FileFilter filter_png;
	filter_png.set_name("PNG files");
	filter_png.add_mime_type("image/png");
	dialog.add_filter(filter_png);

	Gtk::FileFilter filter_jpeg;
	filter_jpeg.set_name("JPEG files");
	filter_jpeg.add_mime_type("image/jpeg");
	dialog.add_filter(filter_jpeg);
	
	Gtk::FileFilter filter_bmp;
	filter_bmp.set_name("BMP files");
	filter_bmp.add_mime_type("image/bmp");
	dialog.add_filter(filter_bmp);
	
  	bool error = true;
  	while (error)
  	{
  		error = false;
		//Show the dialog and wait for a user response:
		int result = dialog.run();
	
		//Handle the response:
		switch(result)
		{
			case Gtk::RESPONSE_OK:
			{
				Glib::ustring filename = dialog.get_filename();
				if (ConfirmOverwrite(filename.c_str()))
				{
					const Gtk::FileFilter *filter = dialog.get_filter();
					bool success = true;
					FileType t = GetTypeFromFilename(filename.c_str());
					switch (t)
					{
						case PNG:
							success = mView.SavePNG(filename.c_str()); break;
						case JPEG:
							success = mView.SaveJPEG(filename.c_str()); break;
						case BMP:
							success = mView.SaveBMP(filename.c_str()); break;
						default:
							if (filter == &filter_png)
								success = mView.SavePNG(filename.c_str());
							else if (filter == &filter_jpeg)
								success = mView.SaveJPEG(filename.c_str());
							else if (filter == &filter_bmp)
								success = mView.SaveBMP(filename.c_str());
							break;
					}
					if (!success)
					{
						Gtk::MessageDialog dialog(*this, "Error exporting the file!", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
						dialog.run();
						error = true;
					}
				}
				break;
			}
			case Gtk::RESPONSE_CANCEL:
			default:
				break;
		}
  	}
}

// ----------------------------------------------------------------------------

bool Window::ConfirmQuit()
{
	return true;
}

// ----------------------------------------------------------------------------

bool Window::ConfirmOverwrite(const char *filename)
{
	int result = Gtk::RESPONSE_YES;
	FILE *fp = fopen(filename, "rb");
	bool exists = (fp != 0);
	if (exists)
	{
		fclose(fp);
		char msgbuf[256];
		sprintf(msgbuf, "%s already exists. Overwrite it?", filename);
		Gtk::MessageDialog dialog(*this, msgbuf, false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
		result = dialog.run();
	}
	return (result == Gtk::RESPONSE_YES);
}

// ----------------------------------------------------------------------------

void Window::on_action_file_quit()
{	
	if (ConfirmQuit())
	{
	  	Gtk::Main::quit();
	}
}

// ----------------------------------------------------------------------------

bool Window::on_delete_event(GdkEventAny *e)
{
	return !ConfirmQuit();
}

// ----------------------------------------------------------------------------

void Window::on_action_view_toolbar()
{	
    Gtk::Widget	*pToolBar = mUIManager->get_widget("/ToolBar");
	if (pToolBar->is_visible())
		pToolBar->hide();
	else
		pToolBar->show();
}

// ----------------------------------------------------------------------------

void Window::on_action_view_statusbar()
{	
	if (mStatusBar.is_visible())
		mStatusBar.hide();
	else
		mStatusBar.show();
}

// ----------------------------------------------------------------------------
