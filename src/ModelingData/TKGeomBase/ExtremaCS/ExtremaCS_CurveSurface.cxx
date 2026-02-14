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

#include <ExtremaCS_CurveSurface.hxx>

// Include adapters for numerical evaluation
#include <ExtremaCS_CurveAdapter.hxx>
#include <ExtremaCS_SurfaceAdapter.hxx>

// Include elementary curve wrappers (for analytical pairs only)
#include <ExtremaCS_Circle.hxx>
#include <ExtremaCS_Ellipse.hxx>
#include <ExtremaCS_Line.hxx>

// Include elementary surface wrappers (for analytical pairs only)
#include <ExtremaCS_Cone.hxx>
#include <ExtremaCS_Cylinder.hxx>
#include <ExtremaCS_Plane.hxx>
#include <ExtremaCS_Sphere.hxx>

// Include analytical pairs
#include <ExtremaCS_CircleCylinder.hxx>
#include <ExtremaCS_CirclePlane.hxx>
#include <ExtremaCS_CircleSphere.hxx>
#include <ExtremaCS_EllipsePlane.hxx>
#include <ExtremaCS_EllipseSphere.hxx>
#include <ExtremaCS_LineCone.hxx>
#include <ExtremaCS_LineCylinder.hxx>
#include <ExtremaCS_LinePlane.hxx>
#include <ExtremaCS_LineSphere.hxx>

// Include grid evaluator for numerical pairs
#include <ExtremaCS_GridEvaluator3D.hxx>
#include <ExtremaCS_Numerical.hxx>

#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_SurfaceType.hxx>

#include <memory>
#include <variant>

namespace
{
//! Curve type category for dispatch.
enum class CurveCategory
{
  Line,
  Circle,
  Ellipse,
  Hyperbola,
  Parabola,
  BezierCurve,
  BSplineCurve,
  OffsetCurve,
  OtherCurve
};

//! Surface type category for dispatch.
enum class SurfaceCategory
{
  Plane,
  Cylinder,
  Cone,
  Sphere,
  Torus,
  BezierSurface,
  BSplineSurface,
  OffsetSurface,
  SurfaceOfRevolution,
  SurfaceOfExtrusion,
  OtherSurface
};

//! Get category from curve type.
CurveCategory getCategory(GeomAbs_CurveType theType)
{
  switch (theType)
  {
    case GeomAbs_Line:
      return CurveCategory::Line;
    case GeomAbs_Circle:
      return CurveCategory::Circle;
    case GeomAbs_Ellipse:
      return CurveCategory::Ellipse;
    case GeomAbs_Hyperbola:
      return CurveCategory::Hyperbola;
    case GeomAbs_Parabola:
      return CurveCategory::Parabola;
    case GeomAbs_BezierCurve:
      return CurveCategory::BezierCurve;
    case GeomAbs_BSplineCurve:
      return CurveCategory::BSplineCurve;
    case GeomAbs_OffsetCurve:
      return CurveCategory::OffsetCurve;
    default:
      return CurveCategory::OtherCurve;
  }
}

//! Get category from surface type.
SurfaceCategory getCategory(GeomAbs_SurfaceType theType)
{
  switch (theType)
  {
    case GeomAbs_Plane:
      return SurfaceCategory::Plane;
    case GeomAbs_Cylinder:
      return SurfaceCategory::Cylinder;
    case GeomAbs_Cone:
      return SurfaceCategory::Cone;
    case GeomAbs_Sphere:
      return SurfaceCategory::Sphere;
    case GeomAbs_Torus:
      return SurfaceCategory::Torus;
    case GeomAbs_BezierSurface:
      return SurfaceCategory::BezierSurface;
    case GeomAbs_BSplineSurface:
      return SurfaceCategory::BSplineSurface;
    case GeomAbs_OffsetSurface:
      return SurfaceCategory::OffsetSurface;
    case GeomAbs_SurfaceOfRevolution:
      return SurfaceCategory::SurfaceOfRevolution;
    case GeomAbs_SurfaceOfExtrusion:
      return SurfaceCategory::SurfaceOfExtrusion;
    default:
      return SurfaceCategory::OtherSurface;
  }
}

// Note: isElementaryCurve/isElementarySurface removed as dispatch
// is done explicitly for known analytical pairs
} // namespace

//! Implementation structure using variant for type-safe dispatch.
struct ExtremaCS_CurveSurface::Impl
{
  using AnalyticalVariant = std::variant<std::monostate,
                                         std::unique_ptr<ExtremaCS_LinePlane>,
                                         std::unique_ptr<ExtremaCS_LineSphere>,
                                         std::unique_ptr<ExtremaCS_LineCylinder>,
                                         std::unique_ptr<ExtremaCS_LineCone>,
                                         std::unique_ptr<ExtremaCS_CirclePlane>,
                                         std::unique_ptr<ExtremaCS_CircleSphere>,
                                         std::unique_ptr<ExtremaCS_CircleCylinder>,
                                         std::unique_ptr<ExtremaCS_EllipsePlane>,
                                         std::unique_ptr<ExtremaCS_EllipseSphere>>;

