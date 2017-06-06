// Created on: 1994-03-17
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

#ifndef _GeomAPI_ProjectPointOnCurve_HeaderFile
#define _GeomAPI_ProjectPointOnCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Extrema_ExtPC.hxx>
#include <GeomAdaptor_Curve.hxx>
class Standard_OutOfRange;
class StdFail_NotDone;
class gp_Pnt;
class Geom_Curve;
class Extrema_ExtPC;



//! This class implements methods for  computing all the orthogonal
//! projections of a 3D point onto a  3D curve.
class GeomAPI_ProjectPointOnCurve 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an empty object. Use an
  //! Init function for further initialization.
  Standard_EXPORT GeomAPI_ProjectPointOnCurve();
  
  //! Create the projection  of a  point  <P> on a curve
  //! <Curve>
  Standard_EXPORT GeomAPI_ProjectPointOnCurve(const gp_Pnt& P, const Handle(Geom_Curve)& Curve);
  
  //! Create  the projection  of a point <P>  on a curve
  //! <Curve> limited by the two points of parameter Umin and Usup.
  Standard_EXPORT GeomAPI_ProjectPointOnCurve(const gp_Pnt& P, const Handle(Geom_Curve)& Curve, const Standard_Real Umin, const Standard_Real Usup);
  
  //! Init the projection  of a  point  <P> on a curve
  //! <Curve>
  Standard_EXPORT void Init (const gp_Pnt& P, const Handle(Geom_Curve)& Curve);
  
  //! Init  the  projection  of a  point <P>  on a curve
  //! <Curve> limited by the two points of parameter Umin and Usup.
  Standard_EXPORT void Init (const gp_Pnt& P, const Handle(Geom_Curve)& Curve, const Standard_Real Umin, const Standard_Real Usup);
  
  //! Init  the  projection  of a  point <P>  on a curve
  //! <Curve> limited by the two points of parameter Umin and Usup.
  Standard_EXPORT void Init (const Handle(Geom_Curve)& Curve, const Standard_Real Umin, const Standard_Real Usup);
  
  //! Performs the projection of a point on the current curve.
  Standard_EXPORT void Perform (const gp_Pnt& P);
  
  //! Returns the number of computed
  //! orthogonal projection points.
  //! Note: if this algorithm fails, NbPoints returns 0.
  Standard_EXPORT Standard_Integer NbPoints() const;
Standard_EXPORT operator Standard_Integer() const;
  
  //! Returns the orthogonal projection
  //! on the curve. Index is a number of a computed point.
  //! Exceptions
  //! Standard_OutOfRange if Index is not in the range [ 1,NbPoints ], where
  //! NbPoints is the number of solution points.
  Standard_EXPORT gp_Pnt Point (const Standard_Integer Index) const;
  
  //! Returns the parameter on the curve
  //! of the point, which is the orthogonal projection. Index is a
  //! number of a computed point.
  //! Exceptions
  //! Standard_OutOfRange if Index is not in the range [ 1,NbPoints ], where
  //! NbPoints is the number of solution points.
  Standard_EXPORT Standard_Real Parameter (const Standard_Integer Index) const;
  
  //! Returns the parameter on the curve
  //! of the point, which is the orthogonal projection. Index is a
  //! number of a computed point.
  //! Exceptions
  //! Standard_OutOfRange if Index is not in the range [ 1,NbPoints ], where
  //! NbPoints is the number of solution points.-
  Standard_EXPORT void Parameter (const Standard_Integer Index, Standard_Real& U) const;
  
  //! Computes the distance between the
  //! point and its orthogonal projection on the curve. Index is a number of a computed point.
  //! Exceptions
  //! Standard_OutOfRange if Index is not in the range [ 1,NbPoints ], where
  //! NbPoints is the number of solution points.
  Standard_EXPORT Standard_Real Distance (const Standard_Integer Index) const;
  
  //! Returns the nearest orthogonal
  //! projection of the point on the curve.
  //! Exceptions: StdFail_NotDone if this algorithm fails.
  Standard_EXPORT gp_Pnt NearestPoint() const;
Standard_EXPORT operator gp_Pnt() const;
  
  //! Returns the parameter on the curve
  //! of the nearest orthogonal projection of the point.
  //! Exceptions: StdFail_NotDone if this algorithm fails.
  Standard_EXPORT Standard_Real LowerDistanceParameter() const;
  
  //! Computes the distance between the
  //! point and its nearest orthogonal projection on the curve.
  //! Exceptions: StdFail_NotDone if this algorithm fails.
  Standard_EXPORT Standard_Real LowerDistance() const;
Standard_EXPORT operator Standard_Real() const;
  
  //! return the algorithmic object from Extrema
    const Extrema_ExtPC& Extrema() const;

private:

  Standard_Boolean myIsDone;
  Standard_Integer myIndex;
  Extrema_ExtPC myExtPC;
  GeomAdaptor_Curve myC;

};


#include <GeomAPI_ProjectPointOnCurve.lxx>





#endif // _GeomAPI_ProjectPointOnCurve_HeaderFile
