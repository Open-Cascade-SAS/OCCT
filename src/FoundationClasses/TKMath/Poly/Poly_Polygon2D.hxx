// Created on: 1995-03-09
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

#ifndef _Poly_Polygon2D_HeaderFile
#define _Poly_Polygon2D_HeaderFile

#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Transient.hxx>

//! Provides a polygon in 2D space (for example, in the
//! parametric space of a surface). It is generally an
//! approximate representation of a curve.
//! A Polygon2D is defined by a table of nodes. Each node is
//! a 2D point. If the polygon is closed, the point of closure is
//! repeated at the end of the table of nodes.
class Poly_Polygon2D : public Standard_Transient
{
public:
  //! Constructs a 2D polygon with specified number of nodes.
  Standard_EXPORT explicit Poly_Polygon2D(const int theNbNodes);

  //! Constructs a 2D polygon defined by the table of points, <Nodes>.
  Standard_EXPORT Poly_Polygon2D(const NCollection_Array1<gp_Pnt2d>& Nodes);

  //! Returns the deflection of this polygon.
  //! Deflection is used in cases where the polygon is an
  //! approximate representation of a curve. Deflection
  //! represents the maximum distance permitted between any
  //! point on the curve and the corresponding point on the polygon.
  //! By default the deflection value is equal to 0. An algorithm
  //! using this 2D polygon with a deflection value equal to 0
  //! considers that it is working with a true polygon and not with
  //! an approximate representation of a curve. The Deflection
  //! function is used to modify the deflection value of this polygon.
  //! The deflection value can be used by any algorithm working with 2D polygons.
  //! For example:
  //! -   An algorithm may use a unique deflection value for all
  //! its polygons. In this case it is not necessary to use the
  //! Deflection function.
  //! -   Or an algorithm may want to attach a different
  //! deflection to each polygon. In this case, the Deflection
  //! function is used to set a value on each polygon, and
  //! later to fetch the value.
  double Deflection() const { return myDeflection; }

  //! Sets the deflection of this polygon.
  void Deflection(const double theDefl) { myDeflection = theDefl; }

  //! Returns the number of nodes in this polygon.
  //! Note: If the polygon is closed, the point of closure is
  //! repeated at the end of its table of nodes. Thus, on a closed
  //! triangle, the function NbNodes returns 4.
  int NbNodes() const { return myNodes.Length(); }

  //! Returns the table of nodes for this polygon.
  const NCollection_Array1<gp_Pnt2d>& Nodes() const { return myNodes; }

  //! Returns the table of nodes for this polygon.
  NCollection_Array1<gp_Pnt2d>& ChangeNodes() { return myNodes; }

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

  DEFINE_STANDARD_RTTIEXT(Poly_Polygon2D, Standard_Transient)

private:
  double                       myDeflection;
  NCollection_Array1<gp_Pnt2d> myNodes;
};

#endif // _Poly_Polygon2D_HeaderFile
