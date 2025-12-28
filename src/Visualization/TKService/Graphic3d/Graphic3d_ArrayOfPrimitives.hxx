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

//! This class furnish services to defined and fill an array of primitives
//! which can be passed directly to graphics rendering API.
//!
//! The basic interface consists of the following parts:
//! 1) Specifying primitive type.
//!    WARNING! Particular primitive types might be unsupported by specific hardware/graphics API
//!    (like quads and polygons).
//!             It is always preferred using one of basic types having maximum compatibility:
//!             Point, Triangle (or Triangle strip), Segment aka Lines (or Polyline aka Line Strip).
//!    Primitive strip types can be used to reduce memory usage as alternative to Indexed arrays.
//! 2) Vertex array.
//!    - Specifying the (maximum) number of vertexes within array.
//!    - Specifying the vertex attributes, complementary to mandatory vertex Position (normal,
//!    color, UV texture coordinates).
//!    - Defining vertex values by using various versions of AddVertex() or SetVertex*() methods.
//! 3) Index array (optional).
//!    - Specifying the (maximum) number of indexes (edges).
//!    - Defining index values by using AddEdge() method; the index value should be within number of
//!    defined Vertexes.
//!
//!    Indexed array allows sharing vertex data across Primitives and thus reducing memory usage,
//!    since index size is much smaller then size of vertex with all its attributes.
//!    It is a preferred way for defining primitive array and main alternative to Primitive Strips
//!    for optimal memory usage, although it is also possible (but unusual) defining Indexed
//!    Primitive Strip. Note that it is NOT possible sharing Vertex Attributes partially (e.g. share
//!    Position, but have different Normals); in such cases Vertex should be entirely duplicated
//!    with all Attributes.
//! 4) Bounds array (optional).
//!    - Specifying the (maximum) number of bounds.
//!    - Defining bounds using AddBound() methods.
//!
//!    Bounds allow splitting Primitive Array into sub-groups.
//!    This is useful only in two cases - for specifying per-group color and for restarting
//!    Primitive Strips. WARNING! Bounds within Primitive Array break rendering batches into parts
//!    (additional for loops),
//!             affecting rendering performance negatively (increasing CPU load).
class Graphic3d_ArrayOfPrimitives : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_ArrayOfPrimitives, Standard_Transient)
public:
  //! Create an array of specified type.
  static occ::handle<Graphic3d_ArrayOfPrimitives> CreateArray(
    Graphic3d_TypeOfPrimitiveArray theType,
    int                            theMaxVertexs,
    int                            theMaxEdges,
    Graphic3d_ArrayFlags           theArrayFlags)
  {
    return CreateArray(theType, theMaxVertexs, 0, theMaxEdges, theArrayFlags);
  }

  //! Create an array of specified type.
  static Standard_EXPORT occ::handle<Graphic3d_ArrayOfPrimitives> CreateArray(
    Graphic3d_TypeOfPrimitiveArray theType,
    int                            theMaxVertexs,
    int                            theMaxBounds,
    int                            theMaxEdges,
    Graphic3d_ArrayFlags           theArrayFlags);

