dnl $Id$
dnl config.m4 for extension write_protection

sinclude(./autoconf/pecl.m4)
sinclude(./autoconf/php-executable.m4)

PECL_INIT([write_protection])

PHP_ARG_ENABLE(write_protection, whether to enable write_protection, [ --enable-write-protection   Enable write_protection])

if test "$PHP_WRITE_PROTECTION" != "no"; then
  AC_DEFINE(HAVE_WRITE_PROTECTION, 1, [whether write_protection is enabled])
  PHP_NEW_EXTENSION(write_protection, write_protection.c, $ext_shared)

  PHP_ADD_MAKEFILE_FRAGMENT
  PHP_INSTALL_HEADERS([ext/write_protection], [php_write_protection.h])
fi
