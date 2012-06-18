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

#include <glib-object.h>
#include "waveformlevelreading.h"

G_DEFINE_TYPE (WaveformLevelReading, waveform_level_reading, G_TYPE_OBJECT);

static void waveform_level_reading_dispose (GObject *gobject);
static void waveform_level_reading_finalize (GObject *gobject);

static void
waveform_level_reading_class_init (WaveformLevelReadingClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->dispose = waveform_level_reading_dispose;
  gobject_class->finalize = waveform_level_reading_finalize;

}

static void
waveform_level_reading_init (WaveformLevelReading *self)
{
  //self->priv = MAMAN_BAR_GET_PRIVATE (self); 

  self->time = 0;
  self->levels = g_array_new (FALSE, FALSE, sizeof (gfloat));
}

static void
waveform_level_reading_dispose (GObject *gobject)
{
  WaveformLevelReading *self = WAVEFORM_LEVEL_READING (gobject);
	
  /* Chain up to the parent class */
  G_OBJECT_CLASS (waveform_level_reading_parent_class)->dispose (gobject);
}

static void
waveform_level_reading_finalize (GObject *gobject)
{
  WaveformLevelReading *self = WAVEFORM_LEVEL_READING (gobject);

	// FIXME should I free all of it? I think yes - when we release it, we release it
	g_array_free(self->levels, TRUE);
		
  /* Chain up to the parent class */
  G_OBJECT_CLASS (waveform_level_reading_parent_class)->finalize (gobject);
}

