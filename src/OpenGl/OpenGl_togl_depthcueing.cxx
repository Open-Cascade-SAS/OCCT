/***********************************************************************

FONCTION :
----------
file OpenGl_togl_depthcueing.c :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : CAL ; Creation.
05-02-97 : FMN ; Suppression de OpenGl_tgl_vis.h

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
#include <OpenGl_tgl_subrvis.hxx>

void EXPORT
call_togl_depthcueing
(
 CALL_DEF_VIEW * aview,
 int tag
 )
{
  call_subr_depthcueing (aview, tag);
  return;
}
