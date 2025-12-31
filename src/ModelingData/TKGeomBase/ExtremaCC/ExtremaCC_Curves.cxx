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

#include <ExtremaCC_Curves.hxx>

// Include all pair implementations
#include <ExtremaCC_BezierCurve.hxx>
#include <ExtremaCC_BSplineCurve.hxx>
#include <ExtremaCC_Circle.hxx>
#include <ExtremaCC_CircleCircle.hxx>
#include <ExtremaCC_CircleEllipse.hxx>
#include <ExtremaCC_CircleHyperbola.hxx>
#include <ExtremaCC_CircleLine.hxx>
#include <ExtremaCC_CircleParabola.hxx>
#include <ExtremaCC_Ellipse.hxx>
#include <ExtremaCC_EllipseEllipse.hxx>
#include <ExtremaCC_EllipseHyperbola.hxx>
#include <ExtremaCC_EllipseLine.hxx>
#include <ExtremaCC_EllipseParabola.hxx>
#include <ExtremaCC_GridEvaluator2D.hxx>
#include <ExtremaCC_Hyperbola.hxx>
#include <ExtremaCC_HyperbolaHyperbola.hxx>
#include <ExtremaCC_HyperbolaLine.hxx>
#include <ExtremaCC_HyperbolaParabola.hxx>
#include <ExtremaCC_Line.hxx>
#include <ExtremaCC_LineLine.hxx>
#include <ExtremaCC_LineParabola.hxx>
#include <ExtremaCC_Numerical.hxx>
#include <ExtremaCC_OffsetCurve.hxx>
#include <ExtremaCC_OtherCurve.hxx>
#include <ExtremaCC_Parabola.hxx>
#include <ExtremaCC_ParabolaParabola.hxx>
#include <GeomAbs_CurveType.hxx>

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

  //! Get category from curve type.
  CurveCategory getCategory(GeomAbs_CurveType theType)
  {
    switch (theType)
    {
      case GeomAbs_Line: return CurveCategory::Line;
      case GeomAbs_Circle: return CurveCategory::Circle;
      case GeomAbs_Ellipse: return CurveCategory::Ellipse;
      case GeomAbs_Hyperbola: return CurveCategory::Hyperbola;
      case GeomAbs_Parabola: return CurveCategory::Parabola;
      case GeomAbs_BezierCurve: return CurveCategory::BezierCurve;
      case GeomAbs_BSplineCurve: return CurveCategory::BSplineCurve;
      case GeomAbs_OffsetCurve: return CurveCategory::OffsetCurve;
      default: return CurveCategory::OtherCurve;
    }
  }

  //! Check if category is elementary (can use analytical methods).
  bool isElementary(CurveCategory theCat)
  {
    return theCat <= CurveCategory::Parabola;
  }
} // namespace

//! Implementation structure using variant for type-safe dispatch.
struct ExtremaCC_Curves::Impl
{
  using AnalyticalVariant = std::variant<std::monostate,
                                         std::unique_ptr<ExtremaCC_LineLine>,
                                         std::unique_ptr<ExtremaCC_CircleLine>,
                                         std::unique_ptr<ExtremaCC_CircleCircle>,
                                         std::unique_ptr<ExtremaCC_EllipseLine>,
                                         std::unique_ptr<ExtremaCC_EllipseEllipse>,
                                         std::unique_ptr<ExtremaCC_CircleEllipse>,
                                         std::unique_ptr<ExtremaCC_HyperbolaLine>,
                                         std::unique_ptr<ExtremaCC_CircleHyperbola>,
                                         std::unique_ptr<ExtremaCC_EllipseHyperbola>,
                                         std::unique_ptr<ExtremaCC_HyperbolaHyperbola>,
                                         std::unique_ptr<ExtremaCC_LineParabola>,
                                         std::unique_ptr<ExtremaCC_CircleParabola>,
                                         std::unique_ptr<ExtremaCC_EllipseParabola>,
                                         std::unique_ptr<ExtremaCC_HyperbolaParabola>,
                                         std::unique_ptr<ExtremaCC_ParabolaParabola>>;

  AnalyticalVariant myAnalytical;

