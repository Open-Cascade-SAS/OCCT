// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

/*
* Fonction
* ~~~~~~~~
*   Gestion des textures sous OpenGL
*
*
* Notes
* ~~~~~
*   Les textures sont toujours initialisee avec des parametres par defaut
*     texture 1D:        WRAP_S = CLAMP
*                        MAG_FILTER = NEAREST
*                        generation de texture automatique en OBJECT_LINEAR
*                        rendu avec DECAL
*
*     texture 2D:        WRAP_S/T = REPEAT
*                        MAG/MIN_FILTER = LINEAR
*                        generation de texture automatique en OBJECT_LINEAR
*                        rendu avec MODULATE
*
*     texture 2D MipMap: WRAP_S/T = REPEAT
*                        MAG_FILTER = LINEAR
*                        MIN_FILTER = LINEAR_MIPMAP_NEAREST
*                        generation de texture automatique en OBJECT_LINEAR
*                        rendu avec MODULATE
*
* Historique des modifications
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*   22-05-97: PCT ; Creation
*   18-06-97: FMN ; Ajout entete
*   20-06-97: PCT ; Correction bug parametres par defaut texture 1D
*   30-06-97: PCT ; Correction bug rechargement de la texture courante
*   04-07-97: PCT ; suppression de l'utilisation de libimage.a de SGI
*   01-08-97: PCT ; suppression InitializeTextureBox()
*   04-08-97: FMN,PCT ; Portage WNT
*   05-08-97: FMN ; ajout GetTextureData...
*   10-09-97: PCT ; ajout commentaires. GetTexture() ne doit pas
*                   etre utilisee dans Cas.Cade ( le chargement est
*                   fait par Graphic3d )
*   06-10-97: FMN ; Portage HP
*   14-10-97: FMN ; Ajout OpenGl_Extension
*   22-10-97: FMN ; Meilleure gestion de l'extension glXGetCurrentDisplayEXT
*   04-11-97: FMN ; Gestion des differentes versions GLX
*   19-11-97: FMN ; Ajout GetCurrentDisplay plus simple que glXGetCurrentDisplayEXT
*   04-12-97: FMN ; On suppose que l'on travaille en OpenGL1.1 (cf OpenGl_Extension)
*   17-12-97: FMN ; Probleme compilation SGI
*   17-12-97: FMN ; Probleme sur Optimisation sur MyBindTextureEXT()
*        Le test sur la texture courante doit tenir compte du contexte.
*   22-07-98: FGU ; Ajout fonctions TransferTexture_To_Data() et TransferData_To_Texture()
*        
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <OpenGl_GlCore11.hxx>
#include <OpenGl_Display.hxx>
#include <OpenGl_TextureBox.hxx>
#include <OpenGl_ImageBox.hxx>
#include <OpenGl_ResourceTexture.hxx>
#include <OpenGl_Context.hxx>

#include <GL/glu.h> // gluBuild2DMipmaps()

#include <NCollection_Vector.hxx>

typedef enum {TEXDATA_NONE, TEXDATA_1D, TEXDATA_2D, TEXDATA_2DMM} texDataStatus;
typedef enum {TEX_NONE, TEX_ALLOCATED} texStatus;

typedef GLfloat SizeType[4]; 

typedef int TextureDataID;
#define TEXTUREDATA_ERROR -1

struct texData
{
  char imageFileName[128];
  int imageWidth, imageHeight;
  GLubyte *image;
  texDataStatus status;
  GLint type;
  int share_count;
  DEFINE_STANDARD_ALLOC
};  

struct contextData
{
  GLuint number;
  GLDRAWABLE drawable;
  GLCONTEXT context;
};

struct texDraw
{
  TextureDataID data;
  NCollection_Vector<contextData> contextdata;
  texStatus status;

  GLint Gen;
  GLint Light;
  GLint Wrap;
  GLfloat Plane1[4];
  GLfloat Plane2[4];
  GLint Render;
  GLfloat scalex, scaley;
  GLfloat transx, transy;
  GLfloat angle;

  DEFINE_STANDARD_ALLOC
};


/*----------------------------------------------------------------------*/
/*
* Variables statiques
*/

