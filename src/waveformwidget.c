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
  GdkRectangle cacheArea;
  cairo_surface_t *sourceSurface;
};

static void waveform_drawing_class_init(WaveformDrawingClass *klass);
static void waveform_drawing_init(WaveformDrawing *waveform);
static void waveform_drawing_dispose(GObject *object);
static void waveform_drawing_finalize(GObject *object);
gboolean waveform_drawing_waveform(GtkWidget *widget, GdkRectangle cairoClipArea);

gboolean waveform_drawing_draw(GtkWidget *widget, cairo_t *cr);

G_DEFINE_TYPE (WaveformDrawing, waveform_drawing, GTK_TYPE_DRAWING_AREA);

static void
waveform_drawing_init(WaveformDrawing *self)
{
self->priv = WAVEFORM_DRAWING_GET_PRIVATE (self);
self->priv->width = -1;
self->priv->height = -1;
self->priv->data = NULL;
self->priv->cacheArea.x = 0;
self->priv->cacheArea.y = 0;
self->priv->cacheArea.width = 0;
self->priv->cacheArea.height = 0;
self->priv->sourceSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 0, 0);
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

gboolean waveform_drawing_draw(GtkWidget *widget, cairo_t *cr) {

	WaveformDrawing *self = (WaveformDrawing*)widget;
	GdkRectangle cairoClipArea;
	gboolean fits = gdk_cairo_get_clip_rectangle(cr, &cairoClipArea);
	
	// if waveform isn't showed, don't care to draw it
	if(fits == FALSE)
		g_message("FALSE");
	if(fits == TRUE)
		g_message("TRUE");
	gboolean success = waveform_drawing_waveform(widget, cairoClipArea);
	cairo_set_source_surface(cr, self->priv->sourceSurface, self->priv->cacheArea.x, self->priv->cacheArea.y);
	cairo_paint(cr);
	return TRUE;
}

gboolean waveform_drawing_waveform(GtkWidget *widget, GdkRectangle cairoClipArea)
{
	g_message("drawing.");
	WaveformDrawing *self = WAVEFORM_DRAWING(widget);

	// get allocated dimensions
	GtkAllocation allocatedArea;
	allocatedArea.width = 0;
	allocatedArea.height = 0;
	allocatedArea.x = 0;
	allocatedArea.y = 0;
	
	gtk_widget_get_allocation(widget, &allocatedArea);
	int width = allocatedArea.width;
	int height = allocatedArea.height;
	//g_message("Allocation: %i %i", width, height);

	// We take tripple of exposed area and set it as cached area
	self->priv->cacheArea.x = cairoClipArea.x - allocatedArea.width;
	self->priv->cacheArea.y = allocatedArea.y;
	self->priv->cacheArea.width = allocatedArea.width*3;
	self->priv->cacheArea.height = allocatedArea.height;

	// creating new surface to draw on
	self->priv->sourceSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, self->priv->cacheArea.width, self->priv->cacheArea.height);
	// to draw we need new context
	cairo_t *context = cairo_create(self->priv->sourceSurface);

	// drawing white background 
	cairo_set_line_width (context, 2.0);
	cairo_set_antialias (context, CAIRO_ANTIALIAS_SUBPIXEL);
	cairo_rectangle(context, 0, 0, self->priv->cacheArea.width, self->priv->cacheArea.height);

	const GdkRGBA background_color = {1.0, 1.0, 1.0, 1.0};
	gdk_cairo_set_source_rgba(context, &background_color);
	cairo_fill(context);
	
	// initialise x coordinates
	int x = 1;
	cairo_move_to (context, 1.0, 1.0);
	
	// if there's WaveformData to draw, let's do it
	if(self->priv->data == NULL)
		g_message("There is no data.");
	if(self->priv->data != NULL) {
		g_message("Data is available.");
		// get data model
		WaveformData *data_model = WAVEFORM_DATA(self->priv->data);
		// get linked list of data
		GList *data = waveform_data_get(data_model);
		// do a loop while we can read data linked list
		data = g_list_first(data);
		int peak = 0;
	  	do {
			 cairo_move_to(context, x-1, peak);
			// first get reading
			//g_message("Get a reading.");
			WaveformLevelReading *reading = (WaveformLevelReading*)data->data;
			// then array of chanel readings
			GArray *levels = (GArray*)reading->levels;
			// level of channel
			gdouble level = g_array_index(levels, gdouble, 0);
			//g_message("Level #1 %f", level);
			// convert to coordinates
			// FIXME temporary fix with using Jokosher method
			gdouble decibel_range = 80;
			// if level is maximum negative floated number, we crop it to -decibel_range
			if(level == -DBL_MAX)
				level = 0 - decibel_range;
			//g_message("Level #2 %f", level);
			level = (gdouble)fmin(level, (gdouble)decibel_range);
			level = (gdouble)fmax(level, (gdouble)-decibel_range);
			level = level  + decibel_range;
			peak = (int)((level/decibel_range) * height);
			//g_message("Level #3 %f", level);
			cairo_line_to(context, x, peak);
			// increase x coordinates
			x = x + 5;
			// currently if we reach end of allocated space, break from loop
			// otherwise countinue until data is empty
			if(x > self->priv->cacheArea.width)
				  break;
			data = g_list_next(data);
		  } while (data != NULL);
    }
	//experimental levels gradient fill
	cairo_pattern_t *gradient = cairo_pattern_create_linear(0.0, 0.0, 0, self->priv->cacheArea.height);
	cairo_pattern_add_color_stop_rgba(gradient, 0.2, 114./255, 159./255, 207./255, 1);
	cairo_pattern_add_color_stop_rgba(gradient, 1, 52./255, 101./255, 164./255, 1);
	cairo_set_source(context, gradient);
	cairo_fill_preserve(context);

	//levels path (on top of the fill)
	const GdkRGBA drawing_color = {0.2, 0.8, 0.0, 1.0};
	gdk_cairo_set_source_rgba(context, &drawing_color);
	cairo_set_line_join(context, CAIRO_LINE_JOIN_ROUND);
	cairo_set_line_width(context, 2.0);
	cairo_stroke(context);
	
	return TRUE;
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