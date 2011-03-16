
#include <OpenGl_tgl_all.hxx>

#include <InterfaceGraphic_Labels.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>

#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl.hxx>

void EXPORT
call_togl_transformstructure
(
 CALL_DEF_STRUCTURE * astructure
 )
{
  call_func_set_edit_mode (CALL_PHIGS_EDIT_REPLACE);
  call_func_open_struct (astructure->Id);
  call_func_set_elem_ptr (0);
  call_func_set_elem_ptr_label (Structure_LABTransformation);
  call_func_offset_elem_ptr (1);
  call_func_set_local_tran3 (astructure->Transformation, CALL_PHIGS_TYPE_REPLACE);
  call_func_close_struct ();

  return;
}
