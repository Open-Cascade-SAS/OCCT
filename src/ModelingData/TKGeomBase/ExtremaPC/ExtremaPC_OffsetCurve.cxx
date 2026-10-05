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

#include <ExtremaPC_OffsetCurve.hxx>

#include <ExtremaPC_GridEvaluator.hxx>
#include <ExtremaPC_Planar.hxx>
#include <ExtremaPC2d_OffsetCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <ProjLib.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>

//==================================================================================================

ExtremaPC_OffsetCurve::ExtremaPC_OffsetCurve(const Adaptor3d_Curve& theCurve)
    : myCurve(&theCurve),
      myDomain{theCurve.FirstParameter(), theCurve.LastParameter()}
{
  buildParams();
}

//==================================================================================================

ExtremaPC_OffsetCurve::ExtremaPC_OffsetCurve(const Adaptor3d_Curve&     theCurve,
                                             const ExtremaPC::Domain1D& theDomain)
    : myCurve(&theCurve),
      myDomain(theDomain)
{
  buildParams();
}

//==================================================================================================

void ExtremaPC_OffsetCurve::buildParams()
{
  if (myCurve == nullptr || !myDomain.IsValid() || !myDomain.IsFinite())
  {
    return;
  }

  math_Vector aParams = ExtremaPC_GridEvaluator::BuildCurveAwareParams(*myCurve, myDomain);

  myEvaluator.SetParams(aParams);
}

//==================================================================================================

gp_Pnt ExtremaPC_OffsetCurve::Value(double theU) const
{
  return myCurve->Value(theU);
}

//=================================================================================================

bool ExtremaPC_OffsetCurve::performPlanar(const gp_Pnt&         theP,
                                          double                theTol,
                                          ExtremaPC::SearchMode theMode,
                                          bool                  theIncludeEndpoints) const
{
  gp_Pln                          aPlane;
  occ::handle<Geom2d_OffsetCurve> anOffset2d;
  try
  {
    OCC_CATCH_SIGNALS
    const GeomAdaptor_Curve* aGeomAdaptor = dynamic_cast<const GeomAdaptor_Curve*>(myCurve);
    if (aGeomAdaptor == nullptr)
    {
      return false;
    }
    const occ::handle<Geom_OffsetCurve> anOffset =
      occ::down_cast<Geom_OffsetCurve>(aGeomAdaptor->Curve());
    if (!ExtremaPC::ProjectOffset(anOffset, aPlane, anOffset2d))
    {
      return false;
    }
  }
  catch (const Standard_Failure&)
  {
    return false;
  }

  Geom2dAdaptor_Curve           anAdaptor2d(anOffset2d, myDomain.Min, myDomain.Max);
  ExtremaPC2d_OffsetCurve       anEvaluator2d(anAdaptor2d,
                                        ExtremaPC2d::Domain1D(myDomain.Min, myDomain.Max));
  const ExtremaPC2d::SearchMode aMode    = ExtremaPC::ToSearchMode2d(theMode);
  const gp_Pnt2d                aPoint2d = ProjLib::Project(aPlane, theP);
  const ExtremaPC2d::Result&    aResult2d =
    theIncludeEndpoints ? anEvaluator2d.PerformWithEndpoints(aPoint2d, theTol, aMode)
                           : anEvaluator2d.Perform(aPoint2d, theTol, aMode);
  return ExtremaPC::ConvertPlanarResult(aResult2d, theP, aPlane, *this, myEvaluator.Result());
}

//==================================================================================================

const ExtremaPC::Result& ExtremaPC_OffsetCurve::Perform(const gp_Pnt&         theP,
                                                        double                theTol,
                                                        ExtremaPC::SearchMode theMode) const
{
  if (myCurve == nullptr)
  {
    myEvaluator.Result().Clear();
    myEvaluator.Result().Status = ExtremaPC::Status::NotDone;
    return myEvaluator.Result();
  }

  if (ExtremaPC::IsValidTolerance(theTol) && myDomain.IsValid()
      && performPlanar(theP, theTol, theMode, false))
  {
    return myEvaluator.Result();
  }

  return myEvaluator.Perform(*myCurve, theP, myDomain, theTol, theMode);
}

//==================================================================================================

const ExtremaPC::Result& ExtremaPC_OffsetCurve::PerformWithEndpoints(
  const gp_Pnt&         theP,
  double                theTol,
  ExtremaPC::SearchMode theMode) const
{
  if (myCurve == nullptr)
  {
    myEvaluator.Result().Clear();
    myEvaluator.Result().Status = ExtremaPC::Status::NotDone;
    return myEvaluator.Result();
  }

  if (ExtremaPC::IsValidTolerance(theTol) && myDomain.IsValid()
      && performPlanar(theP, theTol, theMode, true))
  {
    return myEvaluator.Result();
  }

  // Get interior extrema (populates myEvaluator's result)
  (void)myEvaluator.Perform(*myCurve, theP, myDomain, theTol, theMode);

  // Add endpoints to the result
  ExtremaPC::Result& aResult = myEvaluator.Result();
  if (aResult.Status == ExtremaPC::Status::OK || aResult.Status == ExtremaPC::Status::NoSolution)
  {
    const bool isClosed = ExtremaPC_GridEvaluator::IsClosedDomain(*myCurve, myDomain);
    ExtremaPC::AddEndpointExtrema(aResult, theP, myDomain, *this, theMode, isClosed);

    // Update status if we found any extrema (including endpoints)
    if (!aResult.Extrema.IsEmpty())
    {
      aResult.Status = ExtremaPC::Status::OK;
    }
  }

  return aResult;
}
