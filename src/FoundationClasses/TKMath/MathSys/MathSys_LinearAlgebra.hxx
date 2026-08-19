// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.

#ifndef _MathSys_LinearAlgebra_HeaderFile
#define _MathSys_LinearAlgebra_HeaderFile

#include <MathLin_Gauss.hxx>
#include <MathLin_SVD.hxx>

#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>

namespace MathSys
{
namespace Utils
{

constexpr double THE_RANK_TOL = 64.0 * std::numeric_limits<double>::epsilon();

//! Accumulate a Euclidean norm without avoidable overflow or underflow.
inline double SafeNorm(const math_Vector& theVector)
{
  double aNorm = 0.0;
  for (size_t anIndex = 0; anIndex < theVector.Size(); ++anIndex)
  {
    aNorm = std::hypot(aNorm, theVector.At(anIndex));
  }
  return aNorm;
}

//! Check all vector entries for finite values.
inline bool IsFinite(const math_Vector& theVector)
{
  for (size_t anIndex = 0; anIndex < theVector.Size(); ++anIndex)
  {
    if (!std::isfinite(theVector.At(anIndex)))
    {
      return false;
    }
  }
  return true;
}

//! Check all matrix entries for finite values.
inline bool IsFinite(const math_Matrix& theMatrix)
{
  for (size_t aRow = 0; aRow < theMatrix.RowSize(); ++aRow)
  {
    for (size_t aCol = 0; aCol < theMatrix.ColSize(); ++aCol)
    {
      if (!std::isfinite(theMatrix.At(aRow, aCol)))
      {
        return false;
      }
    }
  }
  return true;
}

struct LinearStep
{
  MathUtils::Status          Status = MathUtils::Status::NotConverged;
  std::optional<math_Vector> Solution;
  size_t                     Rank = 0;
};

//! Compute rank after column equilibration so variable units do not change rank classification.
inline std::optional<size_t> ScaledRank(const math_Matrix& theMatrix,
                                        double             theTolerance = THE_RANK_TOL)
{
  math_Matrix aScaled(theMatrix.RowSize(), theMatrix.ColSize());
  for (size_t aRow = 0; aRow < aScaled.RowSize(); ++aRow)
  {
    for (size_t aCol = 0; aCol < aScaled.ColSize(); ++aCol)
    {
      aScaled.ChangeAt(aRow, aCol) = theMatrix.At(aRow, aCol);
    }
  }
  for (size_t aCol = 0; aCol < aScaled.ColSize(); ++aCol)
  {
    double aNorm = 0.0;
    for (size_t aRow = 0; aRow < aScaled.RowSize(); ++aRow)
    {
      aNorm = std::hypot(aNorm, aScaled.At(aRow, aCol));
    }
    if (aNorm > 0.0)
    {
      for (size_t aRow = 0; aRow < aScaled.RowSize(); ++aRow)
      {
        aScaled.ChangeAt(aRow, aCol) /= aNorm;
      }
    }
  }
  const MathLin::SVDResult aResult = MathLin::SVD(aScaled, theTolerance);
  if (!aResult.IsDone())
  {
    return std::nullopt;
  }
  return aResult.Rank;
}

//! Solve a scaled full-column-rank least-squares system by Householder QR.
inline bool SolveQR(const math_Matrix& theA,
                    const math_Vector& theB,
                    math_Vector&       theX,
                    double             theRankTolerance = THE_RANK_TOL)
{
  const size_t aM = theA.RowSize();
  const size_t aN = theA.ColSize();
  if (aM < aN)
  {
    return false;
  }

  math_Matrix aWork(aM, aN, 0.0);
  math_Vector aRhs(aM, 0.0);
  for (size_t aRow = 0; aRow < aM; ++aRow)
  {
    aRhs.ChangeAt(aRow) = theB.At(aRow);
    for (size_t aCol = 0; aCol < aN; ++aCol)
    {
      aWork.ChangeAt(aRow, aCol) = theA.At(aRow, aCol);
    }
  }

  for (size_t aCol = 0; aCol < aN; ++aCol)
  {
    double aColumnNorm = 0.0;
    for (size_t aRow = aCol; aRow < aM; ++aRow)
    {
      aColumnNorm = std::hypot(aColumnNorm, aWork.At(aRow, aCol));
    }
    if (!(aColumnNorm > 0.0) || !std::isfinite(aColumnNorm))
    {
      return false;
    }

    const double aAlpha = std::copysign(aColumnNorm, aWork.At(aCol, aCol));
    aWork.ChangeAt(aCol, aCol) += aAlpha;
    double aVNormSq = 0.0;
    for (size_t aRow = aCol; aRow < aM; ++aRow)
    {
      aVNormSq += aWork.At(aRow, aCol) * aWork.At(aRow, aCol);
    }
    if (!(aVNormSq > 0.0) || !std::isfinite(aVNormSq))
    {
      return false;
    }

    for (size_t aNextCol = aCol + 1; aNextCol < aN; ++aNextCol)
    {
      double aDot = 0.0;
      for (size_t aRow = aCol; aRow < aM; ++aRow)
      {
        aDot += aWork.At(aRow, aCol) * aWork.At(aRow, aNextCol);
      }
      const double aFactor = 2.0 * aDot / aVNormSq;
      for (size_t aRow = aCol; aRow < aM; ++aRow)
      {
        aWork.ChangeAt(aRow, aNextCol) -= aFactor * aWork.At(aRow, aCol);
      }
    }

    double aDotRhs = 0.0;
    for (size_t aRow = aCol; aRow < aM; ++aRow)
    {
      aDotRhs += aWork.At(aRow, aCol) * aRhs.At(aRow);
    }
    const double aRhsFactor = 2.0 * aDotRhs / aVNormSq;
    for (size_t aRow = aCol; aRow < aM; ++aRow)
    {
      aRhs.ChangeAt(aRow) -= aRhsFactor * aWork.At(aRow, aCol);
    }
    aWork.ChangeAt(aCol, aCol) = -aAlpha;
  }

  for (size_t aCol = aN; aCol-- > 0;)
  {
    double aValue = aRhs.At(aCol);
    for (size_t aNextCol = aCol + 1; aNextCol < aN; ++aNextCol)
    {
      aValue -= aWork.At(aCol, aNextCol) * theX.At(aNextCol);
    }
    const double aDiagonal = aWork.At(aCol, aCol);
    if (!(std::abs(aDiagonal) > theRankTolerance) || !std::isfinite(aDiagonal))
    {
      return false;
    }
    theX.ChangeAt(aCol) = aValue / aDiagonal;
  }
  return IsFinite(theX);
}

//! Rank-aware linear policy: scaled LU for square full rank, QR for tall full rank, SVD otherwise.
inline LinearStep SolveLinearized(const math_Matrix& theA,
                                  const math_Vector& theB,
                                  double             theRankTolerance = THE_RANK_TOL)
{
  LinearStep aResult;
  if (!IsFinite(theA) || !IsFinite(theB) || theA.RowSize() != theB.Size())
  {
    aResult.Status = MathUtils::Status::NumericalError;
    return aResult;
  }

  const size_t aM = theA.RowSize();
  const size_t aN = theA.ColSize();
  math_Matrix  aScaled(aM, aN, 0.0);
  math_Vector  aScale(aN, 1.0);
  for (size_t aCol = 0; aCol < aN; ++aCol)
  {
    double aNorm = 0.0;
    for (size_t aRow = 0; aRow < aM; ++aRow)
    {
      aNorm = std::hypot(aNorm, theA.At(aRow, aCol));
    }
    aScale.ChangeAt(aCol) = (aNorm > 0.0) ? aNorm : 1.0;
    for (size_t aRow = 0; aRow < aM; ++aRow)
    {
      aScaled.ChangeAt(aRow, aCol) = theA.At(aRow, aCol) / aScale.At(aCol);
    }
  }

  const MathLin::SVDResult anSVD = MathLin::SVD(aScaled, theRankTolerance);
  if (!anSVD.IsDone())
  {
    aResult.Status = anSVD.Status;
    return aResult;
  }
  aResult.Rank = anSVD.Rank;

  std::optional<math_Vector> aScaledSolution;
  if (aResult.Rank == aN && aM == aN)
  {
    const MathUtils::LinearResult anLU = MathLin::Solve(aScaled, theB);
    if (anLU.IsDone())
    {
      aScaledSolution = anLU.Solution;
    }
  }
  else if (aResult.Rank == aN && aM > aN)
  {
    math_Vector aQR(aN, 0.0);
    if (SolveQR(aScaled, theB, aQR, theRankTolerance))
    {
      aScaledSolution = aQR;
    }
  }

  if (!aScaledSolution.has_value())
  {
    const MathUtils::LinearResult anSVDSolution =
      MathLin::SolveSVD(theA, theB, theRankTolerance);
    if (!anSVDSolution.IsDone())
    {
      aResult.Status = anSVDSolution.Status;
      return aResult;
    }
    aResult.Solution = anSVDSolution.Solution;
    aResult.Status = IsFinite(*aResult.Solution) ? MathUtils::Status::OK
                                                 : MathUtils::Status::NumericalError;
    return aResult;
  }

  aResult.Solution = math_Vector(aN, 0.0);
  for (size_t aCol = 0; aCol < aN; ++aCol)
  {
    aResult.Solution->ChangeAt(aCol) = aScaledSolution->At(aCol) / aScale.At(aCol);
  }
  aResult.Status =
    IsFinite(*aResult.Solution) ? MathUtils::Status::OK : MathUtils::Status::NumericalError;
  return aResult;
}

} // namespace Utils
} // namespace MathSys

#endif // _MathSys_LinearAlgebra_HeaderFile
