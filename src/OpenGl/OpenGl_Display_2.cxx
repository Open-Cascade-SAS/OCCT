// Created on: 2011-10-25
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
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


#include <OpenGl_GlCore11.hxx>

#include <OpenGl_Display.hxx>

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#ifdef HAVE_GL2PS
  #include <gl2ps.h>
#endif

#define  DOT_LS            0xCCCC
#define  DASH_DOT_LS       0xFF18
#define  DASH_LS           0xFFC0
#define  DASH_DDOT_LS      0xFF24

static const unsigned int myInteriors[TEL_HS_USER_DEF_START][32] =
{
  //TEL_HS_SOLID
  {
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF
  },
  //TEL_HS_CROSS
  {
    0xFFFFFFFF,
    0xBBBBBBBB,
    0xEEEEEEEE,
    0xBBBBBBBB,
    0xEEEEEEEE,
    0xBBBBBBBB,
    0xEEEEEEEE,
    0xBBBBBBBB,
    0xEEEEEEEE,
    0xBBBBBBBB,
    0xEEEEEEEE,
    0xBBBBBBBB,
    0xEEEEEEEE,
    0xBBBBBBBB,
    0xEEEEEEEE,
    0xBBBBBBBB,
    0xEEEEEEEE,
    0xBBBBBBBB,
    0xEEEEEEEE,
    0xBBBBBBBB,
    0xEEEEEEEE,
    0xBBBBBBBB,
    0xEEEEEEEE,
    0xBBBBBBBB,
    0xEEEEEEEE,
    0xBBBBBBBB,
    0xEEEEEEEE,
    0xBBBBBBBB,
    0xEEEEEEEE,
    0xBBBBBBBB,
    0xEEEEEEEE,
    0xBBBBBBBB
  },
  //TEL_HS_CROSS_SPARSE
  {
    0x81818181,
    0x24242424,
    0x18181818,
    0x42424242,
    0x81818181,
    0x24242424,
    0x18181818,
    0x42424242,
    0x81818181,
    0x24242424,
    0x18181818,
    0x42424242,
    0x81818181,
    0x24242424,
    0x18181818,
    0x42424242,
    0x81818181,
    0x24242424,
    0x18181818,
    0x42424242,
    0x81818181,
    0x24242424,
    0x18181818,
    0x42424242,
    0x81818181,
    0x24242424,
    0x18181818,
    0x42424242,
    0x81818181,
    0x24242424,
    0x18181818,
    0x42424242
  },
  //TEL_HS_GRID
  {
    0xFFFFFFFF,
    0x88888888,
    0xFFFFFFFF,
    0x88888888,
    0xFFFFFFFF,
    0x88888888,
    0xFFFFFFFF,
    0x88888888,
    0xFFFFFFFF,
    0x88888888,
    0xFFFFFFFF,
    0x88888888,
    0xFFFFFFFF,
    0x88888888,
    0xFFFFFFFF,
    0x88888888,
    0xFFFFFFFF,
    0x88888888,
    0xFFFFFFFF,
    0x88888888,
    0xFFFFFFFF,
    0x88888888,
    0xFFFFFFFF,
    0x88888888,
    0xFFFFFFFF,
    0x88888888,
    0xFFFFFFFF,
    0x88888888,
    0xFFFFFFFF,
    0x88888888,
    0xFFFFFFFF,
    0x88888888
  },
  //TEL_HS_GRID_SPARSE
  {
    0xFFFFFFFF,
    0x80808080,
    0x80808080,
    0x80808080,
    0xFFFFFFFF,
    0x80808080,
    0x80808080,
    0x80808080,
    0xFFFFFFFF,
    0x80808080,
    0x80808080,
    0x80808080,
    0xFFFFFFFF,
    0x80808080,
    0x80808080,
    0x80808080,
    0xFFFFFFFF,
    0x80808080,
    0x80808080,
    0x80808080,
    0xFFFFFFFF,
    0x80808080,
    0x80808080,
    0x80808080,
    0xFFFFFFFF,
    0x80808080,
    0x80808080,
    0x80808080,
    0xFFFFFFFF,
    0x80808080,
    0x80808080,
    0x80808080
  },
  //TEL_HS_DIAG_45
  {
    0x88888888,
    0x22222222,
    0x88888888,
    0x22222222,
    0x88888888,
    0x22222222,
    0x88888888,
    0x22222222,
    0x88888888,
    0x22222222,
    0x88888888,
    0x22222222,
    0x88888888,
    0x22222222,
    0x88888888,
    0x22222222,
    0x88888888,
    0x22222222,
    0x88888888,
    0x22222222,
    0x88888888,
    0x22222222,
    0x88888888,
    0x22222222,
    0x88888888,
    0x22222222,
    0x88888888,
    0x22222222,
    0x88888888,
    0x22222222,
    0x88888888,
    0x22222222
  },
  //TEL_HS_DIAG_135
  {
    0x11111111,
    0x44444444,
    0x11111111,
    0x44444444,
    0x11111111,
    0x44444444,
    0x11111111,
    0x44444444,
    0x11111111,
    0x44444444,
    0x11111111,
    0x44444444,
    0x11111111,
    0x44444444,
    0x11111111,
    0x44444444,
    0x11111111,
    0x44444444,
    0x11111111,
    0x44444444,
    0x11111111,
    0x44444444,
    0x11111111,
    0x44444444,
    0x11111111,
    0x44444444,
    0x11111111,
    0x44444444,
    0x11111111,
    0x44444444,
    0x11111111,
    0x44444444
  },
  //TEL_HS_HORIZONTAL
  {
    0xFFFFFFFF,
    0x00000000,
    0xFFFFFFFF,
    0x00000000,
    0xFFFFFFFF,
    0x00000000,
    0xFFFFFFFF,
    0x00000000,
    0xFFFFFFFF,
    0x00000000,
    0xFFFFFFFF,
    0x00000000,
    0xFFFFFFFF,
    0x00000000,
    0xFFFFFFFF,
    0x00000000,
    0xFFFFFFFF,
    0x00000000,
    0xFFFFFFFF,
    0x00000000,
    0xFFFFFFFF,
    0x00000000,
    0xFFFFFFFF,
    0x00000000,
    0xFFFFFFFF,
    0x00000000,
    0xFFFFFFFF,
    0x00000000,
    0xFFFFFFFF,
    0x00000000,
    0xFFFFFFFF,
    0x00000000
  },
  //TEL_HS_VERTICAL
  {
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111
  },
  //TEL_HS_DIAG_45_SPARSE
  {
    0x80808080,
    0x20202020,
    0x08080808,
    0x02020202,
    0x80808080,
    0x20202020,
    0x08080808,
    0x02020202,
    0x80808080,
    0x20202020,
    0x08080808,
    0x02020202,
    0x80808080,
    0x20202020,
    0x08080808,
    0x02020202,
    0x80808080,
    0x20202020,
    0x08080808,
    0x02020202,
    0x80808080,
    0x20202020,
    0x08080808,
    0x02020202,
    0x80808080,
    0x20202020,
    0x08080808,
    0x02020202,
    0x80808080,
    0x20202020,
    0x08080808,
    0x02020202
  },
  //TEL_HS_DIAG_135_SPARSE
  {
    0x01010101,
    0x04040404,
    0x10101010,
    0x40404040,
    0x01010101,
    0x04040404,
    0x10101010,
    0x40404040,
    0x01010101,
    0x04040404,
    0x10101010,
    0x40404040,
    0x01010101,
    0x04040404,
    0x10101010,
    0x40404040,
    0x01010101,
    0x04040404,
    0x10101010,
    0x40404040,
    0x01010101,
    0x04040404,
    0x10101010,
    0x40404040,
    0x01010101,
    0x04040404,
    0x10101010,
    0x40404040,
    0x01010101,
    0x04040404,
    0x10101010,
    0x40404040
  },
  //TEL_HS_HORIZONTAL_SPARSE
  {
    0xFFFFFFFF,
    0x00000000,
    0x00000000,
    0x00000000,
    0xFFFFFFFF,
    0x00000000,
    0x00000000,
    0x00000000,
    0xFFFFFFFF,
    0x00000000,
    0x00000000,
    0x00000000,
    0xFFFFFFFF,
    0x00000000,
    0x00000000,
    0x00000000,
    0xFFFFFFFF,
    0x00000000,
    0x00000000,
    0x00000000,
    0xFFFFFFFF,
    0x00000000,
    0x00000000,
    0x00000000,
    0xFFFFFFFF,
    0x00000000,
    0x00000000,
    0x00000000,
    0xFFFFFFFF,
    0x00000000,
    0x00000000,
    0x00000000
  },
  //TEL_HS_VERTICAL_SPARSE
  {
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010,
    0x10101010
  }
};

