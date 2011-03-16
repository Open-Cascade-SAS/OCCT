/***********************************************************************

FONCTION :
----------
File OpenGl_ws :


REMARQUES:
----------       

HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
02-07-96 : FMN ; Suppression WSWSHeight, WSWSWidth et WSType
17-07-96 : FMN ; Ajout WSZbuffer
24-04-97 : FMN ; (PRO4063) Ajout displaylist pour le mode transient
27-07-97 : CAL ; Portage NT (GLuint)
28-08-97 : PCT ; ajout texture mapping
13-03-98 : FMN ; Suppression variable WSNeedsClear
08-07-98 : FMN ; Changement du nom de la primitive call_togl_erase_immediat_mode()
-> call_togl_erase_animation_mode.

************************************************************************/

#define G003  /* EUG 11-01-00 backfacing management 
*/

#define OCC1188 /* SAV 23/12/02 background texture management */

/*----------------------------------------------------------------------*/
/*
* Includes
*/ 

#include <OpenGl_tgl_all.hxx>

#include <OpenGl_tsm.hxx>

#include <OpenGl_tsm_ws.hxx>
#include <OpenGl_animation.hxx>
#include <OpenGl_Memory.hxx>

/*----------------------------------------------------------------------*/
/*
* Constantes
*/ 

#define  WS_HTBL_SIZE  23
#define  BUC61044    /* 25/10/01 SAV ; added functionality to control gl depth testing
from higher API */
#define  BUC61045    /* 25/10/01 SAV ; added functionality to control gl lighting 
from higher API */

/*----------------------------------------------------------------------*/
/*
* Types definitions
*/ 

#ifdef OCC1188
/* background texture properties */
struct WS_BG_TEXTURE
{
  Tuint texId;
  Tint width;
  Tint height;
  Tint style; /* 0 - centered, 1 - tiled, 2 - stretch */
  IMPLEMENT_MEMORY_OPERATORS
};
typedef WS_BG_TEXTURE *ws_bg_texture;
#endif

typedef TSM_BG_GRADIENT WS_BG_GRADIENT;
typedef WS_BG_GRADIENT *ws_bg_gradient;

struct WS_REC
{
  Tint    texture_env;
  Tint    surface_detail;
  void    *filters;
  Tint     window;
  Tint     width;
  Tint     height;
  Tint     d_buff;
  Tint     view_stid;
  void    *lights;
  void    *views;
  Tfloat   backg[3];
  void    *highlights;
  void    *depthcues;
  Tint     update_state;
  Tint     transparency;
  Tint     zbuffer;
  Tint     transient;
  Tint     retainMode;
#ifndef WNT
  Pixmap   pixmap_id;
  GLXPixmap
    glx_pixmap;
  Tint     wsdepth;
#else

#endif
#ifdef G003
  Tint     backfacing;
#endif  /* G003 */

#ifdef BUC61044
  Tint     depth_test;
#endif
#ifdef BUC61045
  Tint     ws_gllight;
#endif
#ifdef OCC1188    
  WS_BG_TEXTURE bgTexture;
#endif /* OCC1188 */
  WS_BG_GRADIENT bgGradient;
  IMPLEMENT_MEMORY_OPERATORS
};
typedef WS_REC *ws_rec;

/*----------------------------------------------------------------------*/
/*
* Variables statiques
*/ 

typedef NCollection_DataMap< Standard_Integer, ws_rec>  WS_Map;
typedef WS_Map::Iterator                                WS_Map_Iterator;
Handle(NCollection_BaseAllocator)  ws_Allocator = NCollection_BaseAllocator::CommonBaseAllocator();
WS_Map  ws_map ( 1 , ws_Allocator );

/*----------------------------------------------------------------------*/

TStatus
TsmRegisterWsid( Tint wsid )
{
  if( ws_Allocator.IsNull() )
    return TFailure;

  ws_rec rec = new WS_REC();
  ws_map.Bind( wsid, rec );

  return TSuccess;
}

/*----------------------------------------------------------------------*/

TStatus
TsmSetWSAttri( Tint wsid, TWSAttri  attri, cmn_key_data data )
{    

  if( ws_Allocator.IsNull() )
    return TFailure;

  if ( !ws_map.IsBound( wsid ) ) {
    ws_rec rec = new WS_REC();
    ws_map.Bind( wsid, new WS_REC() );
  }
  ws_rec wsr = ws_map.ChangeFind( wsid );

  switch( attri )
  {
  case WSTextureEnv:
    wsr->texture_env = data->ldata;
    break;

  case WSSurfaceDetail:
    wsr->surface_detail = data->ldata;
    break;

  case  WSFilters:
    wsr->filters = data->pdata;
    break;

  case  WSWindow:
    wsr->window = data->ldata;
    break;

  case  WSWidth:
    wsr->width = data->ldata;
    break;

  case  WSHeight:
    wsr->height = data->ldata;
    break;

  case  WSDbuff:
    wsr->d_buff = data->ldata;
    break;

  case  WSViewStid:
    wsr->view_stid = data->ldata;
    break;

  case  WSLights:
    wsr->lights = data->pdata;
    break;

  case  WSViews:
    wsr->views = data->pdata;
    break;

  case  WSBackground:
    wsr->backg[0] = ((Tfloat*)data->pdata)[0],
      wsr->backg[1] = ((Tfloat*)data->pdata)[1],
      wsr->backg[2] = ((Tfloat*)data->pdata)[2];
    break;

  case  WSHighlights:
    wsr->highlights = data->pdata;
    break;

  case  WSDepthCues:
    wsr->depthcues = data->pdata;
    break;

  case  WSTransparency:
    wsr->transparency = data->ldata;
    break;

  case  WSZbuffer:
    wsr->zbuffer = data->ldata;
    break;

  case  WSTransient:
    wsr->transient = data->ldata;
    break;

  case  WSRetainMode:
    wsr->retainMode = data->ldata;
    break;

  case  WSUpdateState:
    wsr->update_state = data->ldata;
    break;

#ifndef WNT
  case  WSPixmap:
    wsr->pixmap_id = data->pixmap;
    break;

  case  WSGLXPixmap:
    wsr->glx_pixmap = data->glxpixmap;
    break;

  case  WSDepth:
    wsr->wsdepth = data->ldata;
    break;
#endif
#ifdef G003
  case WSBackfacing:
    wsr -> backfacing = data -> ldata;
    break;
#endif  /* G003 */

#ifdef BUC61044
  case WSDepthTest:
    wsr->depth_test = data->ldata;
    break;
#endif
#ifdef BUC61045
  case WSGLLight:
    wsr->ws_gllight = data->ldata;
    break;
#endif
#ifdef OCC1188
  case WSBgTexture : 
    wsr->bgTexture.texId  = ((Tuint*)data->pdata)[0],
    wsr->bgTexture.width  = ((Tint*)data->pdata)[1],
    wsr->bgTexture.height = ((Tint*)data->pdata)[2];
    wsr->bgTexture.style  = ((Tint*)data->pdata)[3];
    break;
#endif
  case WSBgGradient :
    wsr->bgGradient = *((WS_BG_GRADIENT*)(data->pdata));

  default:
    break;
  }

  return TSuccess;
}

