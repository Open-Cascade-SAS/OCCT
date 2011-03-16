/***********************************************************************

FONCTION :
----------
Include OpenGl_tgl :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
11-03-96 : FMN ; Reference a InterfaceGraphic_Cextern.hxx
03-03-97 : FMN ; Ajout displaylist pour le mode transient
15-03-98 : FMN ; Ajout mode animation
08-07-98 : FMN ; On utilise OpenGl_animation.h pour le mode animation.

************************************************************************/

#ifndef  OPENGL_TGL_H
#define  OPENGL_TGL_H

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#ifndef EXPORT
# if defined(WNT) && !defined(HAVE_NO_DLL)
#  ifdef __OpenGl_DLL
#   define EXPORT __declspec( dllexport )
#  else
#   define EXPORT
#  endif  /* __OpenGl_DLL */
# else
#  define EXPORT
# endif  /* WNT */
#endif  /* EXPORT */


//#include <InterfaceGraphic_Cextern.hxx>
#include <InterfaceGraphic.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#include <OpenGl_inquire.hxx>

/*----------------------------------------------------------------------*/
/*
* Prototypes fonctions externes
*/

extern CALL_DEF_INQUIRE call_facilities_list;

extern  EXPORT void  call_togl_set_environment(char*); /* adisplay */
extern  EXPORT void  call_togl_inquirefacilities(void);
extern  EXPORT void  call_togl_polygon_set(CALL_DEF_GROUP *, CALL_DEF_LISTFACETS *);
extern  EXPORT void  call_togl_curve(CALL_DEF_GROUP *, CALL_DEF_LISTPOINTS *);
extern  EXPORT void  call_togl_redraw_immediat_mode(CALL_DEF_VIEW *);
extern  EXPORT void  call_togl_redraw_layer2d(CALL_DEF_VIEW * aview,
                                              CALL_DEF_LAYER * alayer);

/*----------------------------------------------------------------------*/

#endif
