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

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <OpenGl_tgl_all.hxx>

#include <GL/gl.h>


/*----------------------------------------------------------------------*/
/*
* Constantes
*/

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
  IMPLEMENT_MEMORY_OPERATORS
};
typedef _TextureData TextureData;

/*----------------------------------------------------------------------*/
/*
* Prototypes
*/

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

void FreeTexture(TextureID ID);

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

void SetModeObject(TextureID ID, GLfloat sparams[4], GLfloat tparams[4]);
void SetModeSphere(TextureID ID);
void SetModeEye(TextureID ID, GLfloat sparams[4], GLfloat tparams[4]);
void SetModeManual(TextureID ID);

void SetRenderNearest(TextureID ID);
void SetRenderLinear(TextureID ID);


void SetTexturePosition(TextureID ID,
                        GLfloat scalex, GLfloat scaley,
                        GLfloat transx, GLfloat transy,
                        GLfloat angle);


void SetTextureDefaultParams(TextureID ID);

void TransferTexture_To_Data(TextureID, TextureData *);
void TransferData_To_Texture(TextureData*, TextureID*);

/*----------------------------------------------------------------------*/

#endif /* _OPENGL_TEXTUREBOX_H_ */
