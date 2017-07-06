// Created on: 1995-03-06
// Created by: Laurent PAINNOT
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Poly_Triangulation_HeaderFile
#define _Poly_Triangulation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <TShort_HArray1OfShortReal.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TShort_Array1OfShortReal.hxx>
class Standard_DomainError;
class Standard_NullObject;


class Poly_Triangulation;
DEFINE_STANDARD_HANDLE(Poly_Triangulation, Standard_Transient)

//! Provides a triangulation for a surface, a set of surfaces, or
//! more generally a shape.
//! A triangulation consists of an approximate representation
//! of the actual shape, using a collection of points and
//! triangles. The points are located on the surface. The
//! edges of the triangles connect adjacent points with a
//! straight line that approximates the true curve on the surface.
//! A triangulation comprises:
//! -   A table of 3D nodes (3D points on the surface).
//! -   A table of triangles. Each triangle (Poly_Triangle
//! object) comprises a triplet of indices in the table of 3D
//! nodes specific to the triangulation.
//! -   A table of 2D nodes (2D points), parallel to the table of
//! 3D nodes. This table is optional. If it exists, the
//! coordinates of a 2D point are the (u, v) parameters
//! of the corresponding 3D point on the surface
//! approximated by the triangulation.
//! -   A deflection (optional), which maximizes the distance
//! from a point on the surface to the corresponding point
//! on its approximate triangulation.
//! In many cases, algorithms do not need to work with the
//! exact representation of a surface. A triangular
//! representation induces simpler and more robust adjusting,
//! faster performances, and the results are as good.
//! This is a Transient class.
class Poly_Triangulation : public Standard_Transient
{

public:

  DEFINE_STANDARD_RTTIEXT(Poly_Triangulation, Standard_Transient)

  //! Constructs a triangulation from a set of triangles. The
  //! triangulation is initialized without a triangle or a node, but capable of
  //! containing nbNodes nodes, and nbTriangles
  //! triangles. Here the UVNodes flag indicates whether
  //! 2D nodes will be associated with 3D ones, (i.e. to
  //! enable a 2D representation).
  Standard_EXPORT Poly_Triangulation(const Standard_Integer nbNodes, const Standard_Integer nbTriangles, const Standard_Boolean UVNodes);

  //! Constructs a triangulation from a set of triangles. The
  //! triangulation is initialized with 3D points from Nodes and triangles
  //! from Triangles.
  Standard_EXPORT Poly_Triangulation(const TColgp_Array1OfPnt& Nodes, const Poly_Array1OfTriangle& Triangles);

  //! Constructs a triangulation from a set of triangles. The
  //! triangulation is initialized with 3D points from Nodes, 2D points from
  //! UVNodes and triangles from Triangles, where
  //! coordinates of a 2D point from UVNodes are the
  //! (u, v) parameters of the corresponding 3D point
  //! from Nodes on the surface approximated by the
  //! constructed triangulation.
  Standard_EXPORT Poly_Triangulation(const TColgp_Array1OfPnt& Nodes, const TColgp_Array1OfPnt2d& UVNodes, const Poly_Array1OfTriangle& Triangles);

  //! Creates full copy of current triangulation
  Standard_EXPORT virtual Handle(Poly_Triangulation) Copy() const;

  //! Copy constructor for triangulation.
  Standard_EXPORT Poly_Triangulation (const Handle(Poly_Triangulation)& theTriangulation);

  //! Returns the deflection of this triangulation.
  Standard_Real Deflection() const { return myDeflection; }

  //! Sets the deflection of this triangulation to theDeflection.
  //! See more on deflection in Polygon2D
  Standard_EXPORT void Deflection (const Standard_Real theDeflection);

  //! Deallocates the UV nodes.
  Standard_EXPORT void RemoveUVNodes();

  //! Returns the number of nodes for this triangulation.
  Standard_Integer NbNodes() const { return myNodes.Length(); }

  //! Returns the number of triangles for this triangulation.
  Standard_Integer NbTriangles() const { return myTriangles.Length(); }

