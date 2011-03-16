
#include <OpenGl_tgl_all.hxx>

#include <InterfaceGraphic_Labels.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>

#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_subrs.hxx>

void EXPORT
call_togl_namesetstructure
(
 CALL_DEF_STRUCTURE * astructure
 )
{
  Tint nameset[3];

  nameset[0] = astructure->highlight ? CALL_DEF_STRUCTHIGHLIGHTED : CALL_DEF_STRUCTNOHIGHLIGHTED;
  nameset[1] = astructure->visible   ? CALL_DEF_STRUCTVISIBLE     : CALL_DEF_STRUCTNOVISIBLE;
  nameset[2] = astructure->pick      ? CALL_DEF_STRUCTPICKABLE    : CALL_DEF_STRUCTNOPICKABLE;

  call_func_set_edit_mode(CALL_PHIGS_EDIT_REPLACE);
  call_func_open_struct(astructure->Id);
  call_func_set_elem_ptr(0);
  call_func_set_elem_ptr_label(Structure_LABNameSet);
  call_func_offset_elem_ptr(1);
  call_subr_addnameset(3, nameset);
  call_func_close_struct();

  return;
}
