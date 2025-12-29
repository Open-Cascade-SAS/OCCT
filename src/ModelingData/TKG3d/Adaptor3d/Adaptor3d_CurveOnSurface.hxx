// Created on: 1993-02-22
// Created by: Modelistation
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

#ifndef _Adaptor3d_CurveOnSurface_HeaderFile
#define _Adaptor3d_CurveOnSurface_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_Surface.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_Shape.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>

//! An interface between the services provided by a curve
//! lying on a surface from the package Geom and those
//! required of the curve by algorithms which use it. The
//! curve is defined as a 2D curve from the Geom2d
//! package, in the parametric space of the surface.
class Adaptor3d_CurveOnSurface : public Adaptor3d_Curve
{
  DEFINE_STANDARD_RTTIEXT(Adaptor3d_CurveOnSurface, Adaptor3d_Curve)
public:
  Standard_EXPORT Adaptor3d_CurveOnSurface();

  Standard_EXPORT Adaptor3d_CurveOnSurface(const occ::handle<Adaptor3d_Surface>& S);

  //! Creates a CurveOnSurface from the 2d curve <C> and
  //! the surface <S>.
  Standard_EXPORT Adaptor3d_CurveOnSurface(const occ::handle<Adaptor2d_Curve2d>& C,
                                           const occ::handle<Adaptor3d_Surface>& S);

  //! Shallow copy of adaptor
  Standard_EXPORT occ::handle<Adaptor3d_Curve> ShallowCopy() const override;

  //! Changes the surface.
  Standard_EXPORT void Load(const occ::handle<Adaptor3d_Surface>& S);

  //! Changes the 2d curve.
  Standard_EXPORT void Load(const occ::handle<Adaptor2d_Curve2d>& C);

  //! Load both curve and surface.
  Standard_EXPORT void Load(const occ::handle<Adaptor2d_Curve2d>& C,
                            const occ::handle<Adaptor3d_Surface>& S);

  Standard_EXPORT const occ::handle<Adaptor2d_Curve2d>& GetCurve() const;

  Standard_EXPORT const occ::handle<Adaptor3d_Surface>& GetSurface() const;

  Standard_EXPORT occ::handle<Adaptor2d_Curve2d>& ChangeCurve();

  Standard_EXPORT occ::handle<Adaptor3d_Surface>& ChangeSurface();

  Standard_EXPORT double FirstParameter() const override;

  Standard_EXPORT double LastParameter() const override;

  Standard_EXPORT GeomAbs_Shape Continuity() const override;

  //! Returns the number of intervals for continuity
  //! <S>. May be one if Continuity(me) >= <S>
  Standard_EXPORT int NbIntervals(const GeomAbs_Shape S) const override;

