/*
* Fonction
* ~~~~~~~~
*   Gestion des images sous OpenGL
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
*   19-06-98: FMN ; Portage Optimizer (C++)
*   22-06-98: FGU ; Ajout ReadScaledImage et ReadSizeImage
*/
/*----------------------------------------------------------------------*/

#ifndef _OPENGL_IMAGEBOX_H__
#define _OPENGL_IMAGEBOX_H__


/*----------------------------------------------------------------------*/
/*
* Prototypes
*/

/*
* Points d'entree Public du module 
*/

unsigned *  read_texture(char *name, int *width, int *height, int *components);

void ReadScaledImage(char *file, int xsize, int ysize, char *buf, unsigned short *zsize);

void ReadSizeImage(char *, int *, int *);

void bwtorgba(unsigned char *b,unsigned char *l,int n);

void rgbtorgba(unsigned char *r,unsigned char *g,unsigned char *b,unsigned char *l,int n);

void rgbatorgba(unsigned char *r,unsigned char *g,unsigned char *b,unsigned char *a,unsigned char *l,int n);

/*----------------------------------------------------------------------*/

#endif /* _OPENGL_IMAGEBOX_H__ */
