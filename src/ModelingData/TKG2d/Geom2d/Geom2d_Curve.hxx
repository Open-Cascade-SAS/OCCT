// Created on: 1993-03-24
// Created by: JCV
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

#ifndef _Geom2d_Curve_HeaderFile
#define _Geom2d_Curve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom2d_Geometry.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Geom2d_UndefinedDerivative.hxx>
#include <Geom2d_UndefinedValue.hxx>
#include <GeomAbs_Shape.hxx>

#include <optional>

class gp_Trsf2d;

//! The abstract class Curve describes the common
//! behavior of curves in 2D space. The Geom2d
//! package provides numerous concrete classes of
//! derived curves, including lines, circles, conics, Bezier
//! or BSpline curves, etc.
//! The main characteristic of these curves is that they
//! are parameterized. The Geom2d_Curve class shows:
//! - how to work with the parametric equation of a
//! curve in order to calculate the point of parameter
//! u, together with the vector tangent and the
//! derivative vectors of order 2, 3,..., N at this point;
//! - how to obtain general information about the curve
//! (for example, level of continuity, closed
//! characteristics, periodicity, bounds of the parameter field);
//! - how the parameter changes when a geometric
//! transformation is applied to the curve or when the
//! orientation of the curve is inverted.
//! All curves must have a geometric continuity: a curve is
//! at least "C0". Generally, this property is checked at
//! the time of construction or when the curve is edited.
//! Where this is not the case, the documentation
//! explicitly states so.
//! Warning
//! The Geom2d package does not prevent the
//! construction of curves with null length or curves which
//! self-intersect.
class Geom2d_Curve : public Geom2d_Geometry
{

public:
  //! Result of D1 evaluation: point and first derivative.
  struct ResD1
  {
    gp_Pnt2d Point;
    gp_Vec2d D1;
  };

  //! Result of D2 evaluation: point and first two derivatives.
  struct ResD2
  {
    gp_Pnt2d Point;
    gp_Vec2d D1;
    gp_Vec2d D2;
  };

  //! Result of D3 evaluation: point and first three derivatives.
  struct ResD3
  {
    gp_Pnt2d Point;
    gp_Vec2d D1;
    gp_Vec2d D2;
    gp_Vec2d D3;
  };

  //! Changes the direction of parametrization of <me>.
  //! The "FirstParameter" and the "LastParameter" are not changed
  //! but the orientation of the curve is modified. If the curve
  //! is bounded the StartPoint of the initial curve becomes the
  //! EndPoint of the reversed curve and the EndPoint of the initial
  //! curve becomes the StartPoint of the reversed curve.
  Standard_EXPORT virtual void Reverse() = 0;

  //! Computes the parameter on the reversed curve for
  //! the point of parameter U on this curve.
  //! Note: The point of parameter U on this curve is
  //! identical to the point of parameter
  //! ReversedParameter(U) on the reversed curve.
  Standard_EXPORT virtual double ReversedParameter(const double U) const = 0;

  //! Computes the parameter on the curve transformed by
  //! T for the point of parameter U on this curve.
  //! Note: this function generally returns U but it can be
  //! redefined (for example, on a line).
  Standard_EXPORT virtual double TransformedParameter(const double U, const gp_Trsf2d& T) const;

  //! Returns the coefficient required to compute the
  //! parametric transformation of this curve when
  //! transformation T is applied. This coefficient is the
  //! ratio between the parameter of a point on this curve
  //! and the parameter of the transformed point on the
  //! new curve transformed by T.
  //! Note: this function generally returns 1. but it can be
  //! redefined (for example, on a line).
  Standard_EXPORT virtual double ParametricTransformation(const gp_Trsf2d& T) const;

  //! Creates a reversed duplicate Changes the orientation of this curve. The first and
  //! last parameters are not changed, but the parametric
  //! direction of the curve is reversed.
  //! If the curve is bounded:
  //! - the start point of the initial curve becomes the end
  //! point of the reversed curve, and
  //! - the end point of the initial curve becomes the start
  //! point of the reversed curve.
  //! - Reversed creates a new curve.
  [[nodiscard]] Standard_EXPORT occ::handle<Geom2d_Curve> Reversed() const;

  //! Returns the value of the first parameter.
  //! Warnings :
  //! It can be RealFirst or RealLast from package Standard
  //! if the curve is infinite
  Standard_EXPORT virtual double FirstParameter() const = 0;

  //! Value of the last parameter.
  //! Warnings :
  //! It can be RealFirst or RealLast from package Standard
  //! if the curve is infinite
  Standard_EXPORT virtual double LastParameter() const = 0;

  //! Returns true if the curve is closed.
  //! Examples :
  //! Some curves such as circle are always closed, others such as line
  //! are never closed (by definition).
  //! Some Curves such as OffsetCurve can be closed or not. These curves
  //! are considered as closed if the distance between the first point
  //! and the last point of the curve is lower or equal to the Resolution
  //! from package gp which is a fixed criterion independent of the
  //! application.
  Standard_EXPORT virtual bool IsClosed() const = 0;

