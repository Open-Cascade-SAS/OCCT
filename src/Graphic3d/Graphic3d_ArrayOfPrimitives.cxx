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


#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_InitialisationError.hxx>
#include <NCollection_AlignedAllocator.hxx>
#include <OSD_Environment.hxx>
#include <Quantity_Color.hxx>
#include <Standard.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

#include <stdio.h>
#include <stdlib.h>
IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_ArrayOfPrimitives,MMgt_TShared)

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
    if (theMaxEdges < Standard_Integer(USHRT_MAX))
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
  memset (myAttribs->ChangeData (0), 0, myAttribs->Stride * myAttribs->NbElements);

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

void Graphic3d_ArrayOfPrimitives::Destroy()
{
  myVNor = 0;
  myVTex = 0;
  myVCol = 0;
  myIndices.Nullify();
  myAttribs.Nullify();
  myBounds .Nullify();
}

Standard_Integer Graphic3d_ArrayOfPrimitives::AddVertex (const Standard_ShortReal theX,
                                                         const Standard_ShortReal theY,
                                                         const Standard_ShortReal theZ)
{
  if (myAttribs.IsNull())
  {
    return 0;
  }

  const Standard_Integer anIndex = myAttribs->NbElements + 1;
  SetVertice (anIndex, theX, theY, theZ);
  return anIndex;
}

Standard_Integer Graphic3d_ArrayOfPrimitives::AddVertex (const gp_Pnt&         theVertex,
                                                         const Quantity_Color& theColor)
{
  const Standard_Integer anIndex = AddVertex (theVertex);
  SetVertexColor (anIndex, theColor.Red(), theColor.Green(), theColor.Blue());
  return anIndex;
}

Standard_Integer Graphic3d_ArrayOfPrimitives::AddVertex (const gp_Pnt&          theVertex,
                                                         const Standard_Integer theColor32)
{
  const Standard_Integer anIndex = AddVertex (theVertex);
  SetVertexColor (anIndex, theColor32);
  return anIndex;
}

Standard_Integer Graphic3d_ArrayOfPrimitives::AddVertex (const Standard_ShortReal theX,  const Standard_ShortReal theY,  const Standard_ShortReal theZ,
                                                         const Standard_ShortReal theNX, const Standard_ShortReal theNY, const Standard_ShortReal theNZ)
{
  if (myAttribs.IsNull())
  {
    return 0;
  }

  const Standard_Integer anIndex = myAttribs->NbElements + 1;
  SetVertice      (anIndex, theX,  theY,  theZ);
  SetVertexNormal (anIndex, theNX, theNY, theNZ);
  return anIndex;
}

Standard_Integer Graphic3d_ArrayOfPrimitives::AddVertex (const gp_Pnt&         theVertex,
                                                         const gp_Dir&         theNormal,
                                                         const Quantity_Color& theColor)
{
  const Standard_Integer anIndex = AddVertex (theVertex, theNormal);
  SetVertexColor (anIndex, theColor.Red(), theColor.Green(), theColor.Blue());
  return anIndex;
}

Standard_Integer Graphic3d_ArrayOfPrimitives::AddVertex (const gp_Pnt&          theVertex,
                                                         const gp_Dir&          theNormal,
                                                         const Standard_Integer theColor32)
{
  const Standard_Integer anIndex = AddVertex (theVertex, theNormal);
  SetVertexColor (anIndex, theColor32);
  return anIndex;
}

Standard_Integer Graphic3d_ArrayOfPrimitives::AddVertex (const Standard_ShortReal theX,  const Standard_ShortReal theY, const Standard_ShortReal theZ,
                                                         const Standard_ShortReal theTX, const Standard_ShortReal theTY)
{
  if (myAttribs.IsNull())
  {
    return 0;
  }

  const Standard_Integer anIndex = myAttribs->NbElements + 1;
  SetVertice     (anIndex, theX, theY, theZ);
  SetVertexTexel (anIndex, theTX, theTY);
  return anIndex;
}

