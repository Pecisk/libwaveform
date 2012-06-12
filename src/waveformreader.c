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
#include "waveformreader.h"
#include <gst/gst.h>
#include <stdbool.h>
#include <string.h>

#define WAVEFORM_READER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), WAVEFORM_TYPE_READER, WaveformReaderPrivate))

struct _WaveformReaderPrivate
{
  GMainContext *context;
  GMainLoop *loop;
  WaveformLevelReading *reading;
  GList *readings;
};

static gboolean bus_call(GstBus *bus, GstMessage *msg, void *user_data);
static void waveform_reader_finalize (GObject *gobject);
static void waveform_reader_dispose (GObject *gobject);

G_DEFINE_TYPE (WaveformReader, waveform_reader, G_TYPE_OBJECT);

// instance initialisation
static void
waveform_reader_init (WaveformReader *self)
{
  self->priv = WAVEFORM_READER_GET_PRIVATE (self);

  // at the beginging initialise them to NULL
  // FIXME maybe shouldn't be done at all?
  self->priv->reading = NULL;
  self->priv->readings = NULL;
  self->priv->context = g_main_context_new();
}

// class initialisation
static void
waveform_reader_class_init (WaveformReaderClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->dispose = waveform_reader_dispose;
  gobject_class->finalize = waveform_reader_finalize;

  g_type_class_add_private (klass, sizeof (WaveformReaderPrivate));
}

static void
waveform_reader_dispose (GObject *gobject)
{

  WaveformReader *self = WAVEFORM_READER (gobject);

  if (self->priv->reading)
    {
      g_object_unref (self->priv->reading);
      self->priv->reading = NULL;
    }

  /* Chain up to the parent class */
  G_OBJECT_CLASS (waveform_reader_parent_class)->dispose (gobject);
}

static void
waveform_reader_finalize (GObject *gobject)
{
  WaveformReader *self = WAVEFORM_READER (gobject);

  // free readings list when finished
  g_list_free(self->priv->readings);

  /* Chain up to the parent class */
  G_OBJECT_CLASS (waveform_reader_parent_class)->finalize (gobject);
}

static gboolean bus_call(GstBus *bus, GstMessage *msg, void *user_data)
{
	// get WaveformReader self from user_data pointer
	WaveformReader *self = WAVEFORM_READER ((GObject*)user_data);
	
	switch(GST_MESSAGE_TYPE(msg)) {
		case GST_MESSAGE_EOS: {
			//g_message("End of the song.");
			g_main_loop_quit(self->priv->loop);
			break;
		}
		case GST_MESSAGE_ELEMENT: {
			
			const GstStructure *st = gst_message_get_structure(msg);
			const gchar *name = gst_structure_get_name(st);
			
			// if not equal do not procceed, it's not level element message
			if(name == NULL)
				break;
			if(strcmp(name, "level") != 0)
				break;

			// if it's level, add one sample
			//number_of_samples++;

			// creating new WaveformLevelReading for storing reading values
			self->priv->reading = g_object_new(WAVEFORM_TYPE_LEVEL_READING, NULL);
			
			// get value list of channel median power
			const GValue *list_value = gst_structure_get_value(st, "rms");
			GValueArray *rms_list = (GValueArray *) g_value_get_boxed(list_value);
			const GValue *rms_value;

			// get number of channels
			guint channels = rms_list->n_values;
			//printf("Channels: %i\n",channels);

			// if channels global variable is 0 (this is first sample), changed it
			// FIXME what about if channels is 0 according to messages? Break and issue error
			//if(channels_global == 0)
			//	channels_global = channels;

			// Set end time of the buffer as time of the reading
			const GValue *stream_time = gst_structure_get_value(st, "stream-time");
			const GValue *duration = gst_structure_get_value(st, "duration");
			self->priv->reading->time = ((guint64)g_value_get_uint64(stream_time)) + ((guint64)g_value_get_uint64(duration));
			
			
			// Initialise GArray for storing levels for each channel
			// reading->levels should be already initialised with creation of WaveformLevelReading
			//reading->levels = g_array_new (FALSE, FALSE, sizeof (gfloat));
			
			int i;
			for(i=0;i<channels;i++) {
				// get value from structure field 'rms' array
				rms_value = g_value_array_get_nth(rms_list, i);
				// add rms value to levels array
				gdouble rms_value_double = g_value_get_double(rms_value);
				g_array_append_val(self->priv->reading->levels, rms_value_double);
			}

			// When finished with reading, append it to linked list
			self->priv->readings = g_list_prepend (self->priv->readings, self->priv->reading);
			// unref reading, because it's has readings ref now
			g_object_unref(self->priv->reading);
			// FIXME why this causes segfault?
			//g_value_array_free(rms_list);
		}
		default:
			break;
	}

	return TRUE;
}

/**
 * waveform_reader_new:
 *
 * Creates #WaveformReader which manages reading of audio files.
 *
 * Returns: (transfer full): The new #WaveformReader
 *
 * Since: 0.1
 */

WaveformReader * waveform_reader_new(void) {
	return g_object_new(WAVEFORM_TYPE_READER, NULL);
}

/**
 * waveform_reader_get_levels:
 * @reader: pointer to #WaveformReader object which reads levels.
 * @file_location: a pointer to a #gchar array to file location.
 *
 * Creates a new #GList with audio level readings in #WaveformLevelReading structures.
 *
 * Returns: (transfer container) (element-type Waveform.LevelReading): The new #GList of #WaveformLevelReading
 *
 * Since: 0.1
 */

GList * waveform_reader_get_levels(WaveformReader *reader, const gchar *file_location) {
	
	// We already have created GMainContext as reader->priv->context, use it as default for a thread
	g_main_context_push_thread_default(reader->priv->context);

    // Create main loop
	reader->priv->loop = g_main_loop_new(reader->priv->context, FALSE);

	// Initialising GList for returning readings
	reader->priv->readings = NULL;
	
	// Initialising Gstreamer without params
	gst_init(NULL, NULL);
	
	GstElement *pipeline;
	GstBus *bus;
	
	pipeline = gst_parse_launch("filesrc name=src ! decodebin ! audioconvert ! level message=true name=level_element ! fakesink", NULL);
	
	// set up element properties
	GstElement *filesrc = gst_bin_get_by_name(GST_BIN(pipeline), "src");
	g_object_set(G_OBJECT(filesrc), "location", file_location, NULL);
	gst_object_unref(filesrc);

	// add watch and callback function bus_call, passing WaveformReader *reader as user_data pointer
	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	gst_bus_add_watch(bus, bus_call, reader);

	// playing back pipeline
	gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);

	// kicking off loop
	g_main_loop_run(reader->priv->loop);

	// pausing pipeline
	gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);

	// stop and remove watch as we don't need it for now
	gst_bus_remove_signal_watch(bus);

	// unref pipeline and bus
	gst_object_unref(GST_OBJECT(pipeline));
	gst_object_unref(bus);
	
	g_message("Size: %i", g_list_length(reader->priv->readings));

	// as we prepended objects, reverse list
	reader->priv->readings = g_list_reverse(reader->priv->readings);

	// return pointer to linked list
	return reader->priv->readings;
}