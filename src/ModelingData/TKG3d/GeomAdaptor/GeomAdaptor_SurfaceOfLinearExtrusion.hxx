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

#ifndef _GeomAdaptor_SurfaceOfLinearExtrusion_HeaderFile
#define _GeomAdaptor_SurfaceOfLinearExtrusion_HeaderFile

#include <GeomAdaptor_Surface.hxx>

class gp_Pln;
class gp_Cylinder;
class gp_Cone;
class gp_Sphere;
class gp_Torus;
class Geom_BezierSurface;
class Geom_BSplineSurface;
class gp_Ax1;

//! Generalised cylinder. This surface is obtained by sweeping a curve in a given
//! direction. The parametrization range for the parameter U is defined
//! with referenced the curve.
//! The parametrization range for the parameter V is ]-infinite,+infinite[
//! The position of the curve gives the origin for the parameter V.
//! The continuity of the surface is CN in the V direction.
class GeomAdaptor_SurfaceOfLinearExtrusion : public GeomAdaptor_Surface
{
  DEFINE_STANDARD_RTTIEXT(GeomAdaptor_SurfaceOfLinearExtrusion, GeomAdaptor_Surface)
public:
  Standard_EXPORT GeomAdaptor_SurfaceOfLinearExtrusion();

  //! The Curve is loaded.
  Standard_EXPORT GeomAdaptor_SurfaceOfLinearExtrusion(const occ::handle<Adaptor3d_Curve>& C);

  //! Thew Curve and the Direction are loaded.
  Standard_EXPORT GeomAdaptor_SurfaceOfLinearExtrusion(const occ::handle<Adaptor3d_Curve>& C,
                                                       const gp_Dir&                       V);

  //! Shallow copy of adaptor
  Standard_EXPORT virtual occ::handle<Adaptor3d_Surface> ShallowCopy() const override;

  //! Changes the Curve
  Standard_EXPORT void Load(const occ::handle<Adaptor3d_Curve>& C);

  //! Changes the Direction
  Standard_EXPORT void Load(const gp_Dir& V);

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

  Standard_EXPORT gp_Cone Cone() const override;

  Standard_EXPORT gp_Sphere Sphere() const override;

  Standard_EXPORT gp_Torus Torus() const override;

  Standard_EXPORT int UDegree() const override;

  Standard_EXPORT int NbUPoles() const override;

  Standard_EXPORT bool IsURational() const override;

  Standard_EXPORT bool IsVRational() const override;

  Standard_EXPORT occ::handle<Geom_BezierSurface> Bezier() const override;

  Standard_EXPORT occ::handle<Geom_BSplineSurface> BSpline() const override;

  Standard_EXPORT gp_Ax1 AxeOfRevolution() const override;

  Standard_EXPORT gp_Dir Direction() const override;

  Standard_EXPORT occ::handle<Adaptor3d_Curve> BasisCurve() const override;

private:
  occ::handle<Adaptor3d_Curve> myBasisCurve; ///< extruded curve
  gp_Dir                       myDirection;  ///< direction of extrusion
  bool                         myHaveDir;    ///< whether the direction of extrusion is initialized
};

#endif // _GeomAdaptor_SurfaceOfLinearExtrusion_HeaderFile
