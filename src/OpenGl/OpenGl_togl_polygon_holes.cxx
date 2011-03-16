
#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_subrs.hxx>
#include <OpenGl_tgl_utilgr.hxx>

void EXPORT
call_togl_polygon_holes
(
 CALL_DEF_GROUP * agroup,
 CALL_DEF_LISTFACETS * alfacets
 )
{
#ifdef OK
  if (! agroup->IsOpen) call_togl_opengroup (agroup);
  switch (alfacets->LFacets[0].TypePoints) {
    case 1 : /* Vertex Coordinates Specified */
      if (alfacets->LFacets[0].NormalIsDefined)
        call_subr_polygon_holes_data (alfacets);
      else
        call_subr_polygon_holes (alfacets);
      break;
    case 2 : /* Coordinates and Vertex Normal Specified */
      call_subr_polygon_holes_data (alfacets);
      break;
  }
  if (! agroup->IsOpen) call_togl_closegroup (agroup);
#endif
  return;
}
