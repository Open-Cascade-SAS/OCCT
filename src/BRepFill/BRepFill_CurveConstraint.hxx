// Created on: 1997-10-31
// Created by: Joelle CHAUVET
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _BRepFill_CurveConstraint_HeaderFile
#define _BRepFill_CurveConstraint_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <GeomPlate_CurveConstraint.hxx>
#include <Standard_Integer.hxx>

class BRepFill_CurveConstraint;
DEFINE_STANDARD_HANDLE(BRepFill_CurveConstraint, GeomPlate_CurveConstraint)

//! same as CurveConstraint from GeomPlate
//! with BRepAdaptor_Surface instead of
//! GeomAdaptor_Surface
class BRepFill_CurveConstraint : public GeomPlate_CurveConstraint
{

public:

  
  //! Create a constraint
  //! Order is the order of the constraint. The possible values for order are -1,0,1,2.
  //! Order i means constraints Gi
  //! Npt is the number of points associated with the constraint.
  //! TolDist is the maximum error to satisfy for G0 constraints
  //! TolAng is the maximum error to satisfy for G1 constraints
  //! TolCurv is the maximum error to satisfy for G2 constraints
  //! These errors can be replaced by laws of criterion.
  Standard_EXPORT BRepFill_CurveConstraint(const Handle(Adaptor3d_CurveOnSurface)& Boundary, const Standard_Integer Order, const Standard_Integer NPt = 10, const Standard_Real TolDist = 0.0001, const Standard_Real TolAng = 0.01, const Standard_Real TolCurv = 0.1);
  
  Standard_EXPORT BRepFill_CurveConstraint(const Handle(Adaptor3d_Curve)& Boundary, const Standard_Integer Tang, const Standard_Integer NPt = 10, const Standard_Real TolDist = 0.0001);




  DEFINE_STANDARD_RTTIEXT(BRepFill_CurveConstraint,GeomPlate_CurveConstraint)

protected:




private:




};







#endif // _BRepFill_CurveConstraint_HeaderFile
