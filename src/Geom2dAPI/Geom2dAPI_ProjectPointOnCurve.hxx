// Created on: 1994-03-23
// Created by: Bruno DUMORTIER
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

#ifndef _Geom2dAPI_ProjectPointOnCurve_HeaderFile
#define _Geom2dAPI_ProjectPointOnCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Extrema_ExtPC2d.hxx>
#include <Geom2dAdaptor_Curve.hxx>
class Standard_OutOfRange;
class StdFail_NotDone;
class gp_Pnt2d;
class Geom2d_Curve;
class Extrema_ExtPC2d;



//! This class implements methods for computing all the orthogonal
//! projections of a 2D point onto a 2D curve.
class Geom2dAPI_ProjectPointOnCurve 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs an empty projector algorithm. Use an Init
  //! function to define the point and the curve on which it is going to work.
  Standard_EXPORT Geom2dAPI_ProjectPointOnCurve();
  
  //! Create the projection  of a  point  <P> on a curve
  //! <Curve>
  Standard_EXPORT Geom2dAPI_ProjectPointOnCurve(const gp_Pnt2d& P, const Handle(Geom2d_Curve)& Curve);
  
  //! Create  the projection  of a point <P>  on a curve
  //! <Curve> limited by the two   points of parameter Umin and Usup.
  //! Warning
  //! Use the function NbPoints to obtain the number of solutions. If
  //! projection fails, NbPoints returns 0.
  Standard_EXPORT Geom2dAPI_ProjectPointOnCurve(const gp_Pnt2d& P, const Handle(Geom2d_Curve)& Curve, const Standard_Real Umin, const Standard_Real Usup);
  
  //! Initializes this algorithm with the given arguments, and
  //! computes the orthogonal  projections  of a  point  <P> on a curve <Curve>
  Standard_EXPORT void Init (const gp_Pnt2d& P, const Handle(Geom2d_Curve)& Curve);
  
  //! Initializes this algorithm with the given arguments, and
  //! computes the orthogonal projections of the point P onto the portion
  //! of the curve Curve limited by the two points of parameter Umin and Usup.
  Standard_EXPORT void Init (const gp_Pnt2d& P, const Handle(Geom2d_Curve)& Curve, const Standard_Real Umin, const Standard_Real Usup);
  
  //! return the number of of computed
  //! orthogonal projectionn points.
  Standard_EXPORT Standard_Integer NbPoints() const;
Standard_EXPORT operator Standard_Integer() const;
  
  //! Returns the orthogonal projection
  //! on the curve. Index is a number of a computed point.
  //! Exceptions
  //! Standard_OutOfRange if Index is not in the range [ 1,NbPoints ], where
  //! NbPoints is the number of solution points.
  Standard_EXPORT gp_Pnt2d Point (const Standard_Integer Index) const;
  
  //! Returns the parameter on the curve
  //! of a point which is the orthogonal projection. Index is a number of a
  //! computed projected point.
  //! Exceptions
  //! Standard_OutOfRange if Index is not in the range [ 1,NbPoints ], where
  //! NbPoints is the number of solution points.
  Standard_EXPORT Standard_Real Parameter (const Standard_Integer Index) const;
  
  //! Returns the parameter on the curve
  //! of a point which is the orthogonal projection. Index is a number of a
  //! computed projected point.
  //! Exceptions
  //! Standard_OutOfRange if Index is not in the range [ 1,NbPoints ], where
  //! NbPoints is the number of solution points
  Standard_EXPORT void Parameter (const Standard_Integer Index, Standard_Real& U) const;
  
  //! Computes the distance between the
  //! point and its computed orthogonal projection on the curve. Index is a
  //! number of computed projected point.
  //! Exceptions
  //! Standard_OutOfRange if Index is not in the range [ 1,NbPoints ], where
  //! NbPoints is the number of solution points.
  Standard_EXPORT Standard_Real Distance (const Standard_Integer Index) const;
  
  //! Returns the nearest orthogonal projection of the point on the curve.
  //! Exceptions
  //! StdFail_NotDone if this algorithm fails.
  Standard_EXPORT gp_Pnt2d NearestPoint() const;
Standard_EXPORT operator gp_Pnt2d() const;
  
  //! Returns the parameter on the curve
  //! of the nearest orthogonal projection of the point.
  //! Exceptions
  //! StdFail_NotDone if this algorithm fails.
  Standard_EXPORT Standard_Real LowerDistanceParameter() const;
  
  //! Computes the distance between the
  //! point and its nearest orthogonal projection on the curve.
  //! Exceptions
  //! StdFail_NotDone if this algorithm fails.
  Standard_EXPORT Standard_Real LowerDistance() const;
Standard_EXPORT operator Standard_Real() const;
  
  //! return the algorithmic object from Extrema
    const Extrema_ExtPC2d& Extrema() const;

private:

  Standard_Boolean myIsDone;
  Standard_Integer myIndex;
  Extrema_ExtPC2d myExtPC;
  Geom2dAdaptor_Curve myC;


};


#include <Geom2dAPI_ProjectPointOnCurve.lxx>





#endif // _Geom2dAPI_ProjectPointOnCurve_HeaderFile
