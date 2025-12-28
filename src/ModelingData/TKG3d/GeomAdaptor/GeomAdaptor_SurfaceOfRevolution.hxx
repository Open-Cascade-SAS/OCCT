// Created on: 1993-04-21
// Created by: Bruno DUMORTIER
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _GeomAdaptor_SurfaceOfRevolution_HeaderFile
#define _GeomAdaptor_SurfaceOfRevolution_HeaderFile

#include <GeomAdaptor_Surface.hxx>

class gp_Pln;
class gp_Cylinder;
class gp_Cone;
class gp_Sphere;
class gp_Torus;
class Geom_BezierSurface;
class Geom_BSplineSurface;

//! This class defines a complete surface of revolution.
//! The surface is obtained by rotating a curve a complete revolution
//! about an axis. The curve and the axis must be in the same plane.
//! If the curve and the axis are not in the same plane it is always
//! possible to be in the previous case after a cylindrical projection
//! of the curve in a referenced plane.
//! For a complete surface of revolution the parametric range is
//! 0 <= U <= 2*PI
//! The parametric range for V is defined with the revolved curve.
//! The origin of the U parametrization is given by the position
//! of the revolved curve (reference). The direction of the revolution
//! axis defines the positive sense of rotation (trigonometric sense)
//! corresponding to the increasing of the parametric value U.
//! The derivatives are always defined for the u direction.
//! For the v direction the definition of the derivatives depends on
//! the degree of continuity of the referenced curve.
class GeomAdaptor_SurfaceOfRevolution : public GeomAdaptor_Surface
{
  DEFINE_STANDARD_RTTIEXT(GeomAdaptor_SurfaceOfRevolution, GeomAdaptor_Surface)
public:
  Standard_EXPORT GeomAdaptor_SurfaceOfRevolution();

  //! The Curve is loaded.
  Standard_EXPORT GeomAdaptor_SurfaceOfRevolution(const occ::handle<Adaptor3d_Curve>& C);

  //! The Curve and the Direction are loaded.
  Standard_EXPORT GeomAdaptor_SurfaceOfRevolution(const occ::handle<Adaptor3d_Curve>& C,
                                                  const gp_Ax1&                       V);

  //! Shallow copy of adaptor
  Standard_EXPORT virtual occ::handle<Adaptor3d_Surface> ShallowCopy() const override;

  //! Changes the Curve
  Standard_EXPORT void Load(const occ::handle<Adaptor3d_Curve>& C);

  //! Changes the Direction
  Standard_EXPORT void Load(const gp_Ax1& V);

  Standard_EXPORT gp_Ax1 AxeOfRevolution() const override;

  Standard_EXPORT double FirstUParameter() const override;

  Standard_EXPORT double LastUParameter() const override;

  Standard_EXPORT double FirstVParameter() const override;

  Standard_EXPORT double LastVParameter() const override;

  Standard_EXPORT GeomAbs_Shape UContinuity() const override;

  //! Return CN.
  Standard_EXPORT GeomAbs_Shape VContinuity() const override;

  //! Returns the number of U intervals for continuity
  //! <S>. May be one if UContinuity(me) >= <S>
  Standard_EXPORT int NbUIntervals(const GeomAbs_Shape S) const override;

  //! Returns the number of V intervals for continuity
  //! <S>. May be one if VContinuity(me) >= <S>
  Standard_EXPORT int NbVIntervals(const GeomAbs_Shape S) const override;

  //! Returns the intervals with the requested continuity
  //! in the U direction.
  Standard_EXPORT void UIntervals(NCollection_Array1<double>& T,
                                  const GeomAbs_Shape         S) const override;

  //! Returns the intervals with the requested continuity
  //! in the V direction.
  Standard_EXPORT void VIntervals(NCollection_Array1<double>& T,
                                  const GeomAbs_Shape         S) const override;

  //! Returns a surface trimmed in the U direction
  //! equivalent of <me> between
  //! parameters <First> and <Last>. <Tol> is used to
  //! test for 3d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT occ::handle<Adaptor3d_Surface> UTrim(const double First,
                                                       const double Last,
                                                       const double Tol) const override;

  //! Returns a surface trimmed in the V direction between
  //! parameters <First> and <Last>. <Tol> is used to
  //! test for 3d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT occ::handle<Adaptor3d_Surface> VTrim(const double First,
                                                       const double Last,
                                                       const double Tol) const override;

  Standard_EXPORT bool IsUClosed() const override;

  Standard_EXPORT bool IsVClosed() const override;

  Standard_EXPORT bool IsUPeriodic() const override;

  Standard_EXPORT double UPeriod() const override;

  Standard_EXPORT bool IsVPeriodic() const override;

  Standard_EXPORT double VPeriod() const override;

  //! Returns the parametric U resolution corresponding
  //! to the real space resolution <R3d>.
  Standard_EXPORT double UResolution(const double R3d) const override;

  //! Returns the parametric V resolution corresponding
  //! to the real space resolution <R3d>.
  Standard_EXPORT double VResolution(const double R3d) const override;

  //! Returns the type of the surface: Plane, Cylinder,
  //! Cone, Sphere, Torus, BezierSurface,
  //! BSplineSurface, SurfaceOfRevolution,
  //! SurfaceOfExtrusion, OtherSurface
  Standard_EXPORT GeomAbs_SurfaceType GetType() const override;

  Standard_EXPORT gp_Pln Plane() const override;

  Standard_EXPORT gp_Cylinder Cylinder() const override;

  //! Apex of the Cone = Cone.Position().Location()
  //! ==> ReferenceRadius = 0.
  Standard_EXPORT gp_Cone Cone() const override;

  Standard_EXPORT gp_Sphere Sphere() const override;

  Standard_EXPORT gp_Torus Torus() const override;

  Standard_EXPORT int VDegree() const override;

  Standard_EXPORT int NbVPoles() const override;

  Standard_EXPORT int NbVKnots() const override;

  Standard_EXPORT bool IsURational() const override;

  Standard_EXPORT bool IsVRational() const override;

  Standard_EXPORT occ::handle<Geom_BezierSurface> Bezier() const override;

  Standard_EXPORT occ::handle<Geom_BSplineSurface> BSpline() const override;

  Standard_EXPORT const gp_Ax3& Axis() const;

  Standard_EXPORT occ::handle<Adaptor3d_Curve> BasisCurve() const override;

private:
  occ::handle<Adaptor3d_Curve> myBasisCurve; ///< revolved curve
  gp_Ax1                       myAxis;       ///< axis of revolution
  bool                         myHaveAxis;   ///< whether axis of revolution is initialized
  gp_Ax3                       myAxeRev; ///< auxiliary trihedron according to the curve position
};

#endif // _GeomAdaptor_SurfaceOfRevolution_HeaderFile