/*----------------------------------------------------------------------*/

TStatus
TsmGetWSAttri( Tint wsid, TWSAttri attri, cmn_key_data data )
{
  if( ws_Allocator.IsNull() )
    return TFailure;

  if ( !ws_map.IsBound( wsid ) ) {
    ws_rec rec = new WS_REC();
    ws_map.Bind( wsid, rec );
  }
  ws_rec wsr = ws_map.Find( wsid );

  switch( attri )
  {
  case WSTextureEnv:
    data->ldata = wsr->texture_env;
    break;

  case WSSurfaceDetail:
    data->ldata = wsr->surface_detail;
    break;

  case  WSFilters:
    data->pdata = wsr->filters;
    break;

  case  WSWindow:
    data->ldata = wsr->window;
    break;

  case  WSWidth:
    data->ldata = wsr->width;
    break;

  case  WSHeight:
    data->ldata = wsr->height;
    break;

  case  WSDbuff:
    data->ldata = wsr->d_buff;
    break;

  case  WSViewStid:
    data->ldata = wsr->view_stid;
    break;

  case  WSLights:
    data->pdata = wsr->lights;
    break;

  case  WSViews:
    data->pdata = wsr->views;
    break;

  case  WSBackground:
    data->pdata = wsr->backg;
    break;

  case  WSHighlights:
    data->pdata = wsr->highlights;
    break;

  case  WSDepthCues:
    data->pdata = wsr->depthcues;
    break;

  case  WSTransparency:
    data->ldata = wsr->transparency;
    break;

  case  WSZbuffer:
    data->ldata = wsr->zbuffer;
    break;

  case  WSTransient:
    data->ldata = wsr->transient;
    break;

  case  WSRetainMode:
    data->ldata = wsr->retainMode;
    break;

  case  WSUpdateState:
    data->ldata = wsr->update_state;
    break;

#ifndef WNT
  case  WSPixmap:
    data->pixmap = wsr->pixmap_id;
    break;

  case  WSGLXPixmap:
    data->glxpixmap = wsr->glx_pixmap;
    break;

  case  WSDepth:
    data->ldata = wsr->wsdepth;
    break;
#endif
#ifdef G003
  case WSBackfacing:
    data -> ldata = wsr -> backfacing;
    break;
#endif  /* G003 */

#ifdef BUC61044
  case WSDepthTest:
    data->ldata = wsr->depth_test;
    break;
#endif
#ifdef BUC61045
  case WSGLLight:
    data->ldata = wsr->ws_gllight;
    break;
#endif
#ifdef OCC1188
  case WSBgTexture : 
    data->pdata = &wsr->bgTexture;
    break;
#endif 
  case WSBgGradient : 
    data->pdata = &wsr->bgGradient;
    break; 

  default:
    break;
  }

  return TSuccess;
}

/*----------------------------------------------------------------------*/

TStatus
TsmGetWSPosted( Tint struct_id, Tint size, Tint *ws, Tint *actual_size )
{
  if( ws_Allocator.IsNull() )
    return TFailure;
  Tint i=0;
  WS_Map_Iterator it(ws_map);
  for ( ; it.More(); it.Next()) {
    if( it.Value()->view_stid == struct_id ) {
      (*actual_size)++;
      if( size > i )
        ws[i++] = it.Key();
    }
  }

  return TSuccess;
}

/*----------------------------------------------------------------------*/

TStatus
TsmUnregisterWsid( Tint wsid )
{
  if( ws_Allocator.IsNull() )
    return TFailure;
  if ( !ws_map.IsBound( wsid ) ) {
    return TFailure;
  }
  ws_rec wsr = ws_map.ChangeFind( wsid );
  ws_map.UnBind( wsid );
  delete wsr;

  return TSuccess;
}

/*----------------------------------------------------------------------*/

TStatus
TsmInitUpdateState()
{
  if( ws_Allocator.IsNull() )
    return TFailure; 
  WS_Map_Iterator it(ws_map);
  for ( ; it.More(); it.Next()) {
    it.ChangeValue()->update_state = TNotDone;
    (void) call_togl_erase_animation_mode();
  }
  return TSuccess;
}

/*----------------------------------------------------------------------*/