public:
  //! Destructor.
  Standard_EXPORT virtual ~Graphic3d_ArrayOfPrimitives();

  //! Returns vertex attributes buffer (colors, normals, texture coordinates).
  const occ::handle<Graphic3d_Buffer>& Attributes() const { return myAttribs; }

  //! Returns the type of this primitive
  Graphic3d_TypeOfPrimitiveArray Type() const { return myType; }

  //! Returns the string type of this primitive
  Standard_EXPORT const char* StringType() const;

  //! Returns TRUE when vertex normals array is defined.
  bool HasVertexNormals() const { return myNormData != NULL; }

  //! Returns TRUE when vertex colors array is defined.
  bool HasVertexColors() const { return myColData != NULL; }

  //! Returns TRUE when vertex texels array is defined.
  bool HasVertexTexels() const { return myTexData != NULL; }

  //! Returns the number of defined vertex
  int VertexNumber() const { return myAttribs->NbElements; }

  //! Returns the number of allocated vertex
  int VertexNumberAllocated() const { return myAttribs->NbMaxElements(); }

  //! Returns the number of total items according to the array type.
  Standard_EXPORT int ItemNumber() const;

  //! Returns TRUE only when the contains of this array is available.
  Standard_EXPORT bool IsValid();

  //! Adds a vertice in the array.
  //! @return the actual vertex number
  int AddVertex(const gp_Pnt& theVertex)
  {
    return AddVertex(theVertex.X(), theVertex.Y(), theVertex.Z());
  }

  //! Adds a vertice in the array.
  //! @return the actual vertex number
  int AddVertex(const NCollection_Vec3<float>& theVertex)
  {
    return AddVertex(theVertex.x(), theVertex.y(), theVertex.z());
  }

  //! Adds a vertice in the array.
  //! @return the actual vertex number
  int AddVertex(const double theX, const double theY, const double theZ)
  {
    return AddVertex(RealToShortReal(theX), RealToShortReal(theY), RealToShortReal(theZ));
  }

  //! Adds a vertice in the array.
  //! @return the actual vertex number.
  int AddVertex(const float theX, const float theY, const float theZ)
  {
    const int anIndex = myAttribs->NbElements + 1;
    SetVertice(anIndex, theX, theY, theZ);
    return anIndex;
  }

  //! Adds a vertice and vertex color in the vertex array.
  //! Warning: theColor is ignored when the hasVColors constructor parameter is FALSE
  //! @return the actual vertex number
  int AddVertex(const gp_Pnt& theVertex, const Quantity_Color& theColor)
  {
    const int anIndex = AddVertex(theVertex);
    SetVertexColor(anIndex, theColor.Red(), theColor.Green(), theColor.Blue());
    return anIndex;
  }

  //! Adds a vertice and vertex color in the vertex array.
  //! Warning: theColor is ignored when the hasVColors constructor parameter is FALSE
  //! @code
  //!   theColor32 = Alpha << 24 + Blue << 16 + Green << 8 + Red
  //! @endcode
  //! @return the actual vertex number
  int AddVertex(const gp_Pnt& theVertex, const int theColor32)
  {
    const int anIndex = AddVertex(theVertex);
    SetVertexColor(anIndex, theColor32);
    return anIndex;
  }

  //! Adds a vertice and vertex color in the vertex array.
  //! Warning: theColor is ignored when the hasVColors constructor parameter is FALSE
  //! @return the actual vertex number
  int AddVertex(const gp_Pnt& theVertex, const NCollection_Vec4<uint8_t>& theColor)
  {
    const int anIndex = AddVertex(theVertex);
    SetVertexColor(anIndex, theColor);
    return anIndex;
  }

  //! Adds a vertice and vertex normal in the vertex array.
  //! Warning: theNormal is ignored when the hasVNormals constructor parameter is FALSE.
  //! @return the actual vertex number
  int AddVertex(const gp_Pnt& theVertex, const gp_Dir& theNormal)
  {
    return AddVertex(theVertex.X(),
                     theVertex.Y(),
                     theVertex.Z(),
                     theNormal.X(),
                     theNormal.Y(),
                     theNormal.Z());
  }

  //! Adds a vertice and vertex normal in the vertex array.
  //! Warning: Normal is ignored when the hasVNormals constructor parameter is FALSE.
  //! @return the actual vertex number
  int AddVertex(const double theX,
                const double theY,
                const double theZ,
                const double theNX,
                const double theNY,
                const double theNZ)
  {
    return AddVertex(RealToShortReal(theX),
                     RealToShortReal(theY),
                     RealToShortReal(theZ),
                     float(theNX),
                     float(theNY),
                     float(theNZ));
  }

  //! Adds a vertice and vertex normal in the vertex array.
  //! Warning: Normal is ignored when the hasVNormals constructor parameter is FALSE.
  //! @return the actual vertex number
  int AddVertex(const float theX,
                const float theY,
                const float theZ,
                const float theNX,
                const float theNY,
                const float theNZ)
  {
    const int anIndex = myAttribs->NbElements + 1;
    SetVertice(anIndex, theX, theY, theZ);
    SetVertexNormal(anIndex, theNX, theNY, theNZ);
    return anIndex;
  }

  //! Adds a vertice,vertex normal and color in the vertex array.
  //! Warning: theNormal is ignored when the hasVNormals constructor parameter is FALSE
  //! and      theColor  is ignored when the hasVColors  constructor parameter is FALSE.
  //! @return the actual vertex number
  int AddVertex(const gp_Pnt& theVertex, const gp_Dir& theNormal, const Quantity_Color& theColor)
  {
    const int anIndex = AddVertex(theVertex, theNormal);
    SetVertexColor(anIndex, theColor.Red(), theColor.Green(), theColor.Blue());
    return anIndex;
  }

  //! Adds a vertice,vertex normal and color in the vertex array.
  //! Warning: theNormal is ignored when the hasVNormals constructor parameter is FALSE
  //! and      theColor  is ignored when the hasVColors  constructor parameter is FALSE.
  //! @code
  //!   theColor32 = Alpha << 24 + Blue << 16 + Green << 8 + Red
  //! @endcode
  //! @return the actual vertex number
  int AddVertex(const gp_Pnt& theVertex, const gp_Dir& theNormal, const int theColor32)
  {
    const int anIndex = AddVertex(theVertex, theNormal);
    SetVertexColor(anIndex, theColor32);
    return anIndex;
  }

  //! Adds a vertice and vertex texture in the vertex array.
  //! theTexel is ignored when the hasVTexels constructor parameter is FALSE.
  //! @return the actual vertex number
  int AddVertex(const gp_Pnt& theVertex, const gp_Pnt2d& theTexel)
  {
    return AddVertex(theVertex.X(), theVertex.Y(), theVertex.Z(), theTexel.X(), theTexel.Y());
  }

  //! Adds a vertice and vertex texture coordinates in the vertex array.
  //! Texel is ignored when the hasVTexels constructor parameter is FALSE.
  //! @return the actual vertex number
  int AddVertex(const double theX,
                const double theY,
                const double theZ,
                const double theTX,
                const double theTY)
  {
    return AddVertex(RealToShortReal(theX),
                     RealToShortReal(theY),
                     RealToShortReal(theZ),
                     float(theTX),
                     float(theTY));
  }

  //! Adds a vertice and vertex texture coordinates in the vertex array.
  //! Texel is ignored when the hasVTexels constructor parameter is FALSE.
  //! @return the actual vertex number
  int AddVertex(const float theX,
                const float theY,
                const float theZ,
                const float theTX,
                const float theTY)
  {
    const int anIndex = myAttribs->NbElements + 1;
    SetVertice(anIndex, theX, theY, theZ);
    SetVertexTexel(anIndex, theTX, theTY);
    return anIndex;
  }

  //! Adds a vertice,vertex normal and texture in the vertex array.
  //! Warning: theNormal is ignored when the hasVNormals constructor parameter is FALSE
  //! and      theTexel  is ignored when the hasVTexels  constructor parameter is FALSE.
  //! @return the actual vertex number
  int AddVertex(const gp_Pnt& theVertex, const gp_Dir& theNormal, const gp_Pnt2d& theTexel)
  {
    return AddVertex(theVertex.X(),
                     theVertex.Y(),
                     theVertex.Z(),
                     theNormal.X(),
                     theNormal.Y(),
                     theNormal.Z(),
                     theTexel.X(),
                     theTexel.Y());
  }

  //! Adds a vertice,vertex normal and texture in the vertex array.
  //! Warning: Normal is ignored when the hasVNormals constructor parameter is FALSE
  //! and      Texel  is ignored when the hasVTexels  constructor parameter is FALSE.
  //! @return the actual vertex number
  int AddVertex(const double theX,
                const double theY,
                const double theZ,
                const double theNX,
                const double theNY,
                const double theNZ,
                const double theTX,
                const double theTY)
  {
    return AddVertex(RealToShortReal(theX),
                     RealToShortReal(theY),
                     RealToShortReal(theZ),
                     float(theNX),
                     float(theNY),
                     float(theNZ),
                     float(theTX),
                     float(theTY));
  }

  //! Adds a vertice,vertex normal and texture in the vertex array.
  //! Warning: Normal is ignored when the hasVNormals constructor parameter is FALSE
  //!     and  Texel  is ignored when the hasVTexels  constructor parameter is FALSE.
  //! @return the actual vertex number
  int AddVertex(const float theX,
                const float theY,
                const float theZ,
                const float theNX,
                const float theNY,
                const float theNZ,
                const float theTX,
                const float theTY)
  {
    const int anIndex = myAttribs->NbElements + 1;
    SetVertice(anIndex, theX, theY, theZ);
    SetVertexNormal(anIndex, theNX, theNY, theNZ);
    SetVertexTexel(anIndex, theTX, theTY);
    return anIndex;
  }

  //! Change the vertice of rank theIndex in the array.
  //! @param[in] theIndex  node index within [1, VertexNumberAllocated()] range
  //! @param[in] theVertex 3D coordinates
  void SetVertice(const int theIndex, const gp_Pnt& theVertex)
  {
    SetVertice(theIndex, float(theVertex.X()), float(theVertex.Y()), float(theVertex.Z()));
  }

  //! Change the vertice in the array.
  //! @param[in] theIndex node index within [1, VertexNumberAllocated()] range
  //! @param[in] theX coordinate X
  //! @param[in] theY coordinate Y
  //! @param[in] theZ coordinate Z
  void SetVertice(const int theIndex, const float theX, const float theY, const float theZ)
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > myAttribs->NbMaxElements(),
                                 "BAD VERTEX index");
    NCollection_Vec3<float>& aVec = *reinterpret_cast<NCollection_Vec3<float>*>(
      myAttribs->ChangeData() + myPosStride * ((size_t)theIndex - 1));
    aVec.x() = theX;
    aVec.y() = theY;
    aVec.z() = theZ;
    if (myAttribs->NbElements < theIndex)
    {
      myAttribs->NbElements = theIndex;
    }
  }

  //! Change the vertex color in the array.
  //! @param[in] theIndex node index within [1, VertexNumberAllocated()] range
  //! @param[in] theColor node color
  void SetVertexColor(const int theIndex, const Quantity_Color& theColor)
  {
    SetVertexColor(theIndex, theColor.Red(), theColor.Green(), theColor.Blue());
  }

  //! Change the vertex color in the array.
  //! @param[in] theIndex node index within [1, VertexNumberAllocated()] range
  //! @param[in] theR red   color value within [0, 1] range
  //! @param[in] theG green color value within [0, 1] range
  //! @param[in] theB blue  color value within [0, 1] range
  void SetVertexColor(const int theIndex, const double theR, const double theG, const double theB)
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > myAttribs->NbMaxElements(),
                                 "BAD VERTEX index");
    if (myColData != NULL)
    {
      NCollection_Vec4<uint8_t>* aColorPtr = reinterpret_cast<NCollection_Vec4<uint8_t>*>(
        myColData + myColStride * ((size_t)theIndex - 1));
      aColorPtr->SetValues(static_cast<uint8_t>(theR * 255.0),
                           static_cast<uint8_t>(theG * 255.0),
                           static_cast<uint8_t>(theB * 255.0),
                           255);
    }
    myAttribs->NbElements = (std::max)(theIndex, myAttribs->NbElements);
  }

  //! Change the vertex color in the array.
  //! @param[in] theIndex node index within [1, VertexNumberAllocated()] range
  //! @param[in] theColor node RGBA color values within [0, 255] range
  void SetVertexColor(const int theIndex, const NCollection_Vec4<uint8_t>& theColor)
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > myAttribs->NbMaxElements(),
                                 "BAD VERTEX index");
    if (myColData != NULL)
    {
      NCollection_Vec4<uint8_t>* aColorPtr = reinterpret_cast<NCollection_Vec4<uint8_t>*>(
        myColData + myColStride * ((size_t)theIndex - 1));
      (*aColorPtr) = theColor;
    }
    myAttribs->NbElements = (std::max)(theIndex, myAttribs->NbElements);
  }

  //! Change the vertex color in the array.
  //! @code
  //!   theColor32 = Alpha << 24 + Blue << 16 + Green << 8 + Red
  //! @endcode
  //! @param[in] theIndex   node index within [1, VertexNumberAllocated()] range
  //! @param[in] theColor32 packed RGBA color values
  void SetVertexColor(const int theIndex, const int theColor32)
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > myAttribs->NbMaxElements(),
                                 "BAD VERTEX index");
    if (myColData != NULL)
    {
      *reinterpret_cast<int*>(myColData + myColStride * ((size_t)theIndex - 1)) = theColor32;
    }
  }

  //! Change the vertex normal in the array.
  //! @param[in] theIndex  node index within [1, VertexNumberAllocated()] range
  //! @param[in] theNormal normalized surface normal
  void SetVertexNormal(const int theIndex, const gp_Dir& theNormal)
  {
    SetVertexNormal(theIndex, theNormal.X(), theNormal.Y(), theNormal.Z());
  }

  //! Change the vertex normal in the array.
  //! @param[in] theIndex node index within [1, VertexNumberAllocated()] range
  //! @param[in] theNX surface normal X component
  //! @param[in] theNY surface normal Y component
  //! @param[in] theNZ surface normal Z component
  void SetVertexNormal(const int    theIndex,
                       const double theNX,
                       const double theNY,
                       const double theNZ)
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > myAttribs->NbMaxElements(),
                                 "BAD VERTEX index");
    if (myNormData != NULL)
    {
      NCollection_Vec3<float>& aVec = *reinterpret_cast<NCollection_Vec3<float>*>(
        myNormData + myNormStride * ((size_t)theIndex - 1));
      aVec.x() = float(theNX);
      aVec.y() = float(theNY);
      aVec.z() = float(theNZ);
    }
    myAttribs->NbElements = (std::max)(theIndex, myAttribs->NbElements);
  }

  //! Change the vertex texel in the array.
  //! @param[in] theIndex node index within [1, VertexNumberAllocated()] range
  //! @param[in] theTexel node UV coordinates
  void SetVertexTexel(const int theIndex, const gp_Pnt2d& theTexel)
  {
    SetVertexTexel(theIndex, theTexel.X(), theTexel.Y());
  }

  //! Change the vertex texel in the array.
  //! @param[in] theIndex node index within [1, VertexNumberAllocated()] range
  //! @param[in] theTX node U coordinate
  //! @param[in] theTY node V coordinate
  void SetVertexTexel(const int theIndex, const double theTX, const double theTY)
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > myAttribs->NbMaxElements(),
                                 "BAD VERTEX index");
    if (myTexData != NULL)
    {
      NCollection_Vec2<float>& aVec = *reinterpret_cast<NCollection_Vec2<float>*>(
        myTexData + myTexStride * ((size_t)theIndex - 1));
      aVec.x() = float(theTX);
      aVec.y() = float(theTY);
    }
    myAttribs->NbElements = (std::max)(theIndex, myAttribs->NbElements);
  }

  //! Returns the vertice from the vertex table if defined.
  //! @param[in] theRank node index within [1, VertexNumber()] range
  //! @return node 3D coordinates
  gp_Pnt Vertice(const int theRank) const
  {
    double anXYZ[3];
    Vertice(theRank, anXYZ[0], anXYZ[1], anXYZ[2]);
    return gp_Pnt(anXYZ[0], anXYZ[1], anXYZ[2]);
  }

  //! Returns the vertice coordinates at rank theRank from the vertex table if defined.
  //! @param[in]  theRank node index within [1, VertexNumber()] range
  //! @param[out] theX node X coordinate value
  //! @param[out] theY node Y coordinate value
  //! @param[out] theZ node Z coordinate value
  void Vertice(const int theRank, double& theX, double& theY, double& theZ) const
  {
    theX = theY = theZ = 0.0;
    Standard_OutOfRange_Raise_if(theRank < 1 || theRank > myAttribs->NbElements,
                                 "BAD VERTEX index");
    const NCollection_Vec3<float>& aVec = *reinterpret_cast<const NCollection_Vec3<float>*>(
      myAttribs->Data() + myPosStride * ((size_t)theRank - 1));
    theX = double(aVec.x());
    theY = double(aVec.y());
    theZ = double(aVec.z());
  }

  //! Returns the vertex color at rank theRank from the vertex table if defined.
  //! @param[in] theRank node index within [1, VertexNumber()] range
  //! @return node color RGB value
  Quantity_Color VertexColor(const int theRank) const
  {
    double anRGB[3];
    VertexColor(theRank, anRGB[0], anRGB[1], anRGB[2]);
    return Quantity_Color(anRGB[0], anRGB[1], anRGB[2], Quantity_TOC_RGB);
  }

  //! Returns the vertex color from the vertex table if defined.
  //! @param[in]  theIndex node index within [1, VertexNumber()] range
  //! @param[out] theColor node RGBA color values within [0, 255] range
  void VertexColor(const int theIndex, NCollection_Vec4<uint8_t>& theColor) const
  {
    Standard_OutOfRange_Raise_if(myColData == NULL || theIndex < 1
                                   || theIndex > myAttribs->NbElements,
                                 "BAD VERTEX index");
    theColor = *reinterpret_cast<const NCollection_Vec4<uint8_t>*>(
      myColData + myColStride * ((size_t)theIndex - 1));
  }

  //! Returns the vertex color values from the vertex table if defined.
  //! @param[in]  theRank node index within [1, VertexNumber()] range
  //! @param[out] theR node red   color component value within [0, 1] range
  //! @param[out] theG node green color component value within [0, 1] range
  //! @param[out] theB node blue  color component value within [0, 1] range
  void VertexColor(const int theRank, double& theR, double& theG, double& theB) const
  {
    theR = theG = theB = 0.0;
    Standard_OutOfRange_Raise_if(theRank < 1 || theRank > myAttribs->NbElements,
                                 "BAD VERTEX index");
    if (myColData == NULL)
    {
      return;
    }
    const NCollection_Vec4<uint8_t>& aColor = *reinterpret_cast<const NCollection_Vec4<uint8_t>*>(
      myColData + myColStride * ((size_t)theRank - 1));
    theR = double(aColor.r()) / 255.0;
    theG = double(aColor.g()) / 255.0;
    theB = double(aColor.b()) / 255.0;
  }

  //! Returns the vertex color values from the vertex table if defined.
  //! @param[in]  theRank  node index within [1, VertexNumber()] range
  //! @param[out] theColor node RGBA color packed into 32-bit integer
  void VertexColor(const int theRank, int& theColor) const
  {
    Standard_OutOfRange_Raise_if(theRank < 1 || theRank > myAttribs->NbElements,
                                 "BAD VERTEX index");
    if (myColData != NULL)
    {
      theColor = *reinterpret_cast<const int*>(myColData + myColStride * ((size_t)theRank - 1));
    }
  }

  //! Returns the vertex normal from the vertex table if defined.
  //! @param[in] theRank node index within [1, VertexNumber()] range
  //! @return normalized 3D vector defining surface normal
  gp_Dir VertexNormal(const int theRank) const
  {
    double anXYZ[3];
    VertexNormal(theRank, anXYZ[0], anXYZ[1], anXYZ[2]);
    return gp_Dir(anXYZ[0], anXYZ[1], anXYZ[2]);
  }

  //! Returns the vertex normal coordinates at rank theRank from the vertex table if defined.
  //! @param[in]  theRank node index within [1, VertexNumber()] range
  //! @param[out] theNX   normal X coordinate
  //! @param[out] theNY   normal Y coordinate
  //! @param[out] theNZ   normal Z coordinate
  void VertexNormal(const int theRank, double& theNX, double& theNY, double& theNZ) const
  {
    theNX = theNY = theNZ = 0.0;
    Standard_OutOfRange_Raise_if(theRank < 1 || theRank > myAttribs->NbElements,
                                 "BAD VERTEX index");
    if (myNormData != NULL)
    {
      const NCollection_Vec3<float>& aVec = *reinterpret_cast<const NCollection_Vec3<float>*>(
        myNormData + myNormStride * ((size_t)theRank - 1));
      theNX = double(aVec.x());
      theNY = double(aVec.y());
      theNZ = double(aVec.z());
    }
  }

  //! Returns the vertex texture at rank theRank from the vertex table if defined.
  //! @param[in] theRank node index within [1, VertexNumber()] range
  //! @return UV coordinates
  gp_Pnt2d VertexTexel(const int theRank) const
  {
    double anXY[2];
    VertexTexel(theRank, anXY[0], anXY[1]);
    return gp_Pnt2d(anXY[0], anXY[1]);
  }

  //! Returns the vertex texture coordinates at rank theRank from the vertex table if defined.
  //! @param[in]  theRank node index within [1, VertexNumber()] range
  //! @param[out] theTX texel U coordinate value
  //! @param[out] theTY texel V coordinate value
  void VertexTexel(const int theRank, double& theTX, double& theTY) const
  {
    theTX = theTY = 0.0;
    Standard_OutOfRange_Raise_if(theRank < 1 || theRank > myAttribs->NbElements,
                                 "BAD VERTEX index");
    if (myTexData != NULL)
    {
      const NCollection_Vec2<float>& aVec = *reinterpret_cast<const NCollection_Vec2<float>*>(
        myTexData + myTexStride * ((size_t)theRank - 1));
      theTX = double(aVec.x());
      theTY = double(aVec.y());
    }
  }

