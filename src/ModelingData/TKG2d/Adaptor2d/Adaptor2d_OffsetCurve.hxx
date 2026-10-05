// Created on: 1993-04-15
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

#ifndef _Adaptor2d_OffsetCurve_HeaderFile
#define _Adaptor2d_OffsetCurve_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>

class gp_Pnt2d;
class gp_Vec2d;
class gp_Lin2d;
class gp_Circ2d;
class gp_Elips2d;
class gp_Hypr2d;
class gp_Parab2d;
class Geom2d_BezierCurve;
class Geom2d_BSplineCurve;

//! Defines an Offset curve (algorithmic 2d curve).
class Adaptor2d_OffsetCurve : public Adaptor2d_Curve2d
{
  DEFINE_STANDARD_RTTIEXT(Adaptor2d_OffsetCurve, Adaptor2d_Curve2d)
public:
  //! The Offset is set to 0.
  Standard_EXPORT Adaptor2d_OffsetCurve();

  //! The curve is loaded. The Offset is set to 0.
  Standard_EXPORT Adaptor2d_OffsetCurve(const occ::handle<Adaptor2d_Curve2d>& C);

  //! Creates an OffsetCurve curve.
  //! The Offset is set to Offset.
  Standard_EXPORT Adaptor2d_OffsetCurve(const occ::handle<Adaptor2d_Curve2d>& C,
                                        const double                          Offset);

  //! Create an Offset curve.
  //! WFirst,WLast define the bounds of the Offset curve.
  Standard_EXPORT Adaptor2d_OffsetCurve(const occ::handle<Adaptor2d_Curve2d>& C,
                                        const double                          Offset,
                                        const double                          WFirst,
                                        const double                          WLast);

  //! Shallow copy of adaptor.
  Standard_EXPORT occ::handle<Adaptor2d_Curve2d> ShallowCopy() const override;

  //! Changes the curve. The Offset is reset to 0.
  Standard_EXPORT void Load(const occ::handle<Adaptor2d_Curve2d>& S);

  //! Changes the Offset on the current Curve.
  Standard_EXPORT void Load(const double Offset);

  //! Changes the Offset Curve on the current Curve.
  Standard_EXPORT void Load(const double Offset, const double WFirst, const double WLast);

  const occ::handle<Adaptor2d_Curve2d>& Curve() const { return myCurve; }

  double Offset() const { return myOffset; }

  double FirstParameter() const override { return myFirst; }

  double LastParameter() const override { return myLast; }

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

  //! Evaluates the point on the offset curve.
  //! @param[in] theU curve parameter
  //! @return evaluated point
  [[nodiscard]] Standard_EXPORT gp_Pnt2d EvalD0(const double theU) const final;

  //! Evaluates the point and first derivative on the offset curve.
  //! @param[in] theU curve parameter
  //! @return point and first derivative
  [[nodiscard]] Standard_EXPORT Geom2d_Curve::ResD1 EvalD1(const double theU) const final;

  //! Evaluates the point and first two derivatives on the offset curve.
  //! @param[in] theU curve parameter
  //! @return point and derivatives through the second order
  [[nodiscard]] Standard_EXPORT Geom2d_Curve::ResD2 EvalD2(const double theU) const final;

  //! Evaluates the point and first three derivatives on the offset curve.
  //! @param[in] theU curve parameter
  //! @return point and derivatives through the third order
  [[nodiscard]] Standard_EXPORT Geom2d_Curve::ResD3 EvalD3(const double theU) const final;

  //! Evaluates a derivative of the requested order.
  //! @param[in] theU curve parameter
  //! @param[in] theN derivative order
  //! @return derivative vector of order theN
  //! @throws Standard_NotImplemented this operation is not implemented
  [[nodiscard]] Standard_EXPORT gp_Vec2d EvalDN(const double theU, const int theN) const final;

  //! Returns the parametric resolution corresponding
  //! to the real space resolution <R3d>.
  Standard_EXPORT double Resolution(const double R3d) const override;

  //! Returns the type of the curve in the current
  //! interval: Line, Circle, Ellipse, Hyperbola,
  //! Parabola, BezierCurve, BSplineCurve, OtherCurve.
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

  Standard_EXPORT int NbSamples() const override;

private:
  occ::handle<Adaptor2d_Curve2d> myCurve;
  double                         myOffset;
  double                         myFirst;
  double                         myLast;
};

#endif // _Adaptor2d_OffsetCurve_HeaderFile
