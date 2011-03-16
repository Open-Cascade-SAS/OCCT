
#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_funcs.hxx>

/*
call_togl_closegroup (agroup)
CALL_DEF_GROUP * agroup

Opens the associated structure and sets the pointer
at the end of the primitives.

Method :

- go to the end of the group in the specified structure.
- skip the structure element which set all attributes of the structure.

*/

void EXPORT
call_togl_closegroup
(
 CALL_DEF_GROUP * agroup
 )
{
  call_func_close_struct ();
}
