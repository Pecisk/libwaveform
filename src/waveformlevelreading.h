/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * lib.h
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

/* inclusion guard */
#ifndef __WAVEFORM_LEVEL_READING_H__
#define __WAVEFORM_LEVEL_READING_H__

#include <glib-object.h>

/*
 * Type macros.
 */

#define WAVEFORM_TYPE_LEVEL_READING     (waveform_level_reading_get_type ())
#define WAVEFORM_LEVEL_READING_REFCOUNT(obj)           (((WaveformLevelReading*)(obj))->refcount)
#define WAVEFORM_LEVEL_READING_REFCOUNT_VALUE(obj)     (g_atomic_int_get (&((WaveformLevelReading*)(obj))->refcount))

typedef struct _WaveformLevelReading        WaveformLevelReading;

/**
 * WaveformLevelReading:
 * @time: end time of reading in nanoseconds.
 * @levels: #GArray of gfloat level readings for each channel.
 * 
 * The #WaveformLevelReading structure.
 *
 * Since: 0.1
 **/

struct _WaveformLevelReading
{
  GType type;

  /*< public >*/
  gint refcount;

  guint64 start_time;
  guint64 end_time;
  GArray *levels;
  
  
};

GType waveform_level_reading_get_type (void);

/*
* Method definitions.
*/

WaveformLevelReading *  waveform_level_reading_ref		(WaveformLevelReading *reading);
void					waveform_level_reading_unref	(WaveformLevelReading *reading);

#endif /* __WAVEFORM_LEVEL_READING_H__ */