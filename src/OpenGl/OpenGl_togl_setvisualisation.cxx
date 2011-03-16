/***********************************************************************

FONCTION :
----------
File OpenGl_togl_setvisualisation :

TEST :
------

Le main TestOfDesktop.c permet de tester cette classe.


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
17-07-96 : FMN ; Simplification du code
05-02-97 : FMN ; Suppression de OpenGl_tgl_vis.h
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

#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_subrvis.hxx>

/*----------------------------------------------------------------------*/

void EXPORT
call_togl_setvisualisation
(
 CALL_DEF_VIEW * aview
 )
{
  /* activation */
  call_func_set_edit_mode( CALL_PHIGS_EDIT_REPLACE );
  call_func_open_struct( aview->ViewId );
  call_func_set_elem_ptr( 0 );
  call_func_set_elem_ptr_label( View_LABViewContext );
  call_func_offset_elem_ptr( 1 );

  switch( aview->Context.Visualization )
  {
  case 0 : /* VISUAL3D_TOV_WIREFRAME */
    call_func_appl_data( 0 );

    call_func_set_elem_ptr( 0 );
    call_func_set_elem_ptr_label( View_LABHlhsr );
    call_func_offset_elem_ptr( 1 );
    call_util_setvisualization (
      aview, 
      aview->Context.ZBufferActivity,
      CALL_DEF_DISABLE_ZBUFFER);
    break;

  case 1 : /* VISUAL3D_TOV_SHADING */
    switch( aview->Context.Model )
    {
    case 0 : /* VISUAL3D_TOM_NONE */
      call_func_set_int_shad_meth( CALL_PHIGS_SD_NONE );

      break;

    case 1 : /* VISUAL3D_TOM_INTERP_COLOR */
      call_func_set_int_shad_meth( CALL_PHIGS_SD_COLOUR );
      break;

    case 2 : /* VISUAL3D_TOM_FACET */
      call_func_set_int_shad_meth( CALL_PHIGS_SD_NORMAL );
      break;

    case 3 : /* VISUAL3D_TOM_VERTEX */
      call_func_set_int_shad_meth( CALL_PHIGS_SD_COLOUR );
      break;

    case 4 : /* VISUAL3D_TOM_HIDDENLINE */
      call_func_set_int_shad_meth( CALL_PHIGS_SD_NONE );
      break;
    }
    call_func_set_elem_ptr( 0 );
    call_func_set_elem_ptr_label( View_LABHlhsr );
    call_func_offset_elem_ptr( 1 );
    call_util_setvisualization (
      aview, 
      aview->Context.ZBufferActivity,
      CALL_DEF_ENABLE_ZBUFFER);
    break;
  }

  call_func_close_struct();

  return;
}

/*----------------------------------------------------------------------*/

void call_util_setvisualization
(
 CALL_DEF_VIEW * aView, 
 int ZBuffer,
 int Mode
 )
{
  switch (ZBuffer) {
    case -1 : /* non force */
      call_subr_hlhsr (aView, Mode);
      break;
    case 0 : /* force a "disable" */
      call_subr_hlhsr (aView, CALL_DEF_DISABLE_ZBUFFER);
      break;
    case 1 : /* force a "enable" */
      call_subr_hlhsr (aView, CALL_DEF_ENABLE_ZBUFFER);
      break;
  }
}

/*----------------------------------------------------------------------*/
