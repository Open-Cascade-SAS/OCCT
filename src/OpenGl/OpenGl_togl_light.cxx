
#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

long EXPORT
call_togl_light
(
 CALL_DEF_LIGHT * alight,
 long update
 )
{
  static long NbLights = 1;

  /* rest of the code is redundant */
  return update ? alight->LightId : NbLights++;
}
