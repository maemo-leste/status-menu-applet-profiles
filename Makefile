all: profiles_status_menu_item.so

install:
	install -d "$(DESTDIR)/usr/lib/hildon-desktop/"
	install -d "$(DESTDIR)/usr/share/applications/hildon-status-menu/"
	install -m 644 profiles_status_menu_item.so "$(DESTDIR)/usr/lib/hildon-desktop/"
	install -m 644 status-area-applet-profiles.desktop "$(DESTDIR)/usr/share/applications/hildon-status-menu/"

clean:
	$(RM) profiles_status_menu_item.so

profiles_status_menu_item.so: profiles_status_menu_item.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $(shell pkg-config --cflags --libs hildon-1 profile profile-data libhildondesktop-1) -W -Wall -O2 -shared -fPIC $^ -o $@

.PHONY: all install clean
