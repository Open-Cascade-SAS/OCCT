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

#ifndef _MathUtils_Types_HeaderFile
#define _MathUtils_Types_HeaderFile

#include <math_Vector.hxx>
#include <math_Matrix.hxx>

#include <array>
#include <optional>

//! Modern math solver types and result structures.
namespace MathUtils
{

//! Computation status for all math solvers.
//! Provides detailed information about solver outcome.
enum class Status
{
  OK,                  //!< Computation successful, solution found
  NotConverged,        //!< Did not converge within tolerance
  MaxIterations,       //!< Maximum iterations reached without convergence
  NumericalError,      //!< Numerical issue (overflow, NaN, division by zero, etc.)
  InvalidInput,        //!< Invalid input parameters
  InfiniteSolutions,   //!< Infinite number of solutions (degenerate case)
  NoSolution,          //!< No solution exists
  NotPositiveDefinite, //!< Matrix not positive definite (for Cholesky, Newton, etc.)
  Singular             //!< Matrix is singular or nearly singular
};

//! Result for scalar (1D) root finding and minimization.
//! Contains the found root/minimum location and diagnostic information.
struct ScalarResult
{
  MathUtils::Status     Status       = MathUtils::Status::NotConverged; //!< Computation status
  size_t                NbIterations = 0; //!< Number of iterations performed
  std::optional<double> Root;             //!< Found root or minimum location
  std::optional<double> Value;            //!< Function value at root/minimum
  std::optional<double> Derivative;       //!< Derivative at root (if computed)

  //! Returns true if computation succeeded.
  bool IsDone() const { return Status == MathUtils::Status::OK; }

  //! Conversion to bool for convenient checking.
  //! Example: if (aResult) { use *aResult.Root; }
  explicit operator bool() const { return IsDone(); }
};

//! Result for polynomial root finding.
//! Supports up to 4 real roots (for quartic equations).
struct PolyResult
{
  MathUtils::Status     Status  = MathUtils::Status::NotConverged; //!< Computation status
  size_t                NbRoots = 0;                               //!< Number of real roots found
  std::array<double, 4> Roots   = {0.0, 0.0, 0.0, 0.0};            //!< Array of real roots (sorted)

  //! Returns true if computation succeeded.
  bool IsDone() const { return Status == MathUtils::Status::OK; }

  //! Conversion to bool for convenient checking.
  explicit operator bool() const { return IsDone(); }

  //! Access root by index (0-based).
  //! @param theIndex root index (0 to NbRoots-1)
  //! @return root value
  double operator[](int theIndex) const { return Roots[theIndex]; }
};

//! Result for N-dimensional optimization and system solving.
//! Contains the solution vector and optional gradient/Jacobian information.
struct VectorResult
{
  MathUtils::Status          Status       = MathUtils::Status::NotConverged; //!< Computation status
  size_t                     NbIterations = 0; //!< Number of iterations performed
  std::optional<math_Vector> Solution;         //!< Solution vector (set by solver on success)
  std::optional<double>      Value;            //!< Function value at solution (if computed)
  std::optional<math_Vector> Gradient;         //!< Gradient at solution (if computed)
  std::optional<math_Matrix> Jacobian;         //!< Jacobian at solution (if computed)

  //! Returns true if computation succeeded.
  bool IsDone() const { return Status == MathUtils::Status::OK; }

  //! Conversion to bool for convenient checking.
  explicit operator bool() const { return IsDone(); }
};

//! Result for linear system solving (Ax = b).
//! Contains the solution vector and matrix determinant if computed.
struct LinearResult
{
  MathUtils::Status          Status = MathUtils::Status::NotConverged; //!< Computation status
  std::optional<math_Vector> Solution;    //!< Solution vector X in AX = B (set by solver)
  std::optional<double>      Determinant; //!< Determinant of matrix (if computed)

  //! Returns true if computation succeeded.
  bool IsDone() const { return Status == MathUtils::Status::OK; }

  //! Conversion to bool for convenient checking.
  explicit operator bool() const { return IsDone(); }
};

//! Result for eigenvalue/eigenvector computation.
//! Contains eigenvalues and optionally eigenvectors.
struct EigenResult
{
  MathUtils::Status          Status       = MathUtils::Status::NotConverged; //!< Computation status
  size_t                     NbIterations = 0; //!< Number of iterations performed
  std::optional<math_Vector> EigenValues;      //!< Computed eigenvalues (set by solver)
  std::optional<math_Matrix> EigenVectors;     //!< Computed eigenvectors (set by solver)

  //! Returns true if computation succeeded.
  bool IsDone() const { return Status == MathUtils::Status::OK; }

  //! Conversion to bool for convenient checking.
  explicit operator bool() const { return IsDone(); }
};

//! Result for matrix decomposition (LU, SVD, QR).
//! Structure depends on decomposition type.
struct DecompResult
{
  MathUtils::Status          Status = MathUtils::Status::NotConverged; //!< Computation status
  std::optional<math_Matrix> L;           //!< Lower triangular (LU) or left singular vectors (SVD)
  std::optional<math_Matrix> U;           //!< Upper triangular (LU) or right singular vectors (SVD)
  std::optional<math_Vector> D;           //!< Diagonal elements or singular values
  std::optional<double>      Determinant; //!< Matrix determinant (if computed)

  //! Returns true if decomposition succeeded.
  bool IsDone() const { return Status == MathUtils::Status::OK; }

  //! Conversion to bool for convenient checking.
  explicit operator bool() const { return IsDone(); }
};

//! Result for numerical integration.
//! Contains integral value and error estimates.
struct IntegResult
{
  MathUtils::Status     Status       = MathUtils::Status::NotConverged; //!< Computation status
  size_t                NbIterations = 0; //!< Number of adaptive iterations
  size_t                NbPoints     = 0; //!< Total number of quadrature points used
  std::optional<double> Value;            //!< Computed integral value
  std::optional<double> AbsoluteError;    //!< Estimated absolute error (if computed)
  std::optional<double> RelativeError;    //!< Estimated relative error (if computed)

  //! Returns true if integration succeeded.
  bool IsDone() const { return Status == MathUtils::Status::OK; }

  //! Conversion to bool for convenient checking.
  explicit operator bool() const { return IsDone(); }
};

//! Result for matrix inverse computation.
//! Contains the inverse matrix if computation succeeded.
struct InverseResult
{
  MathUtils::Status          Status = MathUtils::Status::NotConverged; //!< Computation status
  std::optional<math_Matrix> Inverse;                                  //!< Computed inverse matrix
  std::optional<double>      Determinant; //!< Determinant of matrix (if computed)

  //! Returns true if inversion succeeded.
  bool IsDone() const { return Status == MathUtils::Status::OK; }

  //! Conversion to bool for convenient checking.
  explicit operator bool() const { return IsDone(); }
};

} // namespace MathUtils

#endif // _MathUtils_Types_HeaderFile
