/*
File OpenGl_togl_parray.c
Created 16/06/2000 : ATS : G005
*/

#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_subrs.hxx>
#include <OpenGl_tgl_utilgr.hxx>

void EXPORT call_togl_parray
(
 CALL_DEF_GROUP *agroup,
 CALL_DEF_PARRAY *parray
 )
{
  if (! agroup->IsOpen) call_togl_opengroup (agroup);
  call_subr_parray (parray);
  if (! agroup->IsOpen) call_togl_closegroup (agroup);
  return;
}
