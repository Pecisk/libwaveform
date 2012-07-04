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

#include "waveformdata.h"

#define WAVEFORM_DATA_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), WAVEFORM_TYPE_DATA, WaveformDataPrivate))

struct _WaveformDataPrivate
{
  GList *data;
};

G_DEFINE_TYPE (WaveformData, waveform_data, G_TYPE_OBJECT);

static void
waveform_data_init (WaveformData *waveform_data)
{
	self->priv = WAVEFORM_DATA_GET_PRIVATE (self);
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
}


gboolean
waveform_data_add (WaveformData *model, GList *readings)
{
	// If there is no data, just copy it
	// FIXME do we expect just to copy pointer? Deep copying
	// would require implementation of copy function for WaveformLevelReading
	if(model->priv->data == NULL)
		model->priv->data = &readings;
		return TRUE;
	else {
		// if there is already data here, investigate
		// first compare two linked elements and their time deltas
		GList first = g_list_first(model->priv->data);
		WaveformLevelReading readfirst = WAVEFORM_LEVEL_READING(first->data);
		guint64 old_data_period = readfirst->start_time - readfirst->end_time; 

		GList second = g_list_first(readings);
		WaveformLevelReading readsecond = WAVEFORM_LEVEL_READING(second->data);
		guint64 new_data_period = readsecond->start_time - readsecond->end_time;

		if(readfirst % readsecond == 0)
		{
			// do a modulo to see if it's worth even to look at it
			// if there's leftovers, readsecond doesn't fit n=integer times
			if(readfirst/readsecond == 1)
			{
				// FIXME if they are with equal period, copy over
				// could have option that if there is equal period, you could
				// ignore it like now or copy over
				return TRUE;
			} else {
				// readsecond fits in readfirst, and they are not equal
				// FIXME so we should insert those data
				
			}

		}
	}

	/* TODO: Add public function implementation here */
}
