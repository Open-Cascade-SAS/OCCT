// Created on: 2000-06-16
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <Graphic3d_ArrayOfPrimitives.hxx>

#include <NCollection_AlignedAllocator.hxx>
#include <TCollection_AsciiString.hxx>

#include <stdio.h>
#include <stdlib.h>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_ArrayOfPrimitives, Standard_Transient)

// =======================================================================
// function : Graphic3d_ArrayOfPrimitives
// purpose  :
// =======================================================================
Graphic3d_ArrayOfPrimitives::Graphic3d_ArrayOfPrimitives (const Graphic3d_TypeOfPrimitiveArray theType,
                                                          const Standard_Integer               theMaxVertexs,
                                                          const Standard_Integer               theMaxBounds,
                                                          const Standard_Integer               theMaxEdges,
                                                          const Standard_Boolean               theHasVNormals,
                                                          const Standard_Boolean               theHasVColors,
                                                          const Standard_Boolean               theHasFColors,
                                                          const Standard_Boolean               theHasVTexels)
: myType       (theType),
  myMaxBounds  (0),
  myMaxVertexs (0),
  myMaxEdges   (0),
  myVNor       (0),
  myVTex       (0),
  myVCol       (0)
{
  Handle(NCollection_AlignedAllocator) anAlloc = new NCollection_AlignedAllocator (16);
  myAttribs = new Graphic3d_Buffer (anAlloc);
  if (theMaxVertexs < 1)
  {
    return;
  }

  if (theMaxEdges > 0)
  {
    myIndices = new Graphic3d_IndexBuffer (anAlloc);
    if (theMaxVertexs < Standard_Integer(USHRT_MAX))
    {
      if (!myIndices->Init<unsigned short> (theMaxEdges))
      {
        myIndices.Nullify();
        return;
      }
    }
    else
    {
      if (!myIndices->Init<unsigned int> (theMaxEdges))
      {
        myIndices.Nullify();
        return;
      }
    }
    myIndices->NbElements = 0;
  }

  Graphic3d_Attribute anAttribs[4];
  Standard_Integer    aNbAttribs = 0;
  anAttribs[aNbAttribs].Id       = Graphic3d_TOA_POS;
  anAttribs[aNbAttribs].DataType = Graphic3d_TOD_VEC3;
  ++aNbAttribs;
  if (theHasVNormals)
  {
    anAttribs[aNbAttribs].Id       = Graphic3d_TOA_NORM;
    anAttribs[aNbAttribs].DataType = Graphic3d_TOD_VEC3;
    ++aNbAttribs;
  }
  if (theHasVTexels)
  {
    anAttribs[aNbAttribs].Id       = Graphic3d_TOA_UV;
    anAttribs[aNbAttribs].DataType = Graphic3d_TOD_VEC2;
    ++aNbAttribs;
  }
  if (theHasVColors)
  {
    anAttribs[aNbAttribs].Id       = Graphic3d_TOA_COLOR;
    anAttribs[aNbAttribs].DataType = Graphic3d_TOD_VEC4UB;
    ++aNbAttribs;
  }

  if (!myAttribs->Init (theMaxVertexs, anAttribs, aNbAttribs))
  {
    myAttribs.Nullify();
    myIndices.Nullify();
    return;
  }
  memset (myAttribs->ChangeData (0), 0, size_t(myAttribs->Stride) * size_t(myAttribs->NbElements));

  if (theMaxBounds > 0)
  {
    myBounds = new Graphic3d_BoundBuffer (anAlloc);
    if (!myBounds->Init (theMaxBounds, theHasFColors))
    {
      myAttribs.Nullify();
      myIndices.Nullify();
      myBounds .Nullify();
      return;
    }
    myBounds->NbBounds = 0;
  }

  for (Standard_Integer anAttribIter = 0; anAttribIter < aNbAttribs; ++anAttribIter)
  {
    const Graphic3d_Attribute& anAttrib = anAttribs[anAttribIter];
    switch (anAttrib.Id)
    {
      case Graphic3d_TOA_POS:
      case Graphic3d_TOA_CUSTOM:
        break;
      case Graphic3d_TOA_NORM:
      {
        myVNor = static_cast<Standard_Byte>(myAttribs->AttributeOffset (anAttribIter));
        break;
      }
      case Graphic3d_TOA_UV:
      {
        myVTex = static_cast<Standard_Byte>(myAttribs->AttributeOffset (anAttribIter));
        break;
      }
      case Graphic3d_TOA_COLOR:
      {
        myVCol = static_cast<Standard_Byte>(myAttribs->AttributeOffset (anAttribIter));
        break;
      }
    }
  }

  myAttribs->NbElements = 0;
  myMaxVertexs = theMaxVertexs;
  myMaxBounds  = theMaxBounds;
  myMaxEdges   = theMaxEdges;
}

