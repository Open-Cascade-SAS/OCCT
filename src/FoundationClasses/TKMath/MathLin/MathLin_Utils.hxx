// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.

#ifndef _MathLin_Utils_HeaderFile
#define _MathLin_Utils_HeaderFile

#include <math_Matrix.hxx>
#include <math_Vector.hxx>

#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>

namespace MathLin
{
namespace Utils
{

//! Returns true when every matrix element is finite.
inline bool IsFinite(const math_Matrix& theMatrix)
{
  for (size_t i = 0; i < theMatrix.RowSize(); ++i)
  {
    for (size_t j = 0; j < theMatrix.ColSize(); ++j)
    {
      if (!std::isfinite(theMatrix.At(i, j)))
      {
        return false;
      }
    }
  }
  return true;
}

//! Returns true when every vector element is finite.
inline bool IsFinite(const math_Vector& theVector)
{
  for (size_t i = 0; i < theVector.Size(); ++i)
  {
    if (!std::isfinite(theVector[i]))
    {
      return false;
    }
  }
  return true;
}

//! Accumulates a Euclidean norm without forming overflow-prone squares.
//! @return false if the value or accumulation state is not finite
inline bool AccumulateNorm(double theValue, double& theScale, double& theSumSquares)
{
  if (!std::isfinite(theValue) || !std::isfinite(theScale) || !std::isfinite(theSumSquares))
  {
    return false;
  }
  const double anAbs = std::abs(theValue);
  if (anAbs == 0.0)
  {
    return true;
  }
  if (theScale < anAbs)
  {
    const double aRatio = theScale / anAbs;
    theSumSquares       = 1.0 + theSumSquares * aRatio * aRatio;
    theScale            = anAbs;
  }
  else
  {
    const double aRatio = anAbs / theScale;
    theSumSquares += aRatio * aRatio;
  }
  return std::isfinite(theScale) && std::isfinite(theSumSquares);
}

//! Returns the accumulated norm, or no value if the state or result is not finite.
inline std::optional<double> Norm(double theScale, double theSumSquares)
{
  if (!std::isfinite(theScale) || !std::isfinite(theSumSquares) || theScale < 0.0
      || theSumSquares < 0.0)
  {
    return std::nullopt;
  }
  const double aNorm = theScale == 0.0 ? 0.0 : theScale * std::sqrt(theSumSquares);
  return std::isfinite(aNorm) ? std::optional<double>(aNorm) : std::nullopt;
}

//! Converts a user relative tolerance to a valid scale-relative threshold.
inline double RelativeTolerance(double theTolerance, size_t theDimension)
{
  if (!std::isfinite(theTolerance) || theTolerance < 0.0)
  {
    return -1.0;
  }
  return std::max(theTolerance,
                  std::numeric_limits<double>::epsilon()
                    * static_cast<double>(std::max<size_t>(1, theDimension)));
}

} // namespace Utils
} // namespace MathLin

#endif // _MathLin_Utils_HeaderFile
