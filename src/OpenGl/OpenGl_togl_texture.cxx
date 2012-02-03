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
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_TextureBox.hxx>

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

int EXPORT
call_togl_inquiretexture ()
{
#if defined(__sun)
  return 1;
#else
  return 1;
#endif /* SUN */
}
