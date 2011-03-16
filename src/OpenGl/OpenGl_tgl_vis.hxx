/***********************************************************************

FONCTION :
----------
Include OpenGl_tgl_vis.h :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
11-03-96 : FMN ; Reference a InterfaceGraphic_Cextern.hxx
29-01-97 : FMN ; Suppression de call_togl_doublebuffer().
05-02-97 : FMN ; Suppression de call_togl_minmaxvalues().
et de call_togl_projection().

************************************************************************/

#ifndef  OPENGL_TGL_VIS_H
#define  OPENGL_TGL_VIS_H

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <InterfaceGraphic_Visual3d.hxx>

#include <OpenGl_telem_view.hxx>

/*----------------------------------------------------------------------*/
/*
* Variables externes
*/

extern TEL_VIEW_REP  call_viewrep; /* defined in call_togl_cliplimit.c */

/*----------------------------------------------------------------------*/

#endif
