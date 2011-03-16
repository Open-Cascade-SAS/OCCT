/***********************************************************************

FONCTION :
----------
File OpenGl_tsm_ws :


REMARQUES:
----------       

HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
02-07-96 : FMN ; Suppression WSWSHeight, WSWSWidth et WSType
17-07-96 : FMN ; Ajout WSZbuffer
24-04-97 : FMN ; Ajout displaylist pour le mode transient
07-08-97 : PCT ; ajout texture environnante
22-10-01 : SAV ; added flag WSDepthTest to control depth testing
24-10-01 : SAV ; added flag WSGLLight to control GL_LIGHTING
particularly, this thechnique is used in triedron displaying.

************************************************************************/

#ifndef  OPENGL_TSM_WS_H
#define  OPENGL_TSM_WS_H

#define G003  /* EUG 11-01-00 backfacing management
*/

#define  BUC61044    /* SAV ; added functionality to control gl depth testing
from higher API */
#define  BUC61045    /* SAV ; added functionality to control gl lighting 
from higher API */
#define OCC1188 /* SAV Added 23-12-02 background texture attribute */

#include <OpenGl_cmn_varargs.hxx>

typedef  enum
{
  WSTextureEnv, WSSurfaceDetail,
  WSFilters, WSWindow, WSWidth, WSHeight, WSDbuff,
  WSViewStid, WSLights, WSViews, WSBackground, WSNeedsClear, WSHighlights,
  WSDepthCues, WSTransparency, WSZbuffer, WSTransient, WSRetainMode, WSUpdateState, WSPixmap, WSGLXPixmap, WSDepth
#ifdef G003
  , WSBackfacing
#endif  /* G003 */
#ifdef BUC61044
  , WSDepthTest
#endif  
#ifdef BUC61045
  , WSGLLight
#endif  
#ifdef OCC1188
  , WSBgTexture
#endif
  , WSBgGradient    
} TWSAttri;

extern  TStatus  TsmRegisterWsid( Tint );
extern  TStatus  TsmSetWSAttri( Tint, TWSAttri, cmn_key_data );
extern  TStatus  TsmGetWSAttri( Tint, TWSAttri, cmn_key_data );
/* struct_id, size, ws,    actual_size */
extern  TStatus  TsmGetWSPosted( Tint,       Tint, Tint*, Tint* );
extern  TStatus  TsmUnregisterWsid( Tint wsid );
extern  TStatus  TsmInitUpdateState();

#endif