static NCollection_Vector<texDraw> textab;

static NCollection_Vector<texData> texdata;

static TextureDataID current_texture_data = TEXTUREDATA_ERROR;
static TextureID current_texture = TEXTUREBOX_ERROR;

static GLfloat sgenparams[] = { 1.0 ,0.0 ,0.0 ,0.0};
static GLfloat tgenparams[] = { 0.0 ,1.0 ,0.0 ,0.0};

static GLenum status2type[] = { GL_NONE, GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_2D };

/*----------------------------------------------------------------------*/
/*
* Fonctions privees
*/

/*----------------------------------------------------------------------*/
/*
* recherche l'existence de datas de texture par son nom
*/
static TextureDataID FindTextureData(char *FileName)
{
  for (int i = 0; i < texdata.Length(); i++)
  {
    if ( texdata(i).status != TEXDATA_NONE && strcmp(FileName, texdata(i).imageFileName) == 0 )
    {
      return i;
    }
  }

  return TEXTUREDATA_ERROR;
}

/*----------------------------------------------------------------------*/
/*
* recherche un emplacement de data texture libre
*/
static TextureDataID FindFreeTextureData(void)
{
  for (int i = 0; i < texdata.Length(); i++)
  {
    if (texdata(i).status == TEXDATA_NONE)
    {
      return i;
    }
  }

  texData aTexData;
  texdata.Append(aTexData);
  return texdata.Length() - 1;
}

/*----------------------------------------------------------------------*/
/*
* recherche un emplacement de texture libre
*/
static TextureID FindFreeTexture(void)
{
  for (int i = 0; i < textab.Length(); i++)
  {
    if (textab(i).status == TEX_NONE)
    {
      return i;
    }
  }

  texDraw aTexDraw;
  textab.Append(aTexDraw);
  return textab.Length() - 1;
}

/*----------------------------------------------------------------------*/
/*
* regarde si la texture a ete definie pour le contexte courant
*/
static int FindTextureContext(TextureID ID)
{
  int i;

  GLCONTEXT cur = GET_GL_CONTEXT();
  for (i=0; i<textab(ID).contextdata.Length(); i++)
    if (textab(ID).contextdata(i).context == cur)
      return i;

  return TEXTUREBOX_ERROR;
}

/*----------------------------------------------------------------------*/
/*
* chargement d'une texture suivant son type
*/
static void LoadTexture(TextureID ID)
{
  TextureDataID data;

  data = textab(ID).data;
  switch (texdata(data).status)
  {
  case TEXDATA_1D:
    glTexImage1D(GL_TEXTURE_1D, 0, 4, 
      texdata(data).imageWidth, 0,
      GL_RGBA, GL_UNSIGNED_BYTE, texdata(data).image);
    break;

  case TEXDATA_2D:
    glTexImage2D(GL_TEXTURE_2D, 0, 4,
      texdata(data).imageWidth, texdata(data).imageHeight, 0,
      GL_RGBA, GL_UNSIGNED_BYTE, texdata(data).image);
    break;

  case TEXDATA_2DMM:
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4,
      texdata(data).imageWidth, 
      texdata(data).imageHeight,
      GL_RGBA, GL_UNSIGNED_BYTE, texdata(data).image);
    break;
  default:
    break;
  }
}