  // For numerical pairs, we store the curve references
  const Adaptor3d_Curve* myCurve1 = nullptr;
  const Adaptor3d_Curve* myCurve2 = nullptr;
  bool                   myIsNumerical = false;

  const ExtremaCC::Result& performAnalytical(double                     theTol,
                                             ExtremaCC::SearchMode     theMode,
                                             ExtremaCC::Result&         theResult,
                                             const ExtremaCC::Domain2D& /*theDomain*/) const
  {
    // Note: theDomain unused because analytical pairs store domain at construction
    return std::visit(
      [&](const auto& aPair) -> const ExtremaCC::Result& {
        if constexpr (std::is_same_v<std::decay_t<decltype(aPair)>, std::monostate>)
        {
          theResult.Status = ExtremaCC::Status::NotDone;
          return theResult;
        }
        else
        {
          return aPair->Perform(theTol, theMode);
        }
      },
      myAnalytical);
  }

  const ExtremaCC::Result& performAnalyticalWithEndpoints(double                     theTol,
                                                          ExtremaCC::SearchMode     theMode,
                                                          ExtremaCC::Result&         theResult,
                                                          const ExtremaCC::Domain2D& /*theDomain*/) const
  {
    // Note: theDomain unused because analytical pairs store domain at construction
    return std::visit(
      [&](const auto& aPair) -> const ExtremaCC::Result& {
        if constexpr (std::is_same_v<std::decay_t<decltype(aPair)>, std::monostate>)
        {
          theResult.Status = ExtremaCC::Status::NotDone;
          return theResult;
        }
        else
        {
          return aPair->PerformWithEndpoints(theTol, theMode);
        }
      },
      myAnalytical);
  }

  const ExtremaCC::Result& performNumerical(double                     theTol,
                                            ExtremaCC::SearchMode     theMode,
                                            ExtremaCC::Result&         theResult,
                                            const ExtremaCC::Domain2D& theDomain) const
  {
    theResult.Clear();

    if (myCurve1 == nullptr || myCurve2 == nullptr)
    {
      theResult.Status = ExtremaCC::Status::NotDone;
      return theResult;
    }

    ExtremaCC_OtherCurve aEval1(*myCurve1, theDomain.Curve1);
    ExtremaCC_OtherCurve aEval2(*myCurve2, theDomain.Curve2);

    ExtremaCC_GridEvaluator2D<ExtremaCC_OtherCurve, ExtremaCC_OtherCurve> aGridEval(aEval1,
                                                                                    aEval2,
                                                                                    theDomain);
    aGridEval.Perform(theResult, theTol, theMode);

    return theResult;
  }
};

//==================================================================================================

ExtremaCC_Curves::ExtremaCC_Curves(const Adaptor3d_Curve& theCurve1,
                                   const Adaptor3d_Curve& theCurve2)
    : mySwapped(false),
      myImpl(new Impl())
{
  myDomain.Curve1 = {theCurve1.FirstParameter(), theCurve1.LastParameter()};
  myDomain.Curve2 = {theCurve2.FirstParameter(), theCurve2.LastParameter()};
  initPair(theCurve1, theCurve2);
}

//==================================================================================================

ExtremaCC_Curves::ExtremaCC_Curves(const Adaptor3d_Curve&      theCurve1,
                                   const Adaptor3d_Curve&      theCurve2,
                                   const ExtremaCC::Domain2D& theDomain)
    : myDomain(theDomain),
      mySwapped(false),
      myImpl(new Impl())
{
  initPair(theCurve1, theCurve2);
}

//==================================================================================================

ExtremaCC_Curves::~ExtremaCC_Curves()
{
  delete myImpl;
}

//==================================================================================================

ExtremaCC_Curves::ExtremaCC_Curves(ExtremaCC_Curves&& theOther) noexcept
    : myDomain(theOther.myDomain),
      mySwapped(theOther.mySwapped),
      myImpl(theOther.myImpl)
{
  theOther.myImpl = nullptr;
}

//==================================================================================================

