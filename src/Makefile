OBJS=status-menu-applet-profiles.o
SOURCES=status-menu-applet-profiles.c
LIB=profiles_status_menu_item.so
PKG_FLAGS=$(shell pkg-config hildon-1 profile libhildondesktop-1 --libs --cflags)
CCFLAGS=-shared -Wall -g
CC=gcc

all:$(LIB)

$(LIB):$(OBJS)
	$(CC) $(CCFLAGS) $(PKG_FLAGS) $(OBJS) -o $(LIB)

.c.o:
	$(CC) $(CCFLAGS) $(PKG_FLAGS) -c $< -o $@

.PHONE: clean all

clean:
	rm -f $(OBJS) $(LIB)
