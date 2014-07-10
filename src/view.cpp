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

#include "view.h"

#include <gdkmm/pixbuf.h>
#include "complex.h"

#include <algorithm>

// ----------------------------------------------------------------------------

const int num_colors = 16;

Gdk::Color white("white");
Gdk::Color black("black");
Gdk::Color	mandel_colors[num_colors];

double x_min = -2.1;
double x_max = 0.7;
double y_max = 1.2;
double y_min = -1.2;
bool julia = false;
double julia_x = 0.285;
double julia_y = 0.013;

// ----------------------------------------------------------------------------

View::View()
{
	Glib::RefPtr<Gdk::Colormap>	colormap = get_default_colormap();
	colormap->alloc_color(white);
	colormap->alloc_color(black);
	
	Gdk::Color c;
	int r = 0;
	int g = 0;
	int b = 0;
	for (int i = 0; i < num_colors; ++i)
	{
		mandel_colors[i].set_rgb(r, g, b); 
		colormap->alloc_color(mandel_colors[i], false, false);
		
		r += (65536/num_colors);
	}

	set_flags(Gtk::CAN_FOCUS);
	set_events(get_events() | Gdk::BUTTON_PRESS_MASK
							| Gdk::BUTTON_RELEASE_MASK
							| Gdk::KEY_PRESS_MASK
							| Gdk::POINTER_MOTION_HINT_MASK
							| Gdk::POINTER_MOTION_MASK);

	set_size_request((int)((x_max-x_min) * 300.0), (int)((y_max-y_min) * 300.0));
}

// ----------------------------------------------------------------------------

void View::Draw()
{
	if (!is_realized())
		return;
	
	int w, h;
	mPixmap->get_size(w, h);
	
	double w_inc = (x_max - x_min) / (double)w;
	double h_inc = (y_max - y_min) / (double)h;
	double x = x_min, y;
	
	for (int i = 0; i < w; ++i)
	{
		y = y_max;
		for (int j = 0; j < h; ++j)
		{
			Complex<double> j_c(julia_x, julia_y);
			Complex<double> m_c(x,y);
			Complex<double> *c = &m_c;
			if (julia)
				c = &j_c;
		
			Complex<double> z(x,y);
			const int maxiter = 300;
			int iter = 0;
				
			while (z.modulus_squared() < 4.0 && iter < maxiter)
			{
				z = z * z + *c;
				++iter;				
			}
			
			if (iter == maxiter)
				mGC->set_foreground(black);
			else
				mGC->set_foreground(mandel_colors[iter % num_colors]);
			mPixmap->draw_point(mGC, i, j);
			
			y -= h_inc;
		}
		x += w_inc;
	}
		
	queue_draw();    
}

// ----------------------------------------------------------------------------

void View::ResizeCanvas()
{
	Glib::RefPtr<Gdk::Window> window = get_window();
	int x, y, w, h, depth;
	window->get_geometry(x, y, w, h, depth);
	
	Glib::RefPtr<Gdk::Colormap>	colormap = get_default_colormap();
	Glib::RefPtr<Gdk::Pixmap> newPixmap = Gdk::Pixmap::create(window, w, h, depth);
	newPixmap->set_colormap(colormap);

	// if we already had one, then draw	again
	if (mPixmap)
	{
		mPixmap = newPixmap;
		Draw();
	}
	else
	{
		mPixmap = newPixmap;
	}
}

// ----------------------------------------------------------------------------

void View::on_realize()
{
	Gtk::DrawingArea::on_realize();

	Glib::RefPtr<Gdk::Window> window = get_window();
	window->set_background(white);
	mGC = Gdk::GC::create(window);
	
	ResizeCanvas();
	Draw();

	grab_focus();
}

// ----------------------------------------------------------------------------

bool View::on_configure_event(GdkEventConfigure *)
{
	ResizeCanvas();
	return true;
}

// ----------------------------------------------------------------------------

bool View::on_expose_event(GdkEventExpose *e)
{
	// bit the pixmap to the window
	Glib::RefPtr<Gdk::Window> window = get_window();
	window->draw_drawable(mGC, mPixmap,
						e->area.x, e->area.y, 
						e->area.x, e->area.y,
						e->area.width, e->area.height);
	return true;
}

// ----------------------------------------------------------------------------

double x_down, y_down;
double x_up, y_up;

