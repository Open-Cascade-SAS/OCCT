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

#ifndef _GeomProp_HeaderFile
#define _GeomProp_HeaderFile

#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <NCollection_DynamicArray.hxx>
#include <Standard.hxx>

#include <limits>
#include <variant>

//! @brief Namespace containing result structures and free functions for 3D curve
//! and surface differential property computation.
//!
//! Provides lightweight result structures with explicit validity flags instead of
//! exception-based APIs, and geometry-agnostic free functions that compute local
//! differential properties from derivative vectors.
namespace GeomProp
{

// ============================================================================
// Curve result structures
// ============================================================================

//! Result of tangent direction computation.
struct TangentResult
{
  gp_Dir Direction;         //!< Tangent direction (valid only when IsDefined is true)
  bool   IsDefined = false; //!< True if the tangent is well-defined
};

//! Result of curvature computation.
struct CurvatureResult
{
  double Value      = 0.0;   //!< Curvature value (valid only when IsDefined is true)
  bool   IsDefined  = false; //!< True if curvature could be computed
  bool   IsInfinite = false; //!< True if first derivative is null (singular point)
};

//! Result of normal direction computation.
struct NormalResult
{
  gp_Dir Direction;         //!< Normal direction (valid only when IsDefined is true)
  bool   IsDefined = false; //!< True if the normal is well-defined
};

//! Result of centre of curvature computation.
struct CentreResult
{
  gp_Pnt Centre;            //!< Centre of curvature (valid only when IsDefined is true)
  bool   IsDefined = false; //!< True if the centre is well-defined
};

//! Type of a special curve point (curvature extremum or inflection).
enum class CIType
{
  Inflection,   //!< Inflection point (curvature changes sign)
  MinCurvature, //!< Local minimum of the radius of curvature (maximum of |curvature|)
  MaxCurvature  //!< Local maximum of the radius of curvature (minimum of |curvature|)
};

//! A special point on a curve with its parameter and type.
struct CurveSpecialPoint
{
  double Parameter = 0.0;                //!< Curve parameter
  CIType Type      = CIType::Inflection; //!< Point type
};

//! Result of global curve analysis (curvature extrema and inflection points).
struct CurveAnalysis
{
  NCollection_DynamicArray<CurveSpecialPoint> Points; //!< Special points sorted by parameter
  bool                                        IsDone = false; //!< True if analysis completed
};

// ============================================================================
// Surface result structures
// ============================================================================

//! Result of surface normal computation.
struct SurfaceNormalResult
{
  gp_Dir Direction;         //!< Surface normal direction (valid only when IsDefined is true)
  bool   IsDefined = false; //!< True if the normal is well-defined
};

//! Result of surface principal curvature computation.
struct SurfaceCurvatureResult
{
  double MinCurvature = 0.0; //!< Minimum principal curvature (valid only when IsDefined is true)
  double MaxCurvature = 0.0; //!< Maximum principal curvature (valid only when IsDefined is true)
  gp_Dir MinDirection;       //!< Direction of minimum curvature (valid only when IsDefined is true)
  gp_Dir MaxDirection;       //!< Direction of maximum curvature (valid only when IsDefined is true)
  bool   IsDefined = false;  //!< True if curvatures could be computed
  bool   IsUmbilic = false;  //!< True if the point is umbilic (all curvatures equal)
};

//! Result of mean and Gaussian curvature computation.
struct MeanGaussianResult
{
  double MeanCurvature     = 0.0;   //!< Mean curvature H = (k1 + k2) / 2
  double GaussianCurvature = 0.0;   //!< Gaussian curvature K = k1 * k2
  bool   IsDefined         = false; //!< True if curvatures could be computed
};

// ============================================================================
// Derivative caching
// ============================================================================

//! Derivative order for 3D curve property caching.
//! Values must match std::variant alternative indices in CurveCache::Data.
enum class CurveDerivOrder
{
  Undefined      = 0, //!< No caching specified (monostate)
  Value          = 1, //!< Cache point only (gp_Pnt)
  Tangent        = 2, //!< Cache up to D1 (Geom_Curve::ResD1)
  Curvature      = 3, //!< Cache up to D2 (Geom_Curve::ResD2)
  CurvatureDeriv = 4  //!< Cache up to D3 (Geom_Curve::ResD3)
};

//! Derivative order for 3D surface property caching.
//! Values must match std::variant alternative indices in SurfaceCache::Data.
enum class SurfaceDerivOrder
{
  Undefined = 0, //!< No caching specified (monostate)
  Value     = 1, //!< Cache point only (gp_Pnt)
  Normal    = 2, //!< Cache up to D1U + D1V (Geom_Surface::ResD1)
  Curvature = 3  //!< Cache up to D2 (Geom_Surface::ResD2)
};

//! Cached 3D curve derivatives at a parameter value.
//! @note The variant alternative indices must match CurveDerivOrder enum values.
struct CurveCache
{
  double Param = std::numeric_limits<double>::quiet_NaN();
  std::variant<std::monostate, gp_Pnt, Geom_Curve::ResD1, Geom_Curve::ResD2, Geom_Curve::ResD3>
    Data;

