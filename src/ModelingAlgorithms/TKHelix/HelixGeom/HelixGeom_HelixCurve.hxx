// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _HelixGeom_HelixCurve_HeaderFile
#define _HelixGeom_HelixCurve_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <GeomAbs_Shape.hxx>

class gp_Pnt;
class gp_Vec;

DEFINE_STANDARD_HANDLE(HelixGeom_HelixCurve, Adaptor3d_Curve)

//! Adaptor class for calculation of helix curves with analytical expressions.
//!
//! This class provides parametric representation of helix curves including:
//! - Cylindrical helixes (constant radius)
//! - Tapered helixes (variable radius with taper angle)
//! - Both clockwise and counter-clockwise orientations
//!
//! The helix is defined by parametric equations in cylindrical coordinates:
//! - x(t) = r(t) * cos(t)
//! - y(t) = r(t) * sin(t) [* direction factor]
//! - z(t) = pitch * t / (2*PI)
//! where r(t) = rStart + taper_factor * t
//!
//! @sa HelixGeom_BuilderHelix, HelixGeom_BuilderHelixCoil
class HelixGeom_HelixCurve : public Adaptor3d_Curve
{
  DEFINE_STANDARD_RTTIEXT(HelixGeom_HelixCurve, Adaptor3d_Curve)
public:
  //! implementation of analytical expressions
  Standard_EXPORT HelixGeom_HelixCurve();

  //! Sets default values for parameters
  Standard_EXPORT void Load();

  //! Sets helix parameters
  Standard_EXPORT void Load(const Standard_Real    aT1,
                            const Standard_Real    aT2,
                            const Standard_Real    aPitch,
                            const Standard_Real    aRStart,
                            const Standard_Real    aTaperAngle,
                            const Standard_Boolean aIsCW);

  //! Gets first parameter
  Standard_EXPORT virtual Standard_Real FirstParameter() const Standard_OVERRIDE;

  //! Gets last parameter
  Standard_EXPORT virtual Standard_Real LastParameter() const Standard_OVERRIDE;

  //! Gets continuity
  Standard_EXPORT virtual GeomAbs_Shape Continuity() const Standard_OVERRIDE;

  //! Gets number of intervals
  Standard_EXPORT virtual Standard_Integer NbIntervals(const GeomAbs_Shape S) const
    Standard_OVERRIDE;

  //! Gets parametric intervals
  Standard_EXPORT virtual void Intervals(TColStd_Array1OfReal& T,
                                         const GeomAbs_Shape   S) const Standard_OVERRIDE;

  //! Gets parametric resolution
  Standard_EXPORT virtual Standard_Real Resolution(const Standard_Real R3d) const Standard_OVERRIDE;

  //! Returns False
  Standard_EXPORT virtual Standard_Boolean IsClosed() const Standard_OVERRIDE;

  //! Returns False
  Standard_EXPORT virtual Standard_Boolean IsPeriodic() const Standard_OVERRIDE;

  //! Returns 2*PI
  Standard_EXPORT virtual Standard_Real Period() const Standard_OVERRIDE;

  //! Gets curve point for parameter U
  Standard_EXPORT virtual gp_Pnt Value(const Standard_Real U) const Standard_OVERRIDE;

  //! Gets curve point for parameter U
  Standard_EXPORT virtual void D0(const Standard_Real U, gp_Pnt& P) const Standard_OVERRIDE;

  //! Gets curve point and first derivatives for parameter U
  Standard_EXPORT virtual void D1(const Standard_Real U,
                                  gp_Pnt&             P,
                                  gp_Vec&             V1) const Standard_OVERRIDE;

  //! Gets curve point, first and second derivatives for parameter U
  Standard_EXPORT virtual void D2(const Standard_Real U,
                                  gp_Pnt&             P,
                                  gp_Vec&             V1,
                                  gp_Vec&             V2) const Standard_OVERRIDE;

  //! Gets curve derivative of demanded order for parameter U
  Standard_EXPORT virtual gp_Vec DN(const Standard_Real    U,
                                    const Standard_Integer N) const Standard_OVERRIDE;

protected:
  Standard_Real    myFirst;
  Standard_Real    myLast;
  Standard_Real    myPitch;
  Standard_Real    myRStart;
  Standard_Real    myTaperAngle;
  Standard_Boolean myIsClockWise;
  Standard_Real    myC1;
  Standard_Real    myTgBeta;
  Standard_Real    myTolAngle;

private:
};

#endif // _HelixGeom_HelixCurve_HeaderFile
