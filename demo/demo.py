#!/usr/bin/python

from gi.repository import Waveform, Gtk, GObject
import sys

def on_zoom_in(widget, waveform):
	waveform.zoom_in()
	print("zoom in")

def on_zoom_out(widget, waveform):
	waveform.zoom_out()
	print("zoom out")

reader = Waveform.Reader.new()
data = Waveform.Data.new()

# Try to read 
try:
	levels = reader.get_levels("file:///home/peteris/test.wav", 120000000, 0, 0)
except GObject.GError, e:
	if e.domain == Waveform.ReaderError and e.code == Waveform.ReaderError.FILE:
		print "File path is wrong, please correct it."
		sys.exit()
	
sublevels = reader.get_levels("file:///home/peteris/test.wav", 40000000, 0, 120000000)	

data.add(levels)
data.add(sublevels)
shell = Gtk.Box()
shell.orientation = Gtk.Orientation.VERTICAL
zoom_box = Gtk.Box()
zoom_box.orientation = Gtk.Orientation.HORIZONTAL
zoom_in_button = Gtk.Button(label="Zoom In")
zoom_out_button = Gtk.Button(label="Zoom Out")
zoom_box.pack_start(zoom_in_button, True, True, 0);
zoom_box.pack_start(zoom_out_button, True, True, 0);

win = Gtk.Window()
scrolwin = Gtk.ScrolledWindow()
waveform = Waveform.Drawing.new()
zoom_in_button.connect("clicked", on_zoom_in, waveform)
zoom_out_button.connect("clicked", on_zoom_out, waveform)
win.set_size_request(400, 350)
shell.pack_start(zoom_box, True, True, 0)
shell.pack_start(scrolwin, True, True, 0)
win.add(shell)
waveform.set_size_request(800,300)
scrolwin.add_with_viewport(waveform)
waveform.set_model(data)
scrolwin.set_policy(Gtk.PolicyType.NEVER, Gtk.PolicyType.AUTOMATIC)
win.connect("delete-event", Gtk.main_quit)
win.show_all()
Gtk.main()

