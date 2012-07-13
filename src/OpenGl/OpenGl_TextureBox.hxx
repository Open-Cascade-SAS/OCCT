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
*
* Attention:
* ~~~~~~~~~~~
*  Ce package a ete teste sur SGI, OSF, SUN, HP et WNT.
*
*
* Historique des modifications
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*   22-05-97: PCT ; creation
*   01-08-97: PCT ; suppression InitializeTextureBox()
*   05-08-97: FMN ; ajout GetTextureData...
*   19-06-98: FMN ; Portage Optimizer (C++)
*   22-07-98: FGU ; ajout stucture TextureData
*/
/*----------------------------------------------------------------------*/

#ifndef _OPENGL_TEXTUREBOX_H_
#define _OPENGL_TEXTUREBOX_H_

#include <OpenGl_GlCore11.hxx>
#include <Standard_DefineAlloc.hxx>

typedef int TextureID;
#define TEXTUREBOX_ERROR ((TextureID)-1)

/*
*  Structure
*/
struct _TextureData
{
  /* Donnees propre au fichier */
  char path[256];    

  /* Donnees propre a la texture */
  GLint gen;
  GLint wrap;
  GLfloat plane1[4],  plane2[4];
  GLint render;
  GLfloat scalex,  scaley;
  GLfloat transx, transy;
  GLfloat angle;
  DEFINE_STANDARD_ALLOC
};
typedef _TextureData TextureData;

/* 
* Gestion des textures
*/

/* Get texture a partir d'un nom de fichier */
TextureID GetTexture1D(char *FileName);
TextureID GetTexture2D(char *FileName);
TextureID GetTexture2DMipMap(char *FileName);

/* Get texture a partir des donnees (format RGBA) GLubyte data[width][height][4] 
* Le nom est utiliser pour la gesiton en interne, il permet d'eviter de charger
* plusieurs textures avec le meme noms.
*/
TextureID GetTextureData1D(char *FileName, const GLint width, const GLint height, const void *data);
TextureID GetTextureData2D(char *FileName, const GLint width, const GLint height, const void *data);
TextureID GetTextureData2DMipMap(char *FileName, const GLint width, const GLint height, const void *data);

class Handle(OpenGl_Context);
void FreeTexture (const Handle(OpenGl_Context)& theContext,
                  TextureID                     ID);

void SetCurrentTexture(TextureID ID);
GLboolean IsTextureValid(TextureID ID);

void EnableTexture(void);
void DisableTexture(void);
GLboolean IsTextureEnabled(void);

/* 
* Configuration d'une texture 
*/

void SetTextureModulate(TextureID ID);
void SetTextureDecal(TextureID ID);

void SetTextureClamp(TextureID ID);
void SetTextureRepeat(TextureID ID);

void SetModeObject(TextureID ID, const GLfloat sparams[4], const GLfloat tparams[4]);
void SetModeSphere(TextureID ID);
void SetModeEye(TextureID ID, const GLfloat sparams[4], const GLfloat tparams[4]);
void SetModeManual(TextureID ID);

void SetRenderNearest(TextureID ID);
void SetRenderLinear(TextureID ID);


void SetTexturePosition(TextureID ID,
                        GLfloat scalex, GLfloat scaley,
                        GLfloat transx, GLfloat transy,
                        GLfloat angle);


void SetTextureDefaultParams(TextureID ID);

void TransferTexture_To_Data(TextureID, TextureData *);

/*----------------------------------------------------------------------*/

#endif /* _OPENGL_TEXTUREBOX_H_ */
