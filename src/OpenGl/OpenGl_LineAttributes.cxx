// Created on: 2011-10-25
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <OpenGl_GlCore11.hxx>

#include <OpenGl_LineAttributes.hxx>
#include <OpenGl_Context.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_LineAttributes,OpenGl_Resource)

static const unsigned int myInteriors[TEL_HS_USER_DEF_START][32] =
{
  // Aspect_HS_SOLID
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
  // Aspect_HS_GRID_DIAGONAL
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
  // Aspect_HS_GRID_DIAGONAL_WIDE
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
  // Aspect_HS_GRID
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
  // Aspect_HS_GRID_WIDE
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
  // Aspect_HS_DIAGONAL_45
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
  // Aspect_HS_DIAGONAL_135
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
  // Aspect_HS_HORIZONTAL
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
  // Aspect_HS_VERTICAL
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
  // Aspect_HS_DIAGONAL_45_WIDE
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
  // Aspect_HS_DIAGONAL_135_WIDE
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
  // Aspect_HS_HORIZONTAL_WIDE
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
  // Aspect_HS_VERTICAL_WIDE
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

// =======================================================================
// function : OpenGl_LineAttributes
// purpose  :
// =======================================================================
OpenGl_LineAttributes::OpenGl_LineAttributes()
: myPatternBase (0),
  myTypeOfHatch (0),
  myIsEnabled (true)
{
  //
}

// =======================================================================
// function : ~OpenGl_LineAttributes
// purpose  :
// =======================================================================
OpenGl_LineAttributes::~OpenGl_LineAttributes()
{
  Release (NULL);
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_LineAttributes::Release (OpenGl_Context* theGlCtx)
{
  // Delete surface patterns
  if (myPatternBase != 0)
  {
#if !defined(GL_ES_VERSION_2_0)
    if (theGlCtx->IsValid())
    {
      theGlCtx->core11->glDeleteLists ((GLuint )myPatternBase, TEL_HS_USER_DEF_START);
    }
#else
    (void )theGlCtx;
#endif
    myPatternBase = 0;
  }
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void OpenGl_LineAttributes::Init (const OpenGl_Context* theGlCtx)
{
  // Return if already initialized
  if (myPatternBase != 0)
  {
    return;
  }

#if !defined(GL_ES_VERSION_2_0)
  if (theGlCtx->core11 == NULL)
  {
    return;
  }

  // GL_POLYGON_STIPPLE need 32x32 stipple patterns
  const int nbi = sizeof(myInteriors) / (32 * sizeof(unsigned int));
  myPatternBase = glGenLists(TEL_HS_USER_DEF_START);
  for (int i = 1; i < TEL_HS_USER_DEF_START; i++)
  {
    theGlCtx->core11->glNewList ((GLuint )myPatternBase + i, GL_COMPILE);
    theGlCtx->core11->glPolygonStipple ((const GLubyte* )myInteriors[i < nbi ? i : 0]);
    theGlCtx->core11->glEndList();
  }
#else
  (void )theGlCtx;
#endif
}

// =======================================================================
// function : SetTypeOfHatch
// purpose  :
// =======================================================================
int OpenGl_LineAttributes::SetTypeOfHatch (const OpenGl_Context* theGlCtx, const int theType)
{
  // Return if not initialized
  if (myPatternBase == 0)
  {
    return 0;
  }

  const int anOldType = myTypeOfHatch;

#if !defined(GL_ES_VERSION_2_0)
  if (theType != 0)
  {
    theGlCtx->core11->glCallList ((GLuint )myPatternBase + (GLuint )theType);

    if (myIsEnabled)
    {
      theGlCtx->core11fwd->glEnable (GL_POLYGON_STIPPLE);
    }
  }
  else
  {
    theGlCtx->core11fwd->glDisable (GL_POLYGON_STIPPLE);
  }
#else
  (void )theGlCtx;
#endif
  myTypeOfHatch = theType;

  return anOldType;
}

// =======================================================================
// function : SetEnabled
// purpose  :
// =======================================================================
bool OpenGl_LineAttributes::SetEnabled (const OpenGl_Context* theGlCtx,
                                        const bool theToEnable)
{
  // Return if not initialized
  if (myPatternBase == 0)
  {
    return false;
  }

  const bool anOldIsEnabled = myIsEnabled;

#if !defined(GL_ES_VERSION_2_0)
  if (theToEnable)
  {
    if (myTypeOfHatch != 0)
    {
      theGlCtx->core11fwd->glEnable (GL_POLYGON_STIPPLE);
    }
  }
  else
  {
    theGlCtx->core11fwd->glDisable (GL_POLYGON_STIPPLE);
  }
#else
  (void )theGlCtx;
#endif
  myIsEnabled = theToEnable;

  return anOldIsEnabled;
}
