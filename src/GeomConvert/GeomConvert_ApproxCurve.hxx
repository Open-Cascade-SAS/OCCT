// Created on: 1997-09-11
// Created by: Roman BORISOV
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

#ifndef _GeomConvert_ApproxCurve_HeaderFile
#define _GeomConvert_ApproxCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
class Geom_BSplineCurve;
class Standard_OutOfRange;
class Standard_ConstructionError;
class Geom_Curve;
class Adaptor3d_HCurve;


//! A framework to convert a 3D curve to a 3D BSpline.
//! This is done by approximation to a BSpline curve within a given tolerance.
class GeomConvert_ApproxCurve 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs a curve approximation framework defined by -
  //! -      the conic Curve,
  //! -      the tolerance value Tol3d,
  //! -      the degree of continuity Order,
  //! -      the maximum number of segments
  //! MaxSegments allowed in the resulting BSpline curve, and
  //! -      the highest degree MaxDeg which the
  //! polynomial defining the BSpline curve may have.
  Standard_EXPORT GeomConvert_ApproxCurve(const Handle(Geom_Curve)& Curve, const Standard_Real Tol3d, const GeomAbs_Shape Order, const Standard_Integer MaxSegments, const Standard_Integer MaxDegree);
  
  //! Constructs a curve approximation framework defined by -
  //! -      the Curve,
  //! -      the tolerance value Tol3d,
  //! -      the degree of continuity Order,
  //! -      the maximum number of segments
  //! MaxSegments allowed in the resulting BSpline curve, and
  //! -      the highest degree MaxDeg which the
  //! polynomial defining the BSpline curve may have.
  Standard_EXPORT GeomConvert_ApproxCurve(const Handle(Adaptor3d_HCurve)& Curve, const Standard_Real Tol3d, const GeomAbs_Shape Order, const Standard_Integer MaxSegments, const Standard_Integer MaxDegree);
  
  //! Returns the BSpline curve resulting from the approximation algorithm.
  Standard_EXPORT Handle(Geom_BSplineCurve) Curve() const;
  
  //! returns  Standard_True  if  the  approximation  has
  //! been  done  within  requiered tolerance
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns  Standard_True if the approximation did come out
  //! with a result that  is not NECESSARELY within the required tolerance
  Standard_EXPORT Standard_Boolean HasResult() const;
  
  //! Returns the greatest distance between a point on the
  //! source conic and the BSpline curve resulting from the
  //! approximation. (>0 when an approximation
  //! has  been  done, 0  if  no  approximation)
  Standard_EXPORT Standard_Real MaxError() const;
  
  //! Print on the stream  o  information about the object
  Standard_EXPORT void Dump (Standard_OStream& o) const;




protected:





private:

  
  //! Converts a curve to B-spline
  Standard_EXPORT void Approximate (const Handle(Adaptor3d_HCurve)& theCurve, const Standard_Real theTol3d, const GeomAbs_Shape theOrder, const Standard_Integer theMaxSegments, const Standard_Integer theMaxDegree);


  Standard_Boolean myIsDone;
  Standard_Boolean myHasResult;
  Handle(Geom_BSplineCurve) myBSplCurve;
  Standard_Real myMaxError;


};







#endif // _GeomConvert_ApproxCurve_HeaderFile
