// Created on: 1992-01-30
// Created by: Didier PIFFAULT
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _TopTrans_CurveTransition_HeaderFile
#define _TopTrans_CurveTransition_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <gp_Dir.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_State.hxx>


//! This algorithm  is used to  compute the transition
//! of a Curve intersecting a curvilinear boundary.
//!
//! The geometric  elements  are described locally  at
//! the   intersection   point  by    a   second order
//! development.
//!
//! The curve is described  by the intersection point,
//! the tangent vector and the curvature.
//!
//! The  boundary  is described  by   a set  of  curve
//! elements, a curve element is either :
//!
//! - A curve.
//!
//! - A curve and an orientation  called a half-curve,
//! the boundary  of the curve is  before or after the
//! intersection point depending on the orientation.
class TopTrans_CurveTransition 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Create an empty  Curve Transition.
  Standard_EXPORT TopTrans_CurveTransition();
  
  //! Initialize a Transition with the local description
  //! of a Curve.
  Standard_EXPORT void Reset (const gp_Dir& Tgt, const gp_Dir& Norm, const Standard_Real Curv);
  
  //! Initialize a Transition with the local description of a straight line.
  Standard_EXPORT void Reset (const gp_Dir& Tgt);
  
  //! Add  a curve element to the  boundary.    If Or is
  //! REVERSED  the curve  is   before the intersection,
  //! else if  Or  is FORWARD  the   curv  is after  the
  //! intersection   and    if   Or  is   INTERNAL   the
  //! intersection is in the middle of the curv.
  Standard_EXPORT void Compare (const Standard_Real Tole, const gp_Dir& Tang, const gp_Dir& Norm, const Standard_Real Curv, const TopAbs_Orientation S, const TopAbs_Orientation Or);
  
  //! returns   the  state   of  the   curve  before the
  //! intersection, this is the position relative to the
  //! boundary of a point very close to the intersection
  //! on the negative side of the tangent.
  Standard_EXPORT TopAbs_State StateBefore() const;
  
  //! returns  the    state of  the  curve   after   the
  //! intersection, this is the position relative to the
  //! boundary of a point very close to the intersection
  //! on the positive side of the tangent.
  Standard_EXPORT TopAbs_State StateAfter() const;




protected:





private:

  
  //! Compare two curvature and return true  if N1,C1 is
  //! before N2,C2 in the edge orientation
  Standard_EXPORT Standard_Boolean IsBefore (const Standard_Real Tole, const Standard_Real Angl, const gp_Dir& Nor1, const Standard_Real Cur1, const gp_Dir& Nor2, const Standard_Real Cur2) const;
  
  //! Compare two angles at tolerance Tole
  Standard_EXPORT Standard_Integer Compare (const Standard_Real Ang1, const Standard_Real Ang2, const Standard_Real Tole) const;


  gp_Dir myTgt;
  gp_Dir myNorm;
  Standard_Real myCurv;
  Standard_Boolean Init;
  gp_Dir TgtFirst;
  gp_Dir NormFirst;
  Standard_Real CurvFirst;
  TopAbs_Orientation TranFirst;
  gp_Dir TgtLast;
  gp_Dir NormLast;
  Standard_Real CurvLast;
  TopAbs_Orientation TranLast;


};







#endif // _TopTrans_CurveTransition_HeaderFile
