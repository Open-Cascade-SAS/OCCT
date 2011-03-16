/***********************************************************************

FONCTION :
----------
File OpenGl_togl_texture.c :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
05-08-97 : PCT ; Support texture mapping
20-11-97 : FMN ; Ajout call_togl_inquiretexture
Ajout coupure du texture mapping

************************************************************************/

#define OCC1188  /*SAV 23/12/02 - added methods to set background image */

/*----------------------------------------------------------------------*/
/*
* Includes
*/
#include <stdlib.h>
#include <string.h>
#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Labels.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_TextureBox.hxx>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_tsm_ws.hxx>

#ifdef OCC1188
#include <GL/glu.h>
#endif

/*----------------------------------------------------------------------*/

int EXPORT
call_togl_create_texture
(
 int Type,
 unsigned int Width,
 unsigned int Height, 
 unsigned char *Data,
 char *FileName
 )
{
  if (call_togl_inquiretexture ())
  {
    switch (Type)
    {
    case 0:
      return GetTextureData1D(FileName, Width, Height, Data);

    case 1:
      return GetTextureData2D(FileName, Width, Height, Data);

    case 2:
      return GetTextureData2DMipMap(FileName, Width, Height, Data);

    default:
      return -1;
    }
  }
  return -1 ;
}

/*----------------------------------------------------------------------*/

void EXPORT
call_togl_destroy_texture
(
 int TexId
 )
{
  if (call_togl_inquiretexture ())
    FreeTexture(TexId);
}

/*----------------------------------------------------------------------*/

void EXPORT
call_togl_modify_texture
(
 int TexId,
 CALL_DEF_INIT_TEXTURE *init_tex
 )
{
  if (call_togl_inquiretexture ())
  {
    if (init_tex->doModulate)
      SetTextureModulate(TexId);
    else
      SetTextureDecal(TexId);

    if (init_tex->doRepeat)
      SetTextureRepeat(TexId);
    else
      SetTextureClamp(TexId);

    switch (init_tex->Mode)
    {
    case 0:
      SetModeObject(TexId,
        &init_tex->sparams[0], &init_tex->tparams[0]);
      break;

    case 1:
      SetModeSphere(TexId);
      break;

    case 2:
      SetModeEye(TexId,
        &init_tex->sparams[0], &init_tex->tparams[0]);
      break;

    case 3:
      SetModeManual(TexId);
      break;        
    }

    if (init_tex->doLinear)
      SetRenderLinear(TexId);
    else
      SetRenderNearest(TexId);

    SetTexturePosition(TexId,
      init_tex->sx, init_tex->sy,
      init_tex->tx, init_tex->ty,
      init_tex->angle);
  }
}

/*----------------------------------------------------------------------*/

void EXPORT 
call_togl_environment(CALL_DEF_VIEW *aview)
{
  CMN_KEY_DATA data;

  if (call_togl_inquiretexture ())
  {
    data.ldata = aview->Context.TexEnvId;
    TsmSetWSAttri(aview->WsId, WSTextureEnv, &data);

    data.ldata = aview->Context.SurfaceDetail;
    TsmSetWSAttri(aview->WsId, WSSurfaceDetail, &data);
  }
}

/*----------------------------------------------------------------------*/

int EXPORT
call_togl_inquiretexture ()
{
#if defined(__sun)
  return 1;
#else
  return 1;
#endif /* SUN */
}

/*----------------------------------------------------------------------*/

#ifdef OCC1188
void EXPORT call_togl_create_bg_texture( CALL_DEF_VIEW* view, int width, int height, 
                                        unsigned char* data, int style )
{
  TSM_BG_TEXTURE tex;
  tsm_bg_texture createdTex;
  CMN_KEY_DATA  cmnData;
  CMN_KEY_DATA  cmnKey;
  GLuint texture = 0;
  GLubyte *image = (GLubyte *)malloc(width * height * 3 * sizeof(GLubyte));
  memcpy( image, data, ( width * height * 3 ) );

  /* check if some bg texture is already created */
  TsmGetWSAttri( view->WsId, WSBgTexture, &cmnData );
  createdTex = (tsm_bg_texture)cmnData.pdata;
  if ( createdTex->texId != 0 )
    glDeleteTextures( 1, (GLuint*)&(createdTex->texId) );
  glGenTextures( 1, &texture );
  glBindTexture( GL_TEXTURE_2D, texture );
  /* Create MipMapped Texture */
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  gluBuild2DMipmaps( GL_TEXTURE_2D, 3/*4*/, width, height, GL_RGB, GL_UNSIGNED_BYTE, image );

  tex.texId = texture;
  tex.width = width;
  tex.height = height;
  switch ( style ) {
    case 0 :
    case 1 :
      tex.style = TSM_FS_CENTER;
      break;
    case 2 :
      tex.style = TSM_FS_TILE;
      break;
    case 3 :
      tex.style = TSM_FS_STRETCH;
      break;
    default :
      tex.style = TSM_FS_CENTER;
      break;
  }

  /* setting flag to update changes */
  cmnKey.ldata = TNotDone;
  TsmSetWSAttri( view->WsId, WSUpdateState, &cmnKey );

  /* storing background texture */
  cmnData.pdata = &tex;
  TsmSetWSAttri( view->WsId, WSBgTexture, &cmnData );
  free( image );
}

/*----------------------------------------------------------------------*/

void EXPORT call_togl_set_bg_texture_style( CALL_DEF_VIEW* view, int style )
{
  tsm_bg_texture texture;
  CMN_KEY_DATA  cmnData;
  CMN_KEY_DATA  cmnKey;

  /* check if background texture is already created */
  TsmGetWSAttri( view->WsId, WSBgTexture, &cmnData );
  texture = (tsm_bg_texture)cmnData.pdata;
  if ( texture->texId != 0 ) {
    switch ( style ) {
        case 0 :
        case 1 :
          texture->style = TSM_FS_CENTER;
          break;
        case 2 :
          texture->style = TSM_FS_TILE;
          break;
        case 3 :
          texture->style = TSM_FS_STRETCH;
          break;
        default :
          texture->style = TSM_FS_CENTER;
          break;
    }

    /* setting flag to update changes */
    cmnKey.ldata = TNotDone;
    TsmSetWSAttri( view->WsId, WSUpdateState, &cmnKey );

    /* storing background texture */
    cmnData.pdata = texture;
    TsmSetWSAttri( view->WsId, WSBgTexture, &cmnData );
  }
}
#endif /* OCC1188 */
/*----------------------------------------------------------------------*/
