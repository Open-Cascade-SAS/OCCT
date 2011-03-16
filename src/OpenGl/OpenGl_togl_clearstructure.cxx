
#include <OpenGl_tgl_all.hxx>

#include <InterfaceGraphic_Labels.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>

#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_funcs.hxx>

void EXPORT
call_togl_clearstructure
(
 CALL_DEF_STRUCTURE * astructure
 )
{
  call_func_set_edit_mode (CALL_PHIGS_EDIT_REPLACE);
  call_func_open_struct (astructure->Id);
  call_func_set_elem_ptr (0);
  call_func_del_elems_labels
    (astructure->GroupBegin, astructure->GroupEnd);
  call_func_close_struct ();
  return;
}
