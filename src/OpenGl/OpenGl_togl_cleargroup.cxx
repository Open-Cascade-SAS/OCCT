/***********************************************************************

FONCTION :
----------
File OpenGl_togl_cleargroup.c :


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
#include <OpenGl_tgl_funcs.hxx>

/*----------------------------------------------------------------------*/

void EXPORT
call_togl_cleargroup
(
 CALL_DEF_GROUP * agroup
 )
{
  call_func_set_edit_mode (CALL_PHIGS_EDIT_REPLACE);
  call_func_open_struct (agroup->Struct->Id);
  call_func_set_elem_ptr (0);
  call_func_del_elems_labels (agroup->LabelBegin, agroup->LabelEnd);
  call_func_close_struct ();
  return;
}
