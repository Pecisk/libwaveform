/*
 * Copyright/Licensing information.
 */

#ifndef __WAVEFORM_READER_H__
#define __WAVEFORM_READER_H__

#include <glib-object.h>
#include <gst/gst.h>

/*
 * Type macros.
 */
#define WAVEFORM_TYPE_READER     (waveform_reader_get_type ())
#define WAVEFORM_READER(obj)     (G_TYPE_CHECK_INSTANCE_CAST ((obj), WAVEFORM_TYPE_READER, WaveformReader))
#define WAVEFORM_IS_READER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WAVEFORM_TYPE_READER))
#define WAVEFORM_READER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WAVEFORM_TYPE_READER, WaveformReaderClass))
#define WAVEFORM_IS_READER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WAVEFORM_TYPE_READER))
#define WAVEFORM_READER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WAVEFORM_TYPE_READER, WaveformReaderClass))
#define WAVEFORM_READER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), WAVEFORM_TYPE_READER, WaveformReaderPrivate))

typedef struct _WaveformReader        WaveformReader;
typedef struct _WaveformReaderClass   WaveformReaderClass;
typedef struct _WaveformReaderPrivate WaveformReaderPrivate;

struct _WaveformReader
{
  GObject parent_instance;

  /* instance members */
  MamanBarPrivate *priv;
  
};

struct _MamanBarPrivate
{
  static GMainLoop *loop;
  WaveformLevelReading *reading;
  GList *readings;
};

struct _WaveformReaderClass
{
  GObjectClass parent_class;

  /* class members */
  static GMainLoop *loop;
};

/* used by MAMAN_TYPE_BAR */
GType waveform_reader_get_type (void);

/*
 * Method definitions.
 */
WaveformLevelReading *	waveform_reader_get_levels(WaveformReader *reader, gchar *file_location);
static gboolean bus_call(GstBus *bus, GstMessage *msg, void *user_data);
WaveformReader * waveform_reader_new(void);

#endif /* __WAVEFORM_READER_H__ */