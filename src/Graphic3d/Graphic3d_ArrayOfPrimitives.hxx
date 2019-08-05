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

#ifndef _Graphic3d_ArrayOfPrimitives_HeaderFile
#define _Graphic3d_ArrayOfPrimitives_HeaderFile

#include <Graphic3d_BoundBuffer.hxx>
#include <Graphic3d_ArrayFlags.hxx>
#include <Graphic3d_Buffer.hxx>
#include <Graphic3d_IndexBuffer.hxx>
#include <Graphic3d_TypeOfPrimitiveArray.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Quantity_Color.hxx>

class Graphic3d_ArrayOfPrimitives;
DEFINE_STANDARD_HANDLE(Graphic3d_ArrayOfPrimitives, Standard_Transient)

//! This class furnish services to defined and fill an array of primitives
//! which can be passed directly to graphics rendering API.
//!
//! The basic interface consists of the following parts:
//! 1) Specifying primitive type.
//!    WARNING! Particular primitive types might be unsupported by specific hardware/graphics API (like quads and polygons).
//!             It is always preferred using one of basic types having maximum compatibility:
//!             Point, Triangle (or Triangle strip), Segment aka Lines (or Polyline aka Line Strip).
//!    Primitive strip types can be used to reduce memory usage as alternative to Indexed arrays.
//! 2) Vertex array.
//!    - Specifying the (maximum) number of vertexes within array.
//!    - Specifying the vertex attributes, complementary to mandatory vertex Position (normal, color, UV texture coordinates).
//!    - Defining vertex values by using various versions of AddVertex() or SetVertex*() methods.
//! 3) Index array (optional).
//!    - Specifying the (maximum) number of indexes (edges).
//!    - Defining index values by using AddEdge() method; the index value should be within number of defined Vertexes.
//!
//!    Indexed array allows sharing vertex data across Primitives and thus reducing memory usage,
//!    since index size is much smaller then size of vertex with all its attributes.
//!    It is a preferred way for defining primitive array and main alternative to Primitive Strips for optimal memory usage,
//!    although it is also possible (but unusual) defining Indexed Primitive Strip.
//!    Note that it is NOT possible sharing Vertex Attributes partially (e.g. share Position, but have different Normals);
//!    in such cases Vertex should be entirely duplicated with all Attributes.
//! 4) Bounds array (optional).
//!    - Specifying the (maximum) number of bounds.
//!    - Defining bounds using AddBound() methods.
//!
//!    Bounds allow splitting Primitive Array into sub-groups.
//!    This is useful only in two cases - for specifying per-group color and for restarting Primitive Strips.
//!    WARNING! Bounds within Primitive Array break rendering batches into parts (additional for loops),
//!             affecting rendering performance negatively (increasing CPU load).
class Graphic3d_ArrayOfPrimitives : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_ArrayOfPrimitives, Standard_Transient)
public:

  //! Create an array of specified type.
  static Handle(Graphic3d_ArrayOfPrimitives) CreateArray (Graphic3d_TypeOfPrimitiveArray theType,
                                                          Standard_Integer theMaxVertexs,
                                                          Standard_Integer theMaxEdges,
                                                          Graphic3d_ArrayFlags theArrayFlags)
  {
    return CreateArray (theType, theMaxVertexs, 0, theMaxEdges, theArrayFlags);
  }

  //! Create an array of specified type.
  static Standard_EXPORT Handle(Graphic3d_ArrayOfPrimitives) CreateArray (Graphic3d_TypeOfPrimitiveArray theType,
                                                                          Standard_Integer theMaxVertexs,
                                                                          Standard_Integer theMaxBounds,
                                                                          Standard_Integer theMaxEdges,
                                                                          Graphic3d_ArrayFlags theArrayFlags);