public: //! @name optional array of Indices/Edges for using shared Vertex data
  //! Returns optional index buffer.
  const occ::handle<Graphic3d_IndexBuffer>& Indices() const { return myIndices; }

  //! Returns the number of defined edges
  int EdgeNumber() const { return !myIndices.IsNull() ? myIndices->NbElements : -1; }

  //! Returns the number of allocated edges
  int EdgeNumberAllocated() const { return !myIndices.IsNull() ? myIndices->NbMaxElements() : 0; }

  //! Returns the vertex index at rank theRank in the range [1,EdgeNumber()]
  int Edge(const int theRank) const
  {
    Standard_OutOfRange_Raise_if(myIndices.IsNull() || theRank < 1
                                   || theRank > myIndices->NbElements,
                                 "BAD EDGE index");
    return int(myIndices->Index(theRank - 1) + 1);
  }

  //! Adds an edge in the range [1,VertexNumber()] in the array.
  //! @return the actual edges number
  Standard_EXPORT int AddEdge(const int theVertexIndex);

  //! Convenience method, adds two vertex indices (a segment) in the range [1,VertexNumber()] in the
  //! array.
  //! @return the actual edges number
  int AddEdges(int theVertexIndex1, int theVertexIndex2)
  {
    AddEdge(theVertexIndex1);
    return AddEdge(theVertexIndex2);
  }

  //! Convenience method, adds two vertex indices (a segment) in the range [1,VertexNumber()] in the
  //! array of segments (Graphic3d_TOPA_SEGMENTS). Raises exception if array is not of type
  //! Graphic3d_TOPA_SEGMENTS.
  //! @return the actual edges number
  int AddSegmentEdges(int theVertexIndex1, int theVertexIndex2)
  {
    Standard_TypeMismatch_Raise_if(myType != Graphic3d_TOPA_SEGMENTS, "Not array of segments");
    return AddEdges(theVertexIndex1, theVertexIndex2);
  }

  //! Convenience method, adds three vertex indices (a triangle) in the range [1,VertexNumber()] in
  //! the array.
  //! @return the actual edges number
  int AddEdges(int theVertexIndex1, int theVertexIndex2, int theVertexIndex3)
  {
    AddEdge(theVertexIndex1);
    AddEdge(theVertexIndex2);
    return AddEdge(theVertexIndex3);
  }

  //! Convenience method, adds three vertex indices of triangle in the range [1,VertexNumber()] in
  //! the array of triangles. Raises exception if array is not of type Graphic3d_TOPA_TRIANGLES.
  //! @return the actual edges number
  int AddTriangleEdges(int theVertexIndex1, int theVertexIndex2, int theVertexIndex3)
  {
    Standard_TypeMismatch_Raise_if(myType != Graphic3d_TOPA_TRIANGLES, "Not array of triangles");
    return AddEdges(theVertexIndex1, theVertexIndex2, theVertexIndex3);
  }

  //! Convenience method, adds three vertex indices of triangle in the range [1,VertexNumber()] in
  //! the array of triangles. Raises exception if array is not of type Graphic3d_TOPA_TRIANGLES.
  //! @return the actual edges number
  int AddTriangleEdges(const NCollection_Vec3<int>& theIndexes)
  {
    Standard_TypeMismatch_Raise_if(myType != Graphic3d_TOPA_TRIANGLES, "Not array of triangles");
    return AddEdges(theIndexes[0], theIndexes[1], theIndexes[2]);
  }

  //! Convenience method, adds three vertex indices (4th component is ignored) of triangle in the
  //! range [1,VertexNumber()] in the array of triangles. Raises exception if array is not of type
  //! Graphic3d_TOPA_TRIANGLES.
  //! @return the actual edges number
  int AddTriangleEdges(const NCollection_Vec4<int>& theIndexes)
  {
    Standard_TypeMismatch_Raise_if(myType != Graphic3d_TOPA_TRIANGLES, "Not array of triangles");
    return AddEdges(theIndexes[0], theIndexes[1], theIndexes[2]);
  }

  //! Convenience method, adds four vertex indices (a quad) in the range [1,VertexNumber()] in the
  //! array.
  //! @return the actual edges number
  int AddEdges(int theVertexIndex1, int theVertexIndex2, int theVertexIndex3, int theVertexIndex4)
  {
    AddEdge(theVertexIndex1);
    AddEdge(theVertexIndex2);
    AddEdge(theVertexIndex3);
    return AddEdge(theVertexIndex4);
  }

  //! Convenience method, adds four vertex indices (a quad) in the range [1,VertexNumber()] in the
  //! array of quads. Raises exception if array is not of type Graphic3d_TOPA_QUADRANGLES.
  //! @return the actual edges number
  int AddQuadEdges(int theVertexIndex1,
                   int theVertexIndex2,
                   int theVertexIndex3,
                   int theVertexIndex4)
  {
    Standard_TypeMismatch_Raise_if(myType != Graphic3d_TOPA_QUADRANGLES, "Not array of quads");
    return AddEdges(theVertexIndex1, theVertexIndex2, theVertexIndex3, theVertexIndex4);
  }

  //! Convenience method, adds quad indices in the range [1,VertexNumber()] into array or triangles
  //! as two triangles. Raises exception if array is not of type Graphic3d_TOPA_TRIANGLES.
  //! @return the actual edges number
  int AddQuadTriangleEdges(int theVertexIndex1,
                           int theVertexIndex2,
                           int theVertexIndex3,
                           int theVertexIndex4)
  {
    AddTriangleEdges(theVertexIndex3, theVertexIndex1, theVertexIndex2);
    return AddTriangleEdges(theVertexIndex1, theVertexIndex3, theVertexIndex4);
  }

  //! Convenience method, adds quad indices in the range [1,VertexNumber()] into array or triangles
  //! as two triangles. Raises exception if array is not of type Graphic3d_TOPA_TRIANGLES.
  //! @return the actual edges number
  int AddQuadTriangleEdges(const NCollection_Vec4<int>& theIndexes)
  {
    return AddQuadTriangleEdges(theIndexes[0], theIndexes[1], theIndexes[2], theIndexes[3]);
  }

  //! Add triangle strip into indexed triangulation array.
  //! N-2 triangles are added from N input nodes.
  //! Raises exception if array is not of type Graphic3d_TOPA_TRIANGLES.
  //! @param[in] theVertexLower  index of first node defining triangle strip
  //! @param[in] theVertexUpper  index of last  node defining triangle strip
  Standard_EXPORT void AddTriangleStripEdges(int theVertexLower, int theVertexUpper);

  //! Add triangle fan into indexed triangulation array.
  //! N-2 triangles are added from N input nodes (or N-1 with closed flag).
  //! Raises exception if array is not of type Graphic3d_TOPA_TRIANGLES.
  //! @param[in] theVertexLower  index of first node defining triangle fun (center)
  //! @param[in] theVertexUpper  index of last  node defining triangle fun
  //! @param[in] theToClose  close triangle fan (connect first and last points)
  Standard_EXPORT void AddTriangleFanEdges(int theVertexLower, int theVertexUpper, bool theToClose);

  //! Add line strip (polyline) into indexed segments array.
  //! N-1 segments are added from N input nodes (or N with closed flag).
  //! Raises exception if array is not of type Graphic3d_TOPA_SEGMENTS.
  //! @param[in] theVertexLower  index of first node defining line strip fun (center)
  //! @param[in] theVertexUpper  index of last  node defining triangle fun
  //! @param[in] theToClose  close triangle fan (connect first and last points)
  Standard_EXPORT void AddPolylineEdges(int theVertexLower, int theVertexUpper, bool theToClose);