/*----------------------------------------------------------------------*/
/*
* les parametres d'initialisation d'une texture
* NE PAS METTRE DANS UNE DISPLAY LIST POUR L'INSTANT ( pb avec les matrices )
*/
static void SetTextureParam(TextureID ID)
{
  GLint cur_matrix;
  TextureDataID data;


  data = textab(ID).data;
  glGetIntegerv(GL_MATRIX_MODE, &cur_matrix);

  /*
  * MISE EN PLACE DE LA MATRICE DE TEXTURE
  */
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  /* if (textab(ID).Gen != GL_SPHERE_MAP)
  {*/
  glScalef(textab(ID).scalex, textab(ID).scaley, 1.0);
  glTranslatef(-textab(ID).transx, -textab(ID).transy, 0.0);
  glRotatef(-textab(ID).angle, 0.0, 0.0, 1.0);
  /*}*/


  /*
  * GENERATION AUTOMATIQUE DE TEXTURE
  */
  switch (textab(ID).Gen)
  {
  case GL_OBJECT_LINEAR:
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, textab(ID).Plane1);
    if (texdata(data).status != TEXDATA_1D)
    {
      glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
      glTexGenfv(GL_T, GL_OBJECT_PLANE, textab(ID).Plane2);
    }
    break;

  case GL_SPHERE_MAP:
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    if (texdata(data).status != TEXDATA_1D)
      glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    break;

  case GL_EYE_LINEAR:
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGenfv(GL_S, GL_EYE_PLANE, textab(ID).Plane1);

    if (texdata(data).status != TEXDATA_1D)
    {
      glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
      glTexGenfv(GL_T, GL_EYE_PLANE, textab(ID).Plane2);
    }

    glPopMatrix();
    break;
  }


  /*
  * RENDU DE LA TEXTURE AVEC LES LUMIERES
  */
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, textab(ID).Light);


  /*
  * LISSAGE DE LA TEXTURE
  */
  switch (texdata(data).status)
  {
  case TEXDATA_1D:
  case TEXDATA_2D:
    glTexParameteri(texdata(data).type, GL_TEXTURE_MAG_FILTER, textab(ID).Render);
    glTexParameteri(texdata(data).type, GL_TEXTURE_MIN_FILTER, textab(ID).Render);
    break;

  case TEXDATA_2DMM:
    if (textab(ID).Render == GL_NEAREST)
    {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    }
    else
    {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    }
    break;  
  default:
    break;
  }


  /*
  * WRAP DE LA TEXTURE
  */
  switch (texdata(data).status)
  {
  case TEXDATA_1D:
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, textab(ID).Wrap);
    break;

  case TEXDATA_2D:
  case TEXDATA_2DMM:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textab(ID).Wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textab(ID).Wrap);
    break;
  default:
    break;
  }

  glMatrixMode(cur_matrix);
}

/*----------------------------------------------------------------------*/
/*
* simulation du glGenTexturesEXT pour un context
*/
static void MyGenTextureEXT (TextureID ID)
{
  TextureDataID data = textab(ID).data;
  contextData aContextData;

  aContextData.context = GET_GL_CONTEXT();
  aContextData.drawable = GET_GLDEV_CONTEXT();
  glGenTextures (1, &aContextData.number);
  textab(ID).contextdata.Append(aContextData);
  glBindTexture (texdata(data).type, aContextData.number);
  LoadTexture (ID);
}

/*----------------------------------------------------------------------*/
/*
* simulation du glBindTextureEXT
*/
static void MyBindTextureEXT (TextureID ID, int Context)
{
  TextureDataID data = textab(ID).data;
  if (texdata(data).status == TEXDATA_NONE)
    return;

  GLenum aParamName = texdata(data).status == TEXDATA_1D ? 
  GL_TEXTURE_BINDING_1D : GL_TEXTURE_BINDING_2D;

  GLint aCurrTex = -1;
  glGetIntegerv (aParamName, &aCurrTex);
  if (textab(ID).contextdata(Context).number != aCurrTex)
  {
    glBindTexture (texdata(data).type, textab(ID).contextdata(Context).number);
  }
}

