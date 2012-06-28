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

// FIXME implement refcount variable, and ref and unref functions

WaveformLevelReading *  waveform_level_reading_ref		(WaveformLevelReading *reading);
void					waveform_level_reading_unref	(WaveformLevelReading *reading);

WaveformLevelReading *
waveform_level_reading_ref (WaveformLevelReading * reading)
{
  g_return_val_if_fail (reading != NULL, NULL);
  /* we can't assert that the refcount > 0 since the _free functions
   * increments the refcount from 0 to 1 again to allow resurecting
   * the object
   g_return_val_if_fail (mini_object->refcount > 0, NULL);
   */
	reading->refcount = reading->refcount + 1;

  g_atomic_int_inc (&reading->refcount);

  return reading;
}

void
gst_mini_object_unref (GstMiniObject * mini_object)
{
  g_return_if_fail (mini_object != NULL);
	reading->refcount = reading->refcount - 1;
  g_return_if_fail (mini_object->refcount > 0);
	//FIXME what happens after we have reached 0 refs
    }
  }
}