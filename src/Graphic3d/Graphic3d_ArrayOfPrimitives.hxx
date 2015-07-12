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

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Graphic3d_IndexBuffer.hxx>
#include <Graphic3d_Buffer.hxx>
#include <Graphic3d_BoundBuffer.hxx>
#include <Graphic3d_TypeOfPrimitiveArray.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Byte.hxx>
#include <MMgt_TShared.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_ShortReal.hxx>
#include <Standard_CString.hxx>
class Standard_OutOfRange;
class Graphic3d_InitialisationError;
class Graphic3d_Group;
class gp_Pnt;
class Quantity_Color;
class gp_Dir;
class gp_Pnt2d;


class Graphic3d_ArrayOfPrimitives;
DEFINE_STANDARD_HANDLE(Graphic3d_ArrayOfPrimitives, MMgt_TShared)

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
class Graphic3d_ArrayOfPrimitives : public MMgt_TShared
{

public:

  
  Standard_EXPORT void Destroy();
~Graphic3d_ArrayOfPrimitives()
{
  Destroy();
}
  
  //! Adds a vertice in the array.
  //! returns the actual vertex number.
    Standard_Integer AddVertex (const gp_Pnt& aVertice);
Standard_Integer AddVertex (const Graphic3d_Vec3& theVertex);
  
  //! Adds a vertice in the array.
  //! returns the actual vertex number.
    Standard_Integer AddVertex (const Standard_Real X, const Standard_Real Y, const Standard_Real Z);
  
  //! Adds a vertice in the array.
  //! returns the actual vertex number.
  Standard_EXPORT Standard_Integer AddVertex (const Standard_ShortReal X, const Standard_ShortReal Y, const Standard_ShortReal Z);
  
  //! Adds a vertice and vertex color in the vertex array.
  //! returns the actual vertex number.
  //! Warning: <aColor> is ignored when the <hasVColors>
  //! constructor parameter is FALSE
  Standard_EXPORT Standard_Integer AddVertex (const gp_Pnt& aVertice, const Quantity_Color& aColor);
  
  //! Adds a vertice and vertex color in the vertex array.
  //! returns the actual vertex number.
  //! Warning: <aColor> is ignored when the <hasVColors>
  //! constructor parameter is FALSE
  //! aColor = Alpha << 24 + Blue << 16 + Green << 8 + Red
  //! On all architecture proccers type (x86 or SPARC) you can
  //! use this byte order.
  Standard_EXPORT Standard_Integer AddVertex (const gp_Pnt& aVertice, const Standard_Integer aColor);
  
  //! Adds a vertice and vertex normal in the vertex array.
  //! returns the actual vertex number.
  //! Warning: <aNormal> is ignored when the <hasVNormals>
  //! constructor parameter is FALSE.
    Standard_Integer AddVertex (const gp_Pnt& aVertice, const gp_Dir& aNormal);
  
  //! Adds a vertice and vertex normal in the vertex array.
  //! returns the actual vertex number.
  //! Warning: <aNormal> is ignored when the <hasVNormals>
  //! constructor parameter is FALSE.
    Standard_Integer AddVertex (const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Real NX, const Standard_Real NY, const Standard_Real NZ);
  
  //! Adds a vertice and vertex normal in the vertex array.
  //! returns the actual vertex number.
  //! Warning: <aNormal> is ignored when the <hasVNormals>
  //! constructor parameter is FALSE.
  Standard_EXPORT Standard_Integer AddVertex (const Standard_ShortReal X, const Standard_ShortReal Y, const Standard_ShortReal Z, const Standard_ShortReal NX, const Standard_ShortReal NY, const Standard_ShortReal NZ);
  
  //! Adds a vertice,vertex normal and color in the vertex array.
  //! returns the actual vertex number.
  //! Warning: <aNormal> is ignored when the <hasVNormals>
  //! constructor parameter is FALSE.
  //! <aColor> is ignored when the <hasVColors>
  //! constructor parameter is FALSE
  Standard_EXPORT Standard_Integer AddVertex (const gp_Pnt& aVertice, const gp_Dir& aNormal, const Quantity_Color& aColor);
  
