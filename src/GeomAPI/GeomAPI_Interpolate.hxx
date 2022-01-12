// Created on: 1994-08-18
// Created by: Laurent PAINNOT
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

#ifndef _GeomAPI_Interpolate_HeaderFile
#define _GeomAPI_Interpolate_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColgp_HArray1OfVec.hxx>
#include <TColStd_HArray1OfBoolean.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <Geom_BSplineCurve.hxx>

class gp_Vec;


//! This  class  is  used  to  interpolate a  BsplineCurve
//! passing   through  an  array  of  points,  with  a  C2
//! Continuity if tangency is not requested at the point.
//! If tangency is requested at the point the continuity will
//! be C1.  If Perodicity is requested the curve will be closed
//! and the junction will be the first point given. The curve
//! will than be only C1
//! Describes functions for building a constrained 3D BSpline curve.
//! The curve is defined by a table of points
//! through which it passes, and if required:
//! -   by a parallel table of reals which gives the
//! value of the parameter of each point through
//! which the resulting BSpline curve passes, and
//! -   by vectors tangential to these points.
//! An Interpolate object provides a framework for:
//! -   defining the constraints of the BSpline curve,
//! -   implementing the interpolation algorithm, and
//! -   consulting the results.
class GeomAPI_Interpolate 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Initializes an algorithm for constructing a
  //! constrained BSpline curve passing through the points of the table   Points.
  //! Tangential vectors can then be assigned, using the function Load.
  //! If PeriodicFlag is true, the constrained BSpline
  //! curve will be periodic and closed. In this case,
  //! the junction point is the first point of the table Points.
  //! The tolerance value Tolerance is used to check that:
  //! -   points are not too close to each other, or
  //! -   tangential vectors (defined using the
  //! function Load) are not too small.
  //! The resulting BSpline curve will be "C2"
  //! continuous, except where a tangency
  //! constraint is defined on a point through which
  //! the curve passes (by using the Load function).
  //! In this case, it will be only "C1" continuous.
  //! Once all the constraints are defined, use the
  //! function Perform to compute the curve.
  //! Warning
  //! -   There must be at least 2 points in the table Points.
  //! -   If PeriodicFlag is false, there must be as
  //! many parameters in the array Parameters as
  //! there are points in the array Points.
  //! -   If PeriodicFlag is true, there must be one
  //! more parameter in the table Parameters: this
  //! is used to give the parameter on the
  //! resulting BSpline curve of the junction point
  //! of the curve (which is also the first point of the table Points).
  //! Exceptions
  //! -   Standard_ConstructionError if the
  //! distance between two consecutive points in
  //! the table Points is less than or equal to Tolerance.
  //! -   Standard_OutOfRange if:
  //! -   there are less than two points in the table Points, or
  //! -   conditions relating to the respective
  //! number of elements in the parallel tables
  //! Points and Parameters are not respected.
  Standard_EXPORT GeomAPI_Interpolate(const Handle(TColgp_HArray1OfPnt)& Points, const Standard_Boolean PeriodicFlag, const Standard_Real Tolerance);
  
  //! Initializes an algorithm for constructing a
  //! constrained BSpline curve passing through the points of the table
  //! Points, where the parameters of each of its
  //! points are given by the parallel table Parameters.
  //! Tangential vectors can then be assigned, using the function Load.
  //! If PeriodicFlag is true, the constrained BSpline
  //! curve will be periodic and closed. In this case,
  //! the junction point is the first point of the table Points.
  //! The tolerance value Tolerance is used to check that:
  //! -   points are not too close to each other, or
  //! -   tangential vectors (defined using the
  //! function Load) are not too small.
  //! The resulting BSpline curve will be "C2"
  //! continuous, except where a tangency
  //! constraint is defined on a point through which
  //! the curve passes (by using the Load function).
  //! In this case, it will be only "C1" continuous.
  //! Once all the constraints are defined, use the
  //! function Perform to compute the curve.
  //! Warning
  //! -   There must be at least 2 points in the table Points.
  //! -   If PeriodicFlag is false, there must be as
  //! many parameters in the array Parameters as
  //! there are points in the array Points.
  //! -   If PeriodicFlag is true, there must be one
  //! more parameter in the table Parameters: this
  //! is used to give the parameter on the
  //! resulting BSpline curve of the junction point
  //! of the curve (which is also the first point of the table Points).
  //! Exceptions
  //! -   Standard_ConstructionError if the
  //! distance between two consecutive points in
  //! the table Points is less than or equal to Tolerance.
  //! -   Standard_OutOfRange if:
  //! -   there are less than two points in the table Points, or
  //! -   conditions relating to the respective
  //! number of elements in the parallel tables
  //! Points and Parameters are not respected.
  Standard_EXPORT GeomAPI_Interpolate(const Handle(TColgp_HArray1OfPnt)& Points, const Handle(TColStd_HArray1OfReal)& Parameters, const Standard_Boolean PeriodicFlag, const Standard_Real Tolerance);
  
  //! Assigns this constrained BSpline curve to be
  //! tangential to vectors InitialTangent and FinalTangent
  //! at its first and last points respectively (i.e.
  //! the first and last points of the table of
  //! points through which the curve passes, as
  //! defined at the time of initialization).
  Standard_EXPORT void Load (const gp_Vec& InitialTangent, const gp_Vec& FinalTangent, const Standard_Boolean Scale = Standard_True);
  
  //! Assigns this constrained BSpline curve to be
  //! tangential to vectors defined in the table Tangents,
  //! which is parallel to the table of points
  //! through which the curve passes, as
  //! defined at the time of initialization. Vectors
  //! in the table Tangents are defined only if
  //! the flag given in the parallel table
  //! TangentFlags is true: only these vectors
  //! are set as tangency constraints.
  Standard_EXPORT void Load (const TColgp_Array1OfVec& Tangents, const Handle(TColStd_HArray1OfBoolean)& TangentFlags, const Standard_Boolean Scale = Standard_True);
  
  //! Clears all tangency constraints on this
  //! constrained BSpline curve (as initialized by the function Load).
  Standard_EXPORT void ClearTangents();
  
  //! Computes the constrained BSpline curve.
  //! Use the function IsDone to verify that the
  //! computation is successful, and then the function Curve to obtain the result.
  Standard_EXPORT void Perform();
  
  //! Returns the computed BSpline curve.
  //! Raises StdFail_NotDone if the interpolation fails.
  Standard_EXPORT const Handle(Geom_BSplineCurve)& Curve() const;
Standard_EXPORT operator Handle(Geom_BSplineCurve)() const;
  
  //! Returns true if the constrained BSpline curve is successfully constructed.
  //! Note: in this case, the result is given by the function Curve.
  Standard_EXPORT Standard_Boolean IsDone() const;




protected:





private:

  
  //! Interpolates in a non periodic fashion
  Standard_EXPORT void PerformNonPeriodic();
  
  //! Interpolates in a C1 periodic fashion
  Standard_EXPORT void PerformPeriodic();


  Standard_Real myTolerance;
  Handle(TColgp_HArray1OfPnt) myPoints;
  Standard_Boolean myIsDone;
  Handle(Geom_BSplineCurve) myCurve;
  Handle(TColgp_HArray1OfVec) myTangents;
  Handle(TColStd_HArray1OfBoolean) myTangentFlags;
  Handle(TColStd_HArray1OfReal) myParameters;
  Standard_Boolean myPeriodic;
  Standard_Boolean myTangentRequest;


};







#endif // _GeomAPI_Interpolate_HeaderFile
