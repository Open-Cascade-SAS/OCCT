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

#ifndef _Geom_TrimmedCurve_HeaderFile
#define _Geom_TrimmedCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom_BoundedCurve.hxx>
#include <GeomAbs_Shape.hxx>
class Geom_Curve;
class gp_Trsf;
class Geom_Geometry;

//! Describes a portion of a curve (termed the "basis
//! curve") limited by two parameter values inside the
//! parametric domain of the basis curve.
//! The trimmed curve is defined by:
//! - the basis curve, and
//! - the two parameter values which limit it.
//! The trimmed curve can either have the same
//! orientation as the basis curve or the opposite orientation.
class Geom_TrimmedCurve : public Geom_BoundedCurve
{

public:
  //! Constructs a trimmed curve from the basis curve C
  //! which is limited between parameter values U1 and U2.
  //! Note: - U1 can be greater or less than U2; in both cases,
  //! the returned curve is oriented from U1 to U2.
  //! - If the basis curve C is periodic, there is an
  //! ambiguity because two parts are available. In this
  //! case, the trimmed curve has the same orientation
  //! as the basis curve if Sense is true (default value)
  //! or the opposite orientation if Sense is false.
  //! - If the curve is closed but not periodic, it is not
  //! possible to keep the part of the curve which
  //! includes the junction point (except if the junction
  //! point is at the beginning or at the end of the
  //! trimmed curve). If you tried to do this, you could
  //! alter the fundamental characteristics of the basis
  //! curve, which are used, for example, to compute
  //! the derivatives of the trimmed curve. The rules
  //! for a closed curve are therefore the same as
  //! those for an open curve.
  //! Warning: The trimmed curve is built from a copy of curve C.
  //! Therefore, when C is modified, the trimmed curve
  //! is not modified.
  //! - If the basis curve is periodic and theAdjustPeriodic is True,
  //! the bounds of the trimmed curve may be different from U1 and U2
  //! if the parametric origin of the basis curve is within
  //! the arc of the trimmed curve. In this case, the
  //! modified parameter will be equal to U1 or U2
  //! plus or minus the period.
  //! When theAdjustPeriodic is False, parameters U1 and U2 will be
  //! the same, without adjustment into the first period.
  //! Exceptions
  //! Standard_ConstructionError if:
  //! - C is not periodic and U1 or U2 is outside the
  //! bounds of C, or
  //! - U1 is equal to U2.
  Standard_EXPORT Geom_TrimmedCurve(const occ::handle<Geom_Curve>& C,
                                    const double                   U1,
                                    const double                   U2,
                                    const bool                     Sense             = true,
                                    const bool                     theAdjustPeriodic = true);

  //! Changes the orientation of this trimmed curve.
  //! As a result:
  //! - the basis curve is reversed,
  //! - the start point of the initial curve becomes the
  //! end point of the reversed curve,
  //! - the end point of the initial curve becomes the
  //! start point of the reversed curve,
  //! - the first and last parameters are recomputed.
  //! If the trimmed curve was defined by:
  //! - a basis curve whose parameter range is [ 0., 1. ],
  //! - the two trim values U1 (first parameter) and U2 (last parameter),
  //! the reversed trimmed curve is defined by:
  //! - the reversed basis curve, whose parameter range is still [ 0., 1. ],
  //! - the two trim values 1. - U2 (first parameter) and 1. - U1 (last parameter).
  Standard_EXPORT void Reverse() final;

  //! Computes the parameter on the reversed curve for
  //! the point of parameter U on this trimmed curve.
  Standard_EXPORT double ReversedParameter(const double U) const final;

  //! Changes this trimmed curve, by redefining the
  //! parameter values U1 and U2 which limit its basis curve.
  //! Note: If the basis curve is periodic, the trimmed curve
  //! has the same orientation as the basis curve if Sense
  //! is true (default value) or the opposite orientation if Sense is false.
  //! Warning
  //! If the basis curve is periodic and theAdjustPeriodic is True,
  //! the bounds of the trimmed curve may be different from U1 and U2 if the
  //! parametric origin of the basis curve is within the arc of
  //! the trimmed curve. In this case, the modified
  //! parameter will be equal to U1 or U2 plus or minus the period.
  //! When theAdjustPeriodic is False, parameters U1 and U2 will be
  //! the same, without adjustment into the first period.
  //! Exceptions
  //! Standard_ConstructionError if:
  //! - the basis curve is not periodic, and either U1 or U2
  //! are outside the bounds of the basis curve, or
  //! - U1 is equal to U2.
  Standard_EXPORT void SetTrim(const double U1,
                               const double U2,
                               const bool   Sense             = true,
                               const bool   theAdjustPeriodic = true);

