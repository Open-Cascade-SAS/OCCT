/***********************************************************************

FONCTION :
----------
File OpenGl_tgl_all.h :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
07-10-97 : FMN ; Simplification WNT
16-06-2000 : ATS : G005 : Move type declarations to InterfaceGraphic_tgl_all.h

************************************************************************/

#ifndef  OPENGL_TGL_ALL_H
#define  OPENGL_TGL_ALL_H

#if defined(WNT) && !defined(HAVE_NO_DLL)
# ifdef __OpenGl_DLL
#  define EXPORT __declspec(dllexport)
# else
#  define EXPORT
# endif  /* DLL */
#else
# define EXPORT
#endif  /* WNT */

#include <InterfaceGraphic.hxx>
#include <InterfaceGraphic_tgl_all.hxx>
#include <InterfaceGraphic_telem.hxx>


#ifdef WNT
// CAL : pas trouve de definition de PI dans les includes WNT
#define M_PI 3.14159265358979323846
#endif

#include <GL/gl.h>
#include <GL/glu.h>


#endif  /* OPENGL_TGL_ALL_H */
