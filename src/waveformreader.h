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

#ifndef __WAVEFORM_READER_H__
#define __WAVEFORM_READER_H__

#include <glib-object.h>
#include <gst/gst.h>
#include "waveformlevelreading.h"

#define GST_USE_UNSTABLE_API 1

/*
 * Type macros.
 */
#define WAVEFORM_TYPE_READER     (waveform_reader_get_type ())
#define WAVEFORM_READER(obj)     (G_TYPE_CHECK_INSTANCE_CAST ((obj), WAVEFORM_TYPE_READER, WaveformReader))
#define WAVEFORM_IS_READER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WAVEFORM_TYPE_READER))
#define WAVEFORM_READER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WAVEFORM_TYPE_READER, WaveformReaderClass))
#define WAVEFORM_IS_READER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WAVEFORM_TYPE_READER))
#define WAVEFORM_READER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WAVEFORM_TYPE_READER, WaveformReaderClass))

typedef struct _WaveformReader        WaveformReader;
typedef struct _WaveformReaderClass   WaveformReaderClass;
typedef struct _WaveformReaderPrivate WaveformReaderPrivate;


/**
 * WaveformReader:
 *
 * The #WaveformReader structure.
 *
 * Since: 0.1
 **/

struct _WaveformReader
{
  GObject parent_instance;

  /* instance members */
  WaveformReaderPrivate *priv;
  
};

struct _WaveformReaderClass
{
  GObjectClass parent_class;

  /* class members */
  //static GMainLoop *loop;
};

GType waveform_reader_get_type (void);

/*
 * Method definitions.
 */
GList *	waveform_reader_get_levels(WaveformReader *reader, const gchar *file_location);
WaveformReader * waveform_reader_new(void);

#endif /* __WAVEFORM_READER_H__ */