// Created on: 1993-03-10
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

#ifndef _Geom_Curve_HeaderFile
#define _Geom_Curve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom.hxx>
#include <Geom_Geometry.hxx>
#include <GeomAbs_Shape.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <Geom_UndefinedValue.hxx>

#include <optional>

class gp_Trsf;

//! The abstract class Curve describes the common
//! behavior of curves in 3D space. The Geom package
//! provides numerous concrete classes of derived
//! curves, including lines, circles, conics, Bezier or
//! BSpline curves, etc.
//! The main characteristic of these curves is that they
//! are parameterized. The Geom_Curve class shows:
//! - how to work with the parametric equation of a curve
//! in order to calculate the point of parameter u,
//! together with the vector tangent and the derivative
//! vectors of order 2, 3,..., N at this point;
//! - how to obtain general information about the curve
//! (for example, level of continuity, closed
//! characteristics, periodicity, bounds of the parameter field);
//! - how the parameter changes when a geometric
//! transformation is applied to the curve or when the
//! orientation of the curve is inverted.
//! All curves must have a geometric continuity: a curve is
//! at least "C0". Generally, this property is checked at
//! the time of construction or when the curve is edited.
//! Where this is not the case, the documentation states so explicitly.
//! Warning
//! The Geom package does not prevent the
//! construction of curves with null length or curves which
//! self-intersect.
class Geom_Curve : public Geom_Geometry
{

public:
  //! Changes the direction of parametrization of <me>.
  //! The "FirstParameter" and the "LastParameter" are not changed
  //! but the orientation of the curve is modified. If the curve
  //! is bounded the StartPoint of the initial curve becomes the
  //! EndPoint of the reversed curve and the EndPoint of the initial
  //! curve becomes the StartPoint of the reversed curve.
  Standard_EXPORT virtual void Reverse() = 0;

  //! Returns the parameter on the reversed curve for
  //! the point of parameter U on <me>.
  //!
  //! me->Reversed()->Value(me->ReversedParameter(U))
  //!
  //! is the same point as
  //!
  //! me->Value(U)
  Standard_EXPORT virtual double ReversedParameter(const double U) const = 0;

  //! Returns the parameter on the transformed curve for
  //! the transform of the point of parameter U on <me>.
  //!
  //! me->Transformed(T)->Value(me->TransformedParameter(U,T))
  //!
  //! is the same point as
  //!
  //! me->Value(U).Transformed(T)
  //!
  //! This methods returns <U>
  //!
  //! It can be redefined. For example on the Line.
  Standard_EXPORT virtual double TransformedParameter(const double U, const gp_Trsf& T) const;

  //! Returns a coefficient to compute the parameter on
  //! the transformed curve for the transform of the
  //! point on <me>.
  //!
  //! Transformed(T)->Value(U * ParametricTransformation(T))
  //!
  //! is the same point as
  //!
  //! Value(U).Transformed(T)
  //!
  //! This methods returns 1.
  //!
  //! It can be redefined. For example on the Line.
  Standard_EXPORT virtual double ParametricTransformation(const gp_Trsf& T) const;

  //! Returns a copy of <me> reversed.
  [[nodiscard]] Standard_EXPORT occ::handle<Geom_Curve> Reversed() const;

  //! Returns the value of the first parameter.
  //! Warnings :
  //! It can be RealFirst from package Standard
  //! if the curve is infinite
  Standard_EXPORT virtual double FirstParameter() const = 0;

  //! Returns the value of the last parameter.
  //! Warnings :
  //! It can be RealLast from package Standard
  //! if the curve is infinite
  Standard_EXPORT virtual double LastParameter() const = 0;

  //! Returns true if the curve is closed.
  //! Some curves such as circle are always closed, others such as line
  //! are never closed (by definition).
  //! Some Curves such as OffsetCurve can be closed or not. These curves
  //! are considered as closed if the distance between the first point
  //! and the last point of the curve is lower or equal to the Resolution
  //! from package gp which is a fixed criterion independent of the
  //! application.
  Standard_EXPORT virtual bool IsClosed() const = 0;

  //! Is the parametrization of the curve periodic ?
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
  //! Exceptions Standard_NoSuchObject if this curve is not periodic.
  Standard_EXPORT virtual double Period() const;