ExtremaCC_Curves& ExtremaCC_Curves::operator=(ExtremaCC_Curves&& theOther) noexcept
{
  if (this != &theOther)
  {
    delete myImpl;
    myDomain        = theOther.myDomain;
    mySwapped       = theOther.mySwapped;
    myImpl          = theOther.myImpl;
    theOther.myImpl = nullptr;
  }
  return *this;
}

//==================================================================================================

void ExtremaCC_Curves::initPair(const Adaptor3d_Curve& theCurve1, const Adaptor3d_Curve& theCurve2)
{
  const CurveCategory aCat1 = getCategory(theCurve1.GetType());
  const CurveCategory aCat2 = getCategory(theCurve2.GetType());

  // Check if both curves are elementary (can use analytical methods)
  if (isElementary(aCat1) && isElementary(aCat2))
  {
    // Create the appropriate analytical pair (alphabetically ordered)
    // LineLine
    if (aCat1 == CurveCategory::Line && aCat2 == CurveCategory::Line)
    {
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_LineLine>(theCurve1.Line(), theCurve2.Line(), myDomain);
    }
    // CircleCircle
    else if (aCat1 == CurveCategory::Circle && aCat2 == CurveCategory::Circle)
    {
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_CircleCircle>(theCurve1.Circle(), theCurve2.Circle(), myDomain);
    }
    // CircleLine
    else if (aCat1 == CurveCategory::Circle && aCat2 == CurveCategory::Line)
    {
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_CircleLine>(theCurve1.Circle(), theCurve2.Line(), myDomain);
    }
    else if (aCat1 == CurveCategory::Line && aCat2 == CurveCategory::Circle)
    {
      mySwapped = true;
      ExtremaCC::Domain2D aSwappedDomain{myDomain.Curve2, myDomain.Curve1};
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_CircleLine>(theCurve2.Circle(), theCurve1.Line(), aSwappedDomain);
    }
    // EllipseLine
    else if (aCat1 == CurveCategory::Ellipse && aCat2 == CurveCategory::Line)
    {
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_EllipseLine>(theCurve1.Ellipse(), theCurve2.Line(), myDomain);
    }
    else if (aCat1 == CurveCategory::Line && aCat2 == CurveCategory::Ellipse)
    {
      mySwapped = true;
      ExtremaCC::Domain2D aSwappedDomain{myDomain.Curve2, myDomain.Curve1};
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_EllipseLine>(theCurve2.Ellipse(), theCurve1.Line(), aSwappedDomain);
    }
    // HyperbolaLine
    else if (aCat1 == CurveCategory::Hyperbola && aCat2 == CurveCategory::Line)
    {
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_HyperbolaLine>(theCurve1.Hyperbola(), theCurve2.Line(), myDomain);
    }
    else if (aCat1 == CurveCategory::Line && aCat2 == CurveCategory::Hyperbola)
    {
      mySwapped = true;
      ExtremaCC::Domain2D aSwappedDomain{myDomain.Curve2, myDomain.Curve1};
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_HyperbolaLine>(theCurve2.Hyperbola(), theCurve1.Line(), aSwappedDomain);
    }
    // LineParabola
    else if (aCat1 == CurveCategory::Line && aCat2 == CurveCategory::Parabola)
    {
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_LineParabola>(theCurve1.Line(), theCurve2.Parabola(), myDomain);
    }
    else if (aCat1 == CurveCategory::Parabola && aCat2 == CurveCategory::Line)
    {
      mySwapped = true;
      ExtremaCC::Domain2D aSwappedDomain{myDomain.Curve2, myDomain.Curve1};
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_LineParabola>(theCurve2.Line(), theCurve1.Parabola(), aSwappedDomain);
    }
    // CircleEllipse
    else if (aCat1 == CurveCategory::Circle && aCat2 == CurveCategory::Ellipse)
    {
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_CircleEllipse>(theCurve1.Circle(), theCurve2.Ellipse(), myDomain);
    }
    else if (aCat1 == CurveCategory::Ellipse && aCat2 == CurveCategory::Circle)
    {
      mySwapped = true;
      ExtremaCC::Domain2D aSwappedDomain{myDomain.Curve2, myDomain.Curve1};
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_CircleEllipse>(theCurve2.Circle(), theCurve1.Ellipse(), aSwappedDomain);
    }
    // CircleHyperbola
    else if (aCat1 == CurveCategory::Circle && aCat2 == CurveCategory::Hyperbola)
    {
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_CircleHyperbola>(theCurve1.Circle(), theCurve2.Hyperbola(), myDomain);
    }
    else if (aCat1 == CurveCategory::Hyperbola && aCat2 == CurveCategory::Circle)
    {
      mySwapped = true;
      ExtremaCC::Domain2D aSwappedDomain{myDomain.Curve2, myDomain.Curve1};
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_CircleHyperbola>(theCurve2.Circle(), theCurve1.Hyperbola(), aSwappedDomain);
    }
    // CircleParabola
    else if (aCat1 == CurveCategory::Circle && aCat2 == CurveCategory::Parabola)
    {
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_CircleParabola>(theCurve1.Circle(), theCurve2.Parabola(), myDomain);
    }
    else if (aCat1 == CurveCategory::Parabola && aCat2 == CurveCategory::Circle)
    {
      mySwapped = true;
      ExtremaCC::Domain2D aSwappedDomain{myDomain.Curve2, myDomain.Curve1};
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_CircleParabola>(theCurve2.Circle(), theCurve1.Parabola(), aSwappedDomain);
    }
    // EllipseEllipse
    else if (aCat1 == CurveCategory::Ellipse && aCat2 == CurveCategory::Ellipse)
    {
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_EllipseEllipse>(theCurve1.Ellipse(), theCurve2.Ellipse(), myDomain);
    }
    // EllipseHyperbola
    else if (aCat1 == CurveCategory::Ellipse && aCat2 == CurveCategory::Hyperbola)
    {
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_EllipseHyperbola>(theCurve1.Ellipse(), theCurve2.Hyperbola(), myDomain);
    }
    else if (aCat1 == CurveCategory::Hyperbola && aCat2 == CurveCategory::Ellipse)
    {
      mySwapped = true;
      ExtremaCC::Domain2D aSwappedDomain{myDomain.Curve2, myDomain.Curve1};
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_EllipseHyperbola>(theCurve2.Ellipse(), theCurve1.Hyperbola(), aSwappedDomain);
    }
    // EllipseParabola
    else if (aCat1 == CurveCategory::Ellipse && aCat2 == CurveCategory::Parabola)
    {
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_EllipseParabola>(theCurve1.Ellipse(), theCurve2.Parabola(), myDomain);
    }
    else if (aCat1 == CurveCategory::Parabola && aCat2 == CurveCategory::Ellipse)
    {
      mySwapped = true;
      ExtremaCC::Domain2D aSwappedDomain{myDomain.Curve2, myDomain.Curve1};
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_EllipseParabola>(theCurve2.Ellipse(), theCurve1.Parabola(), aSwappedDomain);
    }
    // HyperbolaHyperbola
    else if (aCat1 == CurveCategory::Hyperbola && aCat2 == CurveCategory::Hyperbola)
    {
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_HyperbolaHyperbola>(theCurve1.Hyperbola(), theCurve2.Hyperbola(), myDomain);
    }
    // HyperbolaParabola
    else if (aCat1 == CurveCategory::Hyperbola && aCat2 == CurveCategory::Parabola)
    {
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_HyperbolaParabola>(theCurve1.Hyperbola(), theCurve2.Parabola(), myDomain);
    }
    else if (aCat1 == CurveCategory::Parabola && aCat2 == CurveCategory::Hyperbola)
    {
      mySwapped = true;
      ExtremaCC::Domain2D aSwappedDomain{myDomain.Curve2, myDomain.Curve1};
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_HyperbolaParabola>(theCurve2.Hyperbola(), theCurve1.Parabola(), aSwappedDomain);
    }
    // ParabolaParabola
    else if (aCat1 == CurveCategory::Parabola && aCat2 == CurveCategory::Parabola)
    {
      myImpl->myAnalytical =
        std::make_unique<ExtremaCC_ParabolaParabola>(theCurve1.Parabola(), theCurve2.Parabola(), myDomain);
    }
    // For remaining pairs, fall back to numerical
    else
    {
      myImpl->myIsNumerical = true;
      myImpl->myCurve1      = &theCurve1;
      myImpl->myCurve2      = &theCurve2;
    }
  }
  else
  {
    // Numerical pair
    myImpl->myIsNumerical = true;
    myImpl->myCurve1      = &theCurve1;
    myImpl->myCurve2      = &theCurve2;
  }
}