// =======================================================================
// function : ~Graphic3d_ArrayOfPrimitives
// purpose  :
// =======================================================================
Graphic3d_ArrayOfPrimitives::~Graphic3d_ArrayOfPrimitives()
{
  myVNor = 0;
  myVTex = 0;
  myVCol = 0;
  myIndices.Nullify();
  myAttribs.Nullify();
  myBounds .Nullify();
}

// =======================================================================
// function : AddBound
// purpose  :
// =======================================================================
Standard_Integer Graphic3d_ArrayOfPrimitives::AddBound (const Standard_Integer theEdgeNumber)
{
  if (myBounds.IsNull())
  {
    return 0;
  }
  Standard_Integer anIndex = myBounds->NbBounds;
  if (anIndex >= myMaxBounds)
  {
    throw Standard_OutOfRange("TOO many BOUNDS");
  }

  myBounds->Bounds[anIndex] = theEdgeNumber;
  myBounds->NbBounds        = ++anIndex;
  return anIndex;
}

// =======================================================================
// function : AddBound
// purpose  :
// =======================================================================
Standard_Integer Graphic3d_ArrayOfPrimitives::AddBound (const Standard_Integer theEdgeNumber,
                                                        const Standard_Real    theR,
                                                        const Standard_Real    theG,
                                                        const Standard_Real    theB)
{
  if (myBounds.IsNull())
  {
    return 0;
  }
  Standard_Integer anIndex = myBounds->NbBounds;
  if (anIndex >= myMaxBounds)
  {
    throw Standard_OutOfRange("TOO many BOUND");
  }

  myBounds->Bounds[anIndex] = theEdgeNumber;
  myBounds->NbBounds        = ++anIndex;
  SetBoundColor (anIndex, theR, theG, theB);
  return anIndex;
}

// =======================================================================
// function : AddEdge
// purpose  :
// =======================================================================
Standard_Integer Graphic3d_ArrayOfPrimitives::AddEdge (const Standard_Integer theVertexIndex)
{
  if (myIndices.IsNull())
  {
    return 0;
  }

  Standard_Integer anIndex = myIndices->NbElements;
  if (anIndex >= myMaxEdges)
  {
    throw Standard_OutOfRange("TOO many EDGE");
  }

  Standard_Integer aVertIndex = theVertexIndex - 1;
  if (theVertexIndex <= 0
   || aVertIndex >= myMaxVertexs)
  {
    throw Standard_OutOfRange("BAD EDGE vertex index");
  }

  myIndices->SetIndex (anIndex, aVertIndex);
  myIndices->NbElements = ++anIndex;
  return anIndex;
}

// =======================================================================
// function : StringType
// purpose  :
// =======================================================================
Standard_CString Graphic3d_ArrayOfPrimitives::StringType() const
{
  switch (myType)
  {
    case Graphic3d_TOPA_POINTS:           return "ArrayOfPoints";
    case Graphic3d_TOPA_POLYLINES:        return "ArrayOfPolylines";
    case Graphic3d_TOPA_SEGMENTS:         return "ArrayOfSegments";
    case Graphic3d_TOPA_POLYGONS:         return "ArrayOfPolygons";
    case Graphic3d_TOPA_TRIANGLES:        return "ArrayOfTriangles";
    case Graphic3d_TOPA_QUADRANGLES:      return "ArrayOfQuadrangles";
    case Graphic3d_TOPA_TRIANGLESTRIPS:   return "ArrayOfTriangleStrips";
    case Graphic3d_TOPA_QUADRANGLESTRIPS: return "ArrayOfQuadrangleStrips";
    case Graphic3d_TOPA_TRIANGLEFANS:     return "ArrayOfTriangleFans";
    case Graphic3d_TOPA_UNDEFINED:        return "UndefinedArray";
  }
  return "UndefinedArray";
}

// =======================================================================
// function : ItemNumber
// purpose  :
// =======================================================================
Standard_Integer Graphic3d_ArrayOfPrimitives::ItemNumber() const
{
  if (myAttribs.IsNull())
  {
    return -1;
  }

  switch (myType)
  {
    case Graphic3d_TOPA_POINTS:           return myAttribs->NbElements;
    case Graphic3d_TOPA_POLYLINES:
    case Graphic3d_TOPA_POLYGONS:         return !myBounds.IsNull() ? myBounds->NbBounds : 1;
    case Graphic3d_TOPA_SEGMENTS:         return myIndices.IsNull() || myIndices->NbElements < 1
                                               ? myAttribs->NbElements / 2
                                               : myIndices->NbElements / 2;
    case Graphic3d_TOPA_TRIANGLES:        return myIndices.IsNull() || myIndices->NbElements < 1
                                               ? myAttribs->NbElements / 3
                                               : myIndices->NbElements / 3;
    case Graphic3d_TOPA_QUADRANGLES:      return myIndices.IsNull() || myIndices->NbElements < 1
                                               ? myAttribs->NbElements / 4
                                               : myIndices->NbElements / 4;
    case Graphic3d_TOPA_TRIANGLESTRIPS:   return !myBounds.IsNull()
                                               ? myAttribs->NbElements - 2 * myBounds->NbBounds
                                               : myAttribs->NbElements - 2;
    case Graphic3d_TOPA_QUADRANGLESTRIPS: return !myBounds.IsNull()
                                               ? myAttribs->NbElements / 2 - myBounds->NbBounds
                                               : myAttribs->NbElements / 2 - 1;
    case Graphic3d_TOPA_TRIANGLEFANS:     return !myBounds.IsNull()
                                               ? myAttribs->NbElements - 2 * myBounds->NbBounds
                                               : myAttribs->NbElements - 2;
    case Graphic3d_TOPA_UNDEFINED:        return -1;
  }
  return -1;
}

