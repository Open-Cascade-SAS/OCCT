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

#ifndef _Geom2d_TrimmedCurve_HeaderFile
#define _Geom2d_TrimmedCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom2d_BoundedCurve.hxx>
#include <GeomAbs_Shape.hxx>
class Geom2d_Curve;
class gp_Trsf2d;
class Geom2d_Geometry;

//! Defines a portion of a curve limited by two values of
//! parameters inside the parametric domain of the curve.
//! The trimmed curve is defined by:
//! - the basis curve, and
//! - the two parameter values which limit it.
//! The trimmed curve can either have the same
//! orientation as the basis curve or the opposite orientation.
class Geom2d_TrimmedCurve : public Geom2d_BoundedCurve
{

public:
  //! Creates a trimmed curve from the basis curve C limited between
  //! U1 and U2.
  //!
  //! . U1 can be greater or lower than U2.
  //! . The returned curve is oriented from U1 to U2.
  //! . If the basis curve C is periodic there is an ambiguity
  //! because two parts are available. In this case by default
  //! the trimmed curve has the same orientation as the basis
  //! curve (Sense = True). If Sense = False then the orientation
  //! of the trimmed curve is opposite to the orientation of the
  //! basis curve C.
  //! If the curve is closed but not periodic it is not possible
  //! to keep the part of the curve including the junction point
  //! (except if the junction point is at the beginning or
  //! at the end of the trimmed curve) because you could lose the
  //! fundamental characteristics of the basis curve which are
  //! used for example to compute the derivatives of the trimmed
  //! curve. So for a closed curve the rules are the same as for
  //! a open curve.
  //! Warnings :
  //! In this package the entities are not shared. The TrimmedCurve is
  //! built with a copy of the curve C. So when C is modified the
  //! TrimmedCurve is not modified
  //! Warnings :
  //! If <C> is periodic and <theAdjustPeriodic> is True, parametrics
  //! bounds of the TrimmedCurve, can be different to [<U1>;<U2>},
  //! if <U1> or <U2> are not in the principal period.
  //! Include :
  //! For more explanation see the scheme given with this class.
  //! Raises ConstructionError the C is not periodic and U1 or U2 are out of
  //! the bounds of C.
  //! Raised if U1 = U2.
  Standard_EXPORT Geom2d_TrimmedCurve(const occ::handle<Geom2d_Curve>& C,
                                      const double                     U1,
                                      const double                     U2,
                                      const bool                       Sense             = true,
                                      const bool                       theAdjustPeriodic = true);

  //! Changes the direction of parametrization of <me>. The first and
  //! the last parametric values are modified. The "StartPoint"
  //! of the initial curve becomes the "EndPoint" of the reversed
  //! curve and the "EndPoint" of the initial curve becomes the
  //! "StartPoint" of the reversed curve.
  //! Example  -   If the trimmed curve is defined by:
  //! - a basis curve whose parameter range is [ 0.,1. ], and
  //! - the two trim values U1 (first parameter) and U2 (last parameter),
  //! the reversed trimmed curve is defined by:
  //! - the reversed basis curve, whose parameter range is still [ 0.,1. ], and
  //! - the two trim values 1. - U2 (first parameter)
  //! and 1. - U1 (last parameter).
  Standard_EXPORT void Reverse() final;

  //! Returns the parameter on the reversed curve for
  //! the point of parameter U on <me>.
  //!
  //! returns UFirst + ULast - U
  Standard_EXPORT double ReversedParameter(const double U) const final;

