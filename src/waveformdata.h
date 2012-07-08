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

#ifndef _WAVEFORMDATA_H_
#define _WAVEFORMDATA_H_

#include <glib-object.h>
#include "waveformlevelreading.h"

G_BEGIN_DECLS

#define WAVEFORM_TYPE_DATA             (waveform_data_get_type ())
#define WAVEFORM_DATA(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), WAVEFORM_TYPE_DATA, WaveformData))
#define WAVEFORM_DATA_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), WAVEFORM_TYPE_DATA, WaveformDataClass))
#define WAVEFORM_IS_DATA(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WAVEFORM_TYPE_DATA))
#define WAVEFORM_IS_DATA_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), WAVEFORM_TYPE_DATA))
#define WAVEFORM_DATA_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), WAVEFORM_TYPE_DATA, WaveformDataClass))

typedef struct _WaveformDataClass WaveformDataClass;
typedef struct _WaveformData WaveformData;
typedef struct _WaveformDataPrivate WaveformDataPrivate;

struct _WaveformDataClass
{
	GObjectClass parent_class;
};

struct _WaveformData
{
	GObject parent_instance;
	WaveformDataPrivate *priv;
};

GType waveform_data_get_type (void) G_GNUC_CONST;
gboolean waveform_data_add (WaveformData *data, GList *readings);

G_END_DECLS

#endif /* _WAVEFORMDATA_H_ */
