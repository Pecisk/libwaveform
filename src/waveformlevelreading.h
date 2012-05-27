/*
 * Copyright/Licensing information.
 */

/* inclusion guard */
#ifndef __WAVEFORM_LEVEL_READING_H__
#define __WAVEFORM_LEVEL_READING_H__

#include <glib-object.h>

/*
 * Type macros.
 */
#define WAVEFORM_TYPE_LEVEL_READING     (waveform_level_reading_get_type ())
#define WAVEFORM_LEVEL_READING(obj)     (G_TYPE_CHECK_INSTANCE_CAST ((obj), WAVEFORM_TYPE_LEVEL_READING, WaveformLevelReading))
#define WAVEFORM_IS_LEVEL_READING(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WAVEFORM_TYPE_LEVEL_READING))
#define WAVEFORM_LEVEL_READING_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WAVEFORM_TYPE_LEVEL_READING, WaveformLevelReadingClass))
#define WAVEFORM_IS_LEVEL_READING_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WAVEFORM_TYPE_LEVEL_READING))
#define WAVEFORM_LEVEL_READING_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WAVEFORM_TYPE_LEVEL_READING, WaveformLevelReadingClass))

typedef struct _WaveformLevelReading        WaveformLevelReading;
typedef struct _WaveformLevelReadingClass   WaveformLevelReadingClass;

struct _WaveformLevelReading
{
  GObject parent_instance;
  /* instance members */
  guint64 start_time;
  guint64 end_time;
  guint64 number_of_samples;
  gint number_of_channels;
  /* levels[channel][sample] */
  gfloat **levels;
};

struct _WaveformLevelReadingClass
{
  GObjectClass parent_class;

  /* class members */
};

/* used by MAMAN_TYPE_BAR */
GType waveform_level_reading_get_type (void);

/*
 * Method definitions.
 */

#endif /* __WAVEFORM_LEVEL_READING_H__ */