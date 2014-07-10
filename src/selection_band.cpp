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

#include "selection_band.h"

#include <gdkmm/pixmap.h>
#include <gdkmm/gc.h>
#include <gtkmm/widget.h>

#include <algorithm>

// ----------------------------------------------------------------------------

void SelectionBand::Start(int x, int y)
{
	mActive = true;
	mStartX = x;
	mStartY = y;
	mCurrentX = x;
	mCurrentY = y;
	mOldX = -100;
	mOldY = -100;
}

void SelectionBand::Update(int x, int y)
{
	mCurrentX = x;
	mCurrentY = y;
}

extern Gdk::Color white;

void SelectionBand::Draw(Gtk::Widget &widget, 
						Glib::RefPtr<Gdk::Pixmap> &pixmap, Glib::RefPtr<Gdk::GC> &GC)
{
	if (!mActive) return;

	GC->set_foreground(white);
	GC->set_function(Gdk::XOR);
	
	int x1_old, y1_old, x2_old, y2_old;
	
	if (mOldX != -100)
	{
		x1_old = std::min(mStartX, mOldX);
		y1_old = std::min(mStartY, mOldY);
		x2_old = std::max(mStartX, mOldX);
		y2_old = std::max(mStartY, mOldY);		
		pixmap->draw_rectangle(GC, false, x1_old, y1_old, x2_old-x1_old, y2_old-y1_old);
	}
	
	int x1 = std::min(mStartX, mCurrentX);
	int y1 = std::min(mStartY, mCurrentY);
	int x2 = std::max(mStartX, mCurrentX);
	int y2 = std::max(mStartY, mCurrentY);		
	pixmap->draw_rectangle(GC, false, x1, y1, x2-x1, y2-y1);
	
	GC->set_function(Gdk::COPY);
	
	int x1_area = std::min(x1, x1_old);
	int y1_area = std::min(y1, y1_old);
	int x2_area = std::max(x2, x2_old);
	int y2_area = std::max(y2, y2_old);
	widget.queue_draw_area(x1_area, y1_area, x2_area-x1_area+1, y2_area-y1_area+1);

	mOldX = mCurrentX;
	mOldY = mCurrentY;	
}

void SelectionBand::Stop()
{
	mActive = false;
}

// ----------------------------------------------------------------------------
