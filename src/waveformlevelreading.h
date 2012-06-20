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
#define WAVEFORM_LEVEL_READING(obj)     (G_TYPE_CHECK_INSTANCE_CAST ((obj), WAVEFORM_TYPE_LEVEL_READING, WaveformLevelReading))
#define WAVEFORM_IS_LEVEL_READING(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WAVEFORM_TYPE_LEVEL_READING))
#define WAVEFORM_LEVEL_READING_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WAVEFORM_TYPE_LEVEL_READING, WaveformLevelReadingClass))
#define WAVEFORM_IS_LEVEL_READING_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WAVEFORM_TYPE_LEVEL_READING))
#define WAVEFORM_LEVEL_READING_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WAVEFORM_TYPE_LEVEL_READING, WaveformLevelReadingClass))

typedef struct _WaveformLevelReading        WaveformLevelReading;
typedef struct _WaveformLevelReadingClass   WaveformLevelReadingClass;

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
  GObject parent_instance;

  /* instance members */
  guint64 time;
  GArray *levels;
};

struct _WaveformLevelReadingClass
{
  GObjectClass parent_class;
  /* class members */
};

GType waveform_level_reading_get_type (void);

/*
 * Method definitions.
 */

#endif /* __WAVEFORM_LEVEL_READING_H__ */