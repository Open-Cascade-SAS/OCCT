/***********************************************************************

FONCTION :
----------
file OpenGl_togl_deactivateview.c :


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
#include <OpenGl_tgl_funcs.hxx>

void EXPORT
call_togl_deactivateview
(
 CALL_DEF_VIEW * aview
 )
{
  if (aview->WsId != -1)
    call_func_post_struct (aview->WsId, -1, ( float )1.0);
  return;
}
