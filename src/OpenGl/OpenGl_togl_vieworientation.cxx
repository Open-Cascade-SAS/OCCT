/***********************************************************************

FONCTION :
----------
file OpenGl_togl_vieworientation.c :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : CAL ; Creation.
10-07-96 : FMN ; Suppression #define sur calcul matrice

************************************************************************/
#define GER61454        //GG 14-09-99 Activates the model clipping planes

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
#include <OpenGl_tgl_vis.hxx>
#include <OpenGl_tgl_funcs.hxx>

/*----------------------------------------------------------------------*/

int EXPORT
call_togl_vieworientation
(
 CALL_DEF_VIEW * aview,
 int wait
 )
{
  int i, j;
  Tint waitwait;
  Tint err_ind;           /* OUT error indicator */

  Tfloat Vrp[3];
  Tfloat Vpn[3];
  Tfloat Vup[3];
  Tfloat ScaleFactors[3];

  err_ind = 0;

  Vrp[0] = aview->Orientation.ViewReferencePoint.x;
  Vrp[1] = aview->Orientation.ViewReferencePoint.y;
  Vrp[2] = aview->Orientation.ViewReferencePoint.z;

  Vpn[0] = aview->Orientation.ViewReferencePlane.x;
  Vpn[1] = aview->Orientation.ViewReferencePlane.y;
  Vpn[2] = aview->Orientation.ViewReferencePlane.z;

  Vup[0] = aview->Orientation.ViewReferenceUp.x;
  Vup[1] = aview->Orientation.ViewReferenceUp.y;
  Vup[2] = aview->Orientation.ViewReferenceUp.z;

  ScaleFactors[0] = aview->Orientation.ViewScaleX;
  ScaleFactors[1] = aview->Orientation.ViewScaleY;
  ScaleFactors[2] = aview->Orientation.ViewScaleZ;

  /* use user-defined matrix */
  if ( aview->Orientation.IsCustomMatrix ) {
    for( i = 0; i < 4; i++ )
      for( j = 0; j < 4; j++ )
        call_viewrep.orientation_matrix[i][j] = aview->Orientation.ModelViewMatrix[i][j];
  }
  else
    TelEvalViewOrientationMatrix( Vrp, Vpn, Vup, ScaleFactors, &err_ind, call_viewrep.orientation_matrix);

  if( !err_ind && aview->WsId != -1 )
  {
    call_viewrep.extra.vrp[0] = Vrp[0],
      call_viewrep.extra.vrp[1] = Vrp[1],
      call_viewrep.extra.vrp[2] = Vrp[2];

    call_viewrep.extra.vpn[0] = Vpn[0],
      call_viewrep.extra.vpn[1] = Vpn[1],
      call_viewrep.extra.vpn[2] = Vpn[2];

    call_viewrep.extra.vup[0] = Vup[0],
      call_viewrep.extra.vup[1] = Vup[1],
      call_viewrep.extra.vup[2] = Vup[2];

    call_viewrep.extra.scaleFactors[0] = ScaleFactors[0],
      call_viewrep.extra.scaleFactors[1] = ScaleFactors[1],
      call_viewrep.extra.scaleFactors[2] = ScaleFactors[2];

    if( !wait )
    {
      waitwait = 1;
      call_togl_viewmapping( aview, waitwait );
      call_togl_cliplimit( aview, waitwait );
#ifdef GER61454
      call_togl_setplane( aview );
#endif
      TelSetViewRepresentation( aview->WsId, aview->ViewId, &call_viewrep );
    }
  }
  return err_ind;
}
/*----------------------------------------------------------------------*/
