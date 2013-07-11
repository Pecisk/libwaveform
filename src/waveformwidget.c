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
	gfloat default_zoom_level, current_zoom_level, max_zoom_level, min_zoom_level;
	gint zoom_level_step;
	gfloat default_reading_interval;
	gint min_wave_threshold, max_wave_threshold;
};

static void waveform_drawing_class_init(WaveformDrawingClass *klass);
static void waveform_drawing_init(WaveformDrawing *waveform);
static void waveform_drawing_dispose(GObject *object);
static void waveform_drawing_finalize(GObject *object);
gboolean waveform_drawing_waveform(GtkWidget *widget, GdkRectangle cairoClipArea);
gboolean waveform_drawing_draw(GtkWidget *widget, cairo_t *cr);
// Zoom in and out



G_DEFINE_TYPE (WaveformDrawing, waveform_drawing, GTK_TYPE_DRAWING_AREA);


gboolean waveform_drawing_zoom_out(WaveformDrawing *waveform) {
	WaveformDrawing *self = (WaveformDrawing*)waveform;
	gfloat new_current_zoom_level = self->priv->current_zoom_level / 2;
	if(new_current_zoom_level < self->priv->min_zoom_level)
		return FALSE;
	self->priv->current_zoom_level = new_current_zoom_level;
	g_message("Zoom out: %f", self->priv->current_zoom_level);

	// set widget size accordingly
	// first get current height of widget
	// FIXME This code sauses segfault, must figure out why - at least for clarity sake
	// GtkRequisition *size_req;
	// gtk_widget_get_allocation((GtkWidget*)self, size_req);
	
	// calculcate width - it's length of the piece / zoom level (which is in sec, but we need ns, so multiply)
	gint width = waveform_data_get_length(self->priv->data)/(self->priv->current_zoom_level*1000000000);
	// set new size request with caluclated width
	gtk_widget_set_size_request ((GtkWidget*)self, width, gtk_widget_get_allocated_height((GtkWidget*)self));
	gtk_widget_queue_draw((GtkWidget*)waveform);
	return TRUE;
}

gboolean waveform_drawing_zoom_in(WaveformDrawing *waveform) {
	WaveformDrawing *self = (WaveformDrawing*)waveform;
	gfloat new_current_zoom_level = self->priv->current_zoom_level * 2;
	if(new_current_zoom_level > self->priv->max_zoom_level)
		return FALSE;
	self->priv->current_zoom_level = new_current_zoom_level;
	g_message("Zoom in: %f", self->priv->current_zoom_level);
	
	// set widget size accordingly
	// calculcate width - it's length of the piece / zoom level (which is in sec, but we need ns, so multiply)
	gint width = waveform_data_get_length(self->priv->data)/(self->priv->current_zoom_level*1000000000);
	// set new size request with caluclated width
	gtk_widget_set_size_request ((GtkWidget*)self, width, gtk_widget_get_allocated_height((GtkWidget*)self));

	gtk_widget_queue_draw((GtkWidget*)waveform);
	return TRUE;
}

gboolean waveform_drawing_zoom_default(WaveformDrawing *waveform) {
	WaveformDrawing *self = (WaveformDrawing*)waveform;
	self->priv->current_zoom_level = self->priv->default_zoom_level;

	// set widget size accordingly
	// calculcate width - it's length of the piece / zoom level (which is in sec, but we need ns, so multiply)
	gint width = waveform_data_get_length(self->priv->data)/(self->priv->current_zoom_level*1000000000);
	// set new size request with caluclated width
	gtk_widget_set_size_request ((GtkWidget*)self, width, gtk_widget_get_allocated_height((GtkWidget*)self));
	
	gtk_widget_queue_draw((GtkWidget*)waveform);
	return TRUE;		
}

gboolean waveform_drawing_set_zoom(WaveformDrawing *waveform, gfloat max_zoom_level, gfloat min_zoom_level, gfloat def_zoom_level, gint zoom_level_step)
{
	// first check if values are negative
	if(max_zoom_level <= 0 || min_zoom_level <= 0 || def_zoom_level <= 0 || zoom_level_step <= 0) {
		g_message("One of zoom variables are negative or null. Not changing.");
		// FIXME there's should be GError return
		return FALSE;
	}
	// FIXME do a validity check for "nice" values - compare it to used min_wave_threshold and max_wave_threshold
	WaveformDrawing *self = (WaveformDrawing*)waveform;
	self->priv->max_zoom_level = max_zoom_level;
	self->priv->min_zoom_level = min_zoom_level;
	self->priv->default_zoom_level = def_zoom_level;
	// reset current zoom level to default and redraw
	waveform_drawing_zoom_default(waveform);
	return TRUE;
}

