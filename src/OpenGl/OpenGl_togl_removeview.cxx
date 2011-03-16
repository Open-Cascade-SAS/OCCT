
#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_subrvis.hxx>

void EXPORT
call_togl_removeview
(
 CALL_DEF_VIEW * aview
 )
{
  call_func_del_struct (aview->ViewId);
  call_subr_close_ws (aview);
  return;
}
