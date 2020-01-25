hildondesktoplib_LTLIBRARIES = profiles_status_menu_item.la

profiles_status_menu_item_la_SOURCES = profiles_status_menu_item.c

profiles_status_menu_item_la_CFLAGS = \
	$(LIBHILDON_CFLAGS) $(LIBHILDONDESKTOP_CFLAGS) \
	$(PROFILE_CFLAGS) $(PROFILEDATA_CFLAGS)

profiles_status_menu_item_la_LIBADD = \
	$(LIBHILDON_LIBS) $(LIBHILDONDESKTOP_LIBS) \
	$(PROFILE_LIBS) $(PROFILEDATA)

profiles_status_menu_item_la_LDFLAGS = -module -avoid-version \
    -Wl,--as-needed

hildonstatusmenudesktopentry_DATA = status-area-applet-profiles.desktop
EXTRA_DIST = $(hildonstatusmenudesktopentry_DATA)
