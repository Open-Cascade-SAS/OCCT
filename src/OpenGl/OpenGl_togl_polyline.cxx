
#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_subrs.hxx>
#include <OpenGl_tgl_utilgr.hxx>

void EXPORT
call_togl_polyline
(
 CALL_DEF_GROUP * agroup,
 CALL_DEF_LISTPOINTS * alpoints
 )
{
  if (! agroup->IsOpen) call_togl_opengroup (agroup);
  switch (alpoints->TypePoints) {
    case 1 : /* Vertex Coordinates Specified */
      call_subr_polyline (alpoints);
      break;
    case 3 : /* Coordinates and Vertex Colour Specified */
      call_subr_polyline_data (alpoints);
      break;
  }
  if (! agroup->IsOpen) call_togl_closegroup (agroup);
  return;
}
