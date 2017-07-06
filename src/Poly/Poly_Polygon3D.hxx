// Created on: 1995-03-07
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

#ifndef _Poly_Polygon3D_HeaderFile
#define _Poly_Polygon3D_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Standard_Transient.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Standard_NullObject;


class Poly_Polygon3D;
DEFINE_STANDARD_HANDLE(Poly_Polygon3D, Standard_Transient)

//! This class Provides a polygon in 3D space. It is generally an approximate representation of a curve.
//! A Polygon3D is defined by a table of nodes. Each node is
//! a 3D point. If the polygon is closed, the point of closure is
//! repeated at the end of the table of nodes.
//! If the polygon is an approximate representation of a curve,
//! you can associate with each of its nodes the value of the
//! parameter of the corresponding point on the curve.
class Poly_Polygon3D : public Standard_Transient
{

public:

  
  //! onstructs a 3D polygon defined by the table of points, Nodes.
  Standard_EXPORT Poly_Polygon3D(const TColgp_Array1OfPnt& Nodes);
  
  //! Constructs a 3D polygon defined by
  //! the table of points, Nodes, and the parallel table of
  //! parameters, Parameters, where each value of the table
  //! Parameters is the parameter of the corresponding point
  //! on the curve approximated by the constructed polygon.
  //! Warning
  //! Both the Nodes and Parameters tables must have the
  //! same bounds. This property is not checked at construction time.
  Standard_EXPORT Poly_Polygon3D(const TColgp_Array1OfPnt& Nodes, const TColStd_Array1OfReal& Parameters);

  //! Creates a copy of current polygon
  Standard_EXPORT virtual Handle(Poly_Polygon3D) Copy() const;
  
  //! Returns the deflection of this polygon
  Standard_EXPORT Standard_Real Deflection() const;
  
  //! Sets the deflection of this polygon to D. See more on deflection in Poly_Polygon2D
  Standard_EXPORT void Deflection (const Standard_Real D);
  
  //! Returns the number of nodes in this polygon.
  //! Note: If the polygon is closed, the point of closure is
  //! repeated at the end of its table of nodes. Thus, on a closed
  //! triangle the function NbNodes returns 4.
    Standard_Integer NbNodes() const;
  
  //! Returns the table of nodes for this polygon.
  Standard_EXPORT const TColgp_Array1OfPnt& Nodes() const;
  
  //! Returns the table of the parameters associated with each node in this polygon.
  //! HasParameters function checks if   parameters are associated with the nodes of this polygon.
  Standard_EXPORT Standard_Boolean HasParameters() const;
  
  //! Returns true if parameters are associated with the nodes
  //! in this polygon.
  Standard_EXPORT const TColStd_Array1OfReal& Parameters() const;
  
  //! Returns the table of the parameters associated with each node in this polygon.
  //! ChangeParameters function returnes the  array as shared. Therefore if the table is selected by
  //! reference you can, by simply modifying it, directly modify
  //! the data structure of this polygon.
  Standard_EXPORT TColStd_Array1OfReal& ChangeParameters() const;




  DEFINE_STANDARD_RTTIEXT(Poly_Polygon3D,Standard_Transient)

protected:




private:


  Standard_Real myDeflection;
  TColgp_Array1OfPnt myNodes;
  Handle(TColStd_HArray1OfReal) myParameters;


};


#include <Poly_Polygon3D.lxx>





#endif // _Poly_Polygon3D_HeaderFile
