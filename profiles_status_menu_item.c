 /***********************************************************************************
 *  status-menu-applet-profiles: Open source rewrite of the Maemo 5 profiles applet
 *  Copyright (C) 2011 Mohammad Abu-Garbeyyeh
 *  Copyright (C) 2012 Pali Rohár
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ***********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <hildon/hildon.h>
#include <libintl.h>
#include <profiled/libprofile.h>
#include <libhildondesktop/libhildondesktop.h>

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

struct _ProfilesStatusMenuItemPrivate
{
    GtkWidget *button;
    GtkWidget *dialog;
    gchar *current_profile_name;
    DBusConnection *conn;
    gpointer data;
};

GType profiles_status_menu_item_get_type (void);

HD_DEFINE_PLUGIN_MODULE (ProfilesStatusMenuItem, profiles_status_menu_item, HD_TYPE_STATUS_MENU_ITEM);

static void
profiles_status_menu_item_update_icons (ProfilesStatusMenuItem *plugin, gboolean is_silent)
{
    ProfilesStatusMenuItemPrivate *priv = plugin->priv;
    if (is_silent)
    {
        hildon_button_set_image (HILDON_BUTTON (priv->button),
                                 gtk_image_new_from_icon_name ("statusarea_silent", GTK_ICON_SIZE_DIALOG));
        GtkIconTheme *icon_theme = gtk_icon_theme_get_default ();
        GdkPixbuf *pixbuf = gtk_icon_theme_load_icon (icon_theme, "statusarea_silent",
                            18, GTK_ICON_LOOKUP_NO_SVG, NULL);
        hd_status_plugin_item_set_status_area_icon (HD_STATUS_PLUGIN_ITEM (plugin), pixbuf);
        g_object_unref (pixbuf);
    }
    else
    {
        hildon_button_set_image (HILDON_BUTTON (priv->button),
                                 gtk_image_new_from_icon_name ("general_profile", GTK_ICON_SIZE_DIALOG));
        hd_status_plugin_item_set_status_area_icon (HD_STATUS_PLUGIN_ITEM (plugin), NULL);
    }
}

static void
profiles_status_menu_item_get_current_profile (ProfilesStatusMenuItem *plugin)
{
    ProfilesStatusMenuItemPrivate *priv = plugin->priv;
    gboolean is_silent;

    priv->current_profile_name = profile_get_profile ();
    if (strcmp (priv->current_profile_name, "general") == 0)
    {
        hildon_button_set_value (HILDON_BUTTON (plugin->priv->button), dgettext ("osso-profiles", "profi_va_plugin_general"));
        is_silent = FALSE;
    }
    else if (strcmp (priv->current_profile_name, "silent") == 0)
    {
        hildon_button_set_value (HILDON_BUTTON (plugin->priv->button), dgettext ("osso-profiles", "profi_va_plugin_silent"));
        is_silent = TRUE;
    }
    else
    {
        hildon_button_set_value (HILDON_BUTTON (plugin->priv->button), priv->current_profile_name);
        /* TODO: See if all values in the current profile are silent */
        is_silent = FALSE;
    }

    profiles_status_menu_item_update_icons (plugin, is_silent);
}