  //! Adds a vertice,vertex normal and color in the vertex array.
  //! returns the actual vertex number.
  //! Warning: <aNormal> is ignored when the <hasVNormals>
  //! constructor parameter is FALSE.
  //! <aColor> is ignored when the <hasVColors>
  //! constructor parameter is FALSE
  //! aColor = Alpha << 24 + Blue << 16 + Green << 8 + Red
  //! On all architecture proccers type (x86 or SPARC) you can
  //! use this byte order.
  Standard_EXPORT Standard_Integer AddVertex (const gp_Pnt& aVertice, const gp_Dir& aNormal, const Standard_Integer aColor);
  
  //! Adds a vertice and vertex texture in the vertex array.
  //! returns the actual vertex number.
  //! <aTexel> is ignored when the <hasVTexels>
  //! constructor parameter is FALSE.
    Standard_Integer AddVertex (const gp_Pnt& aVertice, const gp_Pnt2d& aTexel);
  
  //! Adds a vertice and vertex texture coordinates in the vertex array.
  //! returns the actual vertex number.
  //! <aTexel> is ignored when the <hasVTexels>
  //! constructor parameter is FALSE.
    Standard_Integer AddVertex (const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Real TX, const Standard_Real TY);
  
  //! Adds a vertice and vertex texture coordinates in the vertex array.
  //! returns the actual vertex number.
  //! <aTexel> is ignored when the <hasVTexels>
  //! constructor parameter is FALSE.
  Standard_EXPORT Standard_Integer AddVertex (const Standard_ShortReal X, const Standard_ShortReal Y, const Standard_ShortReal Z, const Standard_ShortReal TX, const Standard_ShortReal TY);
  
  //! Adds a vertice,vertex normal and texture in the vertex array.
  //! returns the actual vertex number.
  //! Warning: <aNormal> is ignored when the <hasVNormals>
  //! constructor parameter is FALSE.
  //! <aTexel> is ignored when the <hasVTexels>
  //! constructor parameter is FALSE.
    Standard_Integer AddVertex (const gp_Pnt& aVertice, const gp_Dir& aNormal, const gp_Pnt2d& aTexel);
  
  //! Adds a vertice,vertex normal and texture in the vertex array.
  //! returns the actual vertex number.
  //! Warning: <aNormal> is ignored when the <hasVNormals>
  //! constructor parameter is FALSE.
  //! <aTexel> is ignored when the <hasVTexels>
  //! constructor parameter is FALSE.
    Standard_Integer AddVertex (const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Real NX, const Standard_Real NY, const Standard_Real NZ, const Standard_Real TX, const Standard_Real TY);
  
  //! Adds a vertice,vertex normal and texture in the vertex array.
  //! returns the actual vertex number.
  //! Warning: <aNormal> is ignored when the <hasVNormals>
  //! constructor parameter is FALSE.
  //! <aTexel> is ignored when the <hasVTexels>
  //! constructor parameter is FALSE.
  Standard_EXPORT Standard_Integer AddVertex (const Standard_ShortReal X, const Standard_ShortReal Y, const Standard_ShortReal Z, const Standard_ShortReal NX, const Standard_ShortReal NY, const Standard_ShortReal NZ, const Standard_ShortReal TX, const Standard_ShortReal TY);
  
  //! Adds a bound of length <edgeNumber> in the bound array
  //! returns the actual bounds number.
  Standard_EXPORT Standard_Integer AddBound (const Standard_Integer edgeNumber);
  
  //! Adds a bound of length <edgeNumber> and bound color
  //! <aBColor> in the bound array.
  //! returns the actual bounds number.
  //! Warning: <aBColor> is ignored when the <hasBColors>
  //! constructor parameter is FALSE
  Standard_EXPORT Standard_Integer AddBound (const Standard_Integer edgeNumber, const Quantity_Color& aBColor);
  
  //! Adds a bound of length <edgeNumber> and bound color
  //! coordinates in the bound array.
  //! returns the actual bounds number.
  //! Warning: <R,G,B> are ignored when the <hasBColors>
  //! constructor parameter is FALSE
  Standard_EXPORT Standard_Integer AddBound (const Standard_Integer edgeNumber, const Standard_Real R, const Standard_Real G, const Standard_Real B);
  
