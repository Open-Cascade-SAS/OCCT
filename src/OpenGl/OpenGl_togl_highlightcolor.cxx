
#include <OpenGl_tgl_all.hxx>

#include <InterfaceGraphic_Labels.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>

#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_subrs.hxx>
#include <OpenGl_telem_highlight.hxx>
#include <OpenGl_tgl_utilgr.hxx>

void EXPORT
call_togl_highlightcolor
(
 CALL_DEF_STRUCTURE * astructure,
 CALL_DEF_COLOR * acolor,
 int create
 )
{
  Tint index;
  TEL_HIGHLIGHT hrep = { TelHLForcedColour };

  call_func_set_edit_mode (CALL_PHIGS_EDIT_REPLACE);
  call_func_open_struct (astructure->Id);
  call_func_set_elem_ptr (0);
  call_func_del_elems_labels (Structure_LABHighlight, Structure_LABVisibility);
  call_func_close_struct ();

  if (create) {
    call_func_set_edit_mode (CALL_PHIGS_EDIT_INSERT);
    call_func_open_struct (astructure->Id);
    call_func_set_elem_ptr (0);
    call_func_set_elem_ptr_label(Structure_LABHighlight);
    index = call_util_rgbindex(acolor->r, acolor->g, acolor->b);
    hrep.col.rgb[0] = acolor->r,
      hrep.col.rgb[1] = acolor->g,
      hrep.col.rgb[2] = acolor->b;
    TelSetHighlightRep (0, index, &hrep);
    call_subr_set_highl_rep (index);
    call_func_close_struct ();
  }
  return;
}
