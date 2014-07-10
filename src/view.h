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

#ifndef _VIEW_H_
#define _VIEW_H_

// ----------------------------------------------------------------------------

#include <gtkmm/drawingarea.h>
#include "selection_band.h"

// ----------------------------------------------------------------------------

class View : public Gtk::DrawingArea
{
	public:
		View();
		void Draw();		

		bool SavePNG(const char *filename);
		bool SaveJPEG(const char *filename);
		bool SaveBMP(const char *filename);
		
	private:
		virtual void on_realize();
		virtual bool on_configure_event(GdkEventConfigure *e);
		virtual bool on_expose_event(GdkEventExpose *e);
		virtual bool on_button_press_event(GdkEventButton *e);
		virtual bool on_button_release_event(GdkEventButton *e);
		virtual bool on_motion_notify_event(GdkEventMotion *e);
		virtual bool on_key_press_event(GdkEventKey *e);
		
		void ResizeCanvas();
		
		Glib::RefPtr<Gdk::Pixmap>	mPixmap;
		Glib::RefPtr<Gdk::GC>		mGC;
		SelectionBand				mBand;
};

// ----------------------------------------------------------------------------

#endif //_VIEW_H_