/*----------------------------------------------------------------------*/
/*
* installation de la texture pour le dernier contexte
*/
static int InstallTextureInContext(TextureID ID)
{
#ifdef PRINT
  printf("InstallTextureInContext::installation de la texture dans le context\n");
#endif
  MyGenTextureEXT(ID);
  SetTextureParam(ID);
#ifdef PRINT
  printf("InstallTextureInContext::context ok\n");
#endif
  return 0;
}  

/*----------------------------------------------------------------------*/
static TextureID GetTexture(char *FileName, texDataStatus status)
{
  TextureDataID i;
  TextureID j;
  int dummy;

  /* essait de trouver la texture */
  i = FindTextureData(FileName);
  if (i == TEXTUREDATA_ERROR)
  {
#ifdef PRINT
    printf("GetTexture::la texture %s n'existe pas => chargement\n", FileName);
#endif
    /* creation d'une texture */
    i = FindFreeTextureData();
    if (i == TEXTUREDATA_ERROR) return TEXTUREBOX_ERROR;

    texdata(i).share_count = 0;
    strcpy(texdata(i).imageFileName, FileName);
    texdata(i).image = (GLubyte *)read_texture(FileName, 
      &texdata(i).imageWidth,
      &texdata(i).imageHeight,
      &dummy);
    if (texdata(i).image == NULL) return TEXTUREBOX_ERROR;

    texdata(i).status = status;            
    texdata(i).type = status2type[status];
  }

  j = FindFreeTexture();
  if (j != TEXTUREBOX_ERROR)
  {
#ifdef PRINT
    printf("GetTexture::installation texture pour obj %d\n", j);
#endif
    textab(j).contextdata.Clear();
    textab(j).data = i;
    textab(j).status = TEX_ALLOCATED;
    texdata(i).share_count++;

    SetTextureDefaultParams(j);

#ifdef PRINT
    printf("GetTexture::texture %s(%d)    texture %d   count=%d\n", texdata(i).imageFileName, i, j, texdata(i).share_count);
#endif
  }
  else
    if (texdata(i).share_count != 0)
      delete [] texdata(i).image;  

  return j;
}


/*----------------------------------------------------------------------*/
static TextureID GetTextureData(char *FileName, texDataStatus status, const GLint width, const GLint height, const void *data)
{
  TextureDataID i;
  TextureID j;

  /* essait de trouver la texture */
  i = FindTextureData(FileName);
  if (i == TEXTUREDATA_ERROR)
  {
#ifdef PRINT
    printf("GetTextureData::la texture %s n'existe pas => chargement\n", FileName);
#endif
    /* creation d'une texture */
    i = FindFreeTextureData();
    if (i == TEXTUREDATA_ERROR) return TEXTUREBOX_ERROR;

    texdata(i).share_count = 0;
    strcpy(texdata(i).imageFileName, FileName);
    texdata(i).image = new GLubyte[width*height*4];
    memcpy(texdata(i).image, data, (width*height*4));
    texdata(i).imageWidth = width;
    texdata(i).imageHeight = height;

    if (texdata(i).image == NULL) return TEXTUREBOX_ERROR;

    texdata(i).status = status;            
    texdata(i).type = status2type[status];
  }

  j = FindFreeTexture();
  if (j != TEXTUREBOX_ERROR)
  {
#ifdef PRINT
    printf("GetTextureData::installation texture pour obj %d\n", j);
#endif
    textab(j).contextdata.Clear();
    textab(j).data = i;
    textab(j).status = TEX_ALLOCATED;
    texdata(i).share_count++;

    SetTextureDefaultParams(j);

#ifdef PRINT
    printf("GetTextureData::texture %s(%d)    texture %d   count=%d\n", texdata(i).imageFileName, i, j, texdata(i).share_count);
#endif
  }
  else
    if (texdata(i).share_count != 0)
      delete [] texdata(i).image;  

  return j;
}

/*----------------------------------------------------------------------*/
/*
* Fonctions publiques 
*/


