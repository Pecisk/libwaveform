/* cpu.h */

#ifndef __WAVEFORMWIDGET_H
#define __WAVEFORMWIDGET_H

#include <gtk/gtk.h>
#include <cairo.h>

G_BEGIN_DECLS


#define GTK_WAVEFORM(obj) GTK_CHECK_CAST(obj, gtk_waveform_get_type (), GtkWaveform)
#define GTK_WAVEFORM_CLASS(klass) GTK_CHECK_CLASS_CAST(klass, gtk_waveform_get_type(), GtkWaveformClass)
#define GTK_IS_WAVEFORM(obj) GTK_CHECK_TYPE(obj, gtk_waveform_get_type())


typedef struct _GtkWaveform GtkWaveform;
typedef struct _GtkWaveformClass GtkWaveformClass;


struct _GtkWaveform {
  GtkWidget widget;
};

struct _GtkWaveformClass {
  GtkWidgetClass parent_class;
};


GtkType gtk_waveform_get_type(void);
GtkWidget * gtk_waveform_new();


G_END_DECLS

#endif /* __WAVEFORMWIDGET_H */