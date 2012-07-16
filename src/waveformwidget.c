#include "waveformwidget.h"
#include <gtk/gtk.h>
#include <cairo.h>

static void waveform_drawing_class_init(WaveformDrawingClass *klass);
static void waveform_drawing_init(WaveformDrawing *waveform);
//static void gtk_waveform_size_request(GtkWidget *widget, GtkRequisition *requisition);
//static void gtk_waveform_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
//static void gtk_waveform_realize(GtkWidget *widget);
//static gboolean gtk_waveform_expose(GtkWidget *widget,GdkEventExpose *event);
void waveform_drawing_draw(GtkWidget *widget, cairo_t *cr);
//static void gtk_waveform_destroy(GtkWidget *object);

G_DEFINE_TYPE (WaveformDrawing, waveform_drawing, GTK_TYPE_DRAWING_AREA);

#define WAVEFORM_DRAWING_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), WAVEFORM_TYPE_DRAWING, WaveformDrawingPrivate))

struct _WaveformReaderPrivate
{
  // pointer to WaveformData model
  WaveformData *data;
};

static void
waveform_drawing_init(WaveformDrawing *self)
{
self->priv = WAVEFORM_DRAWING_GET_PRIVATE (self);
}

static void
waveform_drawing_class_init(WaveformDrawingClass *klass)
{
  GtkWidgetClass *widget_class;
  widget_class = (GtkWidgetClass *) klass;

  //widget_class->realize = gtk_waveform_realize;
  //widget_class->size_request = gtk_waveform_size_request;
  //widget_class->size_allocate = gtk_waveform_size_allocate;
  widget_class->draw = waveform_drawing_draw;
  //widget_class->destroy = gtk_waveform_destroy;
}

GtkWidget * waveform_drawing_new()
{
   return g_object_new(WAVEFORM_TYPE_DRAWING, NULL);
}

/*static gboolean
gtk_waveform_expose(GtkWidget *widget, GdkEventExpose *event)
{
  g_return_val_if_fail(widget != NULL, FALSE);
  g_return_val_if_fail(GTK_IS_WAVEFORM(widget), FALSE);
  g_return_val_if_fail(event != NULL, FALSE);

  gtk_waveform_draw(widget);

  return FALSE;
}*/

void waveform_drawing_draw(GtkWidget *widget, cairo_t *cr)
{
  // FIXME taking cairo context from given params segfaults
  // temporary fix, geting cairo context from widget window
  cr = gdk_cairo_create(gtk_widget_get_window(widget));

  // get data from widget
  WaveformDrawing *self = WAVEFORM_DRAWING(widget);
  // if there is something to draw, let's do it
  if(self->priv->data != NULL) {
  	cairo_set_source_rgb(cr, 0, 0, 0);
  	cairo_move_to(cr, );
  	cairo_paint(cr);
  }
  cairo_destroy(cr);
}

/*static void
gtk_waveform_destroy(GtkWidget *object)
{
  GtkWaveform *waveform;
  GtkWaveformClass *klass;

  g_return_if_fail(object != NULL);
  g_return_if_fail(GTK_IS_WAVEFORM(object));

  waveform = GTK_WAVEFORM(object);

  klass = GTK_TYPE_CLASS(gtk_widget_get_type());

  if (GTK_WIDGET_CLASS(klass)->destroy) {
     (* GTK_WIDGET_CLASS(klass)->destroy) (object);
  }
}*/