  //! It is the global continuity of the curve
  //! C0 : only geometric continuity,
  //! C1 : continuity of the first derivative all along the Curve,
  //! C2 : continuity of the second derivative all along the Curve,
  //! C3 : continuity of the third derivative all along the Curve,
  //! G1 : tangency continuity all along the Curve,
  //! G2 : curvature continuity all along the Curve,
  //! CN : the order of continuity is infinite.
  Standard_EXPORT virtual GeomAbs_Shape Continuity() const = 0;

  //! Returns true if the degree of continuity of this curve is at least N.
  //! Exceptions - Standard_RangeError if N is less than 0.
  Standard_EXPORT virtual bool IsCN(const int N) const = 0;

  //! Computes the point of parameter U.
  //! Returns std::nullopt on failure (e.g. OffsetCurve at singular point).
  Standard_EXPORT virtual std::optional<gp_Pnt> EvalD0(const double U) const = 0;

  //! Computes the point and first derivative at parameter U.
  //! Returns std::nullopt if the curve continuity is not C1.
  Standard_EXPORT virtual std::optional<Geom_CurveD1> EvalD1(const double U) const = 0;

  //! Computes the point and first two derivatives at parameter U.
  //! Returns std::nullopt if the curve continuity is not C2.
  Standard_EXPORT virtual std::optional<Geom_CurveD2> EvalD2(const double U) const = 0;

  //! Computes the point and first three derivatives at parameter U.
  //! Returns std::nullopt if the curve continuity is not C3.
  Standard_EXPORT virtual std::optional<Geom_CurveD3> EvalD3(const double U) const = 0;

  //! Computes the Nth derivative at parameter U.
  //! Returns std::nullopt if the curve continuity is not CN, or N < 1.
  Standard_EXPORT virtual std::optional<gp_Vec> EvalDN(const double U, const int N) const = 0;

  //! Returns in P the point of parameter U.
  //! Throws on failure for backward compatibility.
  inline void D0(const double U, gp_Pnt& P) const
  {
    const std::optional<gp_Pnt> aP = EvalD0(U);
    if (!aP)
    {
      throw Geom_UndefinedValue("Geom_Curve::EvalD0(): evaluation failed");
    }
    P = *aP;
  }

  //! Returns the point P of parameter U and the first derivative V1.
  //! Throws on failure for backward compatibility.
  inline void D1(const double U, gp_Pnt& P, gp_Vec& V1) const
  {
    const std::optional<Geom_CurveD1> aR = EvalD1(U);
    if (!aR)
    {
      throw Geom_UndefinedDerivative("Geom_Curve::EvalD1(): evaluation failed");
    }
    P  = aR->Point;
    V1 = aR->D1;
  }

  //! Returns the point P of parameter U, the first and second derivatives V1 and V2.
  //! Throws on failure for backward compatibility.
  inline void D2(const double U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const
  {
    const std::optional<Geom_CurveD2> aR = EvalD2(U);
    if (!aR)
    {
      throw Geom_UndefinedDerivative("Geom_Curve::EvalD2(): evaluation failed");
    }
    P  = aR->Point;
    V1 = aR->D1;
    V2 = aR->D2;
  }

  //! Returns the point P of parameter U, the first, the second and the third derivative.
  //! Throws on failure for backward compatibility.
  inline void D3(const double U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3) const
  {
    const std::optional<Geom_CurveD3> aR = EvalD3(U);
    if (!aR)
    {
      throw Geom_UndefinedDerivative("Geom_Curve::EvalD3(): evaluation failed");
    }
    P  = aR->Point;
    V1 = aR->D1;
    V2 = aR->D2;
    V3 = aR->D3;
  }

  //! The returned vector gives the value of the derivative for the order of derivation N.
  //! Throws on failure for backward compatibility.
  inline gp_Vec DN(const double U, const int N) const
  {
    const std::optional<gp_Vec> aVN = EvalDN(U, N);
    if (!aVN)
    {
      throw Geom_UndefinedDerivative("Geom_Curve::DN(): evaluation failed");
    }
    return *aVN;
  }

  //! Computes the point of parameter U on <me>.
  //! It is implemented with D0.
  Standard_EXPORT gp_Pnt Value(const double U) const;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(Geom_Curve, Geom_Geometry)
};

#endif // _Geom_Curve_HeaderFile
