
#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
#include <OpenGl_tgl_subrvis.hxx>

void EXPORT
call_togl_antialiasing
(
 CALL_DEF_VIEW * aview,
 int tag
 )
{
  call_subr_antialiasing (aview, tag);
  return;
}
