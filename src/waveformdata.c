/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * libwaveform
 * Copyright (C) Peteris Krisjanis 2012 <peteris@pecisk.vsaa.lv>
 * 
libwaveform is free software: you can redistribute it and/or modify it
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

#include <glib-object.h>
#include "waveformlevelreading.h"
#include "waveformdata.h"


#define WAVEFORM_DATA_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), WAVEFORM_TYPE_DATA, WaveformDataPrivate))

struct _WaveformDataPrivate
{
  // pointer to data structure
  GList *readings;
  // Current active element of rough reading 
  GList *active_rough;
  // Current active element of detailed reading of the list you process
  GList *active_detailed;
  // System path of file name
  gchar *file_name;	
};

G_DEFINE_TYPE (WaveformData, waveform_data, G_TYPE_OBJECT);

void add_each_subreading(gpointer data, gpointer user_data);

static void
waveform_data_init (WaveformData *self)
{
	self->priv = WAVEFORM_DATA_GET_PRIVATE (self);
	self->priv->active_rough = NULL;
	self->priv->active_detailed = NULL;
	self->priv->readings = NULL;

}

static void
waveform_data_finalize (GObject *object)
{
	/* TODO: Add deinitalization code here */

	G_OBJECT_CLASS (waveform_data_parent_class)->finalize (object);
}

static void
waveform_data_class_init (WaveformDataClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);
	GObjectClass* parent_class = G_OBJECT_CLASS (klass);

	object_class->finalize = waveform_data_finalize;

	g_type_class_add_private (klass, sizeof (WaveformDataPrivate));
}

/**
 * waveform_data_new:
 *
 * Creates #WaveformData which holds audio level information in data structure.
 *
 * Returns: (transfer full): The new #WaveformData
 *
 * Since: 0.1
 */

WaveformData * waveform_data_new(void) {
	return g_object_new(WAVEFORM_TYPE_DATA, NULL);
}

/**
 * waveform_data_add:
 * @self: pointer to #WaveformData object which holds audio level information in data structure.
 * @readings: (transfer none) (element-type Waveform.LevelReading): a pointer to a #GList of readings from waveform_reader_get_levels.
 *
 * Adds list of #WaveformLevelReading to #WaveformData structure.
 *
 * Returns: #gboolean value of success or failure of adding level readings to #WaveformData structure.
 *
 * Since: 0.1
 */

gboolean
waveform_data_add (WaveformData *self, GList *readings)
{
	// if readings is null, return true and don't change a thing
	g_message("Passed no data, no changes.");
	if(readings == NULL)
		return TRUE;
	g_message("Passed some data, adding.");
	// If there is no data, just copy it
	// FIXME do we expect just to copy pointer? Deep copying
	// would require implementation of copy function for WaveformLevelReading
	if(self->priv->readings == NULL) {
		self->priv->readings = g_list_copy(readings);
		GList *g = g_list_first(self->priv->readings);
		g_message("Data inserted %"G_GUINT64_FORMAT" %"G_GUINT64_FORMAT, (guint64)((WaveformLevelReading*)g->data)->start_time, (guint64)((WaveformLevelReading*)g->data)->end_time);
		return TRUE;
		}
	else {
		// if there is already data here, investigate
		// first compare two linked elements and their time deltas
		GList *base = g_list_first(self->priv->readings);
		WaveformLevelReading *read_base = (WaveformLevelReading*)base->data;
		guint64 base_period = read_base->end_time - read_base->start_time; 

		GList *new = g_list_first(readings);
		WaveformLevelReading *read_new = (WaveformLevelReading*)new->data;
		guint64 new_period = read_new->end_time - read_new->start_time;
		g_message("Periods: %"G_GUINT64_FORMAT" %"G_GUINT64_FORMAT, base_period, new_period);
		if(base_period % new_period == 0)
		{
			g_message("This is divide without modulo.\n");
			// do a modulo to see if it's worth even to look at it
			// if there's leftovers, read_new doesn't fit n=integer times
			if(base_period/new_period == 1)
			{
				g_message("If we are here, base_period and new_period are equal.\n");
				// FIXME if they are with equal period, copy over
				// could have option that if there is equal period, you could
				// ignore it like now or copy over
				// anyway, we done here, and operation is success
				return TRUE;
			} else {
				g_message("This is subreadings.\n");

			    // at the beginging model->priv->active_rough is NULL, get actual
				// first element of the list from model->priv->readings
				if(self->priv->active_rough == NULL)
					self->priv->active_rough = g_list_first(self->priv->readings);
				
				//new_period fits in base_period, and they are not equal
				// so we insert those data
				g_list_foreach(readings, add_each_subreading, self);
				return TRUE;
			}

		} else
		return FALSE;
		// read_new doesn't fit in read_base fully, so we can't put it there
	}
	// end of waveform_data_add
}

/**
 * waveform_data_get:
 * @self: pointer to #WaveformData object which holds audio level information in data structure.
 *
 * Returns: (transfer none) (element-type Waveform.LevelReading): #GList of data model structure.
 *
 * Since: 0.1
 */

