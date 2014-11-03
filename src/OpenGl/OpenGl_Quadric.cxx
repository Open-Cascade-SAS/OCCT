// Created on: 2014-10-15
// Created by: Denis Bogolepov
// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <OpenGl_Quadric.hxx>

#include <NCollection_AlignedAllocator.hxx>

// =======================================================================
// function : OpenGl_Quadric
// purpose  :
// =======================================================================
OpenGl_Quadric::OpenGl_Quadric()
: OpenGl_PrimitiveArray (NULL, Graphic3d_TOPA_TRIANGLES, NULL, NULL, NULL),
  myNbSlices (0),
  myNbStacks (0)
{
  myDrawMode = GL_TRIANGLES;
}

// =======================================================================
// function : init
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Quadric::init (const Standard_Integer theNbSlices,
                                       const Standard_Integer theNbStacks)
{
  myNbSlices = theNbSlices;
  myNbStacks = theNbStacks;
  return createArrays();
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_Quadric::Release (OpenGl_Context* theContext)
{
  myNbSlices = 0;
  myNbStacks = 0;
  OpenGl_PrimitiveArray::Release (theContext);
}

// =======================================================================
// function : createArrays
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Quadric::createArrays() const
{
  // Evaluate surface points and normals
  Graphic3d_Attribute anAttribsInfo[] =
  {
    { Graphic3d_TOA_POS,  Graphic3d_TOD_VEC3 },
    { Graphic3d_TOA_NORM, Graphic3d_TOD_VEC3 }
  };
  Handle(NCollection_AlignedAllocator) anAlloc = new NCollection_AlignedAllocator (16);
  myAttribs = new Graphic3d_Buffer      (anAlloc);
  myIndices = new Graphic3d_IndexBuffer (anAlloc);
  if (!myAttribs->Init (NbVertices(), anAttribsInfo, 2)
   || !myIndices->Init<GLuint> (NbTriangles() * 3))
  {
    return Standard_False;
  }

  const Standard_ShortReal aStepU = 1.0f / myNbSlices;
  const Standard_ShortReal aStepV = 1.0f / myNbStacks;
  for (Standard_Integer aU = 0; aU <= myNbSlices; ++aU)
  {
    const Standard_ShortReal aParamU = aU * aStepU;
    for (Standard_Integer aV = 0; aV <= myNbStacks; ++aV)
    {
      const Standard_ShortReal aParamV = aV * aStepV;
      const Standard_Integer   aVertId = aU * (myNbStacks + 1) + aV;
      Graphic3d_Vec3* aVertData = reinterpret_cast<Graphic3d_Vec3* >(myAttribs->changeValue (aVertId));
      aVertData[0] = evalVertex (aParamU, aParamV);
      aVertData[1] = evalNormal (aParamU, aParamV);
    }
  }

  // Extract triangle indices
  for (Standard_Integer aU = 0, aLastIndex = -1; aU < myNbSlices; ++aU)
  {
    for (Standard_Integer aV = 0; aV < myNbStacks; ++aV)
    {
      myIndices->ChangeValue<GLuint> (++aLastIndex) = aU       * (myNbStacks + 1) + aV;
      myIndices->ChangeValue<GLuint> (++aLastIndex) = (aU + 1) * (myNbStacks + 1) + aV;
      myIndices->ChangeValue<GLuint> (++aLastIndex) = (aU + 1) * (myNbStacks + 1) + (aV + 1);
      myIndices->ChangeValue<GLuint> (++aLastIndex) = (aU + 1) * (myNbStacks + 1) + (aV + 1);
      myIndices->ChangeValue<GLuint> (++aLastIndex) = aU       * (myNbStacks + 1) + (aV + 1);
      myIndices->ChangeValue<GLuint> (++aLastIndex) = aU       * (myNbStacks + 1) + aV;
    }
  }
  return Standard_True;
}