  //! Stores in <T> the parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT void Intervals(NCollection_Array1<double>& T,
                                 const GeomAbs_Shape         S) const override;

  //! Returns a curve equivalent of <me> between
  //! parameters <First> and <Last>. <Tol> is used to
  //! test for 3d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT occ::handle<Adaptor3d_Curve> Trim(const double First,
                                                    const double Last,
                                                    const double Tol) const override;

  Standard_EXPORT bool IsClosed() const override;

  Standard_EXPORT bool IsPeriodic() const override;

  Standard_EXPORT double Period() const override;

  //! Computes the point of parameter U on the curve.
  Standard_EXPORT gp_Pnt Value(const double U) const override;

  //! Computes the point of parameter U on the curve.
  Standard_EXPORT void D0(const double U, gp_Pnt& P) const override;

  //! Computes the point of parameter U on the curve with its
  //! first derivative.
  //! Raised if the continuity of the current interval
  //! is not C1.
  Standard_EXPORT void D1(const double U, gp_Pnt& P, gp_Vec& V) const override;

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  //! Raised if the continuity of the current interval
  //! is not C2.
  Standard_EXPORT void D2(const double U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const override;

  //! Returns the point P of parameter U, the first, the second
  //! and the third derivative.
  //! Raised if the continuity of the current interval
  //! is not C3.
  Standard_EXPORT void D3(const double U,
                          gp_Pnt&      P,
                          gp_Vec&      V1,
                          gp_Vec&      V2,
                          gp_Vec&      V3) const override;

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if the continuity of the current interval
  //! is not CN.
  //! Raised if N < 1.
  Standard_EXPORT gp_Vec DN(const double U, const int N) const override;

  //! Returns the parametric resolution corresponding
  //! to the real space resolution <R3d>.
  Standard_EXPORT double Resolution(const double R3d) const override;

  //! Returns the type of the curve in the current
  //! interval: Line, Circle, Ellipse, Hyperbola,
  //! Parabola, BezierCurve, BSplineCurve, OtherCurve.
  Standard_EXPORT GeomAbs_CurveType GetType() const override;

  Standard_EXPORT gp_Lin Line() const override;

  Standard_EXPORT gp_Circ Circle() const override;

  Standard_EXPORT gp_Elips Ellipse() const override;

  Standard_EXPORT gp_Hypr Hyperbola() const override;

  Standard_EXPORT gp_Parab Parabola() const override;

  Standard_EXPORT int Degree() const override;

  Standard_EXPORT bool IsRational() const override;

  Standard_EXPORT int NbPoles() const override;

  Standard_EXPORT int NbKnots() const override;

  Standard_EXPORT occ::handle<Geom_BezierCurve> Bezier() const override;

  Standard_EXPORT occ::handle<Geom_BSplineCurve> BSpline() const override;

private:
  Standard_EXPORT void EvalKPart();

  //! Evaluates myFirstSurf and myLastSurf
  //! for trimming the curve on surface.
  //! Following methods output left-bottom and right-top points
  //! of located part on surface
  //! for trimming the curve on surface.
  Standard_EXPORT void EvalFirstLastSurf();

  Standard_EXPORT void LocatePart(const gp_Pnt2d&                       UV,
                                  const gp_Vec2d&                       DUV,
                                  const occ::handle<Adaptor3d_Surface>& S,
                                  gp_Pnt2d&                             LeftBot,
                                  gp_Pnt2d&                             RightTop) const;

  Standard_EXPORT bool LocatePart_RevExt(const gp_Pnt2d&                       UV,
                                         const gp_Vec2d&                       DUV,
                                         const occ::handle<Adaptor3d_Surface>& S,
                                         gp_Pnt2d&                             LeftBot,
                                         gp_Pnt2d&                             RightTop) const;

  Standard_EXPORT bool LocatePart_Offset(const gp_Pnt2d&                       UV,
                                         const gp_Vec2d&                       DUV,
                                         const occ::handle<Adaptor3d_Surface>& S,
                                         gp_Pnt2d&                             LeftBot,
                                         gp_Pnt2d&                             RightTop) const;

  //! Extracts the numbers of knots which equal
  //! the point and checks derivative components
  //! by zero equivalence.
  Standard_EXPORT void FindBounds(const NCollection_Array1<double>& Arr,
                                  const double                      XYComp,
                                  const double                      DUVComp,
                                  int&                              Bnd1,
                                  int&                              Bnd2,
                                  bool&                             DerIsNull) const;

private:
  occ::handle<Adaptor3d_Surface>             mySurface;
  occ::handle<Adaptor2d_Curve2d>             myCurve;
  GeomAbs_CurveType                          myType;
  gp_Circ                                    myCirc;
  gp_Lin                                     myLin;
  occ::handle<Adaptor3d_Surface>             myFirstSurf;
  occ::handle<Adaptor3d_Surface>             myLastSurf;
  occ::handle<NCollection_HSequence<double>> myIntervals;
  GeomAbs_Shape                              myIntCont;
};

#endif // _Adaptor3d_CurveOnSurface_HeaderFile