  bool IsValid(double theParam) const { return Param == theParam; }

  CurveDerivOrder Order() const { return static_cast<CurveDerivOrder>(Data.index()); }

  bool HasOrder(CurveDerivOrder theNeeded) const
  {
    return static_cast<int>(Order()) >= static_cast<int>(theNeeded);
  }

  void Invalidate()
  {
    Param = std::numeric_limits<double>::quiet_NaN();
    Data.emplace<std::monostate>();
  }
};

//! Cached 3D surface derivatives at a (U, V) parameter pair.
//! @note The variant alternative indices must match SurfaceDerivOrder enum values.
struct SurfaceCache
{
  double ParamU = std::numeric_limits<double>::quiet_NaN();
  double ParamV = std::numeric_limits<double>::quiet_NaN();
  std::variant<std::monostate, gp_Pnt, Geom_Surface::ResD1, Geom_Surface::ResD2> Data;

  bool IsValid(double theU, double theV) const { return ParamU == theU && ParamV == theV; }

  SurfaceDerivOrder Order() const { return static_cast<SurfaceDerivOrder>(Data.index()); }

  bool HasOrder(SurfaceDerivOrder theNeeded) const
  {
    return static_cast<int>(Order()) >= static_cast<int>(theNeeded);
  }

  void Invalidate()
  {
    ParamU = ParamV = std::numeric_limits<double>::quiet_NaN();
    Data.emplace<std::monostate>();
  }
};

// ============================================================================
// Curve free functions
// ============================================================================

//! Compute tangent direction from derivative vectors.
//! Tries D1 first; if D1 magnitude^2 <= theTol^2, tries D2, then D3.
//! @note When D1 is null and a higher-order derivative is used, the sign of
//!   the direction may be wrong. Use the overload with finite-difference points
//!   for sign correction when the curve parameterization is available.
//! @param[in] theD1 first derivative vector
//! @param[in] theD2 second derivative vector
//! @param[in] theD3 third derivative vector
//! @param[in] theTol linear tolerance for zero-vector detection
//! @return tangent result with validity flag
Standard_EXPORT TangentResult ComputeTangent(const gp_Vec& theD1,
                                             const gp_Vec& theD2,
                                             const gp_Vec& theD3,
                                             double        theTol);

//! Compute tangent direction with sign correction for higher-order derivatives.
//! Same as ComputeTangent, but when D1 is null and a higher-order derivative is
//! used, corrects the sign by comparing with a finite-difference direction vector.
//! @param[in] theD1 first derivative vector
//! @param[in] theD2 second derivative vector
//! @param[in] theD3 third derivative vector
//! @param[in] theTol linear tolerance for zero-vector detection
//! @param[in] thePntBefore point on the curve before the evaluation point
//! @param[in] thePntAfter point on the curve after the evaluation point
//! @return tangent result with validity flag and corrected sign
Standard_EXPORT TangentResult ComputeTangent(const gp_Vec& theD1,
                                             const gp_Vec& theD2,
                                             const gp_Vec& theD3,
                                             double        theTol,
                                             const gp_Pnt& thePntBefore,
                                             const gp_Pnt& thePntAfter);

//! Compute curvature from first and second derivative vectors.
//! Curvature = |D1 x D2| / |D1|^3
//! @param[in] theD1 first derivative vector
//! @param[in] theD2 second derivative vector
//! @param[in] theTol linear tolerance for zero-vector detection
//! @return curvature result with validity and infinity flags
Standard_EXPORT CurvatureResult ComputeCurvature(const gp_Vec& theD1,
                                                 const gp_Vec& theD2,
                                                 double        theTol);

//! Compute normal direction from first and second derivative vectors.
//! Normal = D1 x (D2 x D1) (normalized), perpendicular to tangent pointing toward center.
//! @param[in] theD1 first derivative vector
//! @param[in] theD2 second derivative vector
//! @param[in] theTol linear tolerance for zero-vector detection
//! @return normal result with validity flag
Standard_EXPORT NormalResult ComputeNormal(const gp_Vec& theD1, const gp_Vec& theD2, double theTol);

//! Compute centre of curvature from point and derivative vectors.
//! Centre = Point + Normal / Curvature
//! @param[in] thePnt point on the curve
//! @param[in] theD1 first derivative vector
//! @param[in] theD2 second derivative vector
//! @param[in] theTol linear tolerance for zero-vector detection
//! @return centre result with validity flag
Standard_EXPORT CentreResult ComputeCentreOfCurvature(const gp_Pnt& thePnt,
                                                      const gp_Vec& theD1,
                                                      const gp_Vec& theD2,
                                                      double        theTol);

// ============================================================================
// Surface free functions
// ============================================================================

//! Compute surface normal from first partial derivatives.
//! Normal = D1U x D1V (normalized).
//! @param[in] theD1U first partial derivative in U direction
//! @param[in] theD1V first partial derivative in V direction
//! @param[in] theTol linear tolerance for zero-vector detection
//! @return surface normal result with validity flag
Standard_EXPORT SurfaceNormalResult ComputeSurfaceNormal(const gp_Vec& theD1U,
                                                         const gp_Vec& theD1V,
                                                         double        theTol);

//! Compute principal curvatures and directions from surface derivatives.
//! Uses first and second fundamental forms to compute principal curvatures.
//! @param[in] theD1U first partial derivative in U direction
//! @param[in] theD1V first partial derivative in V direction
//! @param[in] theD2U second partial derivative in U direction
//! @param[in] theD2V second partial derivative in V direction
//! @param[in] theDUV mixed partial derivative
//! @param[in] theTol linear tolerance for zero-vector detection
//! @return surface curvature result with principal curvatures and directions
Standard_EXPORT SurfaceCurvatureResult ComputeSurfaceCurvatures(const gp_Vec& theD1U,
                                                                const gp_Vec& theD1V,
                                                                const gp_Vec& theD2U,
                                                                const gp_Vec& theD2V,
                                                                const gp_Vec& theDUV,
                                                                double        theTol);

//! Compute mean and Gaussian curvatures from surface derivatives.
//! Mean curvature H = (EN - 2FM + GL) / (2(EG - F^2))
//! Gaussian curvature K = (LN - M^2) / (EG - F^2)
//! @param[in] theD1U first partial derivative in U direction
//! @param[in] theD1V first partial derivative in V direction
//! @param[in] theD2U second partial derivative in U direction
//! @param[in] theD2V second partial derivative in V direction
//! @param[in] theDUV mixed partial derivative
//! @param[in] theTol linear tolerance for zero-vector detection
//! @return mean and Gaussian curvature result
Standard_EXPORT MeanGaussianResult ComputeMeanGaussian(const gp_Vec& theD1U,
                                                       const gp_Vec& theD1V,
                                                       const gp_Vec& theD2U,
                                                       const gp_Vec& theD2V,
                                                       const gp_Vec& theDUV,
                                                       double        theTol);

} // namespace GeomProp

#endif // _GeomProp_HeaderFile
