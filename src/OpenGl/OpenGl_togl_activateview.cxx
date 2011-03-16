
#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
#include <OpenGl_tgl_funcs.hxx>

void EXPORT
call_togl_activateview
(
 CALL_DEF_VIEW * aview
 )
{
  if (aview->WsId != -1)
    call_func_post_struct (aview->WsId, aview->ViewId, ( float )1.0);
  return;
}
