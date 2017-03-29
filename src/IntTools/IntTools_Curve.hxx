// Created on: 2000-11-23
// Created by: Michael KLOKOV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _IntTools_Curve_HeaderFile
#define _IntTools_Curve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <GeomAbs_CurveType.hxx>
class Geom_Curve;
class Geom2d_Curve;
class gp_Pnt;

//! The class is a container of one 3D curve, two 2D curves and two Tolerance values.<br>
//! It is used in the Face/Face intersection algorithm to store the results
//! of intersection. In this context:<br>
//! **the 3D curve** is the intersection curve;<br>
//! **the 2D curves** are the PCurves of the 3D curve on the intersecting faces;<br>
//! **the tolerance** is the valid tolerance for 3D curve computed as
//! maximal deviation between 3D curve and 2D curves (or surfaces in case there are no 2D curves);<br>
//! **the tangential tolerance** is the maximal distance from 3D curve to the
//! end of the tangential zone between faces in terms of their tolerance values.
class IntTools_Curve
{
public:

  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT IntTools_Curve();

  //! Constructor taking 3d curve, two 2d curves and two tolerance values
  Standard_EXPORT IntTools_Curve(const Handle(Geom_Curve)& the3dCurve3d,
                                 const Handle(Geom2d_Curve)& the2dCurve1,
                                 const Handle(Geom2d_Curve)& the2dCurve2,
                                 const Standard_Real theTolerance = 0.0,
                                 const Standard_Real theTangentialTolerance = 0.0);

  //! Sets the curves
  void SetCurves(const Handle(Geom_Curve)& the3dCurve,
                 const Handle(Geom2d_Curve)& the2dCurve1,
                 const Handle(Geom2d_Curve)& the2dCurve2)
  {
    my3dCurve = the3dCurve;
    my2dCurve1 = the2dCurve1;
    my2dCurve2 = the2dCurve2;
  }

  //! Sets the 3d curve
  void SetCurve(const Handle(Geom_Curve)& the3dCurve)
  {
    my3dCurve = the3dCurve;
  }

  //! Sets the first 2d curve
  void SetFirstCurve2d(const Handle(Geom2d_Curve)& the2dCurve1)
  {
    my2dCurve1 = the2dCurve1;
  }

  //! Sets the second 2d curve
  void SetSecondCurve2d(const Handle(Geom2d_Curve)& the2dCurve2)
  {
    my2dCurve2 = the2dCurve2;
  }

  //! Sets the tolerance for the curve
  void SetTolerance(const Standard_Real theTolerance)
  {
    myTolerance = theTolerance;
  }

  //! Sets the tangential tolerance
  void SetTangentialTolerance(const Standard_Real theTangentialTolerance)
  {
    myTangentialTolerance = theTangentialTolerance;
  }

  //! Returns 3d curve
  const Handle(Geom_Curve)& Curve() const
  {
    return my3dCurve;
  }

  //! Returns first 2d curve
  const Handle(Geom2d_Curve)& FirstCurve2d() const
  {
    return my2dCurve1;
  }

  //! Returns second 2d curve
  const Handle(Geom2d_Curve)& SecondCurve2d() const
  {
    return my2dCurve2;
  }

  //! Returns the tolerance
  Standard_Real Tolerance() const
  {
    return myTolerance;
  }

  //! Returns the tangential tolerance
  Standard_Real TangentialTolerance() const
  {
    return myTangentialTolerance;
  }

  //! Returns TRUE if 3d curve is BoundedCurve
  Standard_EXPORT Standard_Boolean HasBounds() const;

  //! If the 3d curve is bounded curve the method will return TRUE
  //! and modify the output parameters with boundary parameters of
  //! the curve and corresponded 3d points.<br>
  //! If the curve does not have bounds, the method will return false
  //! and the output parameters will stay untouched.
  Standard_EXPORT Standard_Boolean Bounds(Standard_Real& theFirst,
                                          Standard_Real& theLast,
                                          gp_Pnt& theFirstPnt,
                                          gp_Pnt& theLastPnt) const;

  //! Computes 3d point corresponded to the given parameter if this
  //! parameter is inside the boundaries of the curve.
  //! Returns TRUE in this case. <br>
  //! Otherwise, the point will not be computed and the method will return FALSE.
  Standard_EXPORT Standard_Boolean D0(const Standard_Real& thePar,
                                      gp_Pnt& thePnt) const;

  //! Returns the type of the 3d curve
  Standard_EXPORT GeomAbs_CurveType Type() const;

protected:

private:

  Handle(Geom_Curve) my3dCurve;
  Handle(Geom2d_Curve) my2dCurve1;
  Handle(Geom2d_Curve) my2dCurve2;
  Standard_Real myTolerance;
  Standard_Real myTangentialTolerance;
};

#endif // _IntTools_Curve_HeaderFile
