/***********************************************************************

FONCTION :
----------
file OpenGl_togl_cliplimit.c :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : CAL ; Creation.
05-02-97 : FMN ; Suppression de OpenGl_tgl_vis.h
10-07-96 : FMN ; Suppression #define sur calcul matrice

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_telem_view.hxx>

/*----------------------------------------------------------------------*/
/*
* Variables globales
*/

TEL_VIEW_REP  call_viewrep;

/*----------------------------------------------------------------------*/

void EXPORT
call_togl_cliplimit
(
 CALL_DEF_VIEW * aview,
 int wait
 )
{
  Tint waitwait;

  call_viewrep.clip_limit.xmin = ( float )0.0;
  call_viewrep.clip_limit.xmax = ( float )1.0;
  call_viewrep.clip_limit.ymin = ( float )0.0;
  call_viewrep.clip_limit.ymax = ( float )1.0;

  call_viewrep.clip_limit.zmax =
    ( aview->Context.ZClipFrontPlane - aview->Mapping.BackPlaneDistance ) /
    ( aview->Mapping.FrontPlaneDistance - aview->Mapping.BackPlaneDistance );
  call_viewrep.clip_limit.zmin =
    ( aview->Context.ZClipBackPlane - aview->Mapping.BackPlaneDistance ) /
    ( aview->Mapping.FrontPlaneDistance - aview->Mapping.BackPlaneDistance );
  if( call_viewrep.clip_limit.zmin < 0.0 )
    call_viewrep.clip_limit.zmin = ( float )0.0;
  if( call_viewrep.clip_limit.zmax > 1.0 )
    call_viewrep.clip_limit.zmax = ( float )1.0;
  if( call_viewrep.clip_limit.zmin > call_viewrep.clip_limit.zmax )
  {
    call_viewrep.clip_limit.zmin = ( float )0.0;
    call_viewrep.clip_limit.zmax = ( float )1.0;
  }

  call_viewrep.clip_xy    = CALL_PHIGS_IND_NO_CLIP;
  call_viewrep.clip_back  = CALL_PHIGS_IND_NO_CLIP;
  call_viewrep.clip_front = CALL_PHIGS_IND_NO_CLIP;
  if( aview->Context.BackZClipping )
    call_viewrep.clip_back  = CALL_PHIGS_IND_CLIP;
  if( aview->Context.FrontZClipping )
    call_viewrep.clip_front = CALL_PHIGS_IND_CLIP;

  if( (!wait) && (aview->WsId != -1) )
  {
    waitwait = 1;
    call_togl_viewmapping( aview, waitwait );
    call_togl_vieworientation( aview, waitwait );
    TelSetViewRepresentation( aview->WsId, aview->ViewId, &call_viewrep );
  }
  return;
}
/*----------------------------------------------------------------------*/
