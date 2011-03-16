/***********************************************************************

FONCTION :
----------
File OpenGl_togl_plane :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
05-02-97 : FMN ; Suppression de OpenGl_tgl_vis.h

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

long EXPORT
call_togl_plane
(
 CALL_DEF_PLANE * aplane,
 long update
 )
{
  static  long  NbPlanes = 1;

  /* rest of the code is redundant */

  return  update ? aplane->PlaneId : NbPlanes++;
}