//==================================================================================================

const ExtremaCC::Result& ExtremaCC_Curves::Perform(double                theTol,
                                                   ExtremaCC::SearchMode theMode) const
{
  if (myImpl->myIsNumerical)
  {
    myImpl->performNumerical(theTol, theMode, myResult, myDomain);
  }
  else
  {
    const ExtremaCC::Result& aPairResult =
      myImpl->performAnalytical(theTol, theMode, myResult, myDomain);
    // Copy results from the underlying pair if it's not a monostate case
    if (&aPairResult != &myResult)
    {
      myResult.Clear();
      myResult.Status                = aPairResult.Status;
      myResult.InfiniteSquareDistance = aPairResult.InfiniteSquareDistance;
      for (int i = 0; i < aPairResult.Extrema.Length(); ++i)
      {
        myResult.Extrema.Append(aPairResult.Extrema(i));
      }
    }
  }

  // Swap parameters back if curves were swapped
  if (mySwapped && myResult.Status == ExtremaCC::Status::OK)
  {
    for (int i = 0; i < myResult.Extrema.Length(); ++i)
    {
      std::swap(myResult.Extrema(i).Parameter1, myResult.Extrema(i).Parameter2);
      std::swap(myResult.Extrema(i).Point1, myResult.Extrema(i).Point2);
    }
  }

  return myResult;
}