Standard_Integer Graphic3d_ArrayOfPrimitives::AddVertex (const Standard_ShortReal theX,  const Standard_ShortReal theY,  const Standard_ShortReal theZ,
                                                         const Standard_ShortReal theNX, const Standard_ShortReal theNY, const Standard_ShortReal theNZ,
                                                         const Standard_ShortReal theTX, const Standard_ShortReal theTY)
{
  if (myAttribs.IsNull())
  {
    return 0;
  }

  const Standard_Integer anIndex = myAttribs->NbElements + 1;
  SetVertice      (anIndex, theX,  theY,  theZ);
  SetVertexNormal (anIndex, theNX, theNY, theNZ);
  SetVertexTexel  (anIndex, theTX, theTY);
  return anIndex;
}

Standard_Integer Graphic3d_ArrayOfPrimitives::AddBound (const Standard_Integer theEdgeNumber)
{
  if (myBounds.IsNull())
  {
    return 0;
  }
  Standard_Integer anIndex = myBounds->NbBounds;
  if (anIndex >= myMaxBounds)
  {
    Standard_OutOfRange::Raise ("TOO many BOUNDS");
  }

  myBounds->Bounds[anIndex] = theEdgeNumber;
  myBounds->NbBounds        = ++anIndex;
  return anIndex;
}

Standard_Integer Graphic3d_ArrayOfPrimitives::AddBound (const Standard_Integer theEdgeNumber,
                                                        const Quantity_Color&  theColor)
{
  return AddBound (theEdgeNumber, theColor.Red(), theColor.Green(), theColor.Blue());
}

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
    Standard_OutOfRange::Raise ("TOO many BOUND");
  }

  myBounds->Bounds[anIndex] = theEdgeNumber;
  myBounds->NbBounds        = ++anIndex;
  SetBoundColor (anIndex, theR, theG, theB);
  return anIndex;
}

Standard_Integer Graphic3d_ArrayOfPrimitives::AddEdge (const Standard_Integer theVertexIndex)
{
  if (myIndices.IsNull())
  {
    return 0;
  }

  Standard_Integer anIndex = myIndices->NbElements;
  if (anIndex >= myMaxEdges)
  {
    Standard_OutOfRange::Raise ("TOO many EDGE");
  }

  Standard_Integer aVertIndex = theVertexIndex - 1;
  if (theVertexIndex <= 0
   || aVertIndex >= myMaxVertexs)
  {
    Standard_OutOfRange::Raise ("BAD EDGE vertex index");
  }

  myIndices->SetIndex (anIndex, aVertIndex);
  myIndices->NbElements = ++anIndex;
  return anIndex;
}

void Graphic3d_ArrayOfPrimitives::SetVertice (const Standard_Integer theIndex,
                                              const gp_Pnt&          theVertex)
{
  SetVertice (theIndex,
              Standard_ShortReal (theVertex.X()),
              Standard_ShortReal (theVertex.Y()),
              Standard_ShortReal (theVertex.Z()));
}

void Graphic3d_ArrayOfPrimitives::SetVertexColor (const Standard_Integer theIndex,
                                                  const Quantity_Color&  theColor)
{
  SetVertexColor (theIndex, theColor.Red(), theColor.Green(), theColor.Blue());
}

void Graphic3d_ArrayOfPrimitives::SetVertexColor (const Standard_Integer theIndex,
                                                  const Standard_Integer theColor)
{
  if (myAttribs.IsNull())
  {
    return;
  }

  if (theIndex < 1
   || theIndex > myMaxVertexs)
  {
    Standard_OutOfRange::Raise ("BAD VERTEX index");
  }

  if (myVCol != 0)
  {
    *reinterpret_cast<Standard_Integer* >(myAttribs->changeValue (theIndex - 1) + size_t(myVCol)) = theColor;
  }
}

