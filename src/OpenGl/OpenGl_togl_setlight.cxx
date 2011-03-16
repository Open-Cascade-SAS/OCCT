/***********************************************************************

FONCTION :
----------
Gestion des light sous OpenGL


REMARQUES:
---------- 

HISTORIQUE DES MODIFICATIONS   :
--------------------------------
25-06-97 : PCT ; Appel de la toolkit OpenGl_LightBox
et correction de bug passage
plusieurs lights -> aucune lights

************************************************************************/

#include <OpenGl_tgl_all.hxx>

#include <InterfaceGraphic_Labels.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_subrvis.hxx>
#include <OpenGl_inquire.hxx>

#include <OpenGl_LightBox.hxx>
#include <OpenGl_Memory.hxx>

void EXPORT
call_togl_setlight
(
 CALL_DEF_VIEW * aview
 )
{
  Tint *actl=0;
  Tint i;
  CALL_DEF_LIGHT alight;
  CMN_KEY  k;

  actl = new Tint[call_facilities_list.MaxLights];
  if( !actl )
    return;

  /* PCT ; 25-06-97 */
  ResetWksLight(aview->WsId);

  for( i =0; i < aview->Context.NbActiveLight &&
    i < call_facilities_list.MaxLights; i++ )
  {
    alight = aview->Context.ActiveLight[i];
    actl[i] = alight.LightId;
    call_subr_set_light_src_rep( &alight );
  }

  call_func_set_edit_mode(CALL_PHIGS_EDIT_REPLACE);
  call_func_open_struct(aview->ViewId);
  call_func_set_elem_ptr(0);
  call_func_set_elem_ptr_label(View_LABLight);
  call_func_offset_elem_ptr(1);
  k.id = aview->Context.NbActiveLight;
  k.data.pdata = actl;
  TsmAddToStructure( TelLightSrcState, 1, &k );
  call_func_close_struct();

  delete[] actl;

  return;
}