  //! Adds an edge in the range [1,VertexNumber()] in the array.
  //! Returns the actual edges number.
  Standard_EXPORT Standard_Integer AddEdge (const Standard_Integer vertexIndex);
  
  //! Change the vertice of rank <anIndex> in the array.
  Standard_EXPORT void SetVertice (const Standard_Integer anIndex, const gp_Pnt& aVertice);
  
  //! Change the vertice of rank <anIndex> in the array.
    void SetVertice (const Standard_Integer anIndex, const Standard_ShortReal X, const Standard_ShortReal Y, const Standard_ShortReal Z);
  
  //! Change the vertex color of rank <anIndex> in the array.
  Standard_EXPORT void SetVertexColor (const Standard_Integer anIndex, const Quantity_Color& aColor);
  
  //! Change the vertex color of rank <anIndex> in the array.
    void SetVertexColor (const Standard_Integer anIndex, const Standard_Real R, const Standard_Real G, const Standard_Real B);
  
  //! Change the vertex color of rank <anIndex> in the array.
  //! aColor = Alpha << 24 + Blue << 16 + Green << 8 + Red
  //! On all architecture proccers type (x86 or SPARC) you can
  //! use this byte order.
  Standard_EXPORT void SetVertexColor (const Standard_Integer anIndex, const Standard_Integer aColor);
  
  //! Change the vertex normal of rank <anIndex> in the array.
  Standard_EXPORT void SetVertexNormal (const Standard_Integer anIndex, const gp_Dir& aNormal);
  
  //! Change the vertex normal of rank <anIndex> in the array.
    void SetVertexNormal (const Standard_Integer anIndex, const Standard_Real NX, const Standard_Real NY, const Standard_Real NZ);
  
  //! Change the vertex texel of rank <anIndex> in the array.
  Standard_EXPORT void SetVertexTexel (const Standard_Integer anIndex, const gp_Pnt2d& aTexel);
  
  //! Change the vertex texel of rank <anIndex> in the array.
    void SetVertexTexel (const Standard_Integer anIndex, const Standard_Real TX, const Standard_Real TY);
  
  //! Change the bound color of rank <anIndex> in the array.
  Standard_EXPORT void SetBoundColor (const Standard_Integer anIndex, const Quantity_Color& aColor);
  
  //! Change the bound color of rank <anIndex> in the array.
    void SetBoundColor (const Standard_Integer anIndex, const Standard_Real R, const Standard_Real G, const Standard_Real B);
  
  //! Returns optional index buffer.
    const Handle(Graphic3d_IndexBuffer)& Indices() const;
  
  //! Returns vertex attributes buffer (colors, normals, texture coordinates).
    const Handle(Graphic3d_Buffer)& Attributes() const;
  
  //! Returns optional bounds buffer.
    const Handle(Graphic3d_BoundBuffer)& Bounds() const;
  
  //! Returns the type of this primitive
    Graphic3d_TypeOfPrimitiveArray Type() const;
  
  //! Returns the string type of this primitive
  Standard_EXPORT Standard_CString StringType() const;
  
  //! Returns TRUE when vertex normals array is defined.
    Standard_Boolean HasVertexNormals() const;
  
  //! Returns TRUE when vertex colors array is defined.
    Standard_Boolean HasVertexColors() const;
  
  //! Returns TRUE when vertex texels array is defined.
    Standard_Boolean HasVertexTexels() const;
  
  //! Returns the number of defined vertex
    Standard_Integer VertexNumber() const;
  
  //! Returns the vertice at rank <aRank>
  //! from the vertex table if defined.
  Standard_EXPORT gp_Pnt Vertice (const Standard_Integer aRank) const;
  
