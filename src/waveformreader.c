/*
 * Copyright/Licensing information.
 */

#include <glib-object.h>
#include "waveformlevelreading.h"
#include "waveformreader.h"
#include <gst/gst.h>
#include <stdbool.h>

G_DEFINE_TYPE (WaveformReader, waveform_reader, G_TYPE_OBJECT);

// initialisation function
static void
waveform_reader_init (WaveformReader *self)
{
  self->priv = WAVEFORM_READER_GET_PRIVATE (self);

  // at the beginging initialise them to NULL
  // FIXME maybe shouldn't be done at all?
  self->priv->reading = NULL;
  self->priv->readings = NULL;
}

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
  /* 
   * In dispose, you are supposed to free all types referenced from this
   * object which might themselves hold a reference to self. Generally,
   * the most simple solution is to unref all members on which you own a 
   * reference.
   */

  /* dispose might be called multiple times, so we must guard against
   * calling g_object_unref() on an invalid GObject.
   */
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

  g_free_array(self->priv->readings, TRUE);

  /* Chain up to the parent class */
  G_OBJECT_CLASS (waveform_reader_parent_class)->finalize (gobject);
}

//guint64 number_of_samples = 0;

static gboolean bus_call(GstBus *bus, GstMessage *msg, void *user_data)
{
	switch(GST_MESSAGE_TYPE(msg)) {
		case GST_MESSAGE_EOS: {
			//g_message("End of the song.");
			g_main_loop_quit(loop);
			break;
		}
		case GST_MESSAGE_ELEMENT: {
			
			// FIXME get WaveformReader self from user_data pointer
			// check if it's right macros or right way to do it
			WaveformReader *self = WAVEFORM_READER ((GObject*)user_data);
			
			const GstStructure *st = gst_message_get_structure(msg);
			const gchar *name = gst_structure_get_name(st);
			
			// if not equal do not procceed, it's not level element message
			if(strcmp(name, "level") != 0)
				break;

			// if it's level, add one sample
			//number_of_samples++;

			// creating new WaveformLevelReading for storing reading values
			self->priv->reading = g_object_new(WAVEFORM_TYPE_LEVEL_READING, NULL)
			
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
			self->priv->reading->time = ((guint64)g_value_get_uint(stream_time)) + ((guint64)g_value_get_uint());
			
			
			// Initialise GArray for storing levels for each channel
			// reading->levels should be already initialised with creation of WaveformLevelReading
			//reading->levels = g_array_new (FALSE, FALSE, sizeof (gfloat));
			
			int i;
			for(i=0;i<channels;i++) {
				// get value from structure field 'rms' array
				rms_value = g_value_array_get_nth(rms_list, i);
				// add rms value to levels array
				g_array_append_val(self->priv->reading->levels, g_value_get_double(rms_value));
			}

			// When finished with reading, append it to linked list
			g_list_append (self->priv->readings, self->priv->reading);
			// FIXME unref reading, because it's has readings ref now
			g_object_unref(reading);
			// FIXME unref list_value, rms_value, stream_time, duration?
			g_object_unref(list_value);
			g_object_unref(rms_value);
			g_object_unref(stream_value);
			g_object_unref(duration_value);
		}
		default:
			break;
	}
}

GList * read_levels(WaveformReader *self, gchar *file_location) {
	
	// Create main loop
	// FIXME what happens if loop must be shared with? Not fully understanding this
	self->priv->loop = g_main_loop_new(NULL, FALSE);

	// Initialising GArray for returning readings
	// self->priv->readings = g_array_new (FALSE, FALSE, sizeof (WaveformLevelReading));

	// Initialising GList for returning readings
	self->priv->readings = NULL;
	
	GstElement *pipeline;
	GstBus *bus;
	
	pipeline = gst_parse_launch("filesrc name=src ! decodebin ! audioconvert ! level message=true name=level_element ! fakesink", NULL);
	//pipeline = gst_element_factory_make("playbin", "player");
	
	// set up object values
	GstElement *filesrc = gst_bin_get_by_name(GST_BIN(pipeline), "src");
	g_object_set(G_OBJECT(filesrc), "location", file_location, NULL);
	gst_object_unref(filesrc);
	
	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	gst_bus_add_watch(bus, bus_call, self);
	gst_object_unref(bus);
	
	gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);
	
	g_main_loop_run(self->priv->loop);
	
	gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);
	
	gst_object_unref(GST_OBJECT(pipeline));

	// return pointer to linked list
	return self->priv->readings;
}

