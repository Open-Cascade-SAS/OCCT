/***********************************************************************

FONCTION :
----------
File OpenGl_togl_update :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
20-02-96 : FMN ; Suppression code inutile:
01-04-96 : CAL ; Integration MINSK portage WNT
05-02-97 : FMN ; Suppression de OpenGl_tgl_vis.h
07-10-97 : FMN ; Simplification WNT
08-08-98 : FMN ; ajout PRINT debug
18-11-98 : CAL ; S4062. Ajout des layers.

************************************************************************/

#define RIC120302 /* GG Avoid duplicate code with
//      call_togl_redraw
*/

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tsm_ws.hxx>
#ifndef RIC120302
#include <OpenGl_tgl.hxx>
#include <OpenGl_txgl.hxx>
#include <OpenGl_tgl_tox.hxx>
#include <OpenGl_tgl_funcs.hxx>
#endif

void EXPORT call_togl_update
(
 CALL_DEF_VIEW * aview,
 CALL_DEF_LAYER * anunderlayer,
 CALL_DEF_LAYER * anoverlayer
 )
{
  CMN_KEY_DATA data;
  Tint update_state;
#ifndef RIC120302
  Tint swap = 1; /* swap buffers ? yes */
#endif

  TsmGetWSAttri (aview->WsId, WSUpdateState, &data);
  update_state = data.ldata;
  /*
  * Si l'update_state est NotDone, alors on retrace
  */
  if (update_state == TNotDone) {
#ifdef RIC120302
    call_togl_redraw( aview, anunderlayer, anoverlayer );
#else
    TsmGetWSAttri (aview->WsId, WSWindow, &data);
    if (TxglWinset (call_thedisplay, (Window) data.ldata) == TSuccess) {
      call_func_redraw_all_structs_begin (aview->WsId);
      if (anunderlayer->ptrLayer)
        call_togl_redraw_layer2d (aview, anunderlayer);
      call_func_redraw_all_structs_proc (aview->WsId);
      if (anoverlayer->ptrLayer)
        call_togl_redraw_layer2d (aview, anoverlayer);
      call_func_redraw_all_structs_end (aview->WsId, swap);
      call_togl_redraw_immediat_mode (aview);
    }
#endif
  }

  return;
}