  AnalyticalVariant myAnalytical;

  // For numerical pairs, store adaptor references
  const GeomAdaptor_Curve*   myCurve       = nullptr;
  const GeomAdaptor_Surface* mySurface     = nullptr;
  bool                       myIsNumerical = false;

  const ExtremaCS::Result& performAnalytical(double                theTol,
                                             ExtremaCS::SearchMode theMode,
                                             ExtremaCS::Result&    theResult,
                                             const ExtremaCS::Domain3D& /*theDomain*/) const
  {
    return std::visit(
      [&](const auto& aPair) -> const ExtremaCS::Result& {
        if constexpr (std::is_same_v<std::decay_t<decltype(aPair)>, std::monostate>)
        {
          theResult.Status = ExtremaCS::Status::NotDone;
          return theResult;
        }
        else
        {
          return aPair->Perform(theTol, theMode);
        }
      },
      myAnalytical);
  }

  const ExtremaCS::Result& performNumerical(double                     theTol,
                                            ExtremaCS::SearchMode      theMode,
                                            ExtremaCS::Result&         theResult,
                                            const ExtremaCS::Domain3D& theDomain) const
  {
    theResult.Clear();

    if (myCurve == nullptr || mySurface == nullptr)
    {
      theResult.Status = ExtremaCS::Status::NotDone;
      return theResult;
    }

    ExtremaCS_CurveAdapter   aCurveEval(*myCurve, theDomain.Curve);
    ExtremaCS_SurfaceAdapter aSurfaceEval(*mySurface, theDomain.Surface);

    ExtremaCS_GridEvaluator3D<ExtremaCS_CurveAdapter, ExtremaCS_SurfaceAdapter> aGridEval(
      aCurveEval,
      aSurfaceEval,
      theDomain);
    aGridEval.Perform(theResult, theTol, theMode);

    return theResult;
  }
};

//==================================================================================================

ExtremaCS_CurveSurface::ExtremaCS_CurveSurface(const GeomAdaptor_Curve&   theCurve,
                                               const GeomAdaptor_Surface& theSurface)
    : myImpl(new Impl())
{
  myDomain.Curve   = {theCurve.FirstParameter(), theCurve.LastParameter()};
  myDomain.Surface = {theSurface.FirstUParameter(),
                      theSurface.LastUParameter(),
                      theSurface.FirstVParameter(),
                      theSurface.LastVParameter()};
  initPair(theCurve, theSurface);
}

//==================================================================================================

ExtremaCS_CurveSurface::ExtremaCS_CurveSurface(const GeomAdaptor_Curve&   theCurve,
                                               const GeomAdaptor_Surface& theSurface,
                                               const ExtremaCS::Domain3D& theDomain)
    : myDomain(theDomain),
      myImpl(new Impl())
{
  initPair(theCurve, theSurface);
}

//==================================================================================================

ExtremaCS_CurveSurface::ExtremaCS_CurveSurface(const GeomAdaptor_Curve&              theCurve,
                                               const GeomAdaptor_TransformedSurface& theSurface)
    : myImpl(new Impl())
{
  myDomain.Curve   = {theCurve.FirstParameter(), theCurve.LastParameter()};
  myDomain.Surface = {theSurface.FirstUParameter(),
                      theSurface.LastUParameter(),
                      theSurface.FirstVParameter(),
                      theSurface.LastVParameter()};
  // Use the underlying surface from TransformedSurface
  initPair(theCurve, theSurface.Surface());
}

//==================================================================================================

ExtremaCS_CurveSurface::ExtremaCS_CurveSurface(const GeomAdaptor_Curve&              theCurve,
                                               const GeomAdaptor_TransformedSurface& theSurface,
                                               const ExtremaCS::Domain3D&            theDomain)
    : myDomain(theDomain),
      myImpl(new Impl())
{
  // Use the underlying surface from TransformedSurface
  initPair(theCurve, theSurface.Surface());
}

//==================================================================================================

ExtremaCS_CurveSurface::~ExtremaCS_CurveSurface()
{
  delete myImpl;
}

//==================================================================================================

ExtremaCS_CurveSurface::ExtremaCS_CurveSurface(ExtremaCS_CurveSurface&& theOther) noexcept
    : myDomain(theOther.myDomain),
      myImpl(theOther.myImpl)
{
  theOther.myImpl = nullptr;
}

//==================================================================================================