//==================================================================================================

const ExtremaCC::Result& ExtremaCC_Curves::PerformWithEndpoints(double                theTol,
                                                                ExtremaCC::SearchMode theMode) const
{
  if (myImpl->myIsNumerical)
  {
    // First perform the regular extrema computation
    myImpl->performNumerical(theTol, theMode, myResult, myDomain);

    // Add endpoint extrema for numerical pairs
    if (myResult.Status == ExtremaCC::Status::OK &&
        myImpl->myCurve1 != nullptr && myImpl->myCurve2 != nullptr)
    {
      // Create evaluators for endpoint computation
      ExtremaCC_OtherCurve aEval1(*myImpl->myCurve1, myDomain.Curve1);
      ExtremaCC_OtherCurve aEval2(*myImpl->myCurve2, myDomain.Curve2);

      // Add endpoint extrema (respecting swap if needed)
      if (mySwapped)
      {
        ExtremaCC::Domain2D aSwappedDomain{myDomain.Curve2, myDomain.Curve1};
        ExtremaCC::AddEndpointExtrema(myResult, aSwappedDomain, aEval2, aEval1, theTol, theMode);
      }
      else
      {
        ExtremaCC::AddEndpointExtrema(myResult, myDomain, aEval1, aEval2, theTol, theMode);
      }
    }
  }
  else
  {
    // Use PerformWithEndpoints for analytical pairs
    const ExtremaCC::Result& aPairResult =
      myImpl->performAnalyticalWithEndpoints(theTol, theMode, myResult, myDomain);

    // Copy results from the underlying pair if it's not a monostate case
    if (&aPairResult != &myResult)
    {
      myResult.Clear();
      myResult.Status                = aPairResult.Status;
      myResult.InfiniteSquareDistance = aPairResult.InfiniteSquareDistance;
      for (int i = 0; i < aPairResult.Extrema.Length(); ++i)
      {
        myResult.Extrema.Append(aPairResult.Extrema(i));
      }
    }

    // Swap parameters back if curves were swapped
    if (mySwapped && myResult.Status == ExtremaCC::Status::OK)
    {
      for (int i = 0; i < myResult.Extrema.Length(); ++i)
      {
        std::swap(myResult.Extrema(i).Parameter1, myResult.Extrema(i).Parameter2);
        std::swap(myResult.Extrema(i).Point1, myResult.Extrema(i).Point2);
      }
    }
  }

  return myResult;
}