/*----------------------------------------------------------------------*/

//TsmInitAttributes
void OpenGl_Display::InitAttributes ()
{
  // Return if already initialized
  if (myLinestyleBase) return;

  myLinestyleBase = glGenLists(5);

  /* Line */

  glNewList( (GLuint)myLinestyleBase+(GLuint)Aspect_TOL_DASH, GL_COMPILE );
  glLineStipple(1,  DASH_LS );
  glEndList();

  glNewList( (GLuint)myLinestyleBase+(GLuint)Aspect_TOL_DOT, GL_COMPILE );
  glLineStipple(1,  DOT_LS );
  glEndList();

  glNewList( (GLuint)myLinestyleBase+(GLuint)Aspect_TOL_DOTDASH, GL_COMPILE );
  glLineStipple(1,  DASH_DOT_LS );
  glEndList();

  glNewList( (GLuint)myLinestyleBase+(GLuint)Aspect_TOL_USERDEFINED, GL_COMPILE );
  glLineStipple(1,  DASH_DDOT_LS );
  glEndList();

  /* FSXXX
  * GL_POLYGON_STIPPLE
  * need 32x32 stipple patterns
  */
  const int nbi = sizeof(myInteriors)/(32*sizeof(unsigned int));
  myPatternBase = glGenLists(TEL_HS_USER_DEF_START);
  int i;
  for( i = 1; i < TEL_HS_USER_DEF_START; i++ )
  {
    glNewList( (GLuint)myPatternBase + i, GL_COMPILE );
	glPolygonStipple((const GLubyte *) (myInteriors[i<nbi? i : 0]));
    glEndList();
  }

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

/*----------------------------------------------------------------------*/

void OpenGl_Display::SetTypeOfLine (const Aspect_TypeOfLine AType) const
{
  if (AType != Aspect_TOL_SOLID)
  {
    glCallList((GLuint)myLinestyleBase+(GLuint)AType);
    glEnable( GL_LINE_STIPPLE );
#ifdef HAVE_GL2PS
    gl2psEnable( GL2PS_LINE_STIPPLE );
#endif
  }
  else
  {
    glDisable( GL_LINE_STIPPLE );
#ifdef HAVE_GL2PS
    gl2psDisable( GL2PS_LINE_STIPPLE );
#endif
  }
}

/*----------------------------------------------------------------------*/

void OpenGl_Display::SetTypeOfHatch (const int AType) const
{
  if (AType)
  {
    glCallList((GLuint)myPatternBase+(GLuint)AType);
    glEnable( GL_POLYGON_STIPPLE );
  }
  else
    glDisable( GL_POLYGON_STIPPLE );
}

/*----------------------------------------------------------------------*/