  //! Returns the vertice coordinates at rank <aRank>
  //! from the vertex table if defined.
    void Vertice (const Standard_Integer aRank, Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const;
  
  //! Returns the vertex color at rank <aRank>
  //! from the vertex table if defined.
  Standard_EXPORT Quantity_Color VertexColor (const Standard_Integer aRank) const;
  
  //! Returns the vertex color values at rank <aRank>
  //! from the vertex table if defined.
    void VertexColor (const Standard_Integer aRank, Standard_Real& R, Standard_Real& G, Standard_Real& B) const;
  
  //! Returns the vertex color values at rank <aRank>
  //! from the vertex table if defined.
    void VertexColor (const Standard_Integer aRank, Standard_Integer& aColor) const;
  
  //! Returns the vertex normal at rank <aRank>
  //! from the vertex table if defined.
  Standard_EXPORT gp_Dir VertexNormal (const Standard_Integer aRank) const;
  
  //! Returns the vertex normal coordinates at rank <aRank>
  //! from the vertex table if defined.
    void VertexNormal (const Standard_Integer aRank, Standard_Real& NX, Standard_Real& NY, Standard_Real& NZ) const;
  
  //! Returns the vertex texture at rank <aRank>
  //! from the vertex table if defined.
  Standard_EXPORT gp_Pnt2d VertexTexel (const Standard_Integer aRank) const;
  
  //! Returns the vertex texture coordinates at rank <aRank>
  //! from the vertex table if defined.
    void VertexTexel (const Standard_Integer aRank, Standard_Real& TX, Standard_Real& TY) const;
  
  //! Returns the number of defined edges
    Standard_Integer EdgeNumber() const;
  
  //! Returns the vertex index at rank <aRank>
  //! in the range [1,VertexNumber()]
    Standard_Integer Edge (const Standard_Integer aRank) const;
  
  //! Returns TRUE when bound colors array is defined.
    Standard_Boolean HasBoundColors() const;
  
  //! Returns the number of defined bounds
    Standard_Integer BoundNumber() const;
  
  //! Returns the edge number at rank <aRank>.
    Standard_Integer Bound (const Standard_Integer aRank) const;
  
  //! Returns the bound color at rank <aRank>
  //! from the bound table if defined.
  Standard_EXPORT Quantity_Color BoundColor (const Standard_Integer aRank) const;
  
  //! Returns the bound color values at rank <aRank>
  //! from the bound table if defined.
  Standard_EXPORT void BoundColor (const Standard_Integer aRank, Standard_Real& R, Standard_Real& G, Standard_Real& B) const;
  
  //! Returns the number of total items according to
  //! the array type.
  Standard_EXPORT Standard_Integer ItemNumber() const;
  
  //! Returns TRUE only when the contains of this array is
  //! available.
  Standard_EXPORT Standard_Boolean IsValid();

friend class Graphic3d_Group;


  DEFINE_STANDARD_RTTI(Graphic3d_ArrayOfPrimitives,MMgt_TShared)

protected:

  
  //! Warning
  //! You must use a coherent set of AddVertex() methods according to the
  //! <hasVNormals>,<hasVColors>,<hasVTexels>,<hasBColors>
  //! User is responsible of confuse vertex and bad normal orientation.
  //! You must use AddBound() method only if the <maxBounds>
  //! constructor parameter is > 0.
  //! You must use AddEdge() method only if the <maxEdges>
  //! constructor parameter is > 0.
  Standard_EXPORT Graphic3d_ArrayOfPrimitives(const Graphic3d_TypeOfPrimitiveArray aType, const Standard_Integer maxVertexs, const Standard_Integer maxBounds, const Standard_Integer maxEdges, const Standard_Boolean hasVNormals, const Standard_Boolean hasVColors, const Standard_Boolean hasBColors, const Standard_Boolean hasTexels);



private:

  
  Standard_EXPORT void ComputeVNormals (const Standard_Integer fromIndex, const Standard_Integer toIndex);

  Handle(Graphic3d_IndexBuffer) myIndices;
  Handle(Graphic3d_Buffer) myAttribs;
  Handle(Graphic3d_BoundBuffer) myBounds;
  Graphic3d_TypeOfPrimitiveArray myType;
  Standard_Integer myMaxBounds;
  Standard_Integer myMaxVertexs;
  Standard_Integer myMaxEdges;
  Standard_Byte myVNor;
  Standard_Byte myVTex;
  Standard_Byte myVCol;


};


#include <Graphic3d_ArrayOfPrimitives.lxx>





#endif // _Graphic3d_ArrayOfPrimitives_HeaderFile
