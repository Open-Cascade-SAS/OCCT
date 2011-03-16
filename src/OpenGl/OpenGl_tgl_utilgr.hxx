/***********************************************************************

FONCTION :
----------
file OpenGl_tgl_utilgr.h :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
30-01-97 : FMN ; Suppression call_util_init_indexes()

************************************************************************/

#ifndef  OPENGL_TGL_UTILGR_H
#define  OPENGL_TGL_UTILGR_H

#include <InterfaceGraphic_Graphic3d.hxx>

extern  Tint  call_util_context_group_place(CALL_DEF_GROUP *); /* agroup */
extern  Tint  call_util_osd_putenv(Tchar *, Tchar *); /* symbol, value */
extern  Tint  call_util_rgbindex(Tfloat, Tfloat, Tfloat); /* r, g, b */

#endif
