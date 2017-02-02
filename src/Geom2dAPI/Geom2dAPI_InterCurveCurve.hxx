// Created on: 1994-03-24
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

#ifndef _Geom2dAPI_InterCurveCurve_HeaderFile
#define _Geom2dAPI_InterCurveCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
class Geom2d_Curve;
class Standard_OutOfRange;
class Standard_NullObject;
class gp_Pnt2d;
class Geom2dInt_GInter;


//! This class implements methods for computing
//! -       the intersections between  two 2D curves,
//! -       the self-intersections of a  2D curve.
//! Using the InterCurveCurve algorithm allows to get the following results:
//! -      intersection points in the  case of cross intersections,
//! -      intersection segments in the case of tangential intersections,
//! -       nothing in the case of no intersections.
class Geom2dAPI_InterCurveCurve 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Create an empty intersector. Use the
  //! function Init for further initialization of the intersection
  //! algorithm by curves or curve.
  Standard_EXPORT Geom2dAPI_InterCurveCurve();
  
  //! Creates an object and computes the
  //! intersections between the curves C1 and C2.
  Standard_EXPORT Geom2dAPI_InterCurveCurve(const Handle(Geom2d_Curve)& C1, const Handle(Geom2d_Curve)& C2, const Standard_Real Tol = 1.0e-6);
  

  //! Creates an object and computes self-intersections of the curve C1.
  //! Tolerance value Tol, defaulted to 1.0e-6, defines the precision of
  //! computing the intersection points.
  //! In case of a tangential intersection, Tol also defines the
  //! size of intersection segments (limited portions of the curves)
  //! where the distance between all points from two curves (or a curve
  //! in case of self-intersection) is less than Tol.
  //! Warning
  //! Use functions NbPoints and NbSegments to obtain the number of
  //! solutions. If the algorithm finds no intersections NbPoints and
  //! NbSegments return 0.
  Standard_EXPORT Geom2dAPI_InterCurveCurve(const Handle(Geom2d_Curve)& C1, const Standard_Real Tol = 1.0e-6);
  
  //! Initializes an algorithm with the
  //! given arguments and computes the intersections between the curves C1. and C2.
  Standard_EXPORT void Init (const Handle(Geom2d_Curve)& C1, const Handle(Geom2d_Curve)& C2, const Standard_Real Tol = 1.0e-6);
  
  //! Initializes an algorithm with the
  //! given arguments and computes the self-intersections of the curve C1.
  //! Tolerance value Tol, defaulted to 1.0e-6, defines the precision of
  //! computing the intersection points. In case of a tangential
  //! intersection, Tol also defines the size of intersection segments
  //! (limited portions of the curves) where the distance between all
  //! points from two curves (or a curve in case of self-intersection) is less than Tol.
  //! Warning
  //! Use functions NbPoints and NbSegments to obtain the number
  //! of solutions. If the algorithm finds no intersections NbPoints
  //! and NbSegments return 0.
  Standard_EXPORT void Init (const Handle(Geom2d_Curve)& C1, const Standard_Real Tol = 1.0e-6);
  
  //! Returns the number of intersection-points in case of cross intersections.
  //! NbPoints returns 0 if no intersections were found.
  Standard_EXPORT Standard_Integer NbPoints() const;
  
  //! Returns the intersection point of index Index.
  //! Intersection points are computed in case of cross intersections with a
  //! precision equal to the tolerance value assigned at the time of
  //! construction or in the function Init (this value is defaulted to 1.0e-6).
  //! Exceptions
  //! Standard_OutOfRange if index is not in the range [ 1,NbPoints ], where
  //! NbPoints is the number of computed intersection points
  Standard_EXPORT gp_Pnt2d Point (const Standard_Integer Index) const;
  
  //! Returns the number of tangential intersections.
  //! NbSegments returns 0 if no intersections were found
  Standard_EXPORT Standard_Integer NbSegments() const;
  
  //! Use this syntax only to get
  //! solutions of tangential intersection between two curves.
  //! Output values Curve1 and Curve2 are the intersection segments on the
  //! first curve and on the second curve accordingly. Parameter Index
  //! defines a number of computed solution.
  //! An intersection segment is a portion of an initial curve limited
  //! by two points. The distance from each point of this segment to the
  //! other curve is less or equal to the tolerance value assigned at the
  //! time of construction or in function Init (this value is defaulted to 1.0e-6).
  //! Exceptions
  //! Standard_OutOfRange if Index is not in the range [ 1,NbSegments ],
  //! where NbSegments is the number of computed tangential intersections.
  //! Standard_NullObject if the algorithm is initialized for the
  //! computing of self-intersections on a curve.
  Standard_EXPORT void Segment (const Standard_Integer Index, Handle(Geom2d_Curve)& Curve1, Handle(Geom2d_Curve)& Curve2) const;
  
  //! return the algorithmic object from Intersection.
    const Geom2dInt_GInter& Intersector() const;




protected:





private:



  Standard_Boolean myIsDone;
  Handle(Geom2d_Curve) myCurve1;
  Handle(Geom2d_Curve) myCurve2;
  Geom2dInt_GInter myIntersector;


};


#include <Geom2dAPI_InterCurveCurve.lxx>





#endif // _Geom2dAPI_InterCurveCurve_HeaderFile