public: //! @name optional array of Bounds/Subgroups within primitive array (e.g. restarting
        //! primitives / assigning colors)
  //! Returns optional bounds buffer.
  const occ::handle<Graphic3d_BoundBuffer>& Bounds() const { return myBounds; }

  //! Returns TRUE when bound colors array is defined.
  bool HasBoundColors() const { return !myBounds.IsNull() && myBounds->Colors != NULL; }

  //! Returns the number of defined bounds
  int BoundNumber() const { return !myBounds.IsNull() ? myBounds->NbBounds : -1; }

  //! Returns the number of allocated bounds
  int BoundNumberAllocated() const { return !myBounds.IsNull() ? myBounds->NbMaxBounds : 0; }

  //! Returns the edge number at rank theRank.
  int Bound(const int theRank) const
  {
    Standard_OutOfRange_Raise_if(myBounds.IsNull() || theRank < 1 || theRank > myBounds->NbBounds,
                                 "BAD BOUND index");
    return myBounds->Bounds[theRank - 1];
  }

  //! Returns the bound color at rank theRank from the bound table if defined.
  Quantity_Color BoundColor(const int theRank) const
  {
    double anRGB[3] = {0.0, 0.0, 0.0};
    BoundColor(theRank, anRGB[0], anRGB[1], anRGB[2]);
    return Quantity_Color(anRGB[0], anRGB[1], anRGB[2], Quantity_TOC_RGB);
  }

  //! Returns the bound color values at rank theRank from the bound table if defined.
  void BoundColor(const int theRank, double& theR, double& theG, double& theB) const
  {
    Standard_OutOfRange_Raise_if(myBounds.IsNull() || myBounds->Colors == NULL || theRank < 1
                                   || theRank > myBounds->NbBounds,
                                 "BAD BOUND index");
    const NCollection_Vec4<float>& aVec = myBounds->Colors[theRank - 1];
    theR                                = double(aVec.r());
    theG                                = double(aVec.g());
    theB                                = double(aVec.b());
  }

  //! Adds a bound of length theEdgeNumber in the bound array
  //! @return the actual bounds number
  Standard_EXPORT int AddBound(const int theEdgeNumber);

  //! Adds a bound of length theEdgeNumber and bound color theBColor in the bound array.
  //! Warning: theBColor is ignored when the hasBColors constructor parameter is FALSE
  //! @return the actual bounds number
  int AddBound(const int theEdgeNumber, const Quantity_Color& theBColor)
  {
    return AddBound(theEdgeNumber, theBColor.Red(), theBColor.Green(), theBColor.Blue());
  }

  //! Adds a bound of length theEdgeNumber and bound color coordinates in the bound array.
  //! Warning: <theR,theG,theB> are ignored when the hasBColors constructor parameter is FALSE
  //! @return the actual bounds number
  Standard_EXPORT int AddBound(const int    theEdgeNumber,
                               const double theR,
                               const double theG,
                               const double theB);

  //! Change the bound color of rank theIndex in the array.
  void SetBoundColor(const int theIndex, const Quantity_Color& theColor)
  {
    SetBoundColor(theIndex, theColor.Red(), theColor.Green(), theColor.Blue());
  }

  //! Change the bound color of rank theIndex in the array.
  void SetBoundColor(const int theIndex, const double theR, const double theG, const double theB)
  {
    if (myBounds.IsNull())
    {
      return;
    }
    Standard_OutOfRange_Raise_if(myBounds.IsNull() || myBounds->Colors == NULL || theIndex < 1
                                   || theIndex > myBounds->NbMaxBounds,
                                 "BAD BOUND index");
    NCollection_Vec4<float>& aVec = myBounds->Colors[theIndex - 1];
    aVec.r()                      = float(theR);
    aVec.g()                      = float(theG);
    aVec.b()                      = float(theB);
    aVec.a()                      = 1.0f;
    myBounds->NbBounds            = (std::max)(theIndex, myBounds->NbBounds);
  }