  //! Returns Standard_True if 2D nodes are associated with 3D nodes for this triangulation.
  Standard_Boolean HasUVNodes() const { return !myUVNodes.IsNull(); }

  //! Returns the table of 3D nodes (3D points) for this triangulation.
  const TColgp_Array1OfPnt& Nodes() const { return myNodes; }

  //! Returns the table of 3D nodes (3D points) for this triangulation.
  //! The returned array is
  //! shared. Therefore if the table is selected by reference, you
  //! can, by simply modifying it, directly modify the data
  //! structure of this triangulation.
  TColgp_Array1OfPnt& ChangeNodes() { return myNodes; }

  //! Returns node at the given index.
  //! Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbNodes.
  Standard_EXPORT const gp_Pnt& Node (const Standard_Integer theIndex) const;

  //! Give access to the node at the given index.
  //! Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbNodes.
  Standard_EXPORT gp_Pnt& ChangeNode (const Standard_Integer theIndex);

  //! Returns the table of 2D nodes (2D points) associated with
  //! each 3D node of this triangulation.
  //! The function HasUVNodes  checks if 2D nodes
  //! are associated with the 3D nodes of this triangulation.
  //! Const reference on the 2d nodes values.
  const TColgp_Array1OfPnt2d& UVNodes() const { return myUVNodes->Array1(); }

  //! Returns the table of 2D nodes (2D points) associated with
  //! each 3D node of this triangulation.
  //! Function ChangeUVNodes shares  the returned array.
  //! Therefore if the table is selected by reference,
  //! you can, by simply modifying it, directly modify the data
  //! structure of this triangulation.
  TColgp_Array1OfPnt2d& ChangeUVNodes() { return myUVNodes->ChangeArray1(); }

  //! Returns UVNode at the given index.
  //! Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbNodes.
  Standard_EXPORT const gp_Pnt2d& UVNode (const Standard_Integer theIndex) const;

  //! Give access to the UVNode at the given index.
  //! Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbNodes.
  Standard_EXPORT gp_Pnt2d& ChangeUVNode (const Standard_Integer theIndex);

  //! Returns the table of triangles for this triangulation.
  const Poly_Array1OfTriangle& Triangles() const { return myTriangles; }

  //! Returns the table of triangles for this triangulation.
  //! Function ChangeUVNodes shares  the returned array.
  //! Therefore if the table is selected by reference,
  //! you can, by simply modifying it, directly modify the data
  //! structure of this triangulation.
  Poly_Array1OfTriangle& ChangeTriangles() { return myTriangles; }

  //! Returns triangle at the given index.
  //! Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbTriangles.
  Standard_EXPORT const Poly_Triangle& Triangle (const Standard_Integer theIndex) const;

  //! Give access to the triangle at the given index.
  //! Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbTriangles.
  Standard_EXPORT Poly_Triangle& ChangeTriangle (const Standard_Integer theIndex);

  //! Sets the table of node normals.
  //! raises exception if length of theNormals != 3*NbNodes
  Standard_EXPORT void SetNormals (const Handle(TShort_HArray1OfShortReal)& theNormals);

  //! Returns the table of node normals.
  Standard_EXPORT const TShort_Array1OfShortReal& Normals() const;

  //! Gives access to the table of node normals.
  Standard_EXPORT TShort_Array1OfShortReal& ChangeNormals();

  //! Returns Standard_True if nodal normals are defined.
  Standard_EXPORT Standard_Boolean HasNormals() const;

  //! @return normal at the given index.
  //! Raises Standard_OutOfRange exception.
  Standard_EXPORT const gp_Dir Normal (const Standard_Integer theIndex) const;

  //! Changes normal at the given index.
  //! Raises Standard_OutOfRange exception.
  Standard_EXPORT void SetNormal (const Standard_Integer theIndex,
                                  const gp_Dir&          theNormal);

protected:

  Standard_Real                      myDeflection;
  TColgp_Array1OfPnt                 myNodes;
  Handle(TColgp_HArray1OfPnt2d)      myUVNodes;
  Poly_Array1OfTriangle              myTriangles;
  Handle(TShort_HArray1OfShortReal)  myNormals;

};

#endif // _Poly_Triangulation_HeaderFile