/*----------------------------------------------------------------------*/

GLboolean IsTextureValid(TextureID ID)
{
  if ( (ID < 0) || (ID >= textab.Length()) )
    return GL_FALSE;

  if (textab.Length() > 0) 
  {
    return textab(ID).status == TEX_ALLOCATED;
  }
  else
  {
    return GL_FALSE;
  }
}

/*----------------------------------------------------------------------*/

TextureID GetTexture1D(char *FileName)
{
#ifdef PRINT
  printf("GetTexture1D::loading 1d %s   \n", FileName);
#endif
  return GetTexture(FileName, TEXDATA_1D);
}

/*----------------------------------------------------------------------*/

TextureID GetTexture2D(char *FileName)
{
#ifdef PRINT
  printf("GetTexture2D::loading 2d %s   \n", FileName);
#endif
  return GetTexture(FileName, TEXDATA_2D);
}

/*----------------------------------------------------------------------*/

TextureID GetTexture2DMipMap(char *FileName)
{
#ifdef PRINT
  printf("GetTexture2DMipMap::loading 2dmm %s   \n", FileName);
#endif
  return GetTexture(FileName, TEXDATA_2DMM);
}

/*----------------------------------------------------------------------*/

TextureID GetTextureData1D(char *FileName, const GLint width, const GLint height, const void *data)
{
#ifdef PRINT
  printf("GetTextureData1D::loading 1d %s  \n", FileName);
#endif
  return GetTextureData(FileName, TEXDATA_1D, width, height, data);
}

/*----------------------------------------------------------------------*/

TextureID GetTextureData2D(char *FileName, const GLint width, const GLint height, const void *data)
{
#ifdef PRINT
  printf("GetTextureData2D::loading 2d %s   \n", FileName);
#endif
  return GetTextureData(FileName, TEXDATA_2D, width, height, data);
}

/*----------------------------------------------------------------------*/

TextureID GetTextureData2DMipMap(char *FileName, const GLint width, const GLint height, const void *data)
{
#ifdef PRINT
  printf("GetTextureData2DMipMap::loading 2dmm %s   \n", FileName);
#endif
  return GetTextureData(FileName, TEXDATA_2DMM, width, height, data);
}


/*----------------------------------------------------------------------*/

void SetCurrentTexture(TextureID ID)
{
  int context;

  if (!IsTextureValid(ID)) return;

  context = FindTextureContext(ID);

  /* la texture n'existe pas dans ce contexte */
  if (context == TEXTUREBOX_ERROR)
  {
#ifdef PRINT
    printf("SetCurrentTexture::installation texture %d dans context\n", ID);
#endif
    /* si on a une erreur pendant l'installation dans le context
    * alors on installe la texture sans bind */
    if (InstallTextureInContext(ID) == TEXTUREBOX_ERROR)
    {
      LoadTexture(ID);
      SetTextureParam(ID);
    }
  }

  /*oui, alors on bind directement */
  else
  {
#ifdef PRINT
    printf("SetCurrentTexture: utilisation du bind %d\n", ID);
#endif
    MyBindTextureEXT(ID, context); 
    SetTextureParam(ID);
  }

  current_texture = ID;
  current_texture_data = textab(ID).data;
}

/*----------------------------------------------------------------------*/

void FreeTexture (const Handle(OpenGl_Context)& theContext,
                  TextureID                     ID)
{
  if (!IsTextureValid (ID))
  {
    return;
  }

  TextureDataID data = textab(ID).data;
  texdata(data).share_count--;
  if (texdata(data).share_count == 0)
  {
    // release texture data
    delete [] texdata(data).image;

    // liberation de la texture dans tous les contextes
    for (int i = 0; i < textab(ID).contextdata.Length(); ++i)
    {
      Handle(OpenGl_ResourceTexture) aResource = new OpenGl_ResourceTexture (textab(ID).contextdata(i).number);
      
      if (!theContext.IsNull()) {
        theContext->DelayedRelease (aResource);
      }
    }

    texdata(data).status = TEXDATA_NONE;

    textab(ID).contextdata.Clear();
  }

  textab(ID).status = TEX_NONE;

  current_texture_data = TEXTUREDATA_ERROR;
}

