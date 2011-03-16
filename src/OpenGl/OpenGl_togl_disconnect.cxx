
#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_subrs.hxx>

void EXPORT
call_togl_disconnect
(
 CALL_DEF_STRUCTURE * afather,
 CALL_DEF_STRUCTURE * ason
 )
{
  Tint i;

  if (call_subr_get_exec_struct (afather->Id, ason->Id, &i) == TSuccess) {
    call_func_open_struct (afather->Id);
    call_func_set_elem_ptr (i);
    call_func_del_elem ();
    call_func_close_struct ();
  }
  return;
}
