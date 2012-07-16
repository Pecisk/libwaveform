/* cpu.h */

#ifndef __WAVEFORMWIDGET_H
#define __WAVEFORMWIDGET_H

#include <gtk/gtk.h>
#include <cairo.h>

G_BEGIN_DECLS


#define GTK_TYPE_WAVEFORM             		(gtk_waveform_get_type ())
#define GTK_WAVEFORM(obj)						(G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_WAVEFORM, GtkWaveform))
#define GTK_WAVEFORM_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((obj), GTK_WAVEFORM, GtkWaveformClass))
#define GTK_IS_WAVEFORM(obj) 				(G_TYPE_CHECK_INSTANCE_TYPE((obj), GTK_TYPE_WAVEFORM))
#define GTK_IS_WAVEFORM_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), GTK_TYPE_WAVEFORM))
#define GTK_WAVEFORM_GET_CLASS        (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_WAVEFORM, GtkWaveformClass))

typedef struct _GtkWaveform GtkWaveform;
typedef struct _GtkWaveformClass GtkWaveformClass;


struct _GtkWaveform {
  GtkDrawingArea parent;
};

struct _GtkWaveformClass {
  GtkDrawingAreaClass parent_class;
};


GtkType gtk_waveform_get_type(void);
GtkWidget * gtk_waveform_new();


G_END_DECLS

#endif /* __WAVEFORMWIDGET_H */