
#include <OpenGl_tgl_all.hxx>

#include <InterfaceGraphic_Labels.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>

#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_funcs.hxx>

void EXPORT
call_togl_connect
(
 CALL_DEF_STRUCTURE * afather,
 CALL_DEF_STRUCTURE * ason
 )
{
  call_func_set_edit_mode (CALL_PHIGS_EDIT_INSERT);
  call_func_open_struct (afather->Id);
  call_func_set_elem_ptr (0);
  call_func_set_elem_ptr_label (Structure_LABConnect);
  call_func_exec_struct (ason->Id);
  call_func_close_struct ();
  return;
}
