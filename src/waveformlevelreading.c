/*
 * Copyright/Licensing information.
 */

#include <glib-object.h>
#include "waveformlevelreading.h"

G_DEFINE_TYPE (WaveformLevelReading, waveform_level_reading, G_TYPE_OBJECT);

static void
waveform_level_reading_class_init (WaveformLevelReadingClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->dispose = waveform_level_reading_dispose;
  gobject_class->finalize = waveform_level_reading_finalize;

  //g_type_class_add_private (klass, sizeof (MamanBarPrivate));
}

static void
waveform_level_reading_init (WaveformLevelReading *self)
{
  //self->priv = MAMAN_BAR_GET_PRIVATE (self); 

  self->time = 0;
  self->levels = g_array_new (FALSE, FALSE, sizeof (gfloat));
}

static void
waveform_level_reading_dispose (GObject *gobject)
{
  WaveformLevelReading *self = WAVEFORM_LEVEL_READING (gobject);

	// FIXME should I free all of it? I think yes - when we release it, we release it
	g_array_free(self->levels, TRUE)

  /* Chain up to the parent class */
  G_OBJECT_CLASS (waveform_level_reading_parent_class)->dispose (gobject);
}

static void
waveform_level_reading_finalize (GObject *gobject)
{
  WaveformLevelReading *self = WAVEFORM_LEVEL_READING (gobject);

  //g_free (self->priv->a_string);

  /* Chain up to the parent class */
  G_OBJECT_CLASS (waveform_level_reading_parent_class)->finalize (gobject);
}

