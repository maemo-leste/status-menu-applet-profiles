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
#include <profiled/keys_nokia.h>
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
    gchar *current_profile_name;
    DBusConnection *conn;
};

GType profiles_status_menu_item_get_type (void);

HD_DEFINE_PLUGIN_MODULE_EXTENDED (ProfilesStatusMenuItem, profiles_status_menu_item, HD_TYPE_STATUS_MENU_ITEM, G_ADD_PRIVATE_DYNAMIC(ProfilesStatusMenuItem), , );

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
profiles_status_menu_item_update_profile (ProfilesStatusMenuItem *plugin)
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
        is_silent = FALSE;
    }

    profiles_status_menu_item_update_icons (plugin, is_silent);
}

static void
profiles_status_menu_item_on_profile_changed (const char *profile G_GNUC_UNUSED, void *user_data)
{
    ProfilesStatusMenuItem *plugin = user_data;
    profiles_status_menu_item_update_profile (plugin);
}

static void
profiles_status_menu_item_box_update_vibration (GtkWidget *button, gpointer user_data G_GNUC_UNUSED)
{
    const char *profile;
    int enable;

    profile = g_object_get_data (G_OBJECT (button), "profile");
    if (!profile)
         return;

    enable = hildon_check_button_get_active (HILDON_CHECK_BUTTON (button));
    profile_set_value_as_bool (profile, PROFILEKEY_VIBRATING_ALERT_ENABLED, enable);
}

static void
profiles_status_menu_item_box_update_profile (GtkWidget *button, gpointer user_data G_GNUC_UNUSED)
{
    const char *profile = g_object_get_data (G_OBJECT (button), "profile");
    if (!profile)
         return;

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)))
        profile_set_profile (profile);
}

static void
profiles_status_menu_item_on_button_clicked (GtkWidget *widget G_GNUC_UNUSED, ProfilesStatusMenuItem *plugin)
{
    ProfilesStatusMenuItemPrivate *priv = plugin->priv;
    GtkWidget *dialog = NULL;
    GtkWidget *button;
    GtkWidget *vibrate_button;
    GtkWidget *profiles_vbox;
    GtkWidget *vibration_vbox;
    GtkWidget *main_hbox;
    GtkWidget *radio_group = NULL;
    char **profiles = NULL;
    char **ptr;
    const char *label;
    const char *icon;
    const char *vibrate;
    int vibrating;

    profiles = profile_get_profiles ();
    if (!profiles)
        goto clean;

    dialog = gtk_dialog_new_with_buttons (dgettext ("osso-profiles", "profi_ti_select_profiles"),
                                                GTK_WINDOW (plugin),
                                                GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                dgettext ("hildon-libs", "wdgt_bd_done"),
                                                GTK_RESPONSE_ACCEPT,
                                                GTK_STOCK_CANCEL,
                                                GTK_RESPONSE_REJECT,
                                                NULL);

    if (!dialog)
        goto clean;

    main_hbox = gtk_hbox_new (FALSE, 0);
    if (!main_hbox)
        goto clean;

    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), main_hbox, TRUE, TRUE, 0);

    profiles_vbox = gtk_vbox_new (FALSE, 0);
    if (!profiles_vbox)
        goto clean;

    gtk_box_pack_start (GTK_BOX (main_hbox), profiles_vbox, TRUE, TRUE, HILDON_MARGIN_HALF);

    vibration_vbox = gtk_vbox_new (FALSE, 0);
    if (!vibration_vbox)
        goto clean;

    gtk_box_pack_start (GTK_BOX (main_hbox), vibration_vbox, TRUE, TRUE, HILDON_MARGIN_HALF);

    for (ptr = profiles; *ptr; ++ptr)
    {

        if (strcmp (*ptr, "general") == 0)
        {
            label = dgettext ("osso-profiles", "profi_va_plugin_general");
            vibrate = dgettext ("osso-profiles", "profi_fi_select_general_vibrate");
            icon = "general_profile";
        }
        else if (strcmp (*ptr, "silent") == 0)
        {
            label = dgettext ("osso-profiles", "profi_va_plugin_silent");
            vibrate = dgettext ("osso-profiles", "profi_fi_select_silent_vibrate");
            icon = "statusarea_silent";
        }
        else
        {
            label = *ptr;
            vibrate = dgettext ("osso-profiles", "profi_fi_select_general_vibrate");
            icon = "general_profile";
        }

        button = hildon_gtk_radio_button_new_from_widget (HILDON_SIZE_FINGER_HEIGHT, GTK_RADIO_BUTTON (radio_group));

        if (!button)
            goto clean;

        if (!radio_group)
            radio_group = button;

        g_object_set_data (G_OBJECT (button), "profile", *ptr);
        gtk_button_set_label (GTK_BUTTON (button), label);
        gtk_button_set_image (GTK_BUTTON (button), gtk_image_new_from_icon_name (icon, GTK_ICON_SIZE_DIALOG));
        gtk_button_set_alignment (GTK_BUTTON (button), 0, 0.5);
        gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON (button), FALSE);

        if (strcmp (priv->current_profile_name, *ptr) == 0)
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);
        else
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);


        gtk_box_pack_start (GTK_BOX (profiles_vbox), button, TRUE, TRUE, HILDON_MARGIN_HALF);

        vibrate_button = hildon_check_button_new (HILDON_SIZE_FINGER_HEIGHT);

        if (!button)
            goto clean;

        g_object_set_data (G_OBJECT (vibrate_button), "profile", *ptr);
        gtk_button_set_label (GTK_BUTTON (vibrate_button), vibrate);

        vibrating = profile_get_value_as_bool (*ptr, PROFILEKEY_VIBRATING_ALERT_ENABLED);
        hildon_check_button_set_active (HILDON_CHECK_BUTTON (vibrate_button), vibrating);

        gtk_box_pack_start (GTK_BOX (vibration_vbox), vibrate_button, TRUE, TRUE, HILDON_MARGIN_HALF);

    }

    gtk_widget_show_all (dialog);

    gint result = gtk_dialog_run (GTK_DIALOG (dialog));
    switch (result)
    {
        case GTK_RESPONSE_ACCEPT:
            gtk_container_foreach (GTK_CONTAINER (vibration_vbox), profiles_status_menu_item_box_update_vibration, NULL);
            gtk_container_foreach (GTK_CONTAINER (profiles_vbox), profiles_status_menu_item_box_update_profile, NULL);
            break;
        default:
            break;
    }

