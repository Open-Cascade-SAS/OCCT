// File:	Draw_WindowBase.hxx
// Created:	Fri Mar 17 15:31:19 1995
// Author:	Remi LEQUETTE
//		<rle@phobox>


#ifndef _Draw_WindowBase_HeaderFile
#define _Draw_WindowBase_HeaderFile

#ifndef WNT
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#endif

// Definition de la class Base_Window 
//===================================
struct Base_Window
{
    GC gc;
    XSetWindowAttributes xswa;
};


#endif
