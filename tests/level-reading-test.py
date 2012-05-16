#!/usr/bin/python

from gi.repository import Gst, Gtk

def bus_element(bus, message):
	st = message.get_structure()
	if st.get_name() == "level":
		levels = st.get_value("rms")
		loadingLength = st.get_value("endtime")
		channel = 1
		for level in levels:
			print("Channel #" + str(channel) + " " + str(level) + " " + str(loadingLength))
			channel += 1

def bus_eos(bus, message):
	global loadingPipeline
	loadingPipeline.set_state(Gst.State.NULL)
	Gtk.main_quit()
	return

Gst.init(None)
#LEVEL_INTERVAL = 0.1

pipe = """filesrc name=src ! decodebin ! audioconvert ! level message=true name=level_element ! fakesink"""
loadingPipeline = Gst.parse_launch(pipe)

filesrc = loadingPipeline.get_by_name("src")
#level = loadingPipeline.get_by_name("level_element")

filesrc.set_property("location", "test.flac")
#level.set_property("interval", int(LEVEL_INTERVAL * Gst.SECOND))

bus = loadingPipeline.get_bus()
bus.add_signal_watch()
bus.connect("message::eos", bus_eos)
bus.connect("message::element", bus_element)
loadingPipeline.set_state(Gst.State.PLAYING)

Gtk.main()
