#include "waveformwidget.h"

static void gtk_waveform_class_init(GtkWaveformClass *klass);
static void gtk_waveform_init(GtkWaveform *waveform);
//static void gtk_waveform_size_request(GtkWidget *widget, GtkRequisition *requisition);
//static void gtk_waveform_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
//static void gtk_waveform_realize(GtkWidget *widget);
static gboolean gtk_waveform_expose(GtkWidget *widget,GdkEventExpose *event);
static void gtk_waveform_paint(GtkWidget *widget);
static void gtk_waveform_destroy(GtkObject *object);

G_DEFINE_TYPE (GtkWaveform, gtk_waveform, GTK_TYPE_DRAWING_AREA);

static void
gtk_waveform_init(GtkWaveform *waveform)
{
}

static void
gtk_waveform_class_init(GtkWaveformClass *klass)
{
  GtkWidgetClass *widget_class;
  GtkObjectClass *object_class;


  widget_class = (GtkWidgetClass *) klass;
  object_class = (GtkObjectClass *) klass;

  //widget_class->realize = gtk_waveform_realize;
  //widget_class->size_request = gtk_waveform_size_request;
  //widget_class->size_allocate = gtk_waveform_size_allocate;
  widget_class->expose_event = gtk_waveform_expose;
  object_class->destroy = gtk_waveform_destroy;
}

GtkWidget * gtk_waveform_new()
{
   return g_object_new(GTK_TYPE_WAVEFORM, NULL);
}

static gboolean
gtk_waveform_expose(GtkWidget *widget,
    GdkEventExpose *event)
{
  g_return_val_if_fail(widget != NULL, FALSE);
  g_return_val_if_fail(GTK_IS_WAVEFORM(widget), FALSE);
  g_return_val_if_fail(event != NULL, FALSE);

  gtk_waveform_paint(widget);

  return FALSE;
}

static void
gtk_waveform_paint(GtkWidget *widget)
{
  cairo_t *cr;
  cr = gdk_cairo_create(widget->window);
  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_paint(cr);

  cairo_destroy(cr);
}

static void
gtk_waveform_destroy(GtkObject *object)
{
  GtkWaveform *waveform;
  GtkWaveformClass *klass;

  g_return_if_fail(object != NULL);
  g_return_if_fail(GTK_IS_WAVEFORM(object));

  waveform = gtk_waveform(object);

  klass = gtk_type_class(gtk_widget_get_type());

  if (GTK_OBJECT_CLASS(klass)->destroy) {
     (* GTK_OBJECT_CLASS(klass)->destroy) (object);
  }
}