GList * waveform_data_get(WaveformData *self) {
	//GList *g = g_list_first(self->priv->readings);
	//return g;
	return (GList*)self->priv->readings;
}

void add_each_subreading(gpointer data, gpointer user_data) {
	// first, let's get out pointers
	g_message("add_each_reading");
	WaveformLevelReading *reading = (WaveformLevelReading*)data;
	WaveformData *self = WAVEFORM_DATA ((GObject*)user_data);
	
	// let's get current active rough reading and see if detailed reading
	// fits there. If not, iterate to next and nullify active_detailed

	// need to initialise it first for while check
	WaveformLevelReading *rough_reading = NULL;

	// if we have reach the end of the list, then simply return with discard message
	if(self->priv->active_rough == NULL) {
		g_message("End of rough reading list - discard subreading.");
		return;
	}
	
	do {
	// FIXME what if subreading is wrong entirerly - issue error
	rough_reading = (WaveformLevelReading*)self->priv->active_rough->data;
	g_message("Vajadzetu %"G_GUINT64_FORMAT" <= %"G_GUINT64_FORMAT" %"G_GUINT64_FORMAT" <= %"G_GUINT64_FORMAT, rough_reading->start_time, reading->start_time, reading->end_time, rough_reading->end_time);
	// if reading doesn't fit in rough_reading, iterate to next element in list
	// and nullify active_detailed as we are in new rough period
	if(!(rough_reading->start_time <= reading->start_time && reading->end_time  <= rough_reading->end_time))
		{
			g_message("Hope to next rough");
			self->priv->active_rough = g_list_next(self->priv->active_rough);
			// reverse linked list because we did prepend
			self->priv->active_detailed = g_list_reverse(self->priv->active_detailed);
			// clean it up, we moving on next rough reading
			self->priv->active_detailed = NULL;
			// if we have reach the end of the list, then simply return with discard message
			if(self->priv->active_rough == NULL) {
				g_message("End of rough reading list - discard subreading.");
				return;
			}
			
		}
	} while(!(rough_reading->start_time <= reading->start_time && reading->end_time <= rough_reading->end_time));
	
	// check if we have active_detailed shortcut, because if it's there,
	// activated, then we don't need to lookup where to put it
	if(self->priv->active_detailed != NULL) {
		// just add reading to active detailed subreading list
		self->priv->active_detailed = g_list_prepend(self->priv->active_detailed, reading); 
	} else {
		// if you don't know where to store it, find it out
		// if there is no subreadings pointer array, create it
		// and add reading to newly created list
		if(((WaveformLevelReading*)self->priv->active_rough->data)->subreadings == NULL)
			{
				// if subreadings are NULL, create it, create new GList and add it
				((WaveformLevelReading*)self->priv->active_rough->data)->subreadings = g_ptr_array_sized_new(3);
				GList *new_subreadings_list = NULL;
				new_subreadings_list = g_list_append(new_subreadings_list, reading);
				g_ptr_array_add(((WaveformLevelReading*)self->priv->active_rough->data)->subreadings, (gpointer)new_subreadings_list);
				// define new list as active_detailed
				self->priv->active_detailed = new_subreadings_list;
			} else {
				// if subreadings has something, first let's go trough
				// all lists it has to see if someone fits. This shouldn't
				// happen generally as we don't expect incomplete subreading
				// lists, but in case...
				int i;
				for(i=0;i<((WaveformLevelReading*)self->priv->active_rough->data)->subreadings->len;i++)
					{
						GList *temp_list = (GList*)g_ptr_array_index(((WaveformLevelReading*)self->priv->active_rough->data)->subreadings,i);
						WaveformLevelReading *temp_reading = (WaveformLevelReading*)temp_list->data;
						// if equal, this is right list, prepend reading and make active_detailed as new default
						if((temp_reading->end_time - temp_reading->start_time) == (reading->end_time - reading->start_time)) {
							self->priv->active_detailed = g_list_prepend(temp_list, reading);
							break;
						}
					}
				// if active_detailed still NULL, that means default action - create new list and append reading
				if(self->priv->active_detailed == NULL) {
					GList *new_subreadings_list = NULL;
					new_subreadings_list = g_list_append(new_subreadings_list, reading);
					g_ptr_array_add(((WaveformLevelReading*)self->priv->active_rough->data)->subreadings, (gpointer)new_subreadings_list);
					// define new list as active_detailed
					self->priv->active_detailed = new_subreadings_list;
				}
			}	
			
	}
	g_message("finished.\n");
}

void waveform_data_set_file_name (WaveformData *self, gchar *name) {
	// FIXME this function will provide file name which data is stored by WaveformData
	// FIXME this is temporary solution for passing name from reader to data
	// FIXME what to do if it has valid file name already?
	// FIXME file name validity? same for get_levels
	self->priv = WAVEFORM_DATA_GET_PRIVATE (self);
	self->priv->file_name = name;
	return;
}