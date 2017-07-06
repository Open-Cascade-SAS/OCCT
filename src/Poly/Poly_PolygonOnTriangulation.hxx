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

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Standard_Transient.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Standard_NullObject;


class Poly_PolygonOnTriangulation;
DEFINE_STANDARD_HANDLE(Poly_PolygonOnTriangulation, Standard_Transient)

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

public:

  
  //! Constructs a 3D polygon on the triangulation of a shape,
  //! defined by the table of nodes, <Nodes>.
  Standard_EXPORT Poly_PolygonOnTriangulation(const TColStd_Array1OfInteger& Nodes);
  

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
  Standard_EXPORT Poly_PolygonOnTriangulation(const TColStd_Array1OfInteger& Nodes, const TColStd_Array1OfReal& Parameters);

  //! Creates a copy of current polygon
  Standard_EXPORT virtual Handle(Poly_PolygonOnTriangulation) Copy() const;

  //! Returns the deflection of this polygon
  Standard_EXPORT Standard_Real Deflection() const;
  
  //! Sets the deflection of this polygon to D.
  //! See more on deflection in Poly_Polygones2D.
  Standard_EXPORT void Deflection (const Standard_Real D);
  

  //! Returns the number of nodes for this polygon.
  //! Note: If the polygon is closed, the point of closure is
  //! repeated at the end of its table of nodes. Thus, on a closed
  //! triangle, the function NbNodes returns 4.
    Standard_Integer NbNodes() const;
  
  //! Returns the table of nodes for this polygon. A node value
  //! is an index in the table of nodes specific to an existing
  //! triangulation of a shape.
  Standard_EXPORT const TColStd_Array1OfInteger& Nodes() const;
  

  //! Returns true if parameters are associated with the nodes in this polygon.
  Standard_EXPORT Standard_Boolean HasParameters() const;
  
  //! Returns the table of the parameters associated with each node in this polygon.
  //! Warning
  //! Use the function HasParameters to check if parameters
  //! are associated with the nodes in this polygon.
  Standard_EXPORT Handle(TColStd_HArray1OfReal) Parameters() const;




  DEFINE_STANDARD_RTTIEXT(Poly_PolygonOnTriangulation,Standard_Transient)

protected:




private:


  Standard_Real myDeflection;
  TColStd_Array1OfInteger myNodes;
  Handle(TColStd_HArray1OfReal) myParameters;


};


#include <Poly_PolygonOnTriangulation.lxx>





#endif // _Poly_PolygonOnTriangulation_HeaderFile