/*----------------------------------------------------------------------*/

void EnableTexture(void)
{
  if (!IsTextureValid(current_texture)) return;

  switch (texdata(current_texture_data).status)
  {
  case TEXDATA_1D:
    if (textab(current_texture).Gen != GL_NONE) 
      glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_1D);
    break;

  case TEXDATA_2D:
  case TEXDATA_2DMM:
    if (textab(current_texture).Gen != GL_NONE)
    {
      glEnable(GL_TEXTURE_GEN_S);
      glEnable(GL_TEXTURE_GEN_T);
    }
    glEnable(GL_TEXTURE_2D);
    break;
  default:
    break;
  }
}

/*----------------------------------------------------------------------*/

void DisableTexture(void)
{
  if ( !IsTextureEnabled() ) 
    return;
  if ( !IsTextureValid( current_texture ) ) 
    return;

  switch (texdata(current_texture_data).status)
  {
  case TEXDATA_1D:
    if (textab(current_texture).Gen != GL_NONE) 
      glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_1D);
    break;

  case TEXDATA_2D:
  case TEXDATA_2DMM:
    if (textab(current_texture).Gen != GL_NONE)
    {
      glDisable(GL_TEXTURE_GEN_S);
      glDisable(GL_TEXTURE_GEN_T);
    }
    glDisable(GL_TEXTURE_2D);
    break;
  default:
    break;
  }
}

/*----------------------------------------------------------------------*/

GLboolean IsTextureEnabled(void)
{
  GLboolean isEnabled1D= GL_FALSE, isEnabled2D= GL_FALSE;
  glGetBooleanv( GL_TEXTURE_1D, &isEnabled1D );
  glGetBooleanv( GL_TEXTURE_2D, &isEnabled2D );
  return isEnabled1D || isEnabled2D;
}

/*----------------------------------------------------------------------*/

void SetTextureModulate(TextureID ID)
{
  if (!IsTextureValid(ID)) return;

  textab(ID).Light = GL_MODULATE;
}

/*----------------------------------------------------------------------*/

void SetTextureDecal(TextureID ID)
{
  if (!IsTextureValid(ID)) return;

  textab(ID).Light = GL_DECAL;
}

/*----------------------------------------------------------------------*/

void SetTextureClamp(TextureID ID)
{
  if (!IsTextureValid(ID)) return;

  textab(ID).Wrap = GL_CLAMP;
}

/*----------------------------------------------------------------------*/

void SetTextureRepeat(TextureID ID)
{
  if (!IsTextureValid(ID)) return;

  textab(ID).Wrap = GL_REPEAT;
}

/*----------------------------------------------------------------------*/

/* gestion de la facon d'appliquer la texture */
void SetModeObject(TextureID ID, const GLfloat sparams[4], const GLfloat tparams[4])
{
  if (!IsTextureValid(ID)) return;

  textab(ID).Gen = GL_OBJECT_LINEAR;
  if (sparams != NULL) memcpy(textab(ID).Plane1, sparams, sizeof(sgenparams));
  else                 memcpy(textab(ID).Plane1, sgenparams, sizeof(sgenparams));

  if (texdata(textab(ID).data).status != TEXDATA_1D) {
    if (tparams != NULL) memcpy(textab(ID).Plane2, tparams, sizeof(tgenparams));
    else                 memcpy(textab(ID).Plane2, tgenparams, sizeof(tgenparams));
  }
}

/*----------------------------------------------------------------------*/

void SetModeSphere(TextureID ID)
{
  if (!IsTextureValid(ID)) return;

  textab(ID).Gen = GL_SPHERE_MAP;
}

