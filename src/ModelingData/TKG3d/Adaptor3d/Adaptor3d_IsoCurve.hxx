// Created on: 1993-03-11
// Created by: Isabelle GRIGNON
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

#ifndef _Adaptor3d_IsoCurve_HeaderFile
#define _Adaptor3d_IsoCurve_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <GeomAbs_IsoType.hxx>

//! Defines an isoparametric curve on a surface. The
//! type of isoparametric curve (U or V) is defined
//! with the enumeration IsoType from GeomAbs if
//! NoneIso is given an error is raised.
class Adaptor3d_IsoCurve : public Adaptor3d_Curve
{
  DEFINE_STANDARD_RTTIEXT(Adaptor3d_IsoCurve, Adaptor3d_Curve)
public:
  //! The iso is set to NoneIso.
  Standard_EXPORT Adaptor3d_IsoCurve();

  //! The surface is loaded. The iso is set to NoneIso.
  Standard_EXPORT Adaptor3d_IsoCurve(const occ::handle<Adaptor3d_Surface>& S);

  //! Creates an IsoCurve curve. Iso defines the type
  //! (isoU or isoU) Param defines the value of the
  //! iso. The bounds of the iso are the bounds of
  //! the surface.
  Standard_EXPORT Adaptor3d_IsoCurve(const occ::handle<Adaptor3d_Surface>& S,
                                     const GeomAbs_IsoType                 Iso,
                                     const double                          Param);

  //! Create an IsoCurve curve. Iso defines the type
  //! (isoU or isov). Param defines the value of the
  //! iso. WFirst,WLast define the bounds of the iso.
  Standard_EXPORT Adaptor3d_IsoCurve(const occ::handle<Adaptor3d_Surface>& S,
                                     const GeomAbs_IsoType                 Iso,
                                     const double                          Param,
                                     const double                          WFirst,
                                     const double                          WLast);

  //! Shallow copy of adaptor
  Standard_EXPORT occ::handle<Adaptor3d_Curve> ShallowCopy() const override;

  //! Changes the surface. The iso is reset to NoneIso.
  Standard_EXPORT void Load(const occ::handle<Adaptor3d_Surface>& S);

  //! Changes the iso on the current surface.
  Standard_EXPORT void Load(const GeomAbs_IsoType Iso, const double Param);

  //! Changes the iso on the current surface.
  Standard_EXPORT void Load(const GeomAbs_IsoType Iso,
                            const double          Param,
                            const double          WFirst,
                            const double          WLast);

  const occ::handle<Adaptor3d_Surface>& Surface() const { return mySurface; }

  GeomAbs_IsoType Iso() const { return myIso; }

  double Parameter() const { return myParameter; }

  double FirstParameter() const override { return myFirst; }

  double LastParameter() const override { return myLast; }

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
  //! Raised if the continuity of the current interval is not CN.
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
  occ::handle<Adaptor3d_Surface> mySurface;
  GeomAbs_IsoType                myIso;
  double                         myFirst;
  double                         myLast;
  double                         myParameter;
};

#endif // _Adaptor3d_IsoCurve_HeaderFile