void Graphic3d_ArrayOfPrimitives::SetVertexNormal (const Standard_Integer theIndex,
                                                   const gp_Dir&          theNormal)
{
  SetVertexNormal (theIndex, theNormal.X(), theNormal.Y(), theNormal.Z());
}

void Graphic3d_ArrayOfPrimitives::SetVertexTexel (const Standard_Integer theIndex,
                                                  const gp_Pnt2d&        theTexel)
{
  SetVertexTexel (theIndex, theTexel.X(), theTexel.Y());
}

void Graphic3d_ArrayOfPrimitives::SetBoundColor (const Standard_Integer theIndex,
                                                 const Quantity_Color&  theColor)
{
  SetBoundColor (theIndex, theColor.Red(), theColor.Green(), theColor.Blue());
}

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

gp_Pnt Graphic3d_ArrayOfPrimitives::Vertice (const Standard_Integer theRank) const
{
  Standard_Real anXYZ[3];
  Vertice (theRank, anXYZ[0], anXYZ[1], anXYZ[2]);
  return gp_Pnt (anXYZ[0], anXYZ[1], anXYZ[2]);
}

Quantity_Color Graphic3d_ArrayOfPrimitives::VertexColor (const Standard_Integer theRank) const
{
  Standard_Real anRGB[3];
  VertexColor (theRank, anRGB[0], anRGB[1], anRGB[2]);
  return Quantity_Color (anRGB[0], anRGB[1], anRGB[2], Quantity_TOC_RGB);
}

gp_Dir Graphic3d_ArrayOfPrimitives::VertexNormal (const Standard_Integer theRank) const
{
  Standard_Real anXYZ[3];
  VertexNormal (theRank, anXYZ[0], anXYZ[1], anXYZ[2]);
  return gp_Dir (anXYZ[0], anXYZ[1], anXYZ[2]);
}

gp_Pnt2d Graphic3d_ArrayOfPrimitives::VertexTexel (const Standard_Integer theRank) const
{
  Standard_Real anXY[2];
  VertexTexel (theRank, anXY[0], anXY[1]);
  return gp_Pnt2d (anXY[0], anXY[1]);
}

Quantity_Color Graphic3d_ArrayOfPrimitives::BoundColor (const Standard_Integer theRank) const
{
  Standard_Real anRGB[3] = {0.0, 0.0, 0.0};
  BoundColor (theRank, anRGB[0], anRGB[1], anRGB[2]);
  return Quantity_Color (anRGB[0], anRGB[1], anRGB[2], Quantity_TOC_RGB);
}

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

void Graphic3d_ArrayOfPrimitives::ComputeVNormals (const Standard_Integer theFrom,
                                                   const Standard_Integer theTo)
{
  Standard_Integer aNext = theFrom + 1;
  Standard_Integer aLast = theTo   + 1;
  gp_Pnt aTri[3];
  if (myMaxEdges > 0)
  {
    aTri[0] = Vertice (Edge (aNext++));
    aTri[1] = Vertice (Edge (aNext++));
  }
  else
  {
    aTri[0] = Vertice (aNext++);
    aTri[1] = Vertice (aNext++);
  }

  gp_Vec vn;
  while (aNext <= aLast)
  {
    if (myMaxEdges > 0)
    {
      aTri[2] = Vertice (Edge (aNext));
    }
    else
    {
      aTri[2] = Vertice (aNext);
    }
    gp_Vec v21 (aTri[1], aTri[0]);
    gp_Vec v31 (aTri[2], aTri[0]);
    vn = v21 ^ v31;
    if (vn.SquareMagnitude() > 0.0)
    {
      break;
    }
    aNext++;
  }

  if (aNext > aLast)
  {
    return;
  }

  vn.Normalize();
  if (myMaxEdges > 0)
  {
    for (int i = theFrom + 1; i <= theTo + 1; i++)
    {
      SetVertexNormal (Edge (i), vn);
    }
  }
  else
  {
    for (int i = theFrom + 1; i <= theTo + 1; i++)
    {
      SetVertexNormal (i, vn);
    }
  }
}

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
