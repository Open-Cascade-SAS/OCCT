#include <OpenGl_tgl_all.hxx>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_tsm_ws.hxx>

#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#include <GL/gl.h>

extern GLboolean g_fList;

void EXPORT call_togl_backfacing ( CALL_DEF_VIEW* aView ) {

  CMN_KEY_DATA key;

  key.ldata = aView -> Backfacing;

  TsmSetWSAttri ( aView -> WsId, WSBackfacing, &key );

  g_fList = GL_FALSE;

}  /* end call_togl_backfacing */
