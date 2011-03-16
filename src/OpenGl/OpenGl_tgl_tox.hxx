/***********************************************************************

FONCTION :
----------
Include OpenGl_tgl_tox :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
20-02-96 : FMN ; Suppression code inutile:
on ne garde que call_tox_open_display(). Les autres
point d'entree sont inutiles [cf OpenGl_txgl.c]
11-03-98 : CAL ; Suppression code inutile
12-03-02 ; GG  ; RIC120302 Add call_tox_set_display

************************************************************************/

#ifndef  OPENGL_TGL_TOX_H
#define  OPENGL_TGL_TOX_H

#include <InterfaceGraphic.hxx>

extern  DISPLAY *call_thedisplay; /* display pointer; defined in tgl/tox */

/*----------------------------------------------------------------------*/
/*
* Prototypes fonctions externes
*/

extern  Tint   call_tox_open_display(void);
extern  Tint   call_tox_set_display(void*);
extern  void   call_tox_getscreen(Tint *, Tint *); /* w, h */
extern  void   call_tox_rect(int,WINDOW,int,int) ;
extern  Tfloat call_tox_getpitchsize();


#endif
