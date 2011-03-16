
#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_subrs.hxx>
#include <OpenGl_tgl_utilgr.hxx>

void EXPORT
call_togl_quadrangle
(
 CALL_DEF_GROUP * agroup,
 CALL_DEF_QUAD * aquad
 )
{
  if (! agroup->IsOpen) call_togl_opengroup (agroup);
  call_subr_quadrangle (aquad);
  if (! agroup->IsOpen) call_togl_closegroup (agroup);
  return;
}