// =======================================================================
// function : IsValid
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_ArrayOfPrimitives::IsValid()
{
  if (myAttribs.IsNull())
  {
    return Standard_False;
  }

  Standard_Integer nvertexs = myAttribs->NbElements;
  Standard_Integer nbounds  = myBounds.IsNull()  ? 0 : myBounds->NbBounds;
  Standard_Integer nedges   = myIndices.IsNull() ? 0 : myIndices->NbElements;
  switch (myType)
  {
    case Graphic3d_TOPA_POINTS:
      if (nvertexs < 1)
      {
        return Standard_False;
      }
      break;
    case Graphic3d_TOPA_POLYLINES:
      if (nedges > 0
       && nedges < 2)
      {
        return Standard_False;
      }
      if (nvertexs < 2)
      {
        return Standard_False;
      }
      break;
    case Graphic3d_TOPA_SEGMENTS:
      if (nvertexs < 2)
      {
        return Standard_False;
      }
      break;
    case Graphic3d_TOPA_POLYGONS:
      if (nedges > 0
       && nedges < 3)
      {
        return Standard_False;
      }
      if (nvertexs < 3)
      {
        return Standard_False;
      }
      break;
    case Graphic3d_TOPA_TRIANGLES:
      if (nedges > 0)
      {
        if (nedges < 3
         || nedges % 3 != 0)
        {
          if (nedges <= 3)
          {
            return Standard_False;
          }
          myIndices->NbElements = 3 * (nedges / 3);
        }
      }
      else if (nvertexs < 3
            || nvertexs % 3 != 0 )
      {
        if (nvertexs <= 3)
        {
          return Standard_False;
        }
        myAttribs->NbElements = 3 * (nvertexs / 3);
      }
      break;
    case Graphic3d_TOPA_QUADRANGLES:
      if (nedges > 0)
      {
        if (nedges < 4
         || nedges % 4 != 0)
        {
          if (nedges <= 4)
          {
            return Standard_False;
          }
          myIndices->NbElements = 4 * (nedges / 4);
        }
      }
      else if (nvertexs < 4
            || nvertexs % 4 != 0)
      {
        if (nvertexs <= 4)
        {
          return Standard_False;
        }
        myAttribs->NbElements = 4 * (nvertexs / 4);
      }
      break;
    case Graphic3d_TOPA_TRIANGLEFANS:
    case Graphic3d_TOPA_TRIANGLESTRIPS:
      if (nvertexs < 3)
      {
        return Standard_False;
      }
      break;
    case Graphic3d_TOPA_QUADRANGLESTRIPS:
      if (nvertexs < 4)
      {
        return Standard_False;
      }
      break;
    case Graphic3d_TOPA_UNDEFINED:
    default:
      return Standard_False;
  }

  // total number of edges(vertices) in bounds should be the same as variable
  // of total number of defined edges(vertices); if no edges - only vertices
  // could be in bounds.
  if (nbounds > 0)
  {
    Standard_Integer n = 0;
    for (Standard_Integer aBoundIter = 0; aBoundIter < nbounds; ++aBoundIter)
    {
      n += myBounds->Bounds[aBoundIter];
    }
    if (nedges > 0
     && n != nedges)
    {
      if (nedges <= n)
      {
        return Standard_False;
      }
      myIndices->NbElements = n;
    }
    else if (nedges == 0
          && n != nvertexs)
    {
      if (nvertexs <= n)
      {
        return Standard_False;
      }
      myAttribs->NbElements = n;
    }
  }

  // check that edges (indexes to an array of vertices) are in range.
  if (nedges > 0)
  {
    for (Standard_Integer anEdgeIter = 0; anEdgeIter < nedges; ++anEdgeIter)
    {
      if (myIndices->Index (anEdgeIter) >= myAttribs->NbElements)
      {
        return Standard_False;
      }
    }
  }
  return Standard_True;
}
