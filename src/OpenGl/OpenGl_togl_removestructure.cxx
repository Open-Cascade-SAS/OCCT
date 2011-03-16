
#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <OpenGl_tgl_funcs.hxx>

void EXPORT
call_togl_removestructure
(
 CALL_DEF_STRUCTURE * astructure
 )
{
  call_func_del_struct (astructure->Id);
  return;
}
