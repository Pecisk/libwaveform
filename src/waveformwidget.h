#ifndef __WAVEFORMWIDGET_H
#define __WAVEFORMWIDGET_H

#include <gtk/gtk.h>
#include <cairo.h>
#include <glib-object.h>

G_BEGIN_DECLS


#define WAVEFORM_TYPE_DRAWING             		(waveform_drawing_get_type ())
#define WAVEFORM_DRAWING(obj)						(G_TYPE_CHECK_INSTANCE_CAST((obj), WAVEFORM_TYPE_DRAWING, WaveformDrawing))
#define WAVEFORM_DRAWING_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((obj), WAVEFORM_DRAWING, WaveformDrawingClass))
#define WAVEFORM_IS_DRAWING(obj) 				(G_TYPE_CHECK_INSTANCE_TYPE((obj), WAVEFORM_TYPE_DRAWING))
#define WAVEFORM_IS_DRAWING_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), WAVEFORM_TYPE_DRAWING))
#define WAVEFORM_DRAWING_GET_CLASS        (G_TYPE_INSTANCE_GET_CLASS ((obj), WAVEFORM_TYPE_DRAWING, WaveformDrawingClass))

typedef struct _WaveformDrawing WaveformDrawing;
typedef struct _WaveformDrawingClass WaveformDrawingClass;
typedef struct _WaveformDrawingPrivate WaveformDrawingPrivate;

struct _WaveformDrawing {
  GtkDrawingArea parent;
};

struct _WaveformDrawingClass {
  GtkDrawingAreaClass parent_class;
};


GType waveform_drawing_get_type(void);
WaveformDrawing * waveform_drawing_new(void);
gboolean waveform_drawing_set_model(WaveformDrawing *self, WaveformData *data_model);

G_END_DECLS

#endif /* __WAVEFORMWIDGET_H */