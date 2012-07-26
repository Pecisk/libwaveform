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

#ifndef __WAVEFORMWIDGET_H
#define __WAVEFORMWIDGET_H

#include <gtk/gtk.h>
#include <cairo.h>
#include <glib-object.h>
#include "waveformdata.h"
#include <math.h>

G_BEGIN_DECLS

#define WAVEFORM_TYPE_DRAWING             		(waveform_drawing_get_type ())
#define WAVEFORM_DRAWING(obj)						(G_TYPE_CHECK_INSTANCE_CAST((obj), WAVEFORM_TYPE_DRAWING, WaveformDrawing))
#define WAVEFORM_DRAWING_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((obj), WAVEFORM_DRAWING, WaveformDrawingClass))
#define WAVEFORM_IS_DRAWING(obj) 				(G_TYPE_CHECK_INSTANCE_TYPE((obj), WAVEFORM_TYPE_DRAWING))
#define WAVEFORM_IS_DRAWING_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), WAVEFORM_TYPE_DRAWING))
#define WAVEFORM_DRAWING_GET_CLASS        (G_TYPE_INSTANCE_GET_CLASS ((obj), WAVEFORM_TYPE_DRAWING, WaveformDrawingClass))

typedef struct _WaveformDrawing WaveformDrawing;
typedef struct _WaveformDrawingClass WaveformDrawingClass;
typedef struct _WaveformDrawingPrivate WaveformDrawingPrivate;

/**
 * WaveformDrawing:
 *
 * The #WaveformDrawing structure.
 *
 * Since: 0.1
 **/

struct _WaveformDrawing {
  GtkDrawingArea parent;
  WaveformDrawingPrivate *priv;
};

struct _WaveformDrawingClass {
  GtkDrawingAreaClass parent_class;
};

GType waveform_drawing_get_type(void);
GtkDrawingArea * waveform_drawing_new(void);
gboolean waveform_drawing_set_model(WaveformDrawing *self, WaveformData *data_model);

G_END_DECLS

#endif /* __WAVEFORMWIDGET_H */