/*----------------------------------------------------------------------*/

void SetModeEye(TextureID ID, const GLfloat sparams[4], const GLfloat tparams[4])
{  
  if (!IsTextureValid(ID)) return;

  textab(ID).Gen = GL_EYE_LINEAR;
  if (sparams != NULL) memcpy(textab(ID).Plane1, sparams, sizeof(sgenparams));
  else                 memcpy(textab(ID).Plane1, sgenparams, sizeof(sgenparams));

  if (texdata(textab(ID).data).status != TEXDATA_1D) {
    if (tparams != NULL) memcpy(textab(ID).Plane2, tparams, sizeof(tgenparams));
    else                 memcpy(textab(ID).Plane2, tgenparams, sizeof(tgenparams));
  }
}

/*----------------------------------------------------------------------*/

void SetModeManual(TextureID ID)
{
  if (!IsTextureValid(ID)) return;

  textab(ID).Gen = GL_NONE;
}

/*----------------------------------------------------------------------*/

void SetRenderNearest(TextureID ID)
{
  if (!IsTextureValid(ID)) return;

  textab(ID).Render = GL_NEAREST;
}

/*----------------------------------------------------------------------*/

void SetRenderLinear(TextureID ID)
{
  if (!IsTextureValid(ID)) return;

  textab(ID).Render = GL_LINEAR;
}

/*----------------------------------------------------------------------*/

void SetTexturePosition(TextureID ID,
                        GLfloat scalex, GLfloat scaley,
                        GLfloat transx, GLfloat transy,
                        GLfloat angle)
{
  textab(ID).scalex = scalex;
  textab(ID).scaley = scaley;
  textab(ID).transx = transx;
  textab(ID).transy = transy;
  textab(ID).angle = angle;
}

/*----------------------------------------------------------------------*/

void SetTextureDefaultParams(TextureID ID)
{
  if (!IsTextureValid(ID)) return;

#ifdef PRINT
  printf("SetTextureDefaultParams::set parm par defaut textures\n");
#endif


  textab(ID).scalex = 1.0;
  textab(ID).scaley = 1.0;
  textab(ID).transx = 0.0;
  textab(ID).transy = 0.0;
  textab(ID).angle = 0.0;

  textab(ID).Gen = GL_OBJECT_LINEAR;
  textab(ID).Light = texdata(textab(ID).data).status == TEXDATA_1D ? GL_DECAL : GL_MODULATE;
  textab(ID).Wrap = texdata(textab(ID).data).status == TEXDATA_1D ? GL_CLAMP : GL_REPEAT;
  memcpy(textab(ID).Plane1, sgenparams, sizeof(sgenparams));
  memcpy(textab(ID).Plane2, tgenparams, sizeof(tgenparams));
  textab(ID).Render = texdata(textab(ID).data).status == TEXDATA_1D ? GL_NEAREST : GL_LINEAR;
}

/*----------------------------------------------------------------------*/
/* Transfere de donnee des donnees internes a la structure TransferData */
void TransferTexture_To_Data(TextureID ID, TextureData *TransfDt)
{
  /* affectations */    
  strcpy(TransfDt->path, texdata(textab(ID).data).imageFileName);    
  TransfDt->gen = textab(ID).Gen;
  TransfDt->wrap  = textab(ID).Wrap;
  TransfDt->render  = textab(ID).Light;
  TransfDt->scalex  = textab(ID).scalex;
  TransfDt->scaley  = textab(ID).scaley;
  TransfDt->transx  = textab(ID).transx;
  TransfDt->transy  = textab(ID).transy;
  TransfDt->angle = textab(ID).angle;            
  memcpy(TransfDt->plane1,  textab(ID).Plane1, sizeof(SizeType));
  memcpy(TransfDt->plane2,  textab(ID).Plane2, sizeof(SizeType));                
}
