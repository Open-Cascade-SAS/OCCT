
#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_subrs.hxx>
#include <OpenGl_tgl_utilgr.hxx>

void EXPORT
call_togl_curve
(
 CALL_DEF_GROUP * agroup,
 CALL_DEF_LISTPOINTS * alpoints
 )
{
  if (! agroup->IsOpen) call_togl_opengroup (agroup);
  call_subr_curve (alpoints);
  if (! agroup->IsOpen) call_togl_closegroup (agroup);
  return;
}