  //! Returns true if the parameter of the curve is periodic.
  //! It is possible only if the curve is closed and if the
  //! following relation is satisfied :
  //! for each parametric value U the distance between the point
  //! P(u) and the point P (u + T) is lower or equal to Resolution
  //! from package gp, T is the period and must be a constant.
  //! There are three possibilities :
  //! . the curve is never periodic by definition (SegmentLine)
  //! . the curve is always periodic by definition (Circle)
  //! . the curve can be defined as periodic (BSpline). In this case
  //! a function SetPeriodic allows you to give the shape of the
  //! curve. The general rule for this case is : if a curve can be
  //! periodic or not the default periodicity set is non periodic
  //! and you have to turn (explicitly) the curve into a periodic
  //! curve if you want the curve to be periodic.
  Standard_EXPORT virtual bool IsPeriodic() const = 0;

  //! Returns the period of this curve.
  //! raises if the curve is not periodic
  Standard_EXPORT virtual double Period() const;

  //! It is the global continuity of the curve :
  //! C0 : only geometric continuity,
  //! C1 : continuity of the first derivative all along the Curve,
  //! C2 : continuity of the second derivative all along the Curve,
  //! C3 : continuity of the third derivative all along the Curve,
  //! G1 : tangency continuity all along the Curve,
  //! G2 : curvature continuity all along the Curve,
  //! CN : the order of continuity is infinite.
  Standard_EXPORT virtual GeomAbs_Shape Continuity() const = 0;

  //! Returns true if the degree of continuity of this curve is at least N.
  //! Exceptions Standard_RangeError if N is less than 0.
  Standard_EXPORT virtual bool IsCN(const int N) const = 0;

  //! Computes the point of parameter U.
  //! Returns std::nullopt on failure.
  [[nodiscard]] Standard_EXPORT virtual std::optional<gp_Pnt2d> EvalD0(const double U) const = 0;

  //! Computes the point and first derivative at parameter U.
  //! Returns std::nullopt if the curve continuity is not C1.
  [[nodiscard]] Standard_EXPORT virtual std::optional<ResD1> EvalD1(const double U) const = 0;

  //! Computes the point and first two derivatives at parameter U.
  //! Returns std::nullopt if the curve continuity is not C2.
  [[nodiscard]] Standard_EXPORT virtual std::optional<ResD2> EvalD2(const double U) const = 0;

  //! Computes the point and first three derivatives at parameter U.
  //! Returns std::nullopt if the curve continuity is not C3.
  [[nodiscard]] Standard_EXPORT virtual std::optional<ResD3> EvalD3(const double U) const = 0;

  //! Computes the Nth derivative at parameter U.
  //! Returns std::nullopt if the curve continuity is not CN, or N < 1.
  [[nodiscard]] Standard_EXPORT virtual std::optional<gp_Vec2d> EvalDN(const double U, const int N) const = 0;

  //! Returns in P the point of parameter U.
  //! Throws on failure for backward compatibility.
  inline void D0(const double U, gp_Pnt2d& P) const
  {
    const std::optional<gp_Pnt2d> aP = EvalD0(U);
    if (!aP)
    {
      throw Geom2d_UndefinedValue("Geom2d_Curve::D0(): evaluation failed");
    }
    P = *aP;
  }

  //! Returns the point P of parameter U and the first derivative V1.
  //! Throws on failure for backward compatibility.
  inline void D1(const double U, gp_Pnt2d& P, gp_Vec2d& V1) const
  {
    const std::optional<ResD1> aR = EvalD1(U);
    if (!aR)
    {
      throw Geom2d_UndefinedDerivative("Geom2d_Curve::D1(): evaluation failed");
    }
    P  = aR->Point;
    V1 = aR->D1;
  }

  //! Returns the point P of parameter U, the first and second derivatives V1 and V2.
  //! Throws on failure for backward compatibility.
  inline void D2(const double U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const
  {
    const std::optional<ResD2> aR = EvalD2(U);
    if (!aR)
    {
      throw Geom2d_UndefinedDerivative("Geom2d_Curve::D2(): evaluation failed");
    }
    P  = aR->Point;
    V1 = aR->D1;
    V2 = aR->D2;
  }

  //! Returns the point P of parameter U, the first, the second and the third derivative.
  //! Throws on failure for backward compatibility.
  inline void D3(const double U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3) const
  {
    const std::optional<ResD3> aR = EvalD3(U);
    if (!aR)
    {
      throw Geom2d_UndefinedDerivative("Geom2d_Curve::D3(): evaluation failed");
    }
    P  = aR->Point;
    V1 = aR->D1;
    V2 = aR->D2;
    V3 = aR->D3;
  }

  //! Computes the Nth derivative vector. Throws on failure for backward compatibility.
  inline gp_Vec2d DN(const double U, const int N) const
  {
    const std::optional<gp_Vec2d> aVN = EvalDN(U, N);
    if (!aVN)
    {
      throw Geom2d_UndefinedDerivative("Geom2d_Curve::DN(): evaluation failed");
    }
    return *aVN;
  }

  //! Computes the point of parameter U on <me>. Implemented with D0.
  Standard_EXPORT gp_Pnt2d Value(const double U) const;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(Geom2d_Curve, Geom2d_Geometry)
};

#endif // _Geom2d_Curve_HeaderFile
