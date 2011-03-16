/***********************************************************************

FONCTION :
----------
File OpenGl_togl_opengroup.c :


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

/*
call_togl_opengroup (agroup)
CALL_DEF_GROUP * agroup

Opens the associated structure and sets the pointer
at the end of the primitives.

Method :

- go to the end of the group in the specified structure.
- skip the structure element which set all attributes of the structure.

*/

void EXPORT
call_togl_opengroup
(
 CALL_DEF_GROUP * agroup
 )
{
  call_func_set_edit_mode (CALL_PHIGS_EDIT_INSERT);
  call_func_open_struct (agroup->Struct->Id);
  call_func_set_elem_ptr (0);
  call_func_set_elem_ptr_label (agroup->LabelEnd);
  call_func_offset_elem_ptr (- call_util_context_group_place (agroup) - 1);
}