bool View::on_button_press_event(GdkEventButton *e)
{
	if (e->type == GDK_BUTTON_PRESS)
	{
		// left click
		if (e->button == 1)
		{
			int x, y;
			Gdk::ModifierType state;
			
			if (e->window)
			{
				const Glib::RefPtr<Gdk::Window> refWindow = Glib::wrap((GdkWindowObject*) e->window, true);
				refWindow->get_pointer(x, y, state);
				
				mBand.Start(x, y);
				
				int w, h;
				mPixmap->get_size(w, h);
				x_down = x_min + (x_max - x_min) * (double)x / (double)w;
				y_down = y_max - (y_max - y_min) * (double)y / (double)h;
			}
		}
		// right click
		else if (e->button == 3)
		{
			int x, y;
			Gdk::ModifierType state;
			
			if (e->window)
			{
				const Glib::RefPtr<Gdk::Window> refWindow = Glib::wrap((GdkWindowObject*) e->window, true);
				refWindow->get_pointer(x, y, state);
				
				int w, h;
				mPixmap->get_size(w, h);
				julia_x = x_min + (x_max - x_min) * (double)x / (double)w;
				julia_y = y_max - (y_max - y_min) * (double)y / (double)h;
				
				julia = !julia;
				
				if (julia)
				{
					x_min = -2.0;
					y_min = -2.0;
					x_max = 2.0;
					y_max = 2.0;
				}
				else
				{
					x_min = -2.1;
					x_max = 0.7;
					y_max = 1.2;
					y_min = -1.2;
				}

				Draw();
			}
		}
	}
	else if (e->type == GDK_2BUTTON_PRESS)
	{
		// double (left) click
		if (e->button == 1)
		{
		}		
	}
	
	return true;
}

// ----------------------------------------------------------------------------
	
bool View::on_button_release_event(GdkEventButton *e)
{
	if (e->type == GDK_BUTTON_RELEASE)
	{
		// left release
		if (e->button == 1)
		{
			int x, y;
			Gdk::ModifierType state;
			
			if (e->window)
			{
				const Glib::RefPtr<Gdk::Window> refWindow = Glib::wrap((GdkWindowObject*) e->window, true);
				refWindow->get_pointer(x, y, state);
							
				mBand.Stop();
				
				int w, h;
				mPixmap->get_size(w, h);
				x_up = x_min + (x_max - x_min) * (double)x / (double)w;
				y_up = y_max - (y_max - y_min) * (double)y / (double)h;
				
				x_min = std::min(x_up, x_down);
				x_max = std::max(x_up, x_down);
				y_min = std::min(y_up, y_down);
				y_max = std::max(y_up, y_down);					
				
				Draw();
			}
		}
	}
	
	return true;
}

// ----------------------------------------------------------------------------

bool View::on_key_press_event(GdkEventKey *e)
{	
	return true;
}

// ----------------------------------------------------------------------------

bool View::on_motion_notify_event(GdkEventMotion *e)
{
	int x, y;
	Gdk::ModifierType state;
	
	if (e && e->window)
	{
		const Glib::RefPtr<Gdk::Window> refWindow = Glib::wrap((GdkWindowObject*) e->window, true);
		refWindow->get_pointer(x, y, state);
		
		mBand.Update(x, y);
		mBand.Draw(*this, mPixmap, mGC);
	}
	return true;
}

// ----------------------------------------------------------------------------

bool View::SavePNG(const char *filename)
{
	Glib::RefPtr<Gdk::Colormap>	colormap = get_default_colormap();
	int w, h;
	mPixmap->get_size(w, h);
	Glib::RefPtr<Gdk::Image> image = mPixmap->get_image(0, 0, w, h);
	Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create(image, colormap, 0, 0, 0, 0, w, h);
	bool ret = true;
	try
	{
		pixbuf->save(filename, "png");
	}
	catch (Glib::FileError e)
	{
		ret = false;
	}
	Draw();
	return ret;
}

// ----------------------------------------------------------------------------

bool View::SaveJPEG(const char *filename)
{
	Glib::RefPtr<Gdk::Colormap>	colormap = get_default_colormap();
	int w, h;
	mPixmap->get_size(w, h);
	Glib::RefPtr<Gdk::Image> image = mPixmap->get_image(0, 0, w, h);
	Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create(image, colormap, 0, 0, 0, 0, w, h);
	bool ret = true;
	try
	{
		pixbuf->save(filename, "jpeg");
	}
	catch (Glib::FileError e)
	{
		ret = false;
	}
	Draw();
	return ret;
}

// ----------------------------------------------------------------------------

bool View::SaveBMP(const char *filename)
{
	Glib::RefPtr<Gdk::Colormap>	colormap = get_default_colormap();
	int w, h;
	mPixmap->get_size(w, h);
	Glib::RefPtr<Gdk::Image> image = mPixmap->get_image(0, 0, w, h);
	Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create(image, colormap, 0, 0, 0, 0, w, h);
	bool ret = true;
	try
	{
		pixbuf->save(filename, "bmp");
	}
	catch (Glib::FileError e)
	{
		ret = false;
	}
	Draw();
	return ret;
}

// ----------------------------------------------------------------------------
