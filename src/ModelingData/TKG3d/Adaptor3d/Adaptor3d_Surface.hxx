// Created on: 1993-03-31
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

#ifndef _Adaptor3d_Surface_HeaderFile
#define _Adaptor3d_Surface_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <gp_Ax1.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Cone.hxx>
#include <gp_Pln.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <NCollection_Array1.hxx>

class Geom_BezierSurface;
class Geom_BSplineSurface;

//! Root class for surfaces on which geometric algorithms work.
//! An adapted surface is an interface between the
//! services provided by a surface and those required of
//! the surface by algorithms which use it.
//! A derived concrete class is provided:
//! GeomAdaptor_Surface for a surface from the Geom package.
//! The Surface class describes the standard behaviour
//! of a surface for generic algorithms.
//!
//! The Surface can be decomposed in intervals of any
//! continuity in U and V using the method NbIntervals.
//! A current interval can be set.
//! Most of the methods apply to the current interval.
//! Warning: All the methods are virtual and implemented with a
//! raise to allow to redefined only the methods really used.
//!
//! Polynomial coefficients of BSpline surfaces used for their evaluation are cached for better
//! performance. Therefore these evaluations are not thread-safe and parallel evaluations need to be
//! prevented.
class Adaptor3d_Surface : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Adaptor3d_Surface, Standard_Transient)
public:
  //! Shallow copy of adaptor
  Standard_EXPORT virtual occ::handle<Adaptor3d_Surface> ShallowCopy() const;

  Standard_EXPORT virtual double FirstUParameter() const;

  Standard_EXPORT virtual double LastUParameter() const;

  Standard_EXPORT virtual double FirstVParameter() const;

  Standard_EXPORT virtual double LastVParameter() const;

  Standard_EXPORT virtual GeomAbs_Shape UContinuity() const;

  Standard_EXPORT virtual GeomAbs_Shape VContinuity() const;

  //! Returns the number of U intervals for continuity
  //! <S>. May be one if UContinuity(me) >= <S>
  Standard_EXPORT virtual int NbUIntervals(const GeomAbs_Shape S) const;

  //! Returns the number of V intervals for continuity
  //! <S>. May be one if VContinuity(me) >= <S>
  Standard_EXPORT virtual int NbVIntervals(const GeomAbs_Shape S) const;

  //! Returns the intervals with the requested continuity
  //! in the U direction.
  Standard_EXPORT virtual void UIntervals(NCollection_Array1<double>& T,
                                          const GeomAbs_Shape         S) const;

  //! Returns the intervals with the requested continuity
  //! in the V direction.
  Standard_EXPORT virtual void VIntervals(NCollection_Array1<double>& T,
                                          const GeomAbs_Shape         S) const;

  //! Returns a surface trimmed in the U direction
  //! equivalent of <me> between
  //! parameters <First> and <Last>. <Tol> is used to
  //! test for 3d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT virtual occ::handle<Adaptor3d_Surface> UTrim(const double First,
                                                               const double Last,
                                                               const double Tol) const;

  //! Returns a surface trimmed in the V direction between
  //! parameters <First> and <Last>. <Tol> is used to
  //! test for 3d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT virtual occ::handle<Adaptor3d_Surface> VTrim(const double First,
                                                               const double Last,
                                                               const double Tol) const;

  Standard_EXPORT virtual bool IsUClosed() const;

  Standard_EXPORT virtual bool IsVClosed() const;

  Standard_EXPORT virtual bool IsUPeriodic() const;

  Standard_EXPORT virtual double UPeriod() const;

  Standard_EXPORT virtual bool IsVPeriodic() const;

  Standard_EXPORT virtual double VPeriod() const;

  //! Computes the point of parameters U,V on the surface.
  //! Tip: use GeomLib::NormEstim() to calculate surface normal at specified (U, V) point.
  Standard_EXPORT virtual gp_Pnt Value(const double U, const double V) const;

  //! Computes the point of parameters U,V on the surface.
  Standard_EXPORT virtual void D0(const double U, const double V, gp_Pnt& P) const;

  //! Computes the point and the first derivatives on the surface.
  //! Raised if the continuity of the current intervals is not C1.
  //!
  //! Tip: use GeomLib::NormEstim() to calculate surface normal at specified (U, V) point.
  Standard_EXPORT virtual void D1(const double U,
                                  const double V,
                                  gp_Pnt&      P,
                                  gp_Vec&      D1U,
                                  gp_Vec&      D1V) const;

  //! Computes the point, the first and second
  //! derivatives on the surface.
  //! Raised if the continuity of the current
  //! intervals is not C2.
  Standard_EXPORT virtual void D2(const double U,
                                  const double V,
                                  gp_Pnt&      P,
                                  gp_Vec&      D1U,
                                  gp_Vec&      D1V,
                                  gp_Vec&      D2U,
                                  gp_Vec&      D2V,
                                  gp_Vec&      D2UV) const;

  //! Computes the point, the first, second and third
  //! derivatives on the surface.
  //! Raised if the continuity of the current
  //! intervals is not C3.
  Standard_EXPORT virtual void D3(const double U,
                                  const double V,
                                  gp_Pnt&      P,
                                  gp_Vec&      D1U,
                                  gp_Vec&      D1V,
                                  gp_Vec&      D2U,
                                  gp_Vec&      D2V,
                                  gp_Vec&      D2UV,
                                  gp_Vec&      D3U,
                                  gp_Vec&      D3V,
                                  gp_Vec&      D3UUV,
                                  gp_Vec&      D3UVV) const;

  //! Computes the derivative of order Nu in the direction U and Nv
  //! in the direction V at the point P(U, V).
  //! Raised if the current U interval is not not CNu
  //! and the current V interval is not CNv.
  //! Raised if Nu + Nv < 1 or Nu < 0 or Nv < 0.
  Standard_EXPORT virtual gp_Vec DN(const double U,
                                    const double V,
                                    const int    Nu,
                                    const int    Nv) const;

  //! Returns the parametric U resolution corresponding
  //! to the real space resolution <R3d>.
  Standard_EXPORT virtual double UResolution(const double R3d) const;

  //! Returns the parametric V resolution corresponding
  //! to the real space resolution <R3d>.
  Standard_EXPORT virtual double VResolution(const double R3d) const;

  //! Returns the type of the surface: Plane, Cylinder,
  //! Cone, Sphere, Torus, BezierSurface,
  //! BSplineSurface, SurfaceOfRevolution,
  //! SurfaceOfExtrusion, OtherSurface
  Standard_EXPORT virtual GeomAbs_SurfaceType GetType() const;

  Standard_EXPORT virtual gp_Pln Plane() const;

  Standard_EXPORT virtual gp_Cylinder Cylinder() const;

  Standard_EXPORT virtual gp_Cone Cone() const;

  Standard_EXPORT virtual gp_Sphere Sphere() const;

  Standard_EXPORT virtual gp_Torus Torus() const;

  Standard_EXPORT virtual int UDegree() const;

  Standard_EXPORT virtual int NbUPoles() const;

  Standard_EXPORT virtual int VDegree() const;

  Standard_EXPORT virtual int NbVPoles() const;

  Standard_EXPORT virtual int NbUKnots() const;

  Standard_EXPORT virtual int NbVKnots() const;

  Standard_EXPORT virtual bool IsURational() const;

  Standard_EXPORT virtual bool IsVRational() const;

  Standard_EXPORT virtual occ::handle<Geom_BezierSurface> Bezier() const;

  Standard_EXPORT virtual occ::handle<Geom_BSplineSurface> BSpline() const;

  Standard_EXPORT virtual gp_Ax1 AxeOfRevolution() const;

  Standard_EXPORT virtual gp_Dir Direction() const;

  Standard_EXPORT virtual occ::handle<Adaptor3d_Curve> BasisCurve() const;

  Standard_EXPORT virtual occ::handle<Adaptor3d_Surface> BasisSurface() const;

  Standard_EXPORT virtual double OffsetValue() const;

  //! Computes the point of parameters (U, V) on the surface.
  //! Raises an exception on failure.
  [[nodiscard]] Standard_EXPORT virtual gp_Pnt EvalD0(double U, double V) const;

  //! Computes the point and first partial derivatives at (U, V).
  //! Raises an exception on failure.
  [[nodiscard]] Standard_EXPORT virtual Geom_Surface::ResD1 EvalD1(double U,
                                                                                  double V) const;

  //! Computes the point and partial derivatives up to 2nd order at (U, V).
  //! Raises an exception on failure.
  [[nodiscard]] Standard_EXPORT virtual Geom_Surface::ResD2 EvalD2(double U,
                                                                                  double V) const;

  //! Computes the point and partial derivatives up to 3rd order at (U, V).
  //! Raises an exception on failure.
  [[nodiscard]] Standard_EXPORT virtual Geom_Surface::ResD3 EvalD3(double U,
                                                                                  double V) const;

  //! Computes the derivative of order Nu in U and Nv in V at (U, V).
  //! Raises an exception on failure.
  [[nodiscard]] Standard_EXPORT virtual gp_Vec EvalDN(double U,
                                                                     double V,
                                                                     int    Nu,
                                                                     int    Nv) const;

  Standard_EXPORT ~Adaptor3d_Surface() override;
};

#endif // _Adaptor3d_Surface_HeaderFile
