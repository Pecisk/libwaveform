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

}

// class initialisation
static void
waveform_reader_class_init (WaveformReaderClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->dispose = waveform_reader_dispose;
  gobject_class->finalize = waveform_reader_finalize;

  // Check if we have already initialised Gstreamer, if not, silent warning
  if(!gst_is_initialized ())
		g_message("Gstreamer is not initialised, initialising");
  gst_init(NULL, NULL);
	
  g_type_class_add_private (klass, sizeof (WaveformReaderPrivate));
}

static void
waveform_reader_dispose (GObject *gobject)
{

  WaveformReader *self = WAVEFORM_READER (gobject);

  if (self->priv->reading)
    {
	  waveform_level_reading_unref(self->priv->reading);
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
  g_list_free_full(self->priv->readings, waveform_level_reading_unref);

  /* Chain up to the parent class */
  G_OBJECT_CLASS (waveform_reader_parent_class)->finalize (gobject);
}

static void
on_pad_added (GstElement *element,
              GstPad     *pad,
              gpointer    data)
{
  GstPad *sinkpad;
  GstElement *converter = (GstElement *) data;
  g_print ("Dynamic pad created, linking decoder/converter\n");
  sinkpad = gst_element_get_static_pad (converter, "sink");
  gst_pad_link (pad, sinkpad);
  gst_object_unref (sinkpad);
}

static gboolean bus_call(GstBus *bus, GstMessage *msg, void *user_data)
{
	// get WaveformReader self from user_data pointer
	WaveformReader *self = WAVEFORM_READER ((GObject*)user_data);
	
	switch(GST_MESSAGE_TYPE(msg)) {
		case GST_MESSAGE_ERROR: {
		GError *err = NULL;
		gchar *dbg_info = NULL;
		
		gst_message_parse_error (msg, &err, &dbg_info);
		//g_printerr ("ERROR from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
		//g_printerr ("Debugging info: %s\n", (dbg_info) ? dbg_info : "none");
		
		// FIXME handle well known errors and report it correctly to user
		if(strcmp(GST_OBJECT_NAME (msg->src), "source") == 0 && strcmp(err->message, "Resource not found.") == 0)
			g_message("Stream provided by URI not found.");
		// can't decode
		// don't have audio
		// default
		g_error_free (err);
		g_free (dbg_info);
		g_main_loop_quit(self->priv->loop);
		break;	
		}
		case GST_MESSAGE_SEGMENT_DONE: {
			g_message("End of the fragment");
			g_main_loop_quit(self->priv->loop);
			break;
		}
		case GST_MESSAGE_EOS: {
			g_message("End of the song.");
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
			//self->priv->reading = g_object_new(WAVEFORM_TYPE_LEVEL_READING, NULL);
			//WaveformLevelReading reading = {1, 0, 0, NULL};
			//self->priv->reading = &reading;
			self->priv->reading = waveform_level_reading_new();
			
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
			//g_message("stream-time %"G_GUINT64_FORMAT"\n", (guint64)g_value_get_uint64(stream_time));
			//g_message("duration %"G_GUINT64_FORMAT"\n", (guint64)g_value_get_uint64(duration));
			self->priv->reading->end_time = ((guint64)g_value_get_uint64(stream_time)) + ((guint64)g_value_get_uint64(duration));
			self->priv->reading->start_time = (guint64)g_value_get_uint64(stream_time);
			
			// Initialise GArray for storing levels for each channel
			//self->priv->reading->levels = g_array_new (FALSE, FALSE, sizeof (gfloat));
			
			int i;
			for(i=0;i<channels;i++) {
				// get value from structure field 'rms' array
				rms_value = g_value_array_get_nth(rms_list, i);
				// add rms value to levels array
				gdouble rms_value_double = g_value_get_double(rms_value);
				g_array_append_val(self->priv->reading->levels, rms_value_double);
			}

			// When finished with reading, append it to linked list
			g_message("%i : %"G_GUINT64_FORMAT" : %"G_GUINT64_FORMAT" : %f",self->priv->reading->refcount, self->priv->reading->start_time, self->priv->reading->end_time,   g_array_index(self->priv->reading->levels, gfloat, 0));	
			self->priv->readings = g_list_prepend (self->priv->readings, self->priv->reading);
			
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
 * @interval: #guint64 of level reading period, if zero, then 0.1 s (as 120 000 000 ns) period is used by default.
 * @start: #guint64 of start of reading period in ns
 * @finish: #guint64 of end of reading period in ns
 * 
 * Creates a new #GList with audio level readings in #WaveformLevelReading structures.
 *
 * Returns: (transfer none) (element-type Waveform.LevelReading): The new #GList of #WaveformLevelReading
 *
 * Since: 0.1
 */

GList * waveform_reader_get_levels(WaveformReader *reader, const gchar *file_location, guint64 interval, guint64 start, guint64 finish) {
	
	// Creating our own context
    reader->priv->context = g_main_context_new();
	
    // Creating our own main loop
	reader->priv->loop = g_main_loop_new(reader->priv->context, FALSE);

	// Initialising GList for returning readings
	reader->priv->readings = NULL;
	
	GstElement *decoder, *converter, *level_element, *fakesink;
	GstElement *pipeline;
	GstBus *bus;
	GSource *source;
	guint id;

	pipeline = gst_pipeline_new("level-reader-pipeline");
	decoder = gst_element_factory_make("uridecodebin","codec-decoder");
	converter = gst_element_factory_make("audioconvert","audio-converter");
	level_element = gst_element_factory_make("level","level-element");
	fakesink = gst_element_factory_make("fakesink", "fake-sink");
	
	// FIXME return API error about problems with gstreamer core installation
	if (!pipeline || !decoder || !converter || !level_element || !fakesink) {
    g_printerr ("One element could not be created. Exiting.\n");
    //return -1;
	}

	// set up file location
	// FIXME error if file isn't valid (not found, or can't be processed)
	g_object_set(G_OBJECT(decoder), "uri", file_location, NULL);
	// set caps for exposing only raw audio
	GstCaps *caps = gst_caps_new_empty_simple("audio/x-raw");
	g_object_set(G_OBJECT(decoder), "caps", caps, NULL);
	// setting caps free
	gst_caps_unref(caps);
	// setting interval if it's not zero, otherwise use default
	if(interval != 0)
		g_object_set(G_OBJECT(level_element), "interval", interval, NULL);
	
	// add elements to the pipeline
	gst_bin_add_many (GST_BIN (pipeline), decoder, converter, level_element, fakesink, NULL);
	//g_message("Elements added");
	// link elements
	gst_element_link_many (converter, level_element, fakesink, NULL);
	g_signal_connect (decoder, "pad-added", G_CALLBACK (on_pad_added), converter);
	//g_message("Elements linked");
	
	// add watch and callback function bus_call, passing WaveformReader *reader as user_data pointer
	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));

	// get GSource of bus
	source = gst_bus_create_watch (bus);

	// set helper function so ansync messages would be converted to signals
	g_source_set_callback (source, (GSourceFunc) gst_bus_async_signal_func, NULL, NULL);

	// get id 
    id = g_source_attach (source, reader->priv->context);
	// context has source ref now
	g_source_unref (source);

	// FIXME do something if source can't be attached to context, t.i. id == 0;

	// catch eos messages for stream end
	g_signal_connect (bus, "message::eos", (GCallback) bus_call, reader);
	// catch errors in bus
	g_signal_connect (bus, "message::error", (GCallback) bus_call, reader);

	if(finish != 0) 
		{
			// FIXME let's try to set state to PAUSED
			GstStateChangeReturn r = gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);
			if(r != GST_STATE_CHANGE_FAILURE)
			{
			// FIXME try to get state and work if any error gets issued
				GstStateChangeReturn h = gst_element_get_state(GST_ELEMENT(pipeline), NULL, NULL, GST_CLOCK_TIME_NONE);
			}
			g_message("Doing seeking %"G_GUINT64_FORMAT" %"G_GUINT64_FORMAT"\n", start, finish);
			// do seeking if finish is not zero
			gst_element_seek (pipeline, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH|GST_SEEK_FLAG_ACCURATE,
                         GST_SEEK_TYPE_SET, start,
                         GST_SEEK_TYPE_SET, finish);
			// catch segment done messages
			//g_signal_connect (bus, "message::segment-done", (GCallback) bus_call, reader);
		}

	// catch element messages for 'level'
    g_signal_connect (bus, "message::element", (GCallback) bus_call, reader);
	
	// playing back pipeline
	gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);

	// kicking off our custom loop
	g_main_loop_run(reader->priv->loop);
	
	// pausing pipeline
	gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);
	//g_message("State null");
	// unref/free all stuff

	g_source_remove (id);
	g_main_loop_unref(reader->priv->loop);
	g_main_context_unref(reader->priv->context);

	gst_object_unref(bus);
	gst_object_unref(GST_OBJECT(pipeline));
    //g_message("Business finished");
	
	// as we prepended objects, reverse list
	reader->priv->readings = g_list_reverse(reader->priv->readings);
	g_message("Size: %i", g_list_length(reader->priv->readings));

	// return pointer to linked list
	return reader->priv->readings;
}