public:

  //! Destructor.
  Standard_EXPORT virtual ~Graphic3d_ArrayOfPrimitives();

  //! Returns vertex attributes buffer (colors, normals, texture coordinates).
  const Handle(Graphic3d_Buffer)& Attributes() const { return myAttribs; }

  //! Returns the type of this primitive
  Graphic3d_TypeOfPrimitiveArray Type() const { return myType; }

  //! Returns the string type of this primitive
  Standard_EXPORT Standard_CString StringType() const;

  //! Returns TRUE when vertex normals array is defined.
  Standard_Boolean HasVertexNormals() const { return myNormData != NULL; }

  //! Returns TRUE when vertex colors array is defined.
  Standard_Boolean HasVertexColors() const { return myColData != NULL; }

  //! Returns TRUE when vertex texels array is defined.
  Standard_Boolean HasVertexTexels() const { return myTexData != NULL; }

  //! Returns the number of defined vertex
  Standard_Integer VertexNumber() const { return myAttribs->NbElements; }

  //! Returns the number of allocated vertex
  Standard_Integer VertexNumberAllocated() const { return myAttribs->NbMaxElements(); }

  //! Returns the number of total items according to the array type.
  Standard_EXPORT Standard_Integer ItemNumber() const;

  //! Returns TRUE only when the contains of this array is available.
  Standard_EXPORT Standard_Boolean IsValid();

  //! Adds a vertice in the array.
  //! @return the actual vertex number
  Standard_Integer AddVertex (const gp_Pnt& theVertex) { return AddVertex (theVertex.X(), theVertex.Y(), theVertex.Z()); }

  //! Adds a vertice in the array.
  //! @return the actual vertex number
  Standard_Integer AddVertex (const Graphic3d_Vec3& theVertex) { return AddVertex (theVertex.x(), theVertex.y(), theVertex.z()); }

  //! Adds a vertice in the array.
  //! @return the actual vertex number
  Standard_Integer AddVertex (const Standard_Real theX, const Standard_Real theY, const Standard_Real theZ)
  {
    return AddVertex (RealToShortReal (theX), RealToShortReal (theY), RealToShortReal (theZ));
  }

  //! Adds a vertice in the array.
  //! @return the actual vertex number.
  Standard_Integer AddVertex (const Standard_ShortReal theX, const Standard_ShortReal theY, const Standard_ShortReal theZ)
  {
    const Standard_Integer anIndex = myAttribs->NbElements + 1;
    SetVertice (anIndex, theX, theY, theZ);
    return anIndex;
  }

  //! Adds a vertice and vertex color in the vertex array.
  //! Warning: theColor is ignored when the hasVColors constructor parameter is FALSE
  //! @return the actual vertex number
  Standard_Integer AddVertex (const gp_Pnt& theVertex, const Quantity_Color& theColor)
  {
    const Standard_Integer anIndex = AddVertex (theVertex);
    SetVertexColor (anIndex, theColor.Red(), theColor.Green(), theColor.Blue());
    return anIndex;
  }

  //! Adds a vertice and vertex color in the vertex array.
  //! Warning: theColor is ignored when the hasVColors constructor parameter is FALSE
  //! @code
  //!   theColor32 = Alpha << 24 + Blue << 16 + Green << 8 + Red
  //! @endcode
  //! @return the actual vertex number
  Standard_Integer AddVertex (const gp_Pnt& theVertex, const Standard_Integer theColor32)
  {
    const Standard_Integer anIndex = AddVertex (theVertex);
    SetVertexColor (anIndex, theColor32);
    return anIndex;
  }

  //! Adds a vertice and vertex color in the vertex array.
  //! Warning: theColor is ignored when the hasVColors constructor parameter is FALSE
  //! @return the actual vertex number
  Standard_Integer AddVertex (const gp_Pnt&           theVertex,
                              const Graphic3d_Vec4ub& theColor)
  {
    const Standard_Integer anIndex = AddVertex (theVertex);
    SetVertexColor (anIndex, theColor);
    return anIndex;
  }

  //! Adds a vertice and vertex normal in the vertex array.
  //! Warning: theNormal is ignored when the hasVNormals constructor parameter is FALSE.
  //! @return the actual vertex number
  Standard_Integer AddVertex (const gp_Pnt& theVertex, const gp_Dir& theNormal)
  {
    return AddVertex (theVertex.X(), theVertex.Y(), theVertex.Z(),
                      theNormal.X(), theNormal.Y(), theNormal.Z());
  }

  //! Adds a vertice and vertex normal in the vertex array.
  //! Warning: Normal is ignored when the hasVNormals constructor parameter is FALSE.
  //! @return the actual vertex number
  Standard_Integer AddVertex (const Standard_Real theX,  const Standard_Real theY,  const Standard_Real theZ,
                              const Standard_Real theNX, const Standard_Real theNY, const Standard_Real theNZ)
  {
    return AddVertex (RealToShortReal (theX),  RealToShortReal (theY),  RealToShortReal (theZ),
                      Standard_ShortReal (theNX), Standard_ShortReal (theNY), Standard_ShortReal (theNZ));
  }

  //! Adds a vertice and vertex normal in the vertex array.
  //! Warning: Normal is ignored when the hasVNormals constructor parameter is FALSE.
  //! @return the actual vertex number
  Standard_Integer AddVertex (const Standard_ShortReal theX,  const Standard_ShortReal theY,  const Standard_ShortReal theZ,
                              const Standard_ShortReal theNX, const Standard_ShortReal theNY, const Standard_ShortReal theNZ)
  {
    const Standard_Integer anIndex = myAttribs->NbElements + 1;
    SetVertice      (anIndex, theX,  theY,  theZ);
    SetVertexNormal (anIndex, theNX, theNY, theNZ);
    return anIndex;
  }

  //! Adds a vertice,vertex normal and color in the vertex array.
  //! Warning: theNormal is ignored when the hasVNormals constructor parameter is FALSE
  //! and      theColor  is ignored when the hasVColors  constructor parameter is FALSE.
  //! @return the actual vertex number
  Standard_Integer AddVertex (const gp_Pnt& theVertex, const gp_Dir& theNormal, const Quantity_Color& theColor)
  {
    const Standard_Integer anIndex = AddVertex (theVertex, theNormal);
    SetVertexColor (anIndex, theColor.Red(), theColor.Green(), theColor.Blue());
    return anIndex;
  }

  //! Adds a vertice,vertex normal and color in the vertex array.
  //! Warning: theNormal is ignored when the hasVNormals constructor parameter is FALSE
  //! and      theColor  is ignored when the hasVColors  constructor parameter is FALSE.
  //! @code
  //!   theColor32 = Alpha << 24 + Blue << 16 + Green << 8 + Red
  //! @endcode
  //! @return the actual vertex number
  Standard_Integer AddVertex (const gp_Pnt& theVertex, const gp_Dir& theNormal, const Standard_Integer theColor32)
  {
    const Standard_Integer anIndex = AddVertex (theVertex, theNormal);
    SetVertexColor (anIndex, theColor32);
    return anIndex;
  }

  //! Adds a vertice and vertex texture in the vertex array.
  //! theTexel is ignored when the hasVTexels constructor parameter is FALSE.
  //! @return the actual vertex number
  Standard_Integer AddVertex (const gp_Pnt& theVertex, const gp_Pnt2d& theTexel)
  {
    return AddVertex (theVertex.X(), theVertex.Y(), theVertex.Z(),
                      theTexel.X(), theTexel.Y());
  }

  //! Adds a vertice and vertex texture coordinates in the vertex array.
  //! Texel is ignored when the hasVTexels constructor parameter is FALSE.
  //! @return the actual vertex number
  Standard_Integer AddVertex (const Standard_Real theX, const Standard_Real theY, const Standard_Real theZ,
                              const Standard_Real theTX, const Standard_Real theTY)
  {
    return AddVertex (RealToShortReal (theX),  RealToShortReal (theY),  RealToShortReal (theZ),
                      Standard_ShortReal (theTX), Standard_ShortReal (theTY));
  }

  //! Adds a vertice and vertex texture coordinates in the vertex array.
  //! Texel is ignored when the hasVTexels constructor parameter is FALSE.
  //! @return the actual vertex number
  Standard_Integer AddVertex (const Standard_ShortReal theX, const Standard_ShortReal theY, const Standard_ShortReal theZ,
                              const Standard_ShortReal theTX, const Standard_ShortReal theTY)
  {
    const Standard_Integer anIndex = myAttribs->NbElements + 1;
    SetVertice     (anIndex, theX, theY, theZ);
    SetVertexTexel (anIndex, theTX, theTY);
    return anIndex;
  }

  //! Adds a vertice,vertex normal and texture in the vertex array.
  //! Warning: theNormal is ignored when the hasVNormals constructor parameter is FALSE
  //! and      theTexel  is ignored when the hasVTexels  constructor parameter is FALSE.
  //! @return the actual vertex number
  Standard_Integer AddVertex (const gp_Pnt& theVertex, const gp_Dir& theNormal, const gp_Pnt2d& theTexel)
  {
    return AddVertex (theVertex.X(), theVertex.Y(), theVertex.Z(),
                      theNormal.X(), theNormal.Y(), theNormal.Z(),
                      theTexel.X(),  theTexel.Y());
  }

  //! Adds a vertice,vertex normal and texture in the vertex array.
  //! Warning: Normal is ignored when the hasVNormals constructor parameter is FALSE
  //! and      Texel  is ignored when the hasVTexels  constructor parameter is FALSE.
  //! @return the actual vertex number
  Standard_Integer AddVertex (const Standard_Real theX,  const Standard_Real theY,  const Standard_Real theZ,
                              const Standard_Real theNX, const Standard_Real theNY, const Standard_Real theNZ,
                              const Standard_Real theTX, const Standard_Real theTY)
  {
    return AddVertex (RealToShortReal (theX), RealToShortReal (theY), RealToShortReal (theZ),
                      Standard_ShortReal (theNX), Standard_ShortReal (theNY), Standard_ShortReal (theNZ),
                      Standard_ShortReal (theTX), Standard_ShortReal (theTY));
  }

  //! Adds a vertice,vertex normal and texture in the vertex array.
  //! Warning: Normal is ignored when the hasVNormals constructor parameter is FALSE
  //!     and  Texel  is ignored when the hasVTexels  constructor parameter is FALSE.
  //! @return the actual vertex number
  Standard_Integer AddVertex (const Standard_ShortReal theX,  const Standard_ShortReal theY,  const Standard_ShortReal theZ,
                              const Standard_ShortReal theNX, const Standard_ShortReal theNY, const Standard_ShortReal theNZ,
                              const Standard_ShortReal theTX, const Standard_ShortReal theTY)
  {
    const Standard_Integer anIndex = myAttribs->NbElements + 1;
    SetVertice     (anIndex, theX,  theY,  theZ);
    SetVertexNormal(anIndex, theNX, theNY, theNZ);
    SetVertexTexel (anIndex, theTX, theTY);
    return anIndex;
  }

  //! Change the vertice of rank theIndex in the array.
  void SetVertice (const Standard_Integer theIndex, const gp_Pnt& theVertex)
  {
    SetVertice (theIndex, Standard_ShortReal (theVertex.X()), Standard_ShortReal (theVertex.Y()), Standard_ShortReal (theVertex.Z()));
  }

  //! Change the vertice of rank theIndex in the array.
  void SetVertice (const Standard_Integer theIndex, const Standard_ShortReal theX, const Standard_ShortReal theY, const Standard_ShortReal theZ)
  {
    Standard_OutOfRange_Raise_if (theIndex < 1 || theIndex > myAttribs->NbMaxElements(), "BAD VERTEX index");
    Graphic3d_Vec3& aVec = *reinterpret_cast<Graphic3d_Vec3*> (myAttribs->ChangeData() + myPosStride * (theIndex - 1));
    aVec.x() = theX;
    aVec.y() = theY;
    aVec.z() = theZ;
    if (myAttribs->NbElements < theIndex)
    {
      myAttribs->NbElements = theIndex;
    }
  }

  //! Change the vertex color of rank theIndex in the array.
  void SetVertexColor (const Standard_Integer theIndex, const Quantity_Color& theColor)
  {
    SetVertexColor (theIndex, theColor.Red(), theColor.Green(), theColor.Blue());
  }

  //! Change the vertex color of rank theIndex in the array.
  void SetVertexColor (const Standard_Integer theIndex, const Standard_Real theR, const Standard_Real theG, const Standard_Real theB)
  {
    Standard_OutOfRange_Raise_if (theIndex < 1 || theIndex > myAttribs->NbMaxElements(), "BAD VERTEX index");
    if (myColData != NULL)
    {
      Graphic3d_Vec4ub* aColorPtr = reinterpret_cast<Graphic3d_Vec4ub* >(myColData + myColStride * (theIndex - 1));
      aColorPtr->SetValues (Standard_Byte(theR * 255.0),
                            Standard_Byte(theG * 255.0),
                            Standard_Byte(theB * 255.0), 255);
    }
    myAttribs->NbElements = Max (theIndex, myAttribs->NbElements);
  }

  //! Change the vertex color of rank theIndex in the array.
  void SetVertexColor (const Standard_Integer  theIndex,
                       const Graphic3d_Vec4ub& theColor)
  {
    Standard_OutOfRange_Raise_if (theIndex < 1 || theIndex > myAttribs->NbMaxElements(), "BAD VERTEX index");
    if (myColData != NULL)
    {
      Graphic3d_Vec4ub* aColorPtr =  reinterpret_cast<Graphic3d_Vec4ub* >(myColData + myColStride * (theIndex - 1));
      (*aColorPtr) = theColor;
    }
    myAttribs->NbElements = Max (theIndex, myAttribs->NbElements);
  }

  //! Change the vertex color of rank theIndex> in the array.
  //! @code
  //!   theColor32 = Alpha << 24 + Blue << 16 + Green << 8 + Red
  //! @endcode
  void SetVertexColor (const Standard_Integer theIndex, const Standard_Integer theColor32)
  {
    Standard_OutOfRange_Raise_if (theIndex < 1 || theIndex > myAttribs->NbMaxElements(), "BAD VERTEX index");
    if (myColData != NULL)
    {
      *reinterpret_cast<Standard_Integer* >(myColData + myColStride * (theIndex - 1)) = theColor32;
    }
  }

  //! Change the vertex normal of rank theIndex in the array.
  void SetVertexNormal (const Standard_Integer theIndex, const gp_Dir& theNormal)
  {
    SetVertexNormal (theIndex, theNormal.X(), theNormal.Y(), theNormal.Z());
  }

  //! Change the vertex normal of rank theIndex in the array.
  void SetVertexNormal (const Standard_Integer theIndex, const Standard_Real theNX, const Standard_Real theNY, const Standard_Real theNZ)
  {
    Standard_OutOfRange_Raise_if (theIndex < 1 || theIndex > myAttribs->NbMaxElements(), "BAD VERTEX index");
    if (myNormData != NULL)
    {
      Graphic3d_Vec3& aVec = *reinterpret_cast<Graphic3d_Vec3* >(myNormData + myNormStride * (theIndex - 1));
      aVec.x() = Standard_ShortReal (theNX);
      aVec.y() = Standard_ShortReal (theNY);
      aVec.z() = Standard_ShortReal (theNZ);
    }
    myAttribs->NbElements = Max (theIndex, myAttribs->NbElements);
  }

  //! Change the vertex texel of rank theIndex in the array.
  void SetVertexTexel (const Standard_Integer theIndex, const gp_Pnt2d& theTexel)
  {
    SetVertexTexel (theIndex, theTexel.X(), theTexel.Y());
  }

  //! Change the vertex texel of rank theIndex in the array.
  void SetVertexTexel (const Standard_Integer theIndex, const Standard_Real theTX, const Standard_Real theTY)
  {
    Standard_OutOfRange_Raise_if (theIndex < 1 || theIndex > myAttribs->NbMaxElements(), "BAD VERTEX index");
    if (myTexData != NULL)
    {
      Graphic3d_Vec2& aVec = *reinterpret_cast<Graphic3d_Vec2* >(myTexData + myTexStride * (theIndex - 1));
      aVec.x() = Standard_ShortReal (theTX);
      aVec.y() = Standard_ShortReal (theTY);
    }
    myAttribs->NbElements = Max (theIndex, myAttribs->NbElements);
  }

  //! Returns the vertice at rank theRank from the vertex table if defined.
  gp_Pnt Vertice (const Standard_Integer theRank) const
  {
    Standard_Real anXYZ[3];
    Vertice (theRank, anXYZ[0], anXYZ[1], anXYZ[2]);
    return gp_Pnt (anXYZ[0], anXYZ[1], anXYZ[2]);
  }

  //! Returns the vertice coordinates at rank theRank from the vertex table if defined.
  void Vertice (const Standard_Integer theRank, Standard_Real& theX, Standard_Real& theY, Standard_Real& theZ) const
  {
    theX = theY = theZ = 0.0;
    Standard_OutOfRange_Raise_if (theRank < 1 || theRank > myAttribs->NbElements, "BAD VERTEX index");
    const Graphic3d_Vec3& aVec = *reinterpret_cast<const Graphic3d_Vec3*> (myAttribs->Data() + myPosStride * (theRank - 1));
    theX = Standard_Real(aVec.x());
    theY = Standard_Real(aVec.y());
    theZ = Standard_Real(aVec.z());
  }

  //! Returns the vertex color at rank theRank from the vertex table if defined.
  Quantity_Color VertexColor (const Standard_Integer theRank) const
  {
    Standard_Real anRGB[3];
    VertexColor (theRank, anRGB[0], anRGB[1], anRGB[2]);
    return Quantity_Color (anRGB[0], anRGB[1], anRGB[2], Quantity_TOC_RGB);
  }

  //! Returns the vertex color at rank theIndex from the vertex table if defined.
  void VertexColor (const Standard_Integer theIndex,
                    Graphic3d_Vec4ub&      theColor) const
  {
    Standard_OutOfRange_Raise_if (myColData == NULL || theIndex < 1 || theIndex > myAttribs->NbElements, "BAD VERTEX index");
    theColor = *reinterpret_cast<const Graphic3d_Vec4ub* >(myColData + myColStride * (theIndex - 1));
  }

  //! Returns the vertex color values at rank theRank from the vertex table if defined.
  void VertexColor (const Standard_Integer theRank, Standard_Real& theR, Standard_Real& theG, Standard_Real& theB) const
  {
    theR = theG = theB = 0.0;
    Standard_OutOfRange_Raise_if (theRank < 1 || theRank > myAttribs->NbElements, "BAD VERTEX index");
    if (myColData == NULL)
    {
      return;
    }
    const Graphic3d_Vec4ub& aColor = *reinterpret_cast<const Graphic3d_Vec4ub* >(myColData + myColStride * (theRank - 1));
    theR = Standard_Real(aColor.r()) / 255.0;
    theG = Standard_Real(aColor.g()) / 255.0;
    theB = Standard_Real(aColor.b()) / 255.0;
  }

  //! Returns the vertex color values at rank theRank from the vertex table if defined.
  void VertexColor (const Standard_Integer theRank, Standard_Integer& theColor) const
  {
    Standard_OutOfRange_Raise_if (theRank < 1 || theRank > myAttribs->NbElements, "BAD VERTEX index");
    if (myColData != NULL)
    {
      theColor = *reinterpret_cast<const Standard_Integer* >(myColData + myColStride * (theRank - 1));
    }
  }

  //! Returns the vertex normal at rank theRank from the vertex table if defined.
  gp_Dir VertexNormal (const Standard_Integer theRank) const
  {
    Standard_Real anXYZ[3];
    VertexNormal (theRank, anXYZ[0], anXYZ[1], anXYZ[2]);
    return gp_Dir (anXYZ[0], anXYZ[1], anXYZ[2]);
  }

  //! Returns the vertex normal coordinates at rank theRank from the vertex table if defined.
  void VertexNormal (const Standard_Integer theRank, Standard_Real& theNX, Standard_Real& theNY, Standard_Real& theNZ) const
  {
    theNX = theNY = theNZ = 0.0;
    Standard_OutOfRange_Raise_if (theRank < 1 || theRank > myAttribs->NbElements, "BAD VERTEX index");
    if (myNormData != NULL)
    {
      const Graphic3d_Vec3& aVec = *reinterpret_cast<const Graphic3d_Vec3* >(myNormData + myNormStride * (theRank - 1));
      theNX = Standard_Real(aVec.x());
      theNY = Standard_Real(aVec.y());
      theNZ = Standard_Real(aVec.z());
    }
  }

  //! Returns the vertex texture at rank theRank from the vertex table if defined.
  gp_Pnt2d VertexTexel (const Standard_Integer theRank) const
  {
    Standard_Real anXY[2];
    VertexTexel (theRank, anXY[0], anXY[1]);
    return gp_Pnt2d (anXY[0], anXY[1]);
  }

  //! Returns the vertex texture coordinates at rank theRank from the vertex table if defined.
  void VertexTexel (const Standard_Integer theRank, Standard_Real& theTX, Standard_Real& theTY) const
  {
    theTX = theTY = 0.0;
    Standard_OutOfRange_Raise_if (theRank < 1 || theRank > myAttribs->NbElements, "BAD VERTEX index");
    if (myTexData != NULL)
    {
      const Graphic3d_Vec2& aVec = *reinterpret_cast<const Graphic3d_Vec2* >(myTexData + myTexStride * (theRank - 1));
      theTX = Standard_Real(aVec.x());
      theTY = Standard_Real(aVec.y());
    }
  }

