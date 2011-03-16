#define G003  /* EUG degeneration support
*/

#include <OpenGl_tgl_all.hxx>

#include <InterfaceGraphic_Labels.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>

#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_subrs.hxx>
#include <OpenGl_tgl_util.hxx>

void EXPORT
call_togl_structure
(
 CALL_DEF_STRUCTURE * astructure
 )
{
  Tint  struct_defined;
  Tint  nameset[3];

  nameset[0] = astructure->highlight ? CALL_DEF_STRUCTHIGHLIGHTED
    : CALL_DEF_STRUCTNOHIGHLIGHTED;
  nameset[1] = astructure->visible ? CALL_DEF_STRUCTVISIBLE :
  CALL_DEF_STRUCTNOVISIBLE;
  nameset[2] = astructure->pick ? CALL_DEF_STRUCTPICKABLE :
  CALL_DEF_STRUCTNOPICKABLE;

  struct_defined = call_util_test_structure( astructure->Id );

  if( struct_defined )
    call_func_del_struct( astructure->Id );

  call_func_set_edit_mode( CALL_PHIGS_EDIT_INSERT );
  call_func_open_struct( astructure->Id );
  call_func_label( Structure_LABBegin );
  call_func_label( Structure_LABTransformation );

  /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
  call_func_label( Structure_LABTransPersistence );
  /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */

  call_func_appl_data (0);
#ifdef G003
  call_func_label ( Structure_LABDegenerateModel );
  call_func_offset_elem_ptr ( 1 );
  call_func_set_degenerate_model ( 
    astructure -> ContextFillArea.DegenerationMode, 
    astructure -> ContextFillArea.SkipRatio);
#endif  /* G003 */
  call_func_label( Structure_LABContextLine );
  call_func_label( Structure_LABContextFillArea );
  call_func_label( Structure_LABContextMarker );
  call_func_label( Structure_LABContextText );
  call_func_label( Structure_LABHighlight );
  call_func_label( Structure_LABVisibility );
  call_func_label( Structure_LABPick );
  call_func_label( Structure_LABNameSet );
  call_subr_addnameset( 3, nameset ); 
  call_func_label( Structure_LABConnect );
  call_func_label( astructure->GroupBegin );
  call_func_label( astructure->GroupEnd );
  call_func_close_struct();

  /* call_togl_contextstructure( astructure ); */

  return;
}
