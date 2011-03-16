/***********************************************************************

FONCTION :
----------
File OpenGl_togl_ration_window :


REMARQUES:
---------- 

HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
02-07-96 : FMN ; Suppression WSWSHeight et WSWSWidth
05-02-97 : FMN ; Suppression de OpenGl_tgl_vis.h

************************************************************************/
#define IMP100701 /* GG Prevent window resizing on when redraw
in a pixmap
*/

/*----------------------------------------------------------------------*/
/*
* Includes
*/ 

#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_tsm_ws.hxx>
#include <OpenGl_tgl_subrvis.hxx>

void EXPORT
call_togl_ratio_window
(
 CALL_DEF_VIEW * aview
 )
{
  CMN_KEY_DATA  k;

#ifdef IMP100701
  if( aview->DefBitmap.bitmap ) return;
#endif

  call_subr_resize( aview );

  k.ldata = ( Tint )aview->DefWindow.dx;
  TsmSetWSAttri( aview->WsId, WSWidth, &k );
  k.ldata = ( Tint )aview->DefWindow.dy;
  TsmSetWSAttri( aview->WsId, WSHeight, &k );

  return;
}
