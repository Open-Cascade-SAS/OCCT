// Created on: 1994-09-06
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Geom2dLProp_CurAndInf2d_HeaderFile
#define _Geom2dLProp_CurAndInf2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <LProp_CurAndInf.hxx>
class Geom2d_Curve;


//! An algorithm for computing local properties of a curve.
//! These properties include:
//! - the maximum and minimum curvatures
//! - the inflection points.
//! A CurAndInf2d object provides the framework for:
//! - defining the curve to be analyzed
//! - implementing the computation algorithms
//! - consulting the results.
class Geom2dLProp_CurAndInf2d  : public LProp_CurAndInf
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Initializes the framework.
  //! Note: The curve on which the local properties are
  //! computed is defined using one of the following
  //! functions: Perform, PerformCurExt or PerformInf.
  Standard_EXPORT Geom2dLProp_CurAndInf2d();
  
  //! For the curve C, Computes both the
  //! inflection points and the maximum and minimum curvatures.
  Standard_EXPORT void Perform (const Handle(Geom2d_Curve)& C);
  
  //! For the curve C, Computes the locals extremas of curvature.
  Standard_EXPORT void PerformCurExt (const Handle(Geom2d_Curve)& C);
  
  //! For the curve C, Computes the inflections.
  //! After computation, the following functions can be used:
  //! - IsDone to check if the computation was successful
  //! - NbPoints to obtain the number of computed particular points
  //! - Parameter to obtain the parameter on the curve for
  //! each particular point
  //! - Type to check if the point is an inflection point or an
  //! extremum of curvature of the curve C.
  //! Warning
  //! These functions can be used to analyze a series of
  //! curves, however it is necessary to clear the table of
  //! results between each computation.
  Standard_EXPORT void PerformInf (const Handle(Geom2d_Curve)& C);
  
  //! True if the solutions are found.
  Standard_EXPORT Standard_Boolean IsDone() const;




protected:





private:



  Standard_Boolean isDone;


};







#endif // _Geom2dLProp_CurAndInf2d_HeaderFile
