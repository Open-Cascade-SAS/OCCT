// Copyright (c) 2025 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.

#ifndef _MathPoly_Types_HeaderFile
#define _MathPoly_Types_HeaderFile

#include <MathUtils_Types.hxx>

#include <array>
#include <complex>

namespace MathPoly
{

//! Maximum degree accepted by the polynomial solvers.
inline constexpr int THE_MAX_POLY_DEGREE = 20;

//! Polynomial solver result. Repeated roots are represented once per real root or once per
//! member of a complex-conjugate pair, with algebraic multiplicity stored in the corresponding
//! multiplicity array.
struct PolyResult
{
  MathUtils::Status                                     Status = MathUtils::Status::NotConverged;
  std::array<double, THE_MAX_POLY_DEGREE>               Roots  = {};
  std::array<size_t, THE_MAX_POLY_DEGREE>               Multiplicities        = {};
  std::array<std::complex<double>, THE_MAX_POLY_DEGREE> ComplexRoots          = {};
  std::array<size_t, THE_MAX_POLY_DEGREE>               ComplexMultiplicities = {};
  size_t                                                NbRoots               = 0;
  size_t                                                NbComplexRoots        = 0;

  //! Returns true when all roots were computed and verified.
  bool IsDone() const { return Status == MathUtils::Status::OK; }

  explicit operator bool() const { return IsDone(); }

  double operator[](size_t theIndex) const { return Roots[theIndex]; }

  //! Returns the number of real roots including multiplicity.
  size_t NbRealRootsWithMultiplicity() const
  {
    size_t aCount = 0;
    for (size_t anIndex = 0; anIndex < NbRoots; ++anIndex)
    {
      aCount += Multiplicities[anIndex];
    }
    return aCount;
  }
};

using GeneralPolyResult = PolyResult;

} // namespace MathPoly

#endif // _MathPoly_Types_HeaderFile
