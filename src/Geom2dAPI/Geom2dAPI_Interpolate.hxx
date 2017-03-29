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

#ifndef _Geom2dAPI_Interpolate_HeaderFile
#define _Geom2dAPI_Interpolate_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <Standard_Boolean.hxx>
#include <TColgp_HArray1OfVec2d.hxx>
#include <TColStd_HArray1OfBoolean.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColgp_Array1OfVec2d.hxx>
class Geom2d_BSplineCurve;
class StdFail_NotDone;
class Standard_ConstructionError;
class gp_Vec2d;


//! This  class  is  used  to  interpolate a  BsplineCurve
//! passing   through  an  array  of  points,  with  a  C2
//! Continuity if tangency is not requested at the point.
//! If tangency is requested at the point the continuity will
//! be C1.  If Perodicity is requested the curve will be closed
//! and the junction will be the first point given. The curve will than be only C1
//! The curve is defined by a table of points through which it passes, and if required
//! by a parallel table of reals which gives the value of the parameter of each point through
//! which the resulting BSpline curve passes, and by vectors tangential to these points.
//! An Interpolate object provides a framework for: defining the constraints of the BSpline curve,
//! -   implementing the interpolation algorithm, and consulting the results.
class Geom2dAPI_Interpolate 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Tolerance is to check if the points are not too close to one an other
  //! It is also used to check if the tangent vector is not too small.
  //! There should be at least 2 points
  //! if PeriodicFlag is True then the curve will be periodic.
  Standard_EXPORT Geom2dAPI_Interpolate(const Handle(TColgp_HArray1OfPnt2d)& Points, 
                                        const Standard_Boolean PeriodicFlag, 
                                        const Standard_Real Tolerance);
  
  //! if PeriodicFlag is True then the curve will be periodic
  //! Warning:
  //! There should be as many parameters as there are points
  //! except if PeriodicFlag is True : then there should be one more
  //! parameter to close the curve
  Standard_EXPORT Geom2dAPI_Interpolate(const Handle(TColgp_HArray1OfPnt2d)& Points, 
                                        const Handle(TColStd_HArray1OfReal)& Parameters, 
                                        const Standard_Boolean PeriodicFlag, 
                                        const Standard_Real Tolerance);
  
  //! Assigns this constrained BSpline curve to be
  //! tangential to vectors InitialTangent and FinalTangent
  //! at its first and last points respectively (i.e.
  //! the first and last points of the table of
  //! points through which the curve passes, as
  //! defined at the time of initialization).
  //! <Scale> - boolean flag defining whether tangent vectors are to
  //! be scaled according to derivatives of lagrange interpolation.
  Standard_EXPORT void Load(const gp_Vec2d& InitialTangent, 
                            const gp_Vec2d& FinalTangent, 
                            const Standard_Boolean Scale = Standard_True);
  
  //! Assigns this constrained BSpline curve to be
  //! tangential to vectors defined in the table Tangents,
  //! which is parallel to the table of points
  //! through which the curve passes, as
  //! defined at the time of initialization. Vectors
  //! in the table Tangents are defined only if
  //! the flag given in the parallel table
  //! TangentFlags is true: only these vectors
  //! are set as tangency constraints.
  //! <Scale> - boolean flag defining whether tangent vectors are to
  //! be scaled according to derivatives of lagrange interpolation.
  Standard_EXPORT void Load(const TColgp_Array1OfVec2d& Tangents, 
                            const Handle(TColStd_HArray1OfBoolean)& TangentFlags, 
                            const Standard_Boolean Scale = Standard_True);
  
  //! Clears all tangency constraints on this
  //! constrained BSpline curve (as initialized by the function Load).
  Standard_EXPORT void ClearTangents();
  
  //! Computes the constrained BSpline curve. Use the function IsDone to verify that the
  //! computation is successful, and then the function Curve to obtain the result.
  Standard_EXPORT void Perform();
  
  //! Returns the computed BSpline curve. Raises  StdFail_NotDone if the interpolation fails.
  Standard_EXPORT const Handle(Geom2d_BSplineCurve)& Curve() const;
Standard_EXPORT operator Handle(Geom2d_BSplineCurve)() const;
  
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
  Handle(TColgp_HArray1OfPnt2d) myPoints;
  Standard_Boolean myIsDone;
  Handle(Geom2d_BSplineCurve) myCurve;
  Handle(TColgp_HArray1OfVec2d) myTangents;
  Handle(TColStd_HArray1OfBoolean) myTangentFlags;
  Handle(TColStd_HArray1OfReal) myParameters;
  Standard_Boolean myPeriodic;
  Standard_Boolean myTangentRequest;


};







#endif // _Geom2dAPI_Interpolate_HeaderFile
