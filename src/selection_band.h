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

#ifndef _SELECTION_BAND_H_
#define _SELECTION_BAND_H_

// ----------------------------------------------------------------------------

#include <glibmm/refptr.h>

namespace Gtk { struct Widget; }
namespace Gdk { struct Pixmap; }
namespace Gdk { struct GC; }

// ----------------------------------------------------------------------------

class SelectionBand
{
	public:
		SelectionBand() : mActive(false) {}
		
		void Start(int x, int y);
		void Update(int x, int y);
		void Draw(Gtk::Widget &widget, Glib::RefPtr<Gdk::Pixmap> &pixmap, Glib::RefPtr<Gdk::GC> &GC);
		void Stop();
				
	private:
		int mStartX;
		int mStartY;
		int mOldX;
		int mOldY;
		int mCurrentX;
		int mCurrentY;
		bool mActive;
};

// ----------------------------------------------------------------------------

#endif //_SELECTION_BAND_H_
