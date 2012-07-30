/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * waveformreader.h
 * Copyright (C) 2012 Peteris Krisjanis <pecisk@gmail.com>
 * 
 * libwaveform is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * libwaveform is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.";
 */

#include "waveformwidget.h"
#include <gtk/gtk.h>
#include <cairo.h>
#include <math.h>

#define WAVEFORM_DRAWING_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), WAVEFORM_TYPE_DRAWING, WaveformDrawingPrivate))

struct _WaveformDrawingPrivate
{
  // pointer to WaveformData model
  WaveformData *data;
  // variables for width and height
  int width;
  int height;
};

static void waveform_drawing_class_init(WaveformDrawingClass *klass);
static void waveform_drawing_init(WaveformDrawing *waveform);
static void waveform_drawing_dispose(GObject *object);
static void waveform_drawing_finalize(GObject *object);

gboolean waveform_drawing_draw(GtkWidget *widget, cairo_t *cr);

G_DEFINE_TYPE (WaveformDrawing, waveform_drawing, GTK_TYPE_DRAWING_AREA);

static void
waveform_drawing_init(WaveformDrawing *self)
{
self->priv = WAVEFORM_DRAWING_GET_PRIVATE (self);
self->priv->width = -1;
self->priv->height = -1;
self->priv->data = NULL;
}

static void
waveform_drawing_class_init(WaveformDrawingClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = waveform_drawing_finalize;
  widget_class->draw = waveform_drawing_draw;

  g_type_class_add_private (klass, sizeof (WaveformDrawingPrivate));
}

static void
waveform_drawing_dispose (GObject *gobject)
{
  G_OBJECT_CLASS (waveform_drawing_parent_class)->dispose (gobject);
}


static void
waveform_drawing_finalize (GObject *object)
{
  G_OBJECT_CLASS (waveform_drawing_parent_class)->finalize (object);
}

GtkDrawingArea * waveform_drawing_new(void)
{
   return g_object_new(WAVEFORM_TYPE_DRAWING, NULL);
}

gboolean waveform_drawing_draw(GtkWidget *widget, cairo_t *cr)
{
   g_message("drawing.");
	// FIXME taking cairo context from given params segfaults
	// temporary fix, geting cairo context from widget window
	//cr = gdk_cairo_create(gtk_widget_get_window(widget));

	// FIXME get allocated dimensions
	GtkAllocation allocation = {-1,-1,-1,-1};
	gtk_widget_get_allocation(widget, &allocation);
	int width = allocation.width;
	int height = allocation.height;
	g_message("Allocation: %i %i", width, height);
	// initialise x coordinate
	int x = 1;

	// cairo stuff
	//cairo_set_line_width (cr, 0.1);
	//cairo_set_source_rgb(cr, 0, 0, 0);
	
	
	// get data from widget
	WaveformDrawing *self = WAVEFORM_DRAWING(widget);
	
	// if there's WaveformData to draw, let's do it
	if(self->priv->data != NULL) {
		g_message("Data is available.");
		// get data model
		WaveformData *data_model = WAVEFORM_DATA(self->priv->data);
		// get linked list of data
		GList *data = waveform_data_get(data_model);
		// do a loop while we can read data linked list
		data = g_list_first(data);
	  	do {
			// first get reading
			WaveformLevelReading *reading = (WaveformLevelReading*)data->data;
			// then array of chanel readings
			GArray *levels = (GArray*)reading->levels;
			// level of channel
			gfloat level = g_array_index(levels, gfloat, 0);
			// convert to coordinates
			// FIXME temporary fix with using Jokosher method
			gfloat decibel_range = 80;
			// if level is maximum negative floated number, we crop it to -decibel_range
			if(level == -DBL_MAX)
				  level = 0 - decibel_range;
			level = (gfloat)fminf(level, (float)decibel_range);
			level = (gfloat)fmaxf(level, (float)-decibel_range);
			level = level  + (gfloat)decibel_range;
			int peak = (int)((level/decibel_range) * height);
			//cairo_line_to(cr, x, peak);
			//cairo_stroke(cr);
			// increase x coordinates
			x++;
			// currently if we reach end of allocated space, break from loop
			// otherwise countinue until data is empty
			if(x > width)
				  break;
			data = g_list_next(data);
		  } while (data != NULL);
    }
 //cairo_destroy(cr);
	return FALSE;
}

/**
 * waveform_drawing_set_model:
 * @self: pointer to #WaveformDrawing widget object.
 * @data_model: pointer to #WaveformData object which holds audio level information in data structure.
 *
 * Returns: #gboolean of success or failure of setting data model.
 *
 * Since: 0.1
 */
gboolean waveform_drawing_set_model(WaveformDrawing *self, WaveformData *data_model) {

	g_return_val_if_fail(self != NULL, FALSE);
	g_return_val_if_fail(WAVEFORM_IS_DRAWING(self), FALSE);
	g_return_val_if_fail(data_model != NULL, FALSE);
	g_return_val_if_fail(WAVEFORM_IS_DATA(data_model), FALSE);

	self->priv->data = data_model;
	return TRUE;
}