/***********************************************************************

FONCTION :
----------
File OpenGl_togl_depthtest :
GL_DEPTH_TEST
+
GL_LIGHTING

HISTORIQUE DES MODIFICATIONS   :
--------------------------------
22-10-01 : SAV ; Creation.

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <OpenGl_tgl_all.hxx>
#include <OpenGl_tsm_ws.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>


#define  BUC61044    /* 25/10/01 SAV ; added functionality to control gl depth testing
from higher API */
#define  BUC61045    /* 25/10/01 SAV ; added functionality to control gl lighting 
from higher API */

#ifdef BUC61044
void EXPORT call_togl_depthtest( CALL_DEF_VIEW *aview, Tint dFlag )
{
  CMN_KEY_DATA key;

  key.ldata = dFlag;

  TsmSetWSAttri( aview->WsId, WSDepthTest, &key );
}

Tint EXPORT call_togl_isdepthtest( CALL_DEF_VIEW *aview )
{
  CMN_KEY_DATA key;

  TsmGetWSAttri( aview->WsId, WSDepthTest, &key );

  return key.ldata;
}
#endif

#ifdef BUC61045
void EXPORT call_togl_gllight( CALL_DEF_VIEW *aview, Tint dFlag )
{
  CMN_KEY_DATA key;

  key.ldata = dFlag;

  TsmSetWSAttri( aview->WsId, WSGLLight, &key );
}

Tint EXPORT call_togl_isgllight( CALL_DEF_VIEW *aview )
{
  CMN_KEY_DATA key;

  TsmGetWSAttri( aview->WsId, WSGLLight, &key );

  return key.ldata;
}
#endif
