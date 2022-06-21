// Created on: 1998-07-14
// Created by: data exchange team
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _ShapeConstruct_Curve_HeaderFile
#define _ShapeConstruct_Curve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_Array1OfReal.hxx>
class Geom_Curve;
class gp_Pnt;
class Geom2d_Curve;
class gp_Pnt2d;
class Geom_BSplineCurve;
class Geom2d_BSplineCurve;


//! Adjusts curve to have start and end points at the given
//! points (currently works on lines and B-Splines only)
class ShapeConstruct_Curve 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Modifies a curve in order to make its bounds confused with
  //! given points.
  //! Works only on lines and B-Splines, returns True in this case,
  //! else returns False.
  //! For line considers both bounding points, for B-Splines only
  //! specified.
  //!
  //! Warning : Does not check if curve should be reversed
  Standard_EXPORT Standard_Boolean AdjustCurve (const Handle(Geom_Curve)& C3D, const gp_Pnt& P1, const gp_Pnt& P2, const Standard_Boolean take1 = Standard_True, const Standard_Boolean take2 = Standard_True) const;
  
  //! Modifies a curve in order to make its bounds confused with
  //! given points.
  //! Works only on lines and B-Splines.
  //!
  //! For lines works as previous method, B-Splines are segmented
  //! at the given values and then are adjusted to the points.
  Standard_EXPORT Standard_Boolean AdjustCurveSegment (const Handle(Geom_Curve)& C3D, const gp_Pnt& P1, const gp_Pnt& P2, const Standard_Real U1, const Standard_Real U2) const;
  
  //! Modifies a curve in order to make its bounds confused with
  //! given points.
  //! Works only on lines and B-Splines, returns True in this case,
  //! else returns False.
  //!
  //! For line considers both bounding points, for B-Splines only
  //! specified.
  //!
  //! Warning : Does not check if curve should be reversed
  Standard_EXPORT Standard_Boolean AdjustCurve2d (const Handle(Geom2d_Curve)& C2D, const gp_Pnt2d& P1, const gp_Pnt2d& P2, const Standard_Boolean take1 = Standard_True, const Standard_Boolean take2 = Standard_True) const;
  
  //! Converts a curve of any type (only part from first to last)
  //! to bspline. The method of conversion depends on the type
  //! of original curve:
  //! BSpline -> C.Segment(first,last)
  //! Bezier and Line -> GeomConvert::CurveToBSplineCurve(C).Segment(first,last)
  //! Conic and Other -> Approx_Curve3d(C[first,last],prec,C1,9,1000)
  Standard_EXPORT Handle(Geom_BSplineCurve) ConvertToBSpline (const Handle(Geom_Curve)& C, const Standard_Real first, const Standard_Real last, const Standard_Real prec) const;
  
  //! Converts a curve of any type (only part from first to last)
  //! to bspline. The method of conversion depends on the type
  //! of original curve:
  //! BSpline -> C.Segment(first,last)
  //! Bezier and Line -> GeomConvert::CurveToBSplineCurve(C).Segment(first,last)
  //! Conic and Other -> Approx_Curve2d(C[first,last],prec,C1,9,1000)
  Standard_EXPORT Handle(Geom2d_BSplineCurve) ConvertToBSpline (const Handle(Geom2d_Curve)& C, const Standard_Real first, const Standard_Real last, const Standard_Real prec) const;
  
  Standard_EXPORT static Standard_Boolean FixKnots (Handle(TColStd_HArray1OfReal)& knots);
  
  //! Fix bspline knots to ensure that there is enough
  //! gap between neighbouring values
  //! Returns True if something fixed (by shifting knot)
  Standard_EXPORT static Standard_Boolean FixKnots (TColStd_Array1OfReal& knots);




protected:





private:





};







#endif // _ShapeConstruct_Curve_HeaderFile
