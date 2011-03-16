/***********************************************************************

FONCTION :
----------
file OpenGl_togl_viewmapping.c :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : CAL ; Creation.
10-07-96 : FMN ; Suppression #define sur calcul matrice
25-07-96 : FMN ; Suppression code inutile
18-11-98 : FMN ; Correction sur la gestion de la perspective (cf Programming Guide)
09-12-98 : FMN ; Correction erreur de compilation sur WNT
17-12-98 : FMN ; Correction erreur de compilation sur WNT

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
#include <OpenGl_tgl_vis.hxx>
#include <OpenGl_tgl_funcs.hxx>

#include <GL/gl.h>

/*----------------------------------------------------------------------*/
/*
* Variable globales
*/

EXPORT extern GLboolean env_walkthrow; /* definit dans OpenGl_togl_begin.c */

/*----------------------------------------------------------------------*/

int EXPORT
call_togl_viewmapping
(
 CALL_DEF_VIEW * aview,
 int wait
 )
{
  int     i, j;
  float   ratio;
  Tint    waitwait;
  Tint    err_ind;

  TEL_VIEW_MAPPING   Map;

  err_ind = 0;
  ratio = aview->DefWindow.dy / aview->DefWindow.dx;

  Map.window.xmin = aview->Mapping.WindowLimit.um;
  Map.window.ymin = aview->Mapping.WindowLimit.vm;
  Map.window.xmax = aview->Mapping.WindowLimit.uM;
  Map.window.ymax = aview->Mapping.WindowLimit.vM;

  Map.viewport.xmin = ( float )0.0;
  Map.viewport.xmax = ( ( float )1. < ( float )1./ratio ? ( float )1. : ( float )1./ratio );
  Map.viewport.ymin = ( float )0.0;
  Map.viewport.ymax = ( ( float )1. < ratio ? ( float )1. : ratio );
  Map.viewport.zmin = ( float )0.0;
  Map.viewport.zmax = ( float )1.0;

  /* projection type */
  switch( aview->Mapping.Projection )
  {
  case 0 :
    Map.proj = CALL_PHIGS_TYPE_PERSPECT;
    break;
  case 1 :
    Map.proj = CALL_PHIGS_TYPE_PARAL;
    break;
  }

  /* projection reference point */
  Map.prp[0] = aview->Mapping.ProjectionReferencePoint.x;
  Map.prp[1] = aview->Mapping.ProjectionReferencePoint.y;
  if (env_walkthrow)
    Map.prp[2] = aview->Mapping.ProjectionReferencePoint.z;
  else
    Map.prp[2] = aview->Mapping.FrontPlaneDistance + 
    aview->Mapping.ProjectionReferencePoint.z;

  /* view plane distance */
  Map.vpd = aview->Mapping.ViewPlaneDistance;

  /* back plane distance */
  Map.bpd = aview->Mapping.BackPlaneDistance;

  /* front plane distance */
  Map.fpd = aview->Mapping.FrontPlaneDistance;

  /* use user-defined matrix */
  if ( aview->Mapping.IsCustomMatrix ) {
    for( i = 0; i < 4; i++ )
      for( j = 0; j < 4; j++ )
        call_viewrep.mapping_matrix[i][j] = aview->Mapping.ProjectionMatrix[i][j];
  }
  else 
    TelEvalViewMappingMatrix( &Map, &err_ind, call_viewrep.mapping_matrix );

  if( !err_ind && aview->WsId != -1 )
  {
    call_viewrep.extra.map = Map;
    if( !wait )
    {
      waitwait = 1;
      call_togl_vieworientation( aview, waitwait );
      call_togl_cliplimit( aview, waitwait );
      TelSetViewRepresentation( aview->WsId, aview->ViewId, &call_viewrep );
      call_togl_ratio_window( aview );
    }
  }
  return err_ind;
}
/*----------------------------------------------------------------------*/
