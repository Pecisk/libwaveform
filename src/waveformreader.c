/*
 * Copyright/Licensing information.
 */

#include <glib-object.h>
#include "waveformlevelreading.h"
#include "waveformreader.h"
#include <gst/gst.h>
#include <stdbool.h>

G_DEFINE_TYPE (WaveformReader, waveform_reader, G_TYPE_OBJECT);

static GMainLoop *loop;
WaveformLevelReading *reading;
GList *readings;

gunit number_of_channels = 0;
guint64 number_of_samples = 0;

static gboolean bus_call(GstBus *bus, GstMessage *msg, void *user_data)
{
	switch(GST_MESSAGE_TYPE(msg)) {
		case GST_MESSAGE_EOS: {
			//g_message("End of the song.");
			g_main_loop_quit(loop);
			break;
		}
		case GST_MESSAGE_ELEMENT: {
			
			const GstStructure *st = gst_message_get_structure(msg);
			const gchar *name = gst_structure_get_name(st);
			//printf("%s\n", name);
			// if not equal, break, it's not level element message
			if(strcmp(name, "level") != 0)
				break;

			// if it's level, add one sample
			number_of_samples++;

			// creating new WaveformLevelReading
			reading = g_object_new(WAVEFORM_TYPE_LEVEL_READING, NULL)
			
			// get value list of channel median powers
			const GValue *list_value = gst_structure_get_value(st, "rms");
			GValueArray *rms_list = (GValueArray *) g_value_get_boxed(list_value);
			const GValue *rms_value;
			
			guint channels = rms_list->n_values;
			//printf("Channels: %i\n",channels);

			// if channels global variable is 0 (this is first sample), changed it
			// FIXME what about if channels is 0 according to messages? Break and issue error
			if(channels_global == 0)
				channels_global = channels;

			// FIXME set reading->time here
			
			// Initialise GArray for storing levels for each channel
			// reading->levels should be already initialised with creation of WaveformLevelReading
			reading->levels = g_array_new (FALSE, FALSE, sizeof (gfloat));
			
			int i;
			for(i=0;i<channels;i++) {
				// get value from structure field 'rms' array
				rms_value = g_value_array_get_nth(rms_list, i);
				// add rms value to levels array
				g_array_append_val(levels, g_value_get_double(rms_value));
			}

			// When finished with reading, append it to linked list
			g_list_append (readings, reading);
			// FIXME free reading and levels as we don't need them anymore?
			
		}
		default:
			break;
	}
}

GList * read_levels(gchar *file_location) {
	
	// Create main loop
	loop = g_main_loop_new(NULL, FALSE);

	// Initialising GArray for returning readings
	//readings = g_array_new (FALSE, FALSE, sizeof (WaveformLevelReading));

	// Initialising GList for returning readings
	readings = NULL;
	
	GstElement *pipeline;
	GstBus *bus;
	
	pipeline = gst_parse_launch("filesrc name=src ! decodebin ! audioconvert ! level message=true name=level_element ! fakesink", NULL);
	//pipeline = gst_element_factory_make("playbin", "player");
	
	// set up object values
	GstElement *filesrc = gst_bin_get_by_name(GST_BIN(pipeline), "src");
	g_object_set(G_OBJECT(filesrc), "location", file_location, NULL);
	gst_object_unref(filesrc);
	
	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	gst_bus_add_watch(bus, bus_call, NULL);
	gst_object_unref(bus);
	
	gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);
	
	g_main_loop_run(loop);
	
	gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);
	
	gst_object_unref(GST_OBJECT(pipeline));

	// return pointer to linked list
	return readings;
}

