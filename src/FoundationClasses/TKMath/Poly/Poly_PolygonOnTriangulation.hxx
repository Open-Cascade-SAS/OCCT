// Created on: 1996-02-21
// Created by: Laurent PAINNOT
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _Poly_PolygonOnTriangulation_HeaderFile
#define _Poly_PolygonOnTriangulation_HeaderFile

#include <Standard_NullObject.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//! This class provides a polygon in 3D space, based on the triangulation
//! of a surface. It may be the approximate representation of a
//! curve on the surface, or more generally the shape.
//! A PolygonOnTriangulation is defined by a table of
//! nodes. Each node is an index in the table of nodes specific
//! to a triangulation, and represents a point on the surface. If
//! the polygon is closed, the index of the point of closure is
//! repeated at the end of the table of nodes.
//! If the polygon is an approximate representation of a curve
//! on a surface, you can associate with each of its nodes the
//! value of the parameter of the corresponding point on the
//! curve.represents a 3d Polygon
class Poly_PolygonOnTriangulation : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Poly_PolygonOnTriangulation, Standard_Transient)
public:
  //! Constructs a 3D polygon on the triangulation of a shape with specified size of nodes.
  Standard_EXPORT Poly_PolygonOnTriangulation(const int theNbNodes, const bool theHasParams);

  //! Constructs a 3D polygon on the triangulation of a shape,
  //! defined by the table of nodes, <Nodes>.
  Standard_EXPORT Poly_PolygonOnTriangulation(const NCollection_Array1<int>& Nodes);

  //! Constructs a 3D polygon on the triangulation of a shape, defined by:
  //! -   the table of nodes, Nodes, and the table of parameters, <Parameters>.
  //! where:
  //! -   a node value is an index in the table of nodes specific
  //! to an existing triangulation of a shape
  //! -   and a parameter value is the value of the parameter of
  //! the corresponding point on the curve approximated by
  //! the constructed polygon.
  //! Warning
  //! The tables Nodes and Parameters must be the same size.
  //! This property is not checked at construction time.
  Standard_EXPORT Poly_PolygonOnTriangulation(const NCollection_Array1<int>&    Nodes,
                                              const NCollection_Array1<double>& Parameters);

  //! Creates a copy of current polygon
  Standard_EXPORT virtual occ::handle<Poly_PolygonOnTriangulation> Copy() const;

  //! Returns the deflection of this polygon
  double Deflection() const { return myDeflection; }

  //! Sets the deflection of this polygon.
  //! See more on deflection in Poly_Polygones2D.
  void Deflection(const double theDefl) { myDeflection = theDefl; }

  //! Returns the number of nodes for this polygon.
  //! Note: If the polygon is closed, the point of closure is
  //! repeated at the end of its table of nodes. Thus, on a closed
  //! triangle, the function NbNodes returns 4.
  int NbNodes() const { return myNodes.Length(); }

  //! Returns node at the given index.
  int Node(int theIndex) const { return myNodes.Value(theIndex); }

  //! Sets node at the given index.
  void SetNode(int theIndex, int theNode) { myNodes.SetValue(theIndex, theNode); }

  //! Returns true if parameters are associated with the nodes in this polygon.
  bool HasParameters() const { return !myParameters.IsNull(); }

  //! Returns parameter at the given index.
  double Parameter(int theIndex) const
  {
    Standard_NullObject_Raise_if(myParameters.IsNull(),
                                 "Poly_PolygonOnTriangulation::Parameter : parameters is NULL");
    return myParameters->Value(theIndex);
  }

  //! Sets parameter at the given index.
  void SetParameter(int theIndex, double theValue)
  {
    Standard_NullObject_Raise_if(myParameters.IsNull(),
                                 "Poly_PolygonOnTriangulation::Parameter : parameters is NULL");
    myParameters->SetValue(theIndex, theValue);
  }

  //! Sets the table of the parameters associated with each node in this polygon.
  //! Raises exception if array size doesn't much number of polygon nodes.
  Standard_EXPORT void SetParameters(const occ::handle<NCollection_HArray1<double>>& theParameters);

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

public:
  //! Returns the table of nodes for this polygon.
  //! A node value is an index in the table of nodes specific to an existing triangulation of a
  //! shape.
  const NCollection_Array1<int>& Nodes() const { return myNodes; }

  //! Returns the table of the parameters associated with each node in this polygon.
  //! Warning! Use the function HasParameters to check if parameters are associated with the nodes
  //! in this polygon.
  const occ::handle<NCollection_HArray1<double>>& Parameters() const { return myParameters; }

  Standard_DEPRECATED("Deprecated method, SetNode() should be used instead")
  NCollection_Array1<int>& ChangeNodes() { return myNodes; }

  Standard_DEPRECATED("Deprecated method, SetParameter() should be used instead")
  NCollection_Array1<double>& ChangeParameters() { return myParameters->ChangeArray1(); }

private:
  double                                   myDeflection;
  NCollection_Array1<int>                  myNodes;
  occ::handle<NCollection_HArray1<double>> myParameters;
};

#endif // _Poly_PolygonOnTriangulation_HeaderFile
