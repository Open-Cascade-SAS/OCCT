// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _Geom2d_EvalRepCurveDesc_HeaderFile
#define _Geom2d_EvalRepCurveDesc_HeaderFile

#include <Geom2d_Curve.hxx>
#include <Precision.hxx>
#include <Standard_Transient.hxx>

#include <cmath>
#include <cstddef>
#include <optional>

namespace Geom2d_EvalRepCurveDesc
{
//! 1D affine parameter map: uRep = Scale * u + Offset.
struct Map1d
{
  double Scale  = 1.0;
  double Offset = 0.0;

  bool IsIdentity() const
  {
    return std::abs(Scale - 1.0) < Precision::Confusion() && std::abs(Offset) < Precision::Confusion();
  }

  bool IsValid() const { return std::abs(Scale) > Precision::Confusion(); }

  double Map(const double theU) const { return Scale * theU + Offset; }
};

//! 1D parameter domain interval.
struct Domain1d
{
  double First = 0.0;
  double Last  = 1.0;

  bool Contains(const double theU) const
  {
    return (theU >= First - Precision::Confusion()) && (theU <= Last + Precision::Confusion());
  }
};
//! Abstract base descriptor for 2D curve evaluation representation.
//! Holds the representation handle and a Kind tag for switch-based dispatch.
class Base : public Standard_Transient
{
public:
  //! Descriptor kind for switch-based dispatch (no RTTI needed).
  enum class Kind
  {
    Full,          //!< fully equivalent, no derivative limit, no domain, no map
    DerivBounded,  //!< full domain + identity map, limited to MaxDerivOrder
    Mapped         //!< has MaxDerivOrder + optional Domain + ParamMap
  };

  occ::handle<Geom2d_Curve> Representation; //!< geometry used for evaluation

  //! Returns the descriptor kind.
  virtual Kind GetKind() const = 0;

  DEFINE_STANDARD_RTTI_INLINE(Base, Standard_Transient)

protected:
  Base() = default;
};

//! Fully equivalent descriptor: no derivative limit, no domain, no map.
//! Fastest evaluation path - direct delegation to Representation.
class Full : public Base
{
public:
  Kind GetKind() const override { return Kind::Full; }

  DEFINE_STANDARD_RTTI_INLINE(Full, Base)
};

//! Derivative-bounded descriptor: full domain, identity map, limited to MaxDerivOrder.
class DerivBounded : public Base
{
public:
  std::size_t MaxDerivOrder = 3; //!< max supported derivative order

  Kind GetKind() const override { return Kind::DerivBounded; }

  DEFINE_STANDARD_RTTI_INLINE(DerivBounded, Base)
};

//! Mapped descriptor for 2D curve evaluation representation.
//! Adds optional bounded domain and affine parameter map.
class Mapped : public Base
{
public:
  std::size_t             MaxDerivOrder = 3; //!< max supported derivative order
  std::optional<Domain1d> Domain;            //!< nullopt = full domain
  Map1d                   ParamMap;          //!< affine parameter transform

  Kind GetKind() const override { return Kind::Mapped; }

  DEFINE_STANDARD_RTTI_INLINE(Mapped, Base)
};
} // namespace Geom2d_EvalRepCurveDesc

#endif // _Geom2d_EvalRepCurveDesc_HeaderFile
