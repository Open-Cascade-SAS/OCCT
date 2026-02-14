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

#ifndef _MathSys_NewtonTypes_HeaderFile
#define _MathSys_NewtonTypes_HeaderFile

#include <MathUtils_Config.hxx>
#include <MathUtils_Types.hxx>

#include <array>
#include <cstddef>

//! Shared types for specialized small-dimension Newton solvers.
namespace MathSys
{
using namespace MathUtils;

//! Result of N-dimensional Newton solver.
template <int N>
struct NewtonResultN
{
  MathUtils::Status     Status       = MathUtils::Status::NotConverged; //!< Final solver status
  std::array<double, N> X            = {};                              //!< Solution estimate
  size_t                NbIterations = 0;                               //!< Performed iterations
  double                ResidualNorm = 0.0; //!< Final residual norm ||F||
  double                StepNorm     = 0.0; //!< Final step norm ||dX||

  //! Returns true if computation converged.
  bool IsDone() const { return Status == MathUtils::Status::OK; }

  //! Conversion to bool for convenient checking.
  explicit operator bool() const { return IsDone(); }
};

//! Box bounds for N-dimensional solver.
template <int N>
struct NewtonBoundsN
{
  std::array<double, N> Min       = {};   //!< Lower bounds
  std::array<double, N> Max       = {};   //!< Upper bounds
  bool                  HasBounds = true; //!< True to apply bounds
};

//! Solver options for small-dimension Newton methods.
struct NewtonOptions : MathUtils::Config
{
  double MaxStepRatio        = 0.5;    //!< Max step as ratio of largest domain size
  bool   EnableLineSearch    = true;   //!< Enable Armijo backtracking line search
  bool   AllowSoftBounds     = false;  //!< Allow slight bounds extension
  double SoftBoundsExtension = 1.0e-4; //!< Extension ratio for soft bounds

  //! Default constructor with strict residual/step tolerances for specialized Newton.
  NewtonOptions()
  {
    Tolerance     = 1.0e-10;
    XTolerance    = 1.0e-16;
    FTolerance    = 1.0e-10;
    MaxIterations = 100;
  }
};

} // namespace MathSys

#endif // _MathSys_NewtonTypes_HeaderFile
