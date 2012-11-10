#ifndef __PROFILES_STATUS_PLUGIN_H__
#define __PROFILES_STATUS_PLUGIN_H__

#include <libhildondesktop/libhildondesktop.h>

G_BEGIN_DECLS

#define TYPE_PROFILES_STATUS_PLUGIN            (profiles_status_menu_item_get_type ())

#define PROFILES_STATUS_PLUGIN(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                    TYPE_PROFILES_STATUS_PLUGIN, ProfilesStatusMenuItem))

#define PROFILES_STATUS_PLUGIN_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), \
                                TYPE_PROFILES_STATUS_PLUGIN, ProfilesStatusMenuItemClass))

#define IS_PROFILES_STATUS_PLUGIN(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                                    TYPE_PROFILES_STATUS_PLUGIN))

#define IS_PROFILES_STATUS_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), \
                                                    TYPE_PROFILES_STATUS_PLUGIN))

#define PROFILES_STATUS_PLUGIN_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                            TYPE_PROFILES_STATUS_PLUGIN, ProfilesStatusMenuItemClass))

typedef struct _ProfilesStatusMenuItem        ProfilesStatusMenuItem;
typedef struct _ProfilesStatusMenuItemClass   ProfilesStatusMenuItemClass;
typedef struct _ProfilesStatusMenuItemPrivate ProfilesStatusMenuItemPrivate;

struct _ProfilesStatusMenuItem
{
    HDStatusMenuItem parent;

    ProfilesStatusMenuItemPrivate *priv;
};

struct _ProfilesStatusMenuItemClass
{
    HDStatusMenuItemClass parent;
};

GType profiles_status_menu_item_get_type (void);

G_END_DECLS

#endif