protected: //! @name protected constructors
  //! Main constructor.
  //! @param theType       type of primitive
  //! @param theMaxVertexs length of vertex attributes buffer to be allocated (maximum number of
  //! vertexes, @sa ::AddVertex())
  //! @param theMaxBounds  length of bounds buffer to be allocated (maximum number of bounds, @sa
  //! ::AddBound())
  //! @param theMaxEdges   length of edges (index) buffer to be allocated (maximum number of indexes
  //! @sa ::AddEdge())
  //! @param theArrayFlags array flags
  Graphic3d_ArrayOfPrimitives(Graphic3d_TypeOfPrimitiveArray theType,
                              int                            theMaxVertexs,
                              int                            theMaxBounds,
                              int                            theMaxEdges,
                              Graphic3d_ArrayFlags           theArrayFlags)
      : myNormData(NULL),
        myTexData(NULL),
        myColData(NULL),
        myPosStride(0),
        myNormStride(0),
        myTexStride(0),
        myColStride(0),
        myType(Graphic3d_TOPA_UNDEFINED)
  {
    init(theType, theMaxVertexs, theMaxBounds, theMaxEdges, theArrayFlags);
  }

  //! Array constructor.
  Standard_EXPORT void init(Graphic3d_TypeOfPrimitiveArray theType,
                            int                            theMaxVertexs,
                            int                            theMaxBounds,
                            int                            theMaxEdges,
                            Graphic3d_ArrayFlags           theArrayFlags);

private: //! @name private fields
  occ::handle<Graphic3d_IndexBuffer> myIndices;
  occ::handle<Graphic3d_Buffer>      myAttribs;
  occ::handle<Graphic3d_BoundBuffer> myBounds;
  uint8_t*                           myNormData;
  uint8_t*                           myTexData;
  uint8_t*                           myColData;
  size_t                             myPosStride;
  size_t                             myNormStride;
  size_t                             myTexStride;
  size_t                             myColStride;
  Graphic3d_TypeOfPrimitiveArray     myType;
};

#endif // _Graphic3d_ArrayOfPrimitives_HeaderFile
