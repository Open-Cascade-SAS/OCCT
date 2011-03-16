/***********************************************************************

FONCTION :
----------
File OpenGl_togl_pickid.c :


REMARQUES:
---------- 

HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
03-03-98 : CAL ; Modification des structures CALL_DEF_GROUP et STRUCTURE

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/ 

#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_utilgr.hxx>

/*----------------------------------------------------------------------*/

void EXPORT
call_togl_pickid
(
 CALL_DEF_GROUP * agroup
 )
{
  call_func_open_struct( agroup->Struct->Id );
  call_func_set_elem_ptr( 0 );
  call_func_set_elem_ptr_label( agroup->LabelBegin );
  if( agroup->PickId.IsDef )
  {
    if( agroup->PickId.IsSet )
    {
      call_func_offset_elem_ptr( 1 );
      call_func_set_edit_mode( CALL_PHIGS_EDIT_REPLACE );
    }
    else
      call_func_set_edit_mode( CALL_PHIGS_EDIT_INSERT );

    call_func_set_pick_id( agroup->PickId.Value );
  }
  else
  {
    if( agroup->PickId.IsSet )
    {
      call_func_offset_elem_ptr( 1 );
      call_func_del_elem();
    }
  }
  call_func_close_struct();

  return;
}
