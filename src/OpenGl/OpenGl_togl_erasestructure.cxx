
#include <OpenGl_tgl_all.hxx>
#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_subrs.hxx>

void EXPORT
call_togl_erasestructure
(
 int ViewId,
 int StructId
 )
{
  Tint  i;

  if (call_subr_get_exec_struct (ViewId, StructId, &i) == TSuccess) {
    call_func_open_struct (ViewId);
    call_func_set_elem_ptr (i);
    call_func_del_elem ();
    call_func_close_struct ();
  }
  return;
}