  //! Changes this trimmed curve, by redefining the
  //! parameter values U1 and U2, which limit its basis curve.
  //! Note: If the basis curve is periodic, the trimmed curve
  //! has the same orientation as the basis curve if Sense
  //! is true (default value) or the opposite orientation if Sense is false.
  //! Warning
  //! If the basis curve is periodic and theAdjustPeriodic is True,
  //! the bounds of the trimmed curve may be different from U1 and U2 if the
  //! parametric origin of the basis curve is within the arc
  //! of the trimmed curve. In this case, the modified
  //! parameter will be equal to U1 or U2 plus or minus the period.
  //! If theAdjustPeriodic is False, parameters U1 and U2 will stay unchanged.
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
  Standard_EXPORT occ::handle<Geom2d_Curve> BasisCurve() const;

  //! Returns the global continuity of the basis curve of this trimmed curve.
  //! C0 : only geometric continuity,
  //! C1 : continuity of the first derivative all along the Curve,
  //! C2 : continuity of the second derivative all along the Curve,
  //! C3 : continuity of the third derivative all along the Curve,
  //! CN : the order of continuity is infinite.
  Standard_EXPORT GeomAbs_Shape Continuity() const final;

  //! --- Purpose
  //! Returns True if the order of continuity of the
  //! trimmed curve is N. A trimmed curve is at least "C0" continuous.
  //! Warnings :
  //! The continuity of the trimmed curve can be greater than
  //! the continuity of the basis curve because you consider
  //! only a part of the basis curve.
  //! Raised if N < 0.
  Standard_EXPORT bool IsCN(const int N) const final;

  //! Returns the end point of <me>. This point is the
  //! evaluation of the curve for the "LastParameter".
  Standard_EXPORT gp_Pnt2d EndPoint() const final;

  //! Returns the value of the first parameter of <me>.
  //! The first parameter is the parameter of the "StartPoint"
  //! of the trimmed curve.
  Standard_EXPORT double FirstParameter() const final;

  //! Returns True if the distance between the StartPoint and
  //! the EndPoint is lower or equal to Resolution from package
  //! gp.
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
  //! derivatives of the basis curve.
  Standard_EXPORT gp_Pnt2d StartPoint() const final;

  //! If the basis curve is an OffsetCurve sometimes it is not
  //! possible to do the evaluation of the curve at the parameter
  //! U (see class OffsetCurve).
  Standard_EXPORT gp_Pnt2d EvalD0(const double U) const final;

  //! Raised if the continuity of the curve is not C1.
  Standard_EXPORT Geom2d_Curve::ResD1 EvalD1(const double U) const final;

  //! Raised if the continuity of the curve is not C2.
  Standard_EXPORT Geom2d_Curve::ResD2 EvalD2(const double U) const final;

  //! Raised if the continuity of the curve is not C3.
  Standard_EXPORT Geom2d_Curve::ResD3 EvalD3(const double U) const final;

  //! For the point of parameter U of this trimmed curve,
  //! computes the vector corresponding to the Nth derivative.
  //! Warning
  //! The returned derivative vector has the same
  //! orientation as the derivative vector of the basis curve,
  //! even if the trimmed curve does not have the same
  //! orientation as the basis curve.
  //! Exceptions
  //! Standard_RangeError if N is less than 1.
  //! geometric transformations
  Standard_EXPORT gp_Vec2d EvalDN(const double U, const int N) const final;

  //! Applies the transformation T to this trimmed curve.
  //! Warning The basis curve is also modified.
  Standard_EXPORT void Transform(const gp_Trsf2d& T) final;

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
  Standard_EXPORT double TransformedParameter(const double U, const gp_Trsf2d& T) const final;

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
  Standard_EXPORT double ParametricTransformation(const gp_Trsf2d& T) const final;

  //! Creates a new object, which is a copy of this trimmed curve.
  Standard_EXPORT occ::handle<Geom2d_Geometry> Copy() const final;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const final;

  DEFINE_STANDARD_RTTIEXT(Geom2d_TrimmedCurve, Geom2d_BoundedCurve)

private:
  occ::handle<Geom2d_Curve> basisCurve;
  double                    uTrim1;
  double                    uTrim2;
};

#endif // _Geom2d_TrimmedCurve_HeaderFile
