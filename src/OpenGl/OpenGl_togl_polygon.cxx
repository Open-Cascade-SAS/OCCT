/***********************************************************************

FONCTION :
----------
File OpenGl_togl_polygon.c :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
05-08-97 : PCT ; Support texture mapping

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_subrs.hxx>
#include <OpenGl_tgl_utilgr.hxx>

void EXPORT
call_togl_polygon
(
 CALL_DEF_GROUP * agroup,
 CALL_DEF_FACET * afacet
 )
{
  if (! agroup->IsOpen) call_togl_opengroup (agroup);
  switch (afacet->TypePoints) {
    case 1 : /* Vertex Coordinates Specified */
      if (afacet->NormalIsDefined)
        call_subr_polygon_data (afacet);
      else
        call_subr_polygon (afacet);
      break;
    case 2 : /* Coordinates and Vertex Normal Specified */
    case 5 : /* Coordinates and Vertex Normal and Texture Coordinate Specified */
      call_subr_polygon_data (afacet);
      break;
  }
  if (! agroup->IsOpen) call_togl_closegroup (agroup);
  return;
}
