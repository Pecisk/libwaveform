#!/usr/bin/python

from gi.repository import Waveform, Gtk, GObject
import sys

reader = Waveform.Reader.new()
data = Waveform.Data.new()

# Try to read 
try:
	levels = reader.get_levels("file:///home/user/test.wav", 120000000, 0, 0)
except GObject.GError, e:
	if e.domain == Waveform.ReaderError and e.code == Waveform.ReaderError.FILE:
		print "File path is wrong, please correct it."
		sys.exit()
	
sublevels = reader.get_levels("file:///home/user/test.wav", 40000000, 0, 120000000)	

data.add(levels)
data.add(sublevels)
win = Gtk.Window()
scrolwin = Gtk.ScrolledWindow()
waveform = Waveform.Drawing.new()
win.set_size_request(400, 350)
win.add(scrolwin)
waveform.set_size_request(800,300)
scrolwin.add_with_viewport(waveform)
waveform.set_model(data)
scrolwin.set_policy(Gtk.PolicyType.NEVER, Gtk.PolicyType.AUTOMATIC)
win.connect("delete-event", Gtk.main_quit)
win.show_all()
Gtk.main()