clean:
    if (dialog)
        gtk_widget_destroy (dialog);
    if (profiles)
        profile_free_profiles (profiles);
}

static void
profiles_status_menu_item_init (ProfilesStatusMenuItem *plugin)
{
    plugin->priv = profiles_status_menu_item_get_instance_private(plugin);

    ProfilesStatusMenuItemPrivate *priv = plugin->priv;
    DBusError error;

    dbus_error_init (&error);

    priv->conn = dbus_bus_get (DBUS_BUS_SESSION, &error);

    dbus_error_free (&error);

    if (!priv->conn)
    {
        g_warning ("Could not open D-Bus session bus connection");
        return;
    }

    profile_connection_set (priv->conn);

    if ( profile_tracker_init () != 0 )
    {
        g_warning ("Cound not start listening to profile daemon");
        return;
    }

    profile_track_add_profile_cb (profiles_status_menu_item_on_profile_changed, plugin, NULL);

    priv->button = hildon_button_new (HILDON_SIZE_FINGER_HEIGHT | HILDON_SIZE_AUTO_WIDTH, HILDON_BUTTON_ARRANGEMENT_VERTICAL);

    if (!priv->button)
    {
        g_warning ("Could not create Hildon Button");
        return;
    }

    hildon_button_set_style (HILDON_BUTTON (priv->button), HILDON_BUTTON_STYLE_PICKER);
    hildon_button_set_title (HILDON_BUTTON (priv->button), dgettext ("osso-profiles", "profi_ti_menu_plugin"));
    gtk_button_set_alignment (GTK_BUTTON (priv->button), 0, 0);
    g_signal_connect_after (G_OBJECT (priv->button), "clicked", G_CALLBACK (profiles_status_menu_item_on_button_clicked), plugin);

    profiles_status_menu_item_update_profile (plugin);

    gtk_container_add (GTK_CONTAINER (plugin), priv->button);
    gtk_widget_show_all (GTK_WIDGET (plugin));
}

static void
profiles_status_menu_item_finalize (GObject *object)
{
    ProfilesStatusMenuItem *plugin = G_TYPE_CHECK_INSTANCE_CAST (object, profiles_status_menu_item_get_type (), ProfilesStatusMenuItem);

    profile_track_remove_profile_cb (profiles_status_menu_item_on_profile_changed, plugin);
    profile_tracker_quit ();

    if (plugin->priv->conn)
    {
        dbus_connection_unref (plugin->priv->conn);
        plugin->priv->conn = NULL;
    }

    G_OBJECT_CLASS (profiles_status_menu_item_parent_class)->finalize (object);
}

static void
profiles_status_menu_item_class_finalize (ProfilesStatusMenuItemClass *klass G_GNUC_UNUSED)
{
}

static void
profiles_status_menu_item_class_init (ProfilesStatusMenuItemClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = (GObjectFinalizeFunc) profiles_status_menu_item_finalize;
}
