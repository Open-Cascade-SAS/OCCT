// Created on: 1997-08-22
// Created by: Jeannine PANCIATICI,  Sergey SOKOLOV
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

#ifndef _Approx_CurvilinearParameter_HeaderFile
#define _Approx_CurvilinearParameter_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_Surface.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_OStream.hxx>

//! Approximation of a Curve to make its parameter be its curvilinear abscissa.
//! If the curve is a curve on a surface S, C2D is the corresponding Pcurve,
//! we consider the curve is given by its representation
//! @code
//!   S(C2D(u))
//! @endcode
//! If the curve is a curve on 2 surfaces S1 and S2 and C2D1 C2D2 are the two corresponding Pcurve,
//! we consider the curve is given by its representation
//! @code
//!   1/2(S1(C2D1(u) + S2(C2D2(u)))
//! @endcode
class Approx_CurvilinearParameter 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! case of a free 3D curve
  Standard_EXPORT Approx_CurvilinearParameter(const Handle(Adaptor3d_Curve)& C3D, const Standard_Real Tol, const GeomAbs_Shape Order, const Standard_Integer MaxDegree, const Standard_Integer MaxSegments);
  
  //! case of a curve on one surface
  Standard_EXPORT Approx_CurvilinearParameter(const Handle(Adaptor2d_Curve2d)& C2D, const Handle(Adaptor3d_Surface)& Surf, const Standard_Real Tol, const GeomAbs_Shape Order, const Standard_Integer MaxDegree, const Standard_Integer MaxSegments);
  
  //! case of a curve on two surfaces
  Standard_EXPORT Approx_CurvilinearParameter(const Handle(Adaptor2d_Curve2d)& C2D1, const Handle(Adaptor3d_Surface)& Surf1, const Handle(Adaptor2d_Curve2d)& C2D2, const Handle(Adaptor3d_Surface)& Surf2, const Standard_Real Tol, const GeomAbs_Shape Order, const Standard_Integer MaxDegree, const Standard_Integer MaxSegments);
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT Standard_Boolean HasResult() const;
  
  //! returns the Bspline curve corresponding to the reparametrized 3D curve
  Standard_EXPORT Handle(Geom_BSplineCurve) Curve3d() const;
  
  //! returns the maximum error on the reparametrized 3D curve
  Standard_EXPORT Standard_Real MaxError3d() const;
  
  //! returns the BsplineCurve representing the reparametrized 2D curve on the
  //! first surface (case of a curve on one or two surfaces)
  Standard_EXPORT Handle(Geom2d_BSplineCurve) Curve2d1() const;
  
  //! returns the maximum error on the first reparametrized 2D curve
  Standard_EXPORT Standard_Real MaxError2d1() const;
  
  //! returns the BsplineCurve representing the reparametrized 2D curve on the
  //! second surface (case of a curve on two surfaces)
  Standard_EXPORT Handle(Geom2d_BSplineCurve) Curve2d2() const;
  
  //! returns the maximum error on the second reparametrized 2D curve
  Standard_EXPORT Standard_Real MaxError2d2() const;
  
  //! print the maximum errors(s)
  Standard_EXPORT void Dump (Standard_OStream& o) const;

private:

  Standard_EXPORT static void ToleranceComputation (const Handle(Adaptor2d_Curve2d)& C2D, const Handle(Adaptor3d_Surface)& S, const Standard_Integer MaxNumber, const Standard_Real Tol, Standard_Real& TolV, Standard_Real& TolW);

private:

  Standard_Integer myCase;
  Standard_Boolean myDone;
  Standard_Boolean myHasResult;
  Handle(Geom_BSplineCurve) myCurve3d;
  Standard_Real myMaxError3d;
  Handle(Geom2d_BSplineCurve) myCurve2d1;
  Standard_Real myMaxError2d1;
  Handle(Geom2d_BSplineCurve) myCurve2d2;
  Standard_Real myMaxError2d2;

};

#endif // _Approx_CurvilinearParameter_HeaderFile
