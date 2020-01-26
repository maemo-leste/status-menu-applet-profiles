AC_INIT([Status menu profiles applet],
        [0.14],
        [merlijn@wizzup.org], [status-menu-applet-profiles])

AM_INIT_AUTOMAKE
LT_INIT([shared disable-static])
AC_CONFIG_FILES([Makefile])

AC_PROG_CC
AC_PROG_LIBTOOL

PKG_CHECK_MODULES(LIBHILDON, hildon-1)
AC_SUBST(LIBHILDON_CFLAGS)
AC_SUBST(LIBHILDON_LIBS)

PKG_CHECK_MODULES(LIBHILDONDESKTOP, libhildondesktop-1)
AC_SUBST(LIBHILDONDESKTOP_CFLAGS)
AC_SUBST(LIBHILDONDESKTOP_LIBS)

PKG_CHECK_MODULES(PROFILE, profile)
AC_SUBST(PROFILE_CFLAGS)
AC_SUBST(PROFILE_LIBS)

PKG_CHECK_MODULES(PROFILEDATA, profile-data)
AC_SUBST(PROFILEDATA_CFLAGS)
AC_SUBST(PROFILEDATA_LIBS)


hildonstatusmenudesktopentrydir="`$PKG_CONFIG --variable=hildonstatusmenudesktopentrydir libhildondesktop-1`"
AC_SUBST(hildonstatusmenudesktopentrydir)

hildondesktoplibdir="`$PKG_CONFIG --variable=hildondesktoplibdir osso-af-settings`"
AC_SUBST(hildondesktoplibdir)

AC_OUTPUT