static void
waveform_drawing_init(WaveformDrawing *self)
{
self->priv = WAVEFORM_DRAWING_GET_PRIVATE (self);
self->priv->data = NULL;
self->priv->cacheArea.x = 0;
self->priv->cacheArea.y = 0;
self->priv->cacheArea.width = 0;
self->priv->cacheArea.height = 0;
self->priv->sourceSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 0, 0);
// default zoom level 30 pixels per second, to satisfy 3 pixel per wave requirement
self->priv->current_zoom_level = 30;
self->priv->max_zoom_level = 1000;
self->priv->min_zoom_level = 0.125;
self->priv->zoom_level_step = 2;

// FIXME reading interval for calculcations, must be stored somewhere else
self->priv->default_reading_interval = 0.1;
// default min and max threshold - FIXME this really should be changable from different
// implementations of waveforms
self->priv->min_wave_threshold = 2;
self->priv->max_wave_threshold = 6;
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
	GdkRectangle cairoClipArea = {0, 0, 0, 0};
	gboolean fits = gdk_cairo_get_clip_rectangle(cr, &cairoClipArea);
	g_message("Cairo Clip: %i %i %i %i", cairoClipArea.x, cairoClipArea.y, cairoClipArea.width, cairoClipArea.height);
	// if waveform isn't showed, don't care to draw it
	if(fits == FALSE) {
		g_message("FALSE");
		return TRUE;
	}
	if(fits == TRUE)
		g_message("TRUE");
		
	// FIXME check if clip area fits into cacheArea
	gboolean  success = FALSE;
	if((cairoClipArea.x < self->priv->cacheArea.x) || (cairoClipArea.x + cairoClipArea.width > self->priv->cacheArea.x + self->priv->cacheArea.width)) {
		success = waveform_drawing_waveform(widget, cairoClipArea);
	}
	cairo_set_source_surface(cr, self->priv->sourceSurface, self->priv->cacheArea.x, self->priv->cacheArea.y);
	cairo_paint(cr);
	return TRUE;
}