public: //! @name optional array of Indices/Edges for using shared Vertex data

  //! Returns optional index buffer.
  const Handle(Graphic3d_IndexBuffer)& Indices() const { return myIndices; }

  //! Returns the number of defined edges
  Standard_Integer EdgeNumber() const { return !myIndices.IsNull() ? myIndices->NbElements : -1; }

  //! Returns the number of allocated edges
  Standard_Integer EdgeNumberAllocated() const { return !myIndices.IsNull() ? myIndices->NbMaxElements() : 0; }

  //! Returns the vertex index at rank theRank in the range [1,EdgeNumber()]
  Standard_Integer Edge (const Standard_Integer theRank) const
  {
    Standard_OutOfRange_Raise_if (myIndices.IsNull() || theRank < 1 || theRank > myIndices->NbElements, "BAD EDGE index");
    return Standard_Integer(myIndices->Index (theRank - 1) + 1);
  }

  //! Adds an edge in the range [1,VertexNumber()] in the array.
  //! @return the actual edges number
  Standard_EXPORT Standard_Integer AddEdge (const Standard_Integer theVertexIndex);

  //! Convenience method, adds two vertex indices (a segment) in the range [1,VertexNumber()] in the array.
  //! @return the actual edges number
  Standard_Integer AddEdges (Standard_Integer theVertexIndex1,
                             Standard_Integer theVertexIndex2)
  {
    AddEdge (theVertexIndex1);
    return AddEdge (theVertexIndex2);
  }

  //! Convenience method, adds two vertex indices (a segment) in the range [1,VertexNumber()] in the array of segments (Graphic3d_TOPA_SEGMENTS).
  //! Raises exception if array is not of type Graphic3d_TOPA_SEGMENTS.
  //! @return the actual edges number
  Standard_Integer AddSegmentEdges (Standard_Integer theVertexIndex1,
                                    Standard_Integer theVertexIndex2)
  {
    Standard_TypeMismatch_Raise_if (myType != Graphic3d_TOPA_SEGMENTS, "Not array of segments");
    return AddEdges (theVertexIndex1, theVertexIndex2);
  }

  //! Convenience method, adds three vertex indices (a triangle) in the range [1,VertexNumber()] in the array.
  //! @return the actual edges number
  Standard_Integer AddEdges (Standard_Integer theVertexIndex1,
                             Standard_Integer theVertexIndex2,
                             Standard_Integer theVertexIndex3)
  {
    AddEdge (theVertexIndex1);
    AddEdge (theVertexIndex2);
    return AddEdge (theVertexIndex3);
  }

  //! Convenience method, adds three vertex indices of triangle in the range [1,VertexNumber()] in the array of triangles.
  //! Raises exception if array is not of type Graphic3d_TOPA_TRIANGLES.
  //! @return the actual edges number
  Standard_Integer AddTriangleEdges (Standard_Integer theVertexIndex1,
                                     Standard_Integer theVertexIndex2,
                                     Standard_Integer theVertexIndex3)
  {
    Standard_TypeMismatch_Raise_if (myType != Graphic3d_TOPA_TRIANGLES, "Not array of triangles");
    return AddEdges (theVertexIndex1, theVertexIndex2, theVertexIndex3);
  }

  //! Convenience method, adds three vertex indices of triangle in the range [1,VertexNumber()] in the array of triangles.
  //! Raises exception if array is not of type Graphic3d_TOPA_TRIANGLES.
  //! @return the actual edges number
  Standard_Integer AddTriangleEdges (const Graphic3d_Vec3i& theIndexes)
  {
    Standard_TypeMismatch_Raise_if (myType != Graphic3d_TOPA_TRIANGLES, "Not array of triangles");
    return AddEdges (theIndexes[0], theIndexes[1], theIndexes[2]);
  }

  //! Convenience method, adds three vertex indices (4th component is ignored) of triangle in the range [1,VertexNumber()] in the array of triangles.
  //! Raises exception if array is not of type Graphic3d_TOPA_TRIANGLES.
  //! @return the actual edges number
  Standard_Integer AddTriangleEdges (const Graphic3d_Vec4i& theIndexes)
  {
    Standard_TypeMismatch_Raise_if (myType != Graphic3d_TOPA_TRIANGLES, "Not array of triangles");
    return AddEdges (theIndexes[0], theIndexes[1], theIndexes[2]);
  }

  //! Convenience method, adds four vertex indices (a quad) in the range [1,VertexNumber()] in the array.
  //! @return the actual edges number
  Standard_Integer AddEdges (Standard_Integer theVertexIndex1,
                             Standard_Integer theVertexIndex2,
                             Standard_Integer theVertexIndex3,
                             Standard_Integer theVertexIndex4)
  {
    AddEdge (theVertexIndex1);
    AddEdge (theVertexIndex2);
    AddEdge (theVertexIndex3);
    return AddEdge (theVertexIndex4);
  }

  //! Convenience method, adds four vertex indices (a quad) in the range [1,VertexNumber()] in the array of quads.
  //! Raises exception if array is not of type Graphic3d_TOPA_QUADRANGLES.
  //! @return the actual edges number
  Standard_Integer AddQuadEdges (Standard_Integer theVertexIndex1,
                                 Standard_Integer theVertexIndex2,
                                 Standard_Integer theVertexIndex3,
                                 Standard_Integer theVertexIndex4)
  {
    Standard_TypeMismatch_Raise_if (myType != Graphic3d_TOPA_QUADRANGLES, "Not array of quads");
    return AddEdges (theVertexIndex1, theVertexIndex2, theVertexIndex3, theVertexIndex4);
  }

  //! Convenience method, adds quad indices in the range [1,VertexNumber()] into array or triangles as two triangles.
  //! Raises exception if array is not of type Graphic3d_TOPA_TRIANGLES.
  //! @return the actual edges number
  Standard_Integer AddQuadTriangleEdges (Standard_Integer theVertexIndex1,
                                         Standard_Integer theVertexIndex2,
                                         Standard_Integer theVertexIndex3,
                                         Standard_Integer theVertexIndex4)
  {
    AddTriangleEdges (theVertexIndex3, theVertexIndex1, theVertexIndex2);
    return AddTriangleEdges (theVertexIndex1, theVertexIndex3, theVertexIndex4);
  }

  //! Convenience method, adds quad indices in the range [1,VertexNumber()] into array or triangles as two triangles.
  //! Raises exception if array is not of type Graphic3d_TOPA_TRIANGLES.
  //! @return the actual edges number
  Standard_Integer AddQuadTriangleEdges (const Graphic3d_Vec4i& theIndexes)
  {
    return AddQuadTriangleEdges (theIndexes[0], theIndexes[1], theIndexes[2], theIndexes[3]);
  }

  //! Add triangle strip into indexed triangulation array.
  //! N-2 triangles are added from N input nodes.
  //! Raises exception if array is not of type Graphic3d_TOPA_TRIANGLES.
  //! @param theVertexLower [in] index of first node defining triangle strip
  //! @param theVertexUpper [in] index of last  node defining triangle strip
  Standard_EXPORT void AddTriangleStripEdges (Standard_Integer theVertexLower,
                                              Standard_Integer theVertexUpper);

  //! Add triangle fan into indexed triangulation array.
  //! N-2 triangles are added from N input nodes (or N-1 with closed flag).
  //! Raises exception if array is not of type Graphic3d_TOPA_TRIANGLES.
  //! @param theVertexLower [in] index of first node defining triangle fun (center)
  //! @param theVertexUpper [in] index of last  node defining triangle fun
  //! @param theToClose [in] close triangle fan (connect first and last points)
  Standard_EXPORT void AddTriangleFanEdges (Standard_Integer theVertexLower,
                                            Standard_Integer theVertexUpper,
                                            Standard_Boolean theToClose);

  //! Add line strip (polyline) into indexed segments array.
  //! N-1 segments are added from N input nodes (or N with closed flag).
  //! Raises exception if array is not of type Graphic3d_TOPA_SEGMENTS.
  //! @param theVertexLower [in] index of first node defining line strip fun (center)
  //! @param theVertexUpper [in] index of last  node defining triangle fun
  //! @param theToClose [in] close triangle fan (connect first and last points)
  Standard_EXPORT void AddPolylineEdges (Standard_Integer theVertexLower,
                                         Standard_Integer theVertexUpper,
                                         Standard_Boolean theToClose);

