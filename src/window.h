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

#ifndef _WINDOW_H_
#define _WINDOW_H_

// ----------------------------------------------------------------------------

#include <gtkmm/window.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/box.h>
#include <glibmm/refptr.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/actiongroup.h>
#include <glibmm/ustring.h>
#include <string>

#include "view.h"

// ----------------------------------------------------------------------------

class Window : public Gtk::Window
{
	public:
		Window();
		
	private:
		enum FileType
		{
			NONE,
			PNG,
			JPEG,
			BMP
		};

		Glib::RefPtr<Gtk::UIManager> mUIManager;
		Glib::RefPtr<Gtk::ActionGroup> mActionGroup;
		
		Gtk::VBox		mVBox;
		Gtk::Statusbar	mStatusBar;
		View 			mView;	
		
		std::string		mFilename;
		
		void on_action_file_new();
		void on_action_file_export();
		void on_action_file_quit();
		
		void on_action_view_toolbar();
		void on_action_view_statusbar();
		void on_action_view_answers();
			
		virtual void	on_realize();
		virtual bool	on_delete_event(GdkEventAny *e);	
		void			on_grid_dirty(bool dirty);
		void			on_grid_redraw();
		
		bool	ConfirmQuit();
		bool	ConfirmOverwrite(const char *filename);
		FileType GetTypeFromFilename(const char *filename);		
};

// ----------------------------------------------------------------------------

#endif //_WINDOW_H_