gboolean waveform_drawing_waveform(GtkWidget *widget, GdkRectangle cairoClipArea)
{
	WaveformDrawing *self = WAVEFORM_DRAWING(widget);

	// get allocated dimensions
	GtkAllocation allocatedArea = {0, 0, 0, 0};
	
	gtk_widget_get_allocation(widget, &allocatedArea);
	int width = allocatedArea.width;
	int height = allocatedArea.height;
	g_message("Allocation: %i %i %i %i", allocatedArea.x, allocatedArea.y, width, height);

	//// We take tripple of exposed area and set it as cached area
	//int fake_x = cairoClipArea.x - cairoClipArea.width;
	//int fake_width = cairoClipArea.width*3;
	//// If we are at the beginging, cachedArea.x = 0
	//if(fake_x < 0)
		//fake_x = 0;
	//int fake_y = allocatedArea.y;
	//int fake_height = allocatedArea.height;
	//g_message("Proposed variables for cache: %i %i %i %i", fake_x, fake_y, fake_width, fake_height);
	//// FIXME what to do if allocated width is smaller than three times clipped one?
	
	// We take tripple of exposed area and set it as cached area
	self->priv->cacheArea.x = cairoClipArea.x - cairoClipArea.width;
	self->priv->cacheArea.width = cairoClipArea.width*3;
	// If we are at the beginging, cachedArea.x = 0
	if(self->priv->cacheArea.x < 0)
		self->priv->cacheArea.x = 0;
	self->priv->cacheArea.y = allocatedArea.y;
	self->priv->cacheArea.height = allocatedArea.height;
	
	// This is current way *********************************************
	//self->priv->cacheArea.width = allocatedArea.width;
	//self->priv->cacheArea.height = allocatedArea.height;
	//self->priv->cacheArea.x = allocatedArea.x;
	//self->priv->cacheArea.y = allocatedArea.y;
	
	// creating new surface to draw on
	self->priv->sourceSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, self->priv->cacheArea.width, self->priv->cacheArea.height);
	// to draw we need new context
	cairo_t *context = cairo_create(self->priv->sourceSurface);

	// drawing white background 
	cairo_set_line_width (context, 2.0);
	cairo_set_antialias (context, CAIRO_ANTIALIAS_SUBPIXEL);
	cairo_rectangle(context, 0, 0, allocatedArea.width, allocatedArea.height);

	const GdkRGBA background_color = {1.0, 1.0, 1.0, 1.0};
	gdk_cairo_set_source_rgba(context, &background_color);
	cairo_fill(context);
	
	// initialise coordinates
	gfloat x = 0.0;
	gfloat peak = 0.0;
	
	// if there's WaveformData to draw, let's do it
	if(self->priv->data == NULL) {
		g_message("There is no data.");
		// FIXME how to show nicely that there's no data?
		return TRUE;
	}
	else if(self->priv->data != NULL) {
		g_message("Data is available.");
		// get data model
		WaveformData *data_model = WAVEFORM_DATA(self->priv->data);
		// find a suitable reading interval
		gfloat reading_interval = self->priv->default_reading_interval;
		// this will be step how much wide wave will be
		gfloat pixels_per_reading = 0.0;

		do
		{
			gfloat readings_per_second = 1/reading_interval;
			pixels_per_reading = self->priv->current_zoom_level/readings_per_second;
			g_message("Loop readings_per_second %f reading_interval %f", readings_per_second, reading_interval);
			// checking if wave is not too narrow or too wide
			if(pixels_per_reading < (gfloat)self->priv->min_wave_threshold)
			{
				// if it's too narrow, we should increase reading interval
				reading_interval = reading_interval * 2;
			} else if(pixels_per_reading > (gfloat)self->priv->max_wave_threshold)
			{
				// if it's too wide, we should decrease reading interval
				reading_interval = reading_interval / 2;
			} else
				break;
		}
		while(1);
		
		// now we have pixels_per_reading as step for drawing wave, and reading_interval for interval between readings
		g_message("current_zoom_level %f pixels_per_reading %f reading_interval %f", self->priv->current_zoom_level, pixels_per_reading, reading_interval);
		// get linked list of data
		GList *data = waveform_data_get(data_model);
		// do a loop while we can read data linked list
		data = g_list_first(data);

		// move cairo cursor to the begining
		cairo_move_to(context, 0.0, 0.0);

		
		if(reading_interval > self->priv->default_reading_interval)
		{
		// *********************************************************************
		// if we have reading interval bigger than default
		g_message("reading_interval %f self->priv->default_reading_interval %f", reading_interval, self->priv->default_reading_interval);
		// how many default intervals we have to accumulate to get current interval for zoom
		// FIXME what to do if we don't get clean and nice integer?
		int interval_bunch = (int)(reading_interval/self->priv->default_reading_interval);
		int new_interval_bunch = 0;
		int interval_bunch_sum = 0;
			do {
			// if we start over, reset bunch of intervals
			if(new_interval_bunch == 0) {
				new_interval_bunch = interval_bunch;
				interval_bunch_sum = 0;
			}
			// first get reading
			//g_message("Get a reading.");
			WaveformLevelReading *reading = (WaveformLevelReading*)data->data;
			// then array of chanel readings
			GArray *levels = (GArray*)reading->levels;
			// level of first channel - FIXME this should be configurable
			gdouble level = g_array_index(levels, gdouble, 0);
			
			// convert to coordinates using Jokosher method for creating int from float values
			gdouble decibel_range = 80;
			// if level is maximum negative floated number, we crop it to -decibel_range
			if(level == -DBL_MAX)
				level = 0 - decibel_range;

			level = (gdouble)fmin(level, (gdouble)decibel_range);
			level = (gdouble)fmax(level, (gdouble)-decibel_range);
			level = level  + decibel_range;
			peak = (int)((level/decibel_range) * height);
			
			interval_bunch_sum = interval_bunch_sum + peak;
			new_interval_bunch--;

			// if have collected sum of bunch of intervals
			// let's draw
			if(new_interval_bunch == 0) {
				interval_bunch_sum = (int)round(interval_bunch_sum/interval_bunch);
				cairo_line_to(context, x, interval_bunch_sum);
				// increase x coordinates - adding step
				x = x + pixels_per_reading;
				// currently if we reach end of allocated space, break from loop
				// otherwise countinue until data is empty
				// FIXME really should not be a problem if proper drawing size is allocated
				if((gint)x > self->priv->cacheArea.width)
					break;
				// move cairo cursor back last drawing, so we can start from there
				cairo_move_to(context, x-pixels_per_reading, interval_bunch_sum);
			}
			data = g_list_next(data);
			} while (data != NULL);
		} else if(reading_interval == self->priv->default_reading_interval) {
		// *********************************************************************
		// if we are using default reading interval
			do {
			// first get reading
			//g_message("Get a reading.");
			WaveformLevelReading *reading = (WaveformLevelReading*)data->data;
			// then array of chanel readings
			GArray *levels = (GArray*)reading->levels;
			// level of first channel - FIXME this should be configurable
			gdouble level = g_array_index(levels, gdouble, 0);
			//g_message("Level #1 %f", level);
			
			// convert to coordinates using Jokosher method for creating int from float values
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

			// increase x coordinates - adding step
			x = x + pixels_per_reading;
			
			//g_message("x coordinates %i", (gint)pixels_per_reading);
			// currently if we reach end of allocated space, break from loop
			// otherwise countinue until data is empty
			if((gint)x > self->priv->cacheArea.width)
				break;
			// move cairo cursor back last drawing, so we can start from there
			cairo_move_to(context, x-pixels_per_reading, peak);
			data = g_list_next(data);
			} while (data != NULL);
		} else if(reading_interval < self->priv->default_reading_interval) {
		// *********************************************************************	
		// if we have smaller than default reading interval
			do {
			// first get reading
			//g_message("Get a reading.");
			WaveformLevelReading *reading = (WaveformLevelReading*)data->data;
			// then array of chanel readings
			GArray *levels = (GArray*)reading->levels;
			// level of first channel - FIXME this should be configurable
			gdouble level = g_array_index(levels, gdouble, 0);
			//g_message("Level #1 %f", level);
			
			// convert to coordinates using Jokosher method for creating int from float values
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

			// increase x coordinates - adding step
			x = x + pixels_per_reading;
			
			//g_message("x coordinates %i", (gint)pixels_per_reading);
			// currently if we reach end of allocated space, break from loop
			// otherwise countinue until data is empty
			if((gint)x > self->priv->cacheArea.width)
				break;
			// move cairo cursor back last drawing, so we can start from there
			cairo_move_to(context, x-pixels_per_reading, peak);
			data = g_list_next(data);
			} while (data != NULL);
		}
    
    //experimental levels gradient fill - this is rewrite of Jokosher default waveform
	cairo_pattern_t *gradient = cairo_pattern_create_linear(0.0, 0.0, 0, self->priv->cacheArea.height);
	cairo_pattern_add_color_stop_rgba(gradient, 0.2, 114./255, 159./255, 207./255, 1);
	cairo_pattern_add_color_stop_rgba(gradient, 1, 52./255, 101./255, 164./255, 1);
	cairo_set_source(context, gradient);
	cairo_fill_preserve(context);

	//levels path (on top of the fill)
	const GdkRGBA drawing_color = {0.2, 0.8, 0.0, 1.0};
	gdk_cairo_set_source_rgba (context, &drawing_color);
	cairo_set_line_join (context, CAIRO_LINE_JOIN_ROUND);
	cairo_set_line_width (context, 2.0);
	cairo_stroke (context);
    // end of if there is data to draw
    }
    
    cairo_destroy (context);
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
	
	// set widget size accordingly
	// first get current height of widget
	//GtkRequisition *size_req;
	//gtk_widget_size_request ((GtkWidget*)self, size_req);
	// calculcate width - it's length of the piece / zoom level (which is in sec, but we need ns, so multiply)
	guint width = (guint)(((guint)(waveform_data_get_length(self->priv->data)/1000000000))*self->priv->current_zoom_level);
	// set new size request with caluclated width
	g_message("width=length/zoom_evel %i=%lli", width, waveform_data_get_length(self->priv->data));
	//gtk_widget_set_size_request ((GtkWidget*)self, width, size_req->height);
	gtk_widget_set_size_request ((GtkWidget*)self, width, -1);
	return TRUE;
}