static void
profiles_status_menu_item_on_button_clicked (GtkWidget *button G_GNUC_UNUSED, ProfilesStatusMenuItem *plugin)
{
    ProfilesStatusMenuItemPrivate *priv = plugin->priv;
    GtkWidget *general_button;
    GtkWidget *silent_button;
    GtkWidget *general_vibrate_button;
    GtkWidget *silent_vibrate_button;
    GtkWidget *profiles_vbox;
    GtkWidget *vibration_vbox;
    GtkWidget *main_hbox;
    GSList *profile_buttons_list;

    priv->dialog = gtk_dialog_new_with_buttons (dgettext ("osso-profiles", "profi_ti_select_profiles"),
                                                GTK_WINDOW (plugin),
                                                GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                dgettext ("hildon-libs", "wdgt_bd_done"),
                                                GTK_RESPONSE_ACCEPT,
                                                GTK_STOCK_CANCEL,
                                                GTK_RESPONSE_REJECT,
                                                NULL);

    /* TODO: create profile_buttons_list */

    general_button = hildon_gtk_radio_button_new (HILDON_SIZE_FINGER_HEIGHT, profile_buttons_list);
    gtk_button_set_label (GTK_BUTTON (general_button), dgettext ("osso-profiles", "profi_va_plugin_general"));
    gtk_button_set_image (GTK_BUTTON (general_button), gtk_image_new_from_icon_name ("general_profile", GTK_ICON_SIZE_DIALOG));
    gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON (general_button), FALSE);
    gtk_button_set_alignment (GTK_BUTTON (general_button), 0, 0.5);

    silent_button = hildon_gtk_radio_button_new (HILDON_SIZE_FINGER_HEIGHT, profile_buttons_list);
    gtk_button_set_label (GTK_BUTTON (silent_button), dgettext ("osso-profiles", "profi_va_plugin_silent"));
    gtk_button_set_image (GTK_BUTTON (silent_button), gtk_image_new_from_icon_name ("statusarea_silent", GTK_ICON_SIZE_DIALOG));
    gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON (silent_button), FALSE);
    gtk_button_set_alignment (GTK_BUTTON (silent_button), 0, 0.5);

    general_vibrate_button = hildon_check_button_new (HILDON_SIZE_FINGER_HEIGHT);
    gtk_button_set_label (GTK_BUTTON (general_vibrate_button), dgettext ("osso-profiles", "profi_fi_select_general_vibrate"));
    silent_vibrate_button = hildon_check_button_new (HILDON_SIZE_FINGER_HEIGHT);
    gtk_button_set_label (GTK_BUTTON (silent_vibrate_button), dgettext ("osso-profiles", "profi_fi_select_silent_vibrate"));

    profiles_vbox = gtk_vbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (profiles_vbox), general_button, FALSE, FALSE, HILDON_MARGIN_HALF);
    gtk_box_pack_start (GTK_BOX (profiles_vbox), silent_button, FALSE, FALSE, HILDON_MARGIN_HALF);

    vibration_vbox = gtk_vbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (vibration_vbox), general_vibrate_button, FALSE, FALSE, HILDON_MARGIN_HALF);
    gtk_box_pack_start (GTK_BOX (vibration_vbox), silent_vibrate_button, FALSE, FALSE, HILDON_MARGIN_HALF);

    main_hbox = gtk_hbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (main_hbox), profiles_vbox, TRUE, TRUE, HILDON_MARGIN_HALF);
    gtk_box_pack_start (GTK_BOX (main_hbox), vibration_vbox, FALSE, FALSE, HILDON_MARGIN_HALF);

    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (priv->dialog)->vbox), main_hbox, TRUE, TRUE, 0);

    if (strcmp (priv->current_profile_name, "general") == 0)
        gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON (general_button), TRUE);
    else if (strcmp (priv->current_profile_name, "silent") == 0)
        gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON (silent_button), TRUE);

    /* This is causing a segfault for some reason */
    /* int is_general_vibration_active = profile_get_value_as_bool ("general", "vibrating.alert.enabled");
    gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON (general_vibrate_button), is_general_vibration_active);

    int is_silent_vibrtaion_active = profile_get_value_as_bool ("silent", "vibrating.alert.enabled");
    gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON (silent_vibrate_button), is_silent_vibrtaion_active);*/

    gtk_widget_show_all (priv->dialog);

    gint result = gtk_dialog_run (GTK_DIALOG (priv->dialog));
    switch (result)
    {
      case GTK_RESPONSE_ACCEPT:
         break;
      default:
         break;
    }
    gtk_widget_destroy (priv->dialog);
    g_slist_free (profile_buttons_list);
}

static void
profiles_status_menu_item_init (ProfilesStatusMenuItem *plugin)
{
    plugin->priv = G_TYPE_INSTANCE_GET_PRIVATE (plugin, profiles_status_menu_item_get_type (), ProfilesStatusMenuItemPrivate);

    ProfilesStatusMenuItemPrivate *priv = plugin->priv;
    DBusError error;

    dbus_error_init (&error);

    priv->conn = dbus_bus_get (DBUS_BUS_SESSION, &error);
    if (dbus_error_is_set (&error))
    {
        g_warning ("Could not open D-Bus session bus connection.");
        dbus_error_free (&error);
    }
    profile_connection_set (priv->conn);

    priv->button = hildon_button_new (HILDON_SIZE_FINGER_HEIGHT | HILDON_SIZE_AUTO_WIDTH, HILDON_BUTTON_ARRANGEMENT_VERTICAL);
    hildon_button_set_style (HILDON_BUTTON (priv->button), HILDON_BUTTON_STYLE_PICKER);
    hildon_button_set_title (HILDON_BUTTON (priv->button), dgettext ("osso-profiles", "profi_ti_menu_plugin"));
    gtk_button_set_alignment (GTK_BUTTON (priv->button), 0, 0);
    g_signal_connect_after (priv->button, "clicked", G_CALLBACK (profiles_status_menu_item_on_button_clicked), plugin);

    profiles_status_menu_item_get_current_profile (plugin);

    gtk_container_add (GTK_CONTAINER (plugin), priv->button);

    gtk_widget_show_all (priv->button);

    gtk_widget_show (GTK_WIDGET (plugin));
}

static void
profiles_status_menu_item_class_finalize (ProfilesStatusMenuItemClass *klass G_GNUC_UNUSED) {}

static void
profiles_status_menu_item_class_init (ProfilesStatusMenuItemClass *klass)
{
    g_type_class_add_private (klass, sizeof (ProfilesStatusMenuItemPrivate));
}