public: //! @name optional array of Bounds/Subgroups within primitive array (e.g. restarting primitives / assigning colors)

  //! Returns optional bounds buffer.
  const Handle(Graphic3d_BoundBuffer)& Bounds() const { return myBounds; }

  //! Returns TRUE when bound colors array is defined.
  Standard_Boolean HasBoundColors() const { return !myBounds.IsNull() && myBounds->Colors != NULL; }

  //! Returns the number of defined bounds
  Standard_Integer BoundNumber() const { return !myBounds.IsNull() ? myBounds->NbBounds : -1; }

  //! Returns the number of allocated bounds
  Standard_Integer BoundNumberAllocated() const { return !myBounds.IsNull() ? myBounds->NbMaxBounds : 0; }

  //! Returns the edge number at rank theRank.
  Standard_Integer Bound (const Standard_Integer theRank) const
  {
    Standard_OutOfRange_Raise_if (myBounds.IsNull() || theRank < 1 || theRank > myBounds->NbBounds, "BAD BOUND index");
    return myBounds->Bounds[theRank - 1];
  }

  //! Returns the bound color at rank theRank from the bound table if defined.
  Quantity_Color BoundColor (const Standard_Integer theRank) const
  {
    Standard_Real anRGB[3] = {0.0, 0.0, 0.0};
    BoundColor (theRank, anRGB[0], anRGB[1], anRGB[2]);
    return Quantity_Color (anRGB[0], anRGB[1], anRGB[2], Quantity_TOC_RGB);
  }

  //! Returns the bound color values at rank theRank from the bound table if defined.
  void BoundColor (const Standard_Integer theRank, Standard_Real& theR, Standard_Real& theG, Standard_Real& theB) const
  {
    Standard_OutOfRange_Raise_if (myBounds.IsNull() || myBounds->Colors == NULL || theRank < 1 || theRank > myBounds->NbBounds, "BAD BOUND index");
    const Graphic3d_Vec4& aVec = myBounds->Colors[theRank - 1];
    theR = Standard_Real(aVec.r());
    theG = Standard_Real(aVec.g());
    theB = Standard_Real(aVec.b());
  }

  //! Adds a bound of length theEdgeNumber in the bound array
  //! @return the actual bounds number
  Standard_EXPORT Standard_Integer AddBound (const Standard_Integer theEdgeNumber);

  //! Adds a bound of length theEdgeNumber and bound color theBColor in the bound array.
  //! Warning: theBColor is ignored when the hasBColors constructor parameter is FALSE
  //! @return the actual bounds number
  Standard_Integer AddBound (const Standard_Integer theEdgeNumber, const Quantity_Color& theBColor)
  {
    return AddBound (theEdgeNumber, theBColor.Red(), theBColor.Green(), theBColor.Blue());
  }

  //! Adds a bound of length theEdgeNumber and bound color coordinates in the bound array.
  //! Warning: <theR,theG,theB> are ignored when the hasBColors constructor parameter is FALSE
  //! @return the actual bounds number
  Standard_EXPORT Standard_Integer AddBound (const Standard_Integer theEdgeNumber, const Standard_Real theR, const Standard_Real theG, const Standard_Real theB);

  //! Change the bound color of rank theIndex in the array.
  void SetBoundColor (const Standard_Integer theIndex, const Quantity_Color& theColor)
  {
    SetBoundColor (theIndex, theColor.Red(), theColor.Green(), theColor.Blue());
  }

  //! Change the bound color of rank theIndex in the array.
  void SetBoundColor (const Standard_Integer theIndex, const Standard_Real theR, const Standard_Real theG, const Standard_Real theB)
  {
    if (myBounds.IsNull())
    {
      return;
    }
    Standard_OutOfRange_Raise_if (myBounds.IsNull() || myBounds->Colors == NULL || theIndex < 1 || theIndex > myBounds->NbMaxBounds, "BAD BOUND index");
    Graphic3d_Vec4& aVec = myBounds->Colors[theIndex - 1];
    aVec.r() = Standard_ShortReal (theR);
    aVec.g() = Standard_ShortReal (theG);
    aVec.b() = Standard_ShortReal (theB);
    aVec.a() = 1.0f;
    myBounds->NbBounds = Max (theIndex, myBounds->NbBounds);
  }

