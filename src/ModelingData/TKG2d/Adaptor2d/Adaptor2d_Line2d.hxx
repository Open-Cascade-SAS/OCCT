// Created on: 1995-05-02
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Adaptor2d_Line2d_HeaderFile
#define _Adaptor2d_Line2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Real.hxx>
#include <Adaptor2d_Curve2d.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Boolean.hxx>
#include <GeomAbs_CurveType.hxx>
class gp_Pnt2d;
class gp_Dir2d;
class gp_Lin2d;
class gp_Vec2d;
class gp_Circ2d;
class gp_Elips2d;
class gp_Hypr2d;
class gp_Parab2d;
class Geom2d_BezierCurve;
class Geom2d_BSplineCurve;

//! Use by the TopolTool to trim a surface.

class Adaptor2d_Line2d : public Adaptor2d_Curve2d
{
  DEFINE_STANDARD_RTTIEXT(Adaptor2d_Line2d, Adaptor2d_Curve2d)
public:
  Standard_EXPORT Adaptor2d_Line2d();

  Standard_EXPORT Adaptor2d_Line2d(const gp_Pnt2d& P,
                                   const gp_Dir2d& D,
                                   const double    UFirst,
                                   const double    ULast);

  //! Shallow copy of adaptor
  Standard_EXPORT virtual occ::handle<Adaptor2d_Curve2d> ShallowCopy() const override;

  Standard_EXPORT void Load(const gp_Lin2d& L);

  Standard_EXPORT void Load(const gp_Lin2d& L, const double UFirst, const double ULast);

  Standard_EXPORT double FirstParameter() const override;

  Standard_EXPORT double LastParameter() const override;

  Standard_EXPORT GeomAbs_Shape Continuity() const override;

  //! If necessary, breaks the curve in intervals of
  //! continuity <S>. And returns the number of
  //! intervals.
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
  Standard_EXPORT occ::handle<Adaptor2d_Curve2d> Trim(const double First,
                                                      const double Last,
                                                      const double Tol) const override;

  Standard_EXPORT bool IsClosed() const override;

  Standard_EXPORT bool IsPeriodic() const override;

  Standard_EXPORT double Period() const override;

  Standard_EXPORT gp_Pnt2d Value(const double X) const override;

  Standard_EXPORT void D0(const double X, gp_Pnt2d& P) const override;

  Standard_EXPORT void D1(const double X, gp_Pnt2d& P, gp_Vec2d& V) const override;

  Standard_EXPORT void D2(const double X, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const override;

  Standard_EXPORT void D3(const double X,
                          gp_Pnt2d&    P,
                          gp_Vec2d&    V1,
                          gp_Vec2d&    V2,
                          gp_Vec2d&    V3) const override;

  Standard_EXPORT gp_Vec2d DN(const double U, const int N) const override;

  Standard_EXPORT double Resolution(const double R3d) const override;

  Standard_EXPORT GeomAbs_CurveType GetType() const override;

  Standard_EXPORT gp_Lin2d Line() const override;

  Standard_EXPORT gp_Circ2d Circle() const override;

  Standard_EXPORT gp_Elips2d Ellipse() const override;

  Standard_EXPORT gp_Hypr2d Hyperbola() const override;

  Standard_EXPORT gp_Parab2d Parabola() const override;

  Standard_EXPORT int Degree() const override;

  Standard_EXPORT bool IsRational() const override;

  Standard_EXPORT int NbPoles() const override;

  Standard_EXPORT int NbKnots() const override;

  Standard_EXPORT occ::handle<Geom2d_BezierCurve> Bezier() const override;

  Standard_EXPORT occ::handle<Geom2d_BSplineCurve> BSpline() const override;

private:
  double  myUfirst;
  double  myUlast;
  gp_Ax2d myAx2d;
};

#endif // _Adaptor2d_Line2d_HeaderFile
