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

// =======================================================================
// function : OpenGl_LineAttributes
// purpose  :
// =======================================================================
OpenGl_LineAttributes::OpenGl_LineAttributes()
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
  if (theGlCtx != NULL
   && theGlCtx->IsValid())
  {
    for (OpenGl_MapOfHatchStylesAndIds::Iterator anIter (myStyles); anIter.More(); anIter.Next())
    {
      theGlCtx->core11ffp->glDeleteLists ((GLuint)anIter.Value(), 1);
    }
  }
  myStyles.Clear();
}

// =======================================================================
// function : init
// purpose  :
// =======================================================================
unsigned int OpenGl_LineAttributes::init (const OpenGl_Context* theGlCtx,
                                          const Handle(Graphic3d_HatchStyle)& theStyle)
{
  const unsigned int aListId = theGlCtx->core11ffp->glGenLists(1);
  theGlCtx->core11ffp->glNewList ((GLuint)aListId, GL_COMPILE);
  theGlCtx->core11ffp->glPolygonStipple ((const GLubyte*)theStyle->Pattern());
  theGlCtx->core11ffp->glEndList();
  return aListId;
}

// =======================================================================
// function : SetTypeOfHatch
// purpose  :
// =======================================================================
bool OpenGl_LineAttributes::SetTypeOfHatch (const OpenGl_Context*               theGlCtx,
                                            const Handle(Graphic3d_HatchStyle)& theStyle)
{
  if (theStyle.IsNull()
   || theStyle->HatchType() == Aspect_HS_SOLID
   || theGlCtx->core11ffp == NULL)
  {
    return false;
  }

  unsigned int aGpuListId = 0;
  if (!myStyles.Find (theStyle, aGpuListId))
  {
    aGpuListId = init (theGlCtx, theStyle);
    myStyles.Bind (theStyle, aGpuListId);
  }

  theGlCtx->core11ffp->glCallList ((GLuint)aGpuListId);
  return true;
}