ExtremaCS_CurveSurface& ExtremaCS_CurveSurface::operator=(
  ExtremaCS_CurveSurface&& theOther) noexcept
{
  if (this != &theOther)
  {
    delete myImpl;
    myDomain        = theOther.myDomain;
    myImpl          = theOther.myImpl;
    theOther.myImpl = nullptr;
  }
  return *this;
}

//==================================================================================================

void ExtremaCS_CurveSurface::initPair(const GeomAdaptor_Curve&   theCurve,
                                      const GeomAdaptor_Surface& theSurface)
{
  const CurveCategory   aCurveCat   = getCategory(theCurve.GetType());
  const SurfaceCategory aSurfaceCat = getCategory(theSurface.GetType());

  // Check if we have an analytical solution
  bool hasAnalytical = false;

  // Line-Surface pairs
  if (aCurveCat == CurveCategory::Line)
  {
    if (aSurfaceCat == SurfaceCategory::Plane)
    {
      myImpl->myAnalytical =
        std::make_unique<ExtremaCS_LinePlane>(theCurve.Line(), theSurface.Plane(), myDomain);
      hasAnalytical = true;
    }
    else if (aSurfaceCat == SurfaceCategory::Sphere)
    {
      myImpl->myAnalytical =
        std::make_unique<ExtremaCS_LineSphere>(theCurve.Line(), theSurface.Sphere(), myDomain);
      hasAnalytical = true;
    }
    else if (aSurfaceCat == SurfaceCategory::Cylinder)
    {
      myImpl->myAnalytical =
        std::make_unique<ExtremaCS_LineCylinder>(theCurve.Line(), theSurface.Cylinder(), myDomain);
      hasAnalytical = true;
    }
    else if (aSurfaceCat == SurfaceCategory::Cone)
    {
      myImpl->myAnalytical =
        std::make_unique<ExtremaCS_LineCone>(theCurve.Line(), theSurface.Cone(), myDomain);
      hasAnalytical = true;
    }
  }
  // Circle-Surface pairs
  else if (aCurveCat == CurveCategory::Circle)
  {
    if (aSurfaceCat == SurfaceCategory::Plane)
    {
      myImpl->myAnalytical =
        std::make_unique<ExtremaCS_CirclePlane>(theCurve.Circle(), theSurface.Plane(), myDomain);
      hasAnalytical = true;
    }
    else if (aSurfaceCat == SurfaceCategory::Sphere)
    {
      myImpl->myAnalytical =
        std::make_unique<ExtremaCS_CircleSphere>(theCurve.Circle(), theSurface.Sphere(), myDomain);
      hasAnalytical = true;
    }
    else if (aSurfaceCat == SurfaceCategory::Cylinder)
    {
      myImpl->myAnalytical = std::make_unique<ExtremaCS_CircleCylinder>(theCurve.Circle(),
                                                                        theSurface.Cylinder(),
                                                                        myDomain);
      hasAnalytical        = true;
    }
  }
  // Ellipse-Surface pairs
  else if (aCurveCat == CurveCategory::Ellipse)
  {
    if (aSurfaceCat == SurfaceCategory::Plane)
    {
      myImpl->myAnalytical =
        std::make_unique<ExtremaCS_EllipsePlane>(theCurve.Ellipse(), theSurface.Plane(), myDomain);
      hasAnalytical = true;
    }
    else if (aSurfaceCat == SurfaceCategory::Sphere)
    {
      myImpl->myAnalytical = std::make_unique<ExtremaCS_EllipseSphere>(theCurve.Ellipse(),
                                                                       theSurface.Sphere(),
                                                                       myDomain);
      hasAnalytical        = true;
    }
  }

  if (!hasAnalytical)
  {
    // Use numerical pair
    myImpl->myIsNumerical = true;
    myImpl->myCurve       = &theCurve;
    myImpl->mySurface     = &theSurface;
  }
}

//==================================================================================================

const ExtremaCS::Result& ExtremaCS_CurveSurface::Perform(double                theTol,
                                                         ExtremaCS::SearchMode theMode) const
{
  if (myImpl->myIsNumerical)
  {
    myImpl->performNumerical(theTol, theMode, myResult, myDomain);
  }
  else
  {
    const ExtremaCS::Result& aPairResult =
      myImpl->performAnalytical(theTol, theMode, myResult, myDomain);

    // Copy results from the underlying pair if it's not a monostate case
    if (&aPairResult != &myResult)
    {
      myResult.Clear();
      myResult.Status                 = aPairResult.Status;
      myResult.InfiniteSquareDistance = aPairResult.InfiniteSquareDistance;
      for (int i = 0; i < aPairResult.Extrema.Length(); ++i)
      {
        myResult.Extrema.Append(aPairResult.Extrema(i));
      }
    }
  }

  return myResult;
}
