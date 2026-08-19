// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.

#ifndef _MathSys_Types_HeaderFile
#define _MathSys_Types_HeaderFile

#include <MathUtils_Types.hxx>

#include <cstddef>
#include <optional>

namespace MathSys
{

using MathUtils::Status;

//! Result shared by generic nonlinear system and least-squares solvers.
struct SystemResult
{
  MathUtils::Status          Status       = MathUtils::Status::NotConverged;
  uint32_t                   NbIterations = 0;
  std::optional<math_Vector> Solution;
  std::optional<double>      Value; //!< Final squared residual norm.
  std::optional<math_Vector> Gradient;
  std::optional<math_Matrix> Jacobian;
  double                     ResidualNorm = 0.0;
  double                     StepNorm     = 0.0;
  size_t                     Rank         = 0;
  bool                       IsRoot       = false;
  bool                       IsStationary = false;

  //! Returns true when the requested computation completed successfully.
  bool IsDone() const { return Status == MathUtils::Status::OK; }

  //! Conversion to bool for convenient checking.
  explicit operator bool() const { return IsDone(); }
};

} // namespace MathSys

#endif // _MathSys_Types_HeaderFile