protected: //! @name protected constructors

  //! Main constructor.
  //! @param theType       type of primitive
  //! @param theMaxVertexs length of vertex attributes buffer to be allocated (maximum number of vertexes, @sa ::AddVertex())
  //! @param theMaxBounds  length of bounds buffer to be allocated (maximum number of bounds, @sa ::AddBound())
  //! @param theMaxEdges   length of edges (index) buffer to be allocated (maximum number of indexes @sa ::AddEdge())
  //! @param theArrayFlags array flags
  Graphic3d_ArrayOfPrimitives (Graphic3d_TypeOfPrimitiveArray theType,
                               Standard_Integer theMaxVertexs,
                               Standard_Integer theMaxBounds,
                               Standard_Integer theMaxEdges,
                               Graphic3d_ArrayFlags theArrayFlags)
  : myNormData (NULL), myTexData (NULL), myColData (NULL), myPosStride (0), myNormStride (0), myTexStride (0), myColStride (0),
    myType (Graphic3d_TOPA_UNDEFINED)
  {
    init (theType, theMaxVertexs, theMaxBounds, theMaxEdges, theArrayFlags);
  }

  //! Array constructor.
  Standard_EXPORT void init (Graphic3d_TypeOfPrimitiveArray theType,
                             Standard_Integer theMaxVertexs,
                             Standard_Integer theMaxBounds,
                             Standard_Integer theMaxEdges,
                             Graphic3d_ArrayFlags theArrayFlags);

private: //! @name private fields

  Handle(Graphic3d_IndexBuffer)  myIndices;
  Handle(Graphic3d_Buffer)       myAttribs;
  Handle(Graphic3d_BoundBuffer)  myBounds;
  Standard_Byte* myNormData;
  Standard_Byte* myTexData;
  Standard_Byte* myColData;
  Standard_Size  myPosStride;
  Standard_Size  myNormStride;
  Standard_Size  myTexStride;
  Standard_Size  myColStride;
  Graphic3d_TypeOfPrimitiveArray myType;

};

#endif // _Graphic3d_ArrayOfPrimitives_HeaderFile
