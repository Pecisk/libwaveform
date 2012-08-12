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

G_DEFINE_BOXED_TYPE (WaveformLevelReading, waveform_level_reading, waveform_level_reading_ref, waveform_level_reading_unref);

GType waveform_level_reading_get_type (void);
// FIXME implement refcount variable, and ref and unref functions

WaveformLevelReading *
waveform_level_reading_ref (WaveformLevelReading * reading)
{
  g_return_val_if_fail (reading != NULL, NULL);
  g_return_val_if_fail (WAVEFORM_LEVEL_READING_REFCOUNT_VALUE (reading) > 0, NULL);
  g_atomic_int_inc (&reading->refcount);

  return reading;
}

void
waveform_level_reading_unref (WaveformLevelReading * reading)
{
  g_return_if_fail (reading != NULL);
  g_return_if_fail (WAVEFORM_LEVEL_READING_REFCOUNT_VALUE (reading) > 0);
  if (G_UNLIKELY (g_atomic_int_dec_and_test (&reading->refcount)))
    g_slice_free(WaveformLevelReading, reading);
	
}

WaveformLevelReading * waveform_level_reading_new (void)
{
	WaveformLevelReading *reading = g_slice_new(WaveformLevelReading);
	reading->refcount = 1;
	reading->start_time = 0;
	reading->end_time = 0;
	reading->levels = g_array_sized_new (FALSE, TRUE, sizeof (gfloat), 2);
	// initialise subreadings with g_ptr_array_new only then when neccessary
	reading->subreadings = NULL;
	return reading;
}