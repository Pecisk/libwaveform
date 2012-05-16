#include <stdio.h>
#include <gst/gst.h>
#include <stdbool.h>

static GMainLoop *loop;

static gboolean bus_call(GstBus *bus, GstMessage *msg, void *user_data)
{
	switch(GST_MESSAGE_TYPE(msg)) {
		case GST_MESSAGE_EOS: {
			//g_message("Dziesmas beigas.");
			g_main_loop_quit(loop);
			break;
		}
		case GST_MESSAGE_ELEMENT: {
			
			const GstStructure *st = gst_message_get_structure(msg);
			const gchar *name = gst_structure_get_name(st);
			printf("%s\n", name);
			// if not equal, break, it's not level element message
			if(strcmp(name, "level") != 0)
				break;
			
			// get value list of channel median powers
			const GValue *list_value = gst_structure_get_value(st, "rms");
			GValueArray *rms_list = (GValueArray *) g_value_get_boxed(list_value);
			const GValue *rms_value;
			
			guint channels = rms_list->n_values;
			printf("Channels: %i\n",channels);
			
			int i;
			for(i=0;i<channels;i++) {
				rms_value = g_value_array_get_nth(rms_list, i);
				printf("RMS #%i: %f\n", i+1, g_value_get_double(rms_value));
			}
		}
		default:
			break;
	}
}

int main(int argc, char *argv[]) {
// This is main
	gst_init(&argc, &argv);
	
	// Create main loop
	loop = g_main_loop_new(NULL, FALSE);
	
	GstElement *pipeline;
	GstBus *bus;
	gchar location[] = "test.flac";
	
	pipeline = gst_parse_launch("filesrc name=src ! decodebin ! audioconvert ! level message=true name=level_element ! fakesink", NULL);
	//pipeline = gst_element_factory_make("playbin", "player");
	
	// set up object values
	GstElement *filesrc = gst_bin_get_by_name(GST_BIN(pipeline), "src");
	g_object_set(G_OBJECT(filesrc), "location", location, NULL);
	gst_object_unref(filesrc);
	
	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	gst_bus_add_watch(bus, bus_call, NULL);
	gst_object_unref(bus);
	
	gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);
	
	g_main_loop_run(loop);
	
	gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);
	
	gst_object_unref(GST_OBJECT(pipeline));
}
