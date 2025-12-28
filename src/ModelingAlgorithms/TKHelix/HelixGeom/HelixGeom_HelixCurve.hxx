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
  Standard_EXPORT void Load(const double    aT1,
                            const double    aT2,
                            const double    aPitch,
                            const double    aRStart,
                            const double    aTaperAngle,
                            const bool aIsCW);

  //! Gets first parameter
  Standard_EXPORT virtual double FirstParameter() const override;

  //! Gets last parameter
  Standard_EXPORT virtual double LastParameter() const override;

  //! Gets continuity
  Standard_EXPORT virtual GeomAbs_Shape Continuity() const override;

  //! Gets number of intervals
  Standard_EXPORT virtual int NbIntervals(const GeomAbs_Shape S) const
    override;

  //! Gets parametric intervals
  Standard_EXPORT virtual void Intervals(NCollection_Array1<double>& T,
                                         const GeomAbs_Shape   S) const override;

  //! Gets parametric resolution
  Standard_EXPORT virtual double Resolution(const double R3d) const override;

  //! Returns False
  Standard_EXPORT virtual bool IsClosed() const override;

  //! Returns False
  Standard_EXPORT virtual bool IsPeriodic() const override;

  //! Returns 2*PI
  Standard_EXPORT virtual double Period() const override;

  //! Gets curve point for parameter U
  Standard_EXPORT virtual gp_Pnt Value(const double U) const override;

  //! Gets curve point for parameter U
  Standard_EXPORT virtual void D0(const double U, gp_Pnt& P) const override;

  //! Gets curve point and first derivatives for parameter U
  Standard_EXPORT virtual void D1(const double U,
                                  gp_Pnt&             P,
                                  gp_Vec&             V1) const override;

  //! Gets curve point, first and second derivatives for parameter U
  Standard_EXPORT virtual void D2(const double U,
                                  gp_Pnt&             P,
                                  gp_Vec&             V1,
                                  gp_Vec&             V2) const override;

  //! Gets curve derivative of demanded order for parameter U
  Standard_EXPORT virtual gp_Vec DN(const double    U,
                                    const int N) const override;

protected:
  double    myFirst;
  double    myLast;
  double    myPitch;
  double    myRStart;
  double    myTaperAngle;
  bool myIsClockWise;
  double    myC1;
  double    myTgBeta;
  double    myTolAngle;

};

#endif // _HelixGeom_HelixCurve_HeaderFile
