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

#ifndef _GeomEval_RepSurfaceDesc_HeaderFile
#define _GeomEval_RepSurfaceDesc_HeaderFile

#include <Geom_Surface.hxx>
#include <Precision.hxx>
#include <Standard_Transient.hxx>

#include <cmath>
#include <cstddef>
#include <optional>

namespace GeomEval_RepSurfaceDesc
{
//! 2D diagonal affine parameter map with optional UV swap.
//! Without swap: uRep = ScaleU*u + OffsetU, vRep = ScaleV*v + OffsetV.
//! With swap:    uRep = ScaleU*v + OffsetU, vRep = ScaleV*u + OffsetV.
struct Map2d
{
  double ScaleU  = 1.0;
  double OffsetU = 0.0;
  double ScaleV  = 1.0;
  double OffsetV = 0.0;
  bool   SwapUV  = false;

  bool IsIdentity() const
  {
    return std::abs(ScaleU - 1.0) < Precision::Confusion()
           && std::abs(ScaleV - 1.0) < Precision::Confusion()
           && std::abs(OffsetU) < Precision::Confusion()
           && std::abs(OffsetV) < Precision::Confusion() && !SwapUV;
  }

  bool IsValid() const
  {
    return std::abs(ScaleU) > Precision::Confusion() && std::abs(ScaleV) > Precision::Confusion();
  }

  void Map(const double theU, const double theV, double& theURep, double& theVRep) const
  {
    if (SwapUV)
    {
      theURep = ScaleU * theV + OffsetU;
      theVRep = ScaleV * theU + OffsetV;
    }
    else
    {
      theURep = ScaleU * theU + OffsetU;
      theVRep = ScaleV * theV + OffsetV;
    }
  }
};

//! 2D parameter domain.
struct Domain2d
{
  double UFirst = 0.0;
  double ULast  = 1.0;
  double VFirst = 0.0;
  double VLast  = 1.0;

  bool Contains(const double theU, const double theV) const
  {
    return (theU >= UFirst - Precision::Confusion()) && (theU <= ULast + Precision::Confusion())
           && (theV >= VFirst - Precision::Confusion()) && (theV <= VLast + Precision::Confusion());
  }
};

//! Abstract base descriptor for surface evaluation representation.
//! Holds the representation handle and a Kind tag for switch-based dispatch.
class Base : public Standard_Transient
{
public:
  //! Descriptor kind for switch-based dispatch (no RTTI needed).
  enum class Kind
  {
    Full,         //!< fully equivalent, no derivative limit, no domain, no map
    DerivBounded, //!< full domain + identity map, limited to MaxDerivOrder
    Mapped        //!< has MaxDerivOrder + optional Domain + ParamMap
  };

  occ::handle<Geom_Surface> Representation; //!< geometry used for evaluation

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

//! Mapped descriptor for surface evaluation representation.
//! Adds optional bounded domain and diagonal affine parameter map with optional UV swap.
//! Evaluation requires: domain check -> map parameters -> evaluate -> scale derivatives.
//! Future subclasses can support multi-region descriptors with per-patch UV domains and maps.
class Mapped : public Base
{
public:
  std::size_t             MaxDerivOrder = 3; //!< max supported derivative order
  std::optional<Domain2d> Domain;            //!< nullopt = full domain
  Map2d                   ParamMap;          //!< affine parameter transform

  Kind GetKind() const override { return Kind::Mapped; }

  DEFINE_STANDARD_RTTI_INLINE(Mapped, Base)
};
} // namespace GeomEval_RepSurfaceDesc

#endif // _GeomEval_RepSurfaceDesc_HeaderFile