/**
 * waveform_reader_get_end_time:
 * @reading: pointer to #WaveformLevelReading object which has time and channel information of reading.
 *
 * Returns: (transfer full): #guint64 of end time of level reading buffer in nanoseconds. 
 *
 * Since: 0.1
 */
guint64 waveform_reader_get_end_time (WaveformLevelReading * reading) {
//	g_message("End time: %lld\n", (guint64)reading->end_time);
	return reading->end_time;
}

/**
 * waveform_reader_get_start_time:
 * @reading: pointer to #WaveformLevelReading object which has time and channel information of reading.
 *
 * Returns: (transfer full): #guint64 of start time of level reading buffer in nanoseconds. 
 *
 * Since: 0.1
 */
guint64 waveform_reader_get_start_time (WaveformLevelReading * reading) {
//	g_message("Start time: %lld\n", (guint64)reading->start_time);
	return reading->start_time;
}

/**
 * waveform_reader_get_channel_readings:
 * @reading: pointer to #WaveformLevelReading object which has time and channel information of reading.
 *
 * Returns: (transfer full) (element-type float): The #GArray of level readings by channel.
 *
 * Since: 0.1
 */
GArray * waveform_reader_get_channel_readings (WaveformLevelReading * reading) {
	return reading->levels;
}

/**
 * waveform_reader_get_subreadings:
 * @reading: pointer to #WaveformLevelReading object which has time and channel information of reading.
 *
 * Returns: (transfer full) (element-type Waveform.LevelReading): The #GPtrArray of subreadings of period covered by reading.
 *
 * Since: 0.1
 */
GList * waveform_reader_get_subreadings (WaveformLevelReading * reading) {

	// FIXME temporary solution is to return first actual GList from GPtrArray
	// first, check is subreadings isn't empty

	if(reading->subreadings == NULL)
			return NULL;
	else
			return (GList*)g_ptr_array_index(reading->subreadings, 0);
}
