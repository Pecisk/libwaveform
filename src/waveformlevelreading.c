/*
 * Copyright/Licensing information.
 */

#include <glib-object.h>
#include "waveformlevelreading.h"

G_DEFINE_TYPE (WaveformLevelReading, waveform_level_reading, G_TYPE_OBJECT);

waveform_level_reading_init (WaveformLevelReading *self)
{
  self->priv = MAMAN_BAR_GET_PRIVATE (self); 

  /* initialize all public and private members to reasonable default values. */

  /* If you need specific construction properties to complete initialization,
   * delay initialization completion until the property is set. 
   */
}

static void
maman_bar_dispose (GObject *gobject)
{
  MamanBar *self = MAMAN_BAR (gobject);

  /* 
   * In dispose, you are supposed to free all types referenced from this
   * object which might themselves hold a reference to self. Generally,
   * the most simple solution is to unref all members on which you own a 
   * reference.
   */

  /* dispose might be called multiple times, so we must guard against
   * calling g_object_unref() on an invalid GObject.
   */
  if (self->priv->an_object)
    {
      g_object_unref (self->priv->an_object);

      self->priv->an_object = NULL;
    }

  /* Chain up to the parent class */
  G_OBJECT_CLASS (maman_bar_parent_class)->dispose (gobject);
}

static void
maman_bar_finalize (GObject *gobject)
{
  MamanBar *self = MAMAN_BAR (gobject);

  g_free (self->priv->a_string);

  /* Chain up to the parent class */
  G_OBJECT_CLASS (maman_bar_parent_class)->finalize (gobject);
}

static void
maman_bar_class_init (MamanBarClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->dispose = maman_bar_dispose;
  gobject_class->finalize = maman_bar_finalize;

  g_type_class_add_private (klass, sizeof (MamanBarPrivate));
}

static void
maman_bar_init (MamanBar *self);
{
  self->priv = MAMAN_BAR_GET_PRIVATE (self);

  self->priv->an_object = g_object_new (MAMAN_TYPE_BAZ, NULL);
  self->priv->a_string = g_strdup ("Maman");
}