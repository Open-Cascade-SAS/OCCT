#ifndef InterfaceGraphic_X11Header
# define InterfaceGraphic_X11Header

/* 

Copyright (C) 1991,1992,1993 by

MATRA DATAVISION, FRANCE

This software is furnished in accordance with the terms and conditions
of the contract and with the inclusion of the above copyright notice.
This software or any other copy thereof may not be provided or otherwise
be made available to any other person. No title to an ownership of the
software is hereby transferred.

At the termination of the contract, the software and all copies of this
software must be deleted.

Facility : CAS-CADE V1

*/ 
# ifndef WNT
#  include <stdio.h>

#  include <X11/Xlib.h>
#  include <X11/Xutil.h>
#  include <X11/Xatom.h>
#  include <GL/glx.h>

#  if defined (sun) || defined (SUNOS) || defined (__alpha) || defined (DECOSF1) || defined (sgi) || defined (IRIX) || defined (__hpux)|| defined (HPUX) 
#   ifndef icon_width
#    include <X11/bitmaps/icon>
#   endif

#  endif /* SUNOS or DECOSF1 or SOLARIS or HPUX or IRIX */

#  ifdef ULTRIX

#   ifndef icon_width
/* le contenu de #include <X11/bitmaps/icon> sur SUN */
#define icon_width 16
#define icon_height 16
static unsigned char icon_bits[] = {
  0xff, 0xff, 0xab, 0xaa, 0x55, 0xd5, 0xab, 0xaa, 0x05, 0xd0, 0x0b, 0xa0,
    0x05, 0xd0, 0x0b, 0xa0, 0x05, 0xd0, 0x0b, 0xa0, 0x05, 0xd0, 0x0b, 0xa0,
    0x55, 0xd5, 0xab, 0xaa, 0x55, 0xd5, 0xff, 0xff};
#   endif

#  endif /* ULTRIX */
#  define WINDOW     Window
#  define DISPLAY    Display
#  define GLCONTEXT  GLXContext
#  define GLDRAWABLE GLXDrawable

#  define GET_GL_CONTEXT()       glXGetCurrentContext()
#  define GET_GLDEV_CONTEXT()    glXGetCurrentDrawable()
#  define GL_MAKE_CURRENT(a,b,c) glXMakeCurrent(a,b,c)

#  ifndef EXPORT
#   define EXPORT
#  endif  /* EXPORT */
# endif  /* WNT */
#endif /* InterfaceGraphic_X11Header */
