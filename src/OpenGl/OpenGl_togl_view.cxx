/***********************************************************************

FONCTION :
----------
File OpenGl_togl_view :


REMARQUES:
----------       

HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
08-07-96 : FMN ; Suppression appel call_togl_ car deja fait dans
call_togl_cliplimit.
17-07-96 : FMN ; Suppression appel call_subr_hlhsr
30-01-97 : FMN ; Suppression call_util_init_indexes()
30-06-97 : FMN ; Suppression code inutile
08-09-97 : CAL ; subr_open_ws retourne 0 si pb.
24-12-97 : FMN ; Suppression de call_func_set_back_int_shad_meth()

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/ 

#include <OpenGl_tgl_all.hxx>

#include <InterfaceGraphic_Labels.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#include <OpenGl_tgl_vis.hxx>
#include <OpenGl_tgl_util.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_subrvis.hxx>
#include <OpenGl_tgl_utilgr.hxx>

/*----------------------------------------------------------------------*/

int EXPORT
call_togl_view
(
 CALL_DEF_VIEW * aview
 )
{

  if (! call_subr_open_ws(aview))
    return 0;

  call_util_init_filters(aview->WsId);

  if( call_util_test_structure(aview->ViewId) )
    call_func_del_struct(aview->ViewId);

  call_func_set_edit_mode(CALL_PHIGS_EDIT_INSERT);

  call_func_open_struct(aview->ViewId);
  call_func_label (View_LABViewIndex);
  call_func_set_view_ind (aview->ViewId);

  call_func_label (View_LABViewContext);
  switch (aview->Context.Visualization)
  {
  case 0 : /* VISUAL3D_TOV_WIREFRAME */
    call_func_appl_data (0);
    break;
  case 1 : /* VISUAL3D_TOV_SHADING */
    switch (aview->Context.Model)
    {
    case 0 : /* VISUAL3D_TOM_NONE */
      call_func_set_int_shad_meth (CALL_PHIGS_SD_NONE);
      break;
    case 1 : /* VISUAL3D_TOM_INTERP_COLOR */
      call_func_set_int_shad_meth (CALL_PHIGS_SD_COLOUR);
      break;
    case 2 : /* VISUAL3D_TOM_FACET */
      call_func_set_int_shad_meth
        (CALL_PHIGS_SD_DOT_PRODUCT);
      break;
    case 3 : /* VISUAL3D_TOM_VERTEX */
      call_func_set_int_shad_meth (CALL_PHIGS_SD_NORMAL);
      break;
    case 4 : /* VISUAL3D_TOM_HIDDENLINE */
      call_func_set_int_shad_meth (CALL_PHIGS_SD_NONE);
      break;
    }
    break;
  }

  call_func_label (View_LABHlhsr);
  call_func_appl_data (0);

  call_func_label (View_LABLight);
  call_func_appl_data (0);

  call_func_label (View_LABPlane);
  call_func_appl_data (0);

  call_func_label (View_LABAliasing);
  call_func_appl_data (0);

  call_func_label (View_LABDepthCueing);
  call_func_appl_data (0);

  call_func_label (View_LABPriority00);
  call_func_label (View_LABPriority01);
  call_func_label (View_LABPriority02);
  call_func_label (View_LABPriority03);
  call_func_label (View_LABPriority04);
  call_func_label (View_LABPriority05);
  call_func_label (View_LABPriority06);
  call_func_label (View_LABPriority07);
  call_func_label (View_LABPriority08);
  call_func_label (View_LABPriority09);
  call_func_label (View_LABPriority10);
  call_func_label (View_LABEnd);
  call_func_label (View_LABImmediat1);
  call_func_label (View_LABImmediat2);
  call_func_close_struct ();

  if( call_viewrep.active_status == TOff )
  {/* First time. Set defaults */
    call_viewrep.active_status = TOn;
    call_viewrep.shield_indicator = TOn;
    call_viewrep.border_indicator = TOff; /* non utilise */
    call_viewrep.shield_colour.rgb[0] = ( float )0.0,
      call_viewrep.shield_colour.rgb[1] = ( float )0.0,
      call_viewrep.shield_colour.rgb[2] = ( float )0.0;
    call_viewrep.border_colour.rgb[0] = ( float )0.0,
      call_viewrep.border_colour.rgb[1] = ( float )0.0,
      call_viewrep.border_colour.rgb[2] = ( float )0.0;
  }

  return 1;
}
