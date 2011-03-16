
#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
#include <OpenGl_tgl_subrvis.hxx>

void EXPORT
call_togl_background
(
 CALL_DEF_VIEW * aview
 )
{
  TEL_COLOUR  col;

  col.rgb[0]  = aview->DefWindow.Background.r,
    col.rgb[1]  = aview->DefWindow.Background.g,
    col.rgb[2]  = aview->DefWindow.Background.b;
  call_subr_set_background (aview->WsId, &col);
  return;
}
