// Created on: 2016-11-10
// Created by: Anton KOZULIN
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _TDataXtd_Triangulation_HeaderFile
#define _TDataXtd_Triangulation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Poly_Triangulation.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
class Standard_GUID;
class TDF_Label;
class TDF_Attribute;
class TDF_RelocationTable;

class TDataXtd_Triangulation;
DEFINE_STANDARD_HANDLE(TDataXtd_Triangulation, TDF_Attribute)

//! An Ocaf attribute containing a mesh (Poly_Triangulation).
//! It duplicates all methods from Poly_Triangulation.
//! It is highly recommended to modify the mesh through the methods of this attribute,
//! but not directly via the underlying Poly_Triangulation object.
//! In this case Undo/Redo will work fine and robust.
class TDataXtd_Triangulation : public TDF_Attribute
{
public:

  //! Static methods
  //  ==============

  //! Returns the ID of the triangulation attribute.
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Finds or creates a triangulation attribute.
  Standard_EXPORT static Handle(TDataXtd_Triangulation) Set(const TDF_Label& theLabel);

  //! Finds or creates a triangulation attribute.
  //! Initializes the attribute by a Poly_Triangulation object.
  Standard_EXPORT static Handle(TDataXtd_Triangulation) Set(const TDF_Label& theLabel, const Handle(Poly_Triangulation)& theTriangulation);

  //! Object methods
  //  ==============

  //! A constructor.
  //! Don't use it directly, 
  //! use please the static method Set(),
  //! which returns the attribute attached to a label.
  Standard_EXPORT TDataXtd_Triangulation();

  //! Sets the triangulation.
  Standard_EXPORT void Set(const Handle(Poly_Triangulation)& theTriangulation);

  //! Returns the underlying triangulation.
  Standard_EXPORT const Handle(Poly_Triangulation)& Get() const;


  //! Poly_Triangulation methods
  //  =================

  //! The methods are "covered" by this attribute to prevent direct modification of the mesh.
  //! There is no performance problem to call Poly_Triangulation method through this attribute.
  //! The most of the methods are considered as "inline" by the compiler in release mode.

  //! Returns the deflection of this triangulation.
  Standard_EXPORT Standard_Real Deflection() const;

  //! Sets the deflection of this triangulation to theDeflection.
  //! See more on deflection in Polygon2D
  Standard_EXPORT void Deflection (const Standard_Real theDeflection);

  //! Deallocates the UV nodes.
  Standard_EXPORT void RemoveUVNodes();

  //! @return the number of nodes for this triangulation.
  Standard_EXPORT Standard_Integer NbNodes() const;

  //! @return the number of triangles for this triangulation.
  Standard_EXPORT Standard_Integer NbTriangles() const;

  //! @return Standard_True if 2D nodes are associated with 3D nodes for this triangulation.
  Standard_EXPORT Standard_Boolean HasUVNodes() const;

  //! @return node at the given index.
  //! Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbNodes.
  Standard_EXPORT gp_Pnt Node (const Standard_Integer theIndex) const;

  //! The method differs from Poly_Triangulation!
  //! Sets a node at the given index.
  //! Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbNodes.
  Standard_EXPORT void SetNode (const Standard_Integer theIndex, const gp_Pnt& theNode);

  //! @return UVNode at the given index.
  //! Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbNodes.
  Standard_EXPORT gp_Pnt2d UVNode (const Standard_Integer theIndex) const;

  //! The method differs from Poly_Triangulation!
  //! Sets a UVNode at the given index.
  //! Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbNodes.
  Standard_EXPORT void SetUVNode (const Standard_Integer theIndex, const gp_Pnt2d& theUVNode);

  //! @return triangle at the given index.
  //! Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbTriangles.
  Standard_EXPORT Poly_Triangle Triangle (const Standard_Integer theIndex) const;

  //! The method differs from Poly_Triangulation!
  //! Sets a triangle at the given index.
  //! Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbTriangles.
  Standard_EXPORT void SetTriangle (const Standard_Integer theIndex, const Poly_Triangle& theTriangle);

  //! Changes normal at the given index.
  //! Raises Standard_OutOfRange exception.
  Standard_EXPORT void SetNormal (const Standard_Integer theIndex,
                                  const gp_Dir&          theNormal);

  //! Returns Standard_True if nodal normals are defined.
  Standard_EXPORT Standard_Boolean HasNormals() const;

  //! @return normal at the given index.
  //! Raises Standard_OutOfRange exception.
  Standard_EXPORT gp_Dir Normal (const Standard_Integer theIndex) const;

  //! Inherited attribute methods
  //  ===========================

  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& theAttribute) Standard_OVERRIDE;

  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& Into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTI_INLINE(TDataXtd_Triangulation,TDF_Attribute)

private:

  Handle(Poly_Triangulation) myTriangulation;
};

#endif // _TDataXtd_Triangulation_HeaderFile
