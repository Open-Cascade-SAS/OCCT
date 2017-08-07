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
#include <Graphic3d_Buffer.hxx>
#include <Graphic3d_IndexBuffer.hxx>
#include <Graphic3d_TypeOfPrimitiveArray.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <Standard_OutOfRange.hxx>
#include <Quantity_Color.hxx>

class Quantity_Color;

class Graphic3d_ArrayOfPrimitives;
DEFINE_STANDARD_HANDLE(Graphic3d_ArrayOfPrimitives, Standard_Transient)

//! This class furnish services to defined and fill an
//! array of primitives compatible with the use of
//! the OPENGl glDrawArrays() or glDrawElements() functions.
//! NOTE that the main goal of this kind of primitive
//! is to avoid multiple copies of datas between
//! each layer of the software.
//! So the array datas exist only one time and the use
//! of SetXxxxxx() methods enable to change dynamically
//! the aspect of this primitive.
//!
//! Advantages are :
//! 1) Decrease strongly the loading time.
//! 2) Decrease strongly the display time using optimized Opengl
//! primitives.
//! 3) Enable to change dynamically the components of the primitive
//! (vertice,normal,color,texture coordinates).
//! 4) Add true triangle and quadrangle strips or fans capabilities.
class Graphic3d_ArrayOfPrimitives : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_ArrayOfPrimitives, Standard_Transient)
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
  Standard_Boolean HasVertexNormals() const { return myVNor != 0; }

  //! Returns TRUE when vertex colors array is defined.
  Standard_Boolean HasVertexColors() const { return myVCol != 0; }

  //! Returns TRUE when vertex texels array is defined.
  Standard_Boolean HasVertexTexels() const { return myVTex != 0; }

  //! Returns the number of defined vertex
  Standard_Integer VertexNumber() const { return !myAttribs.IsNull() ? myAttribs->NbElements : -1; }

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
    if (myAttribs.IsNull())
    {
      return 0;
    }
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
    if (myAttribs.IsNull())
    {
      return 0;
    }
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
    if (myAttribs.IsNull())
    {
      return 0;
    }
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
    if (myAttribs.IsNull())
    {
      return 0;
    }
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
    if (myAttribs.IsNull())
    {
      return;
    }
    else if (theIndex < 1
          || theIndex > myMaxVertexs)
    {
      throw Standard_OutOfRange ("BAD VERTEX index");
    }

    Graphic3d_Vec3& aVec = myAttribs->ChangeValue<Graphic3d_Vec3> (theIndex - 1);
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
    if (myAttribs.IsNull())
    {
      return;
    }
    else if (theIndex < 1
          || theIndex > myMaxVertexs)
    {
      throw Standard_OutOfRange ("BAD VERTEX index");
    }

    if (myVCol != 0)
    {
      Graphic3d_Vec4ub aColor (Standard_Byte(theR * 255.0),
                               Standard_Byte(theG * 255.0),
                               Standard_Byte(theB * 255.0), 255);
      SetVertexColor (theIndex, *reinterpret_cast<Standard_Integer*>(&aColor));
    }
    myAttribs->NbElements = Max (theIndex, myAttribs->NbElements);
  }

  //! Change the vertex color of rank theIndex in the array.
  void SetVertexColor (const Standard_Integer  theIndex,
                       const Graphic3d_Vec4ub& theColor)
  {
    SetVertexColor (theIndex, *reinterpret_cast<const Standard_Integer*> (&theColor));
  }

  //! Change the vertex color of rank theIndex> in the array.
  //! @code
  //!   theColor32 = Alpha << 24 + Blue << 16 + Green << 8 + Red
  //! @endcode
  void SetVertexColor (const Standard_Integer theIndex, const Standard_Integer theColor32)
  {
    if (myAttribs.IsNull())
    {
      return;
    }
    else if (theIndex < 1
          || theIndex > myMaxVertexs)
    {
      throw Standard_OutOfRange ("BAD VERTEX index");
    }

    if (myVCol != 0)
    {
      *reinterpret_cast<Standard_Integer* >(myAttribs->changeValue (theIndex - 1) + size_t(myVCol)) = theColor32;
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
    if (myAttribs.IsNull())
    {
      return;
    }
    else if (theIndex < 1
          || theIndex > myMaxVertexs)
    {
      throw Standard_OutOfRange("BAD VERTEX index");
    }

    if (myVNor != 0)
    {
      Graphic3d_Vec3& aVec = *reinterpret_cast<Graphic3d_Vec3* >(myAttribs->changeValue (theIndex - 1) + size_t(myVNor));
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
    if (myAttribs.IsNull())
    {
      return;
    }
    else if (theIndex < 1
          || theIndex > myMaxVertexs)
    {
      throw Standard_OutOfRange("BAD VERTEX index");
    }

    if (myVTex != 0)
    {
      Graphic3d_Vec2& aVec = *reinterpret_cast<Graphic3d_Vec2* >(myAttribs->changeValue (theIndex - 1) + size_t(myVTex));
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
    if (myAttribs.IsNull())
    {
      return;
    }
    else if (theRank < 1
          || theRank > myAttribs->NbElements)
    {
      throw Standard_OutOfRange ("BAD VERTEX index");
    }

    const Graphic3d_Vec3& aVec = myAttribs->Value<Graphic3d_Vec3> (theRank - 1);
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
    if (myAttribs.IsNull()
     || myVCol == 0)
    {
      throw Standard_OutOfRange ("Primitive array does not define color attribute");
    }

    if (theIndex < 1
     || theIndex > myAttribs->NbElements)
    {
      throw Standard_OutOfRange ("BAD VERTEX index");
    }

    theColor = *reinterpret_cast<const Graphic3d_Vec4ub* >(myAttribs->value (theIndex - 1) + size_t(myVCol));
  }

  //! Returns the vertex color values at rank theRank from the vertex table if defined.
  void VertexColor (const Standard_Integer theRank, Standard_Real& theR, Standard_Real& theG, Standard_Real& theB) const
  {
    theR = theG = theB = 0.0;
    if (myAttribs.IsNull()
     || myVCol == 0)
    {
      return;
    }
    else if (theRank < 1
          || theRank > myAttribs->NbElements)
    {
      throw Standard_OutOfRange ("BAD VERTEX index");
    }

    const Graphic3d_Vec4ub& aColor = *reinterpret_cast<const Graphic3d_Vec4ub* >(myAttribs->value (theRank - 1) + size_t(myVCol));
    theR = Standard_Real(aColor.r()) / 255.0;
    theG = Standard_Real(aColor.g()) / 255.0;
    theB = Standard_Real(aColor.b()) / 255.0;
  }

  //! Returns the vertex color values at rank theRank from the vertex table if defined.
  void VertexColor (const Standard_Integer theRank, Standard_Integer& theColor) const
  {
    if (myVCol != 0)
    {
      theColor = *reinterpret_cast<const Standard_Integer* >(myAttribs->value (theRank - 1) + size_t(myVCol));
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
    if (myAttribs.IsNull())
    {
      return;
    }
    else if (theRank < 1
          || theRank > myAttribs->NbElements)
    {
      throw Standard_OutOfRange ("BAD VERTEX index");
    }

    if (myVNor != 0)
    {
      const Graphic3d_Vec3& aVec = *reinterpret_cast<const Graphic3d_Vec3* >(myAttribs->value (theRank - 1) + size_t(myVNor));
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
    if (myAttribs.IsNull())
    {
      return;
    }
    else if (theRank < 1
          || theRank > myAttribs->NbElements)
    {
      throw Standard_OutOfRange ("BAD VERTEX index");
    }

    if (myVTex != 0)
    {
      const Graphic3d_Vec2& aVec = *reinterpret_cast<const Graphic3d_Vec2* >(myAttribs->value (theRank - 1) + size_t(myVTex));
      theTX = Standard_Real(aVec.x());
      theTY = Standard_Real(aVec.y());
    }
  }

public: //! @name optional array of Indices/Edges for using shared Vertex data

  //! Returns optional index buffer.
  const Handle(Graphic3d_IndexBuffer)& Indices() const { return myIndices; }

  //! Returns the number of defined edges
  Standard_Integer EdgeNumber() const { return !myIndices.IsNull() ? myIndices->NbElements : -1; }

  //! Returns the vertex index at rank theRank in the range [1,EdgeNumber()]
  Standard_Integer Edge (const Standard_Integer theRank) const
  {
    if (myIndices.IsNull()
     || theRank <= 0
     || theRank > myIndices->NbElements)
    {
      throw Standard_OutOfRange ("BAD EDGE index");
    }
    return Standard_Integer(myIndices->Index (theRank - 1) + 1);
  }

  //! Adds an edge in the range [1,VertexNumber()] in the array.
  //! @return the actual edges number
  Standard_EXPORT Standard_Integer AddEdge (const Standard_Integer theVertexIndex);

public: //! @name optional array of Bounds/Subgroups within primitive array (e.g. restarting primitives / assigning colors)

  //! Returns optional bounds buffer.
  const Handle(Graphic3d_BoundBuffer)& Bounds() const { return myBounds; }

  //! Returns TRUE when bound colors array is defined.
  Standard_Boolean HasBoundColors() const { return !myBounds.IsNull() && myBounds->Colors != NULL; }

  //! Returns the number of defined bounds
  Standard_Integer BoundNumber() const { return !myBounds.IsNull() ? myBounds->NbBounds : -1; }

  //! Returns the edge number at rank theRank.
  Standard_Integer Bound (const Standard_Integer theRank) const
  {
    if (myBounds.IsNull()
     || theRank <= 0
     || theRank > myBounds->NbBounds)
    {
      throw Standard_OutOfRange ("BAD BOUND index");
    }
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
    if (myBounds.IsNull()
     || myBounds->Colors == NULL
     || theRank <= 0
     || theRank > myBounds->NbBounds)
    {
      throw Standard_OutOfRange (" BAD BOUND index");
    }

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
    else if (theIndex < 1
          || theIndex > myMaxBounds)
    {
      throw Standard_OutOfRange("BAD BOUND index");
    }

    Graphic3d_Vec4& aVec = myBounds->Colors[theIndex - 1];
    aVec.r() = Standard_ShortReal (theR);
    aVec.g() = Standard_ShortReal (theG);
    aVec.b() = Standard_ShortReal (theB);
    aVec.a() = 1.0f;
    myBounds->NbBounds = Max (theIndex, myBounds->NbBounds);
  }

protected: //! @name protected constructors

  //! Warning
  //! You must use a coherent set of AddVertex() methods according to the theHasVNormals,theHasVColors,theHasVTexels,theHasBColors.
  //! User is responsible of confuse vertex and bad normal orientation.
  //! You must use AddBound() method only if the theMaxBounds constructor parameter is > 0.
  //! You must use AddEdge()  method only if the theMaxEdges  constructor parameter is > 0.
  Standard_EXPORT Graphic3d_ArrayOfPrimitives (const Graphic3d_TypeOfPrimitiveArray theType,
                                               const Standard_Integer theMaxVertexs,
                                               const Standard_Integer theMaxBounds,
                                               const Standard_Integer theMaxEdges,
                                               const Standard_Boolean theHasVNormals,
                                               const Standard_Boolean theHasVColors,
                                               const Standard_Boolean theHasBColors,
                                               const Standard_Boolean theHasVTexels);

private: //! @name private fields

  Handle(Graphic3d_IndexBuffer)  myIndices;
  Handle(Graphic3d_Buffer)       myAttribs;
  Handle(Graphic3d_BoundBuffer)  myBounds;
  Graphic3d_TypeOfPrimitiveArray myType;
  Standard_Integer myMaxBounds;
  Standard_Integer myMaxVertexs;
  Standard_Integer myMaxEdges;
  Standard_Byte myVNor;
  Standard_Byte myVTex;
  Standard_Byte myVCol;

};

#endif // _Graphic3d_ArrayOfPrimitives_HeaderFile
