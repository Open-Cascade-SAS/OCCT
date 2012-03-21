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