  //! Returns the basis curve.
  //! Warning
  //! This function does not return a constant reference.
  //! Consequently, any modification of the returned value
  //! directly modifies the trimmed curve.
  Standard_EXPORT occ::handle<Geom_Curve> BasisCurve() const;

  //! Returns the continuity of the curve :
  //! C0 : only geometric continuity,
  //! C1 : continuity of the first derivative all along the Curve,
  //! C2 : continuity of the second derivative all along the Curve,
  //! C3 : continuity of the third derivative all along the Curve,
  //! CN : the order of continuity is infinite.
  Standard_EXPORT GeomAbs_Shape Continuity() const final;

  //! Returns true if the degree of continuity of the basis
  //! curve of this trimmed curve is at least N. A trimmed
  //! curve is at least "C0" continuous.
  //! Warnings :
  //! The continuity of the trimmed curve can be greater than
  //! the continuity of the basis curve because you consider
  //! only a part of the basis curve.
  //! Raised if N < 0.
  Standard_EXPORT bool IsCN(const int N) const final;

  //! Returns the end point of <me>. This point is the
  //! evaluation of the curve for the "LastParameter".
  Standard_EXPORT gp_Pnt EndPoint() const final;

  //! Returns the value of the first parameter of <me>.
  //! The first parameter is the parameter of the "StartPoint"
  //! of the trimmed curve.
  Standard_EXPORT double FirstParameter() const final;

  //! Returns True if the distance between the StartPoint and
  //! the EndPoint is lower or equal to Resolution from package gp.
  Standard_EXPORT bool IsClosed() const final;

  //! Always returns FALSE (independently of the type of basis curve).
  Standard_EXPORT bool IsPeriodic() const final;

  //! Returns the period of the basis curve of this trimmed curve.
  //! Exceptions
  //! Standard_NoSuchObject if the basis curve is not periodic.
  Standard_EXPORT double Period() const final;

  //! Returns the value of the last parameter of <me>.
  //! The last parameter is the parameter of the "EndPoint" of the
  //! trimmed curve.
  Standard_EXPORT double LastParameter() const final;

  //! Returns the start point of <me>.
  //! This point is the evaluation of the curve from the
  //! "FirstParameter".
  //! value and derivatives
  //! Warnings :
  //! The returned derivatives have the same orientation as the
  //! derivatives of the basis curve even if the trimmed curve
  //! has not the same orientation as the basis curve.
  Standard_EXPORT gp_Pnt StartPoint() const final;

  //! Returns the point of parameter U.
  //!
  //! If the basis curve is an OffsetCurve sometimes it is not
  //! possible to do the evaluation of the curve at the parameter
  //! U (see class OffsetCurve).
  Standard_EXPORT std::optional<gp_Pnt> EvalD0(const double U) const final;

  //! Raised if the continuity of the curve is not C1.
  Standard_EXPORT std::optional<Geom_Curve::ResD1> EvalD1(const double U) const final;

  //! Raised if the continuity of the curve is not C2.
  Standard_EXPORT std::optional<Geom_Curve::ResD2> EvalD2(const double U) const final;

  //! Raised if the continuity of the curve is not C3.
  Standard_EXPORT std::optional<Geom_Curve::ResD3> EvalD3(const double U) const final;

  //! N is the order of derivation.
  //! Raised if the continuity of the curve is not CN.
  //! Raised if N < 1.
  //! geometric transformations
  Standard_EXPORT std::optional<gp_Vec> EvalDN(const double U, const int N) const final;

  //! Applies the transformation T to this trimmed curve.
  //! Warning The basis curve is also modified.
  Standard_EXPORT void Transform(const gp_Trsf& T) final;

  //! Returns the parameter on the transformed curve for
  //! the transform of the point of parameter U on <me>.
  //!
  //! me->Transformed(T)->Value(me->TransformedParameter(U,T))
  //!
  //! is the same point as
  //!
  //! me->Value(U).Transformed(T)
  //!
  //! This methods calls the basis curve method.
  Standard_EXPORT double TransformedParameter(const double U, const gp_Trsf& T) const final;

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
  //! This methods calls the basis curve method.
  Standard_EXPORT double ParametricTransformation(const gp_Trsf& T) const final;

  //! Creates a new object which is a copy of this trimmed curve.
  Standard_EXPORT occ::handle<Geom_Geometry> Copy() const final;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const final;

  DEFINE_STANDARD_RTTIEXT(Geom_TrimmedCurve, Geom_BoundedCurve)

private:
  occ::handle<Geom_Curve> basisCurve;
  double                  uTrim1;
  double                  uTrim2;
};

#endif // _Geom_TrimmedCurve_HeaderFile
