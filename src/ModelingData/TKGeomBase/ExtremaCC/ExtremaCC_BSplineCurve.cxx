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

#include <ExtremaCC_BSplineCurve.hxx>

//==================================================================================================

ExtremaCC_BSplineCurve::ExtremaCC_BSplineCurve(const occ::handle<Geom_BSplineCurve>& theCurve)
    : myCurve(theCurve)
{
  if (!myCurve.IsNull())
  {
    myDomain.Min = myCurve->FirstParameter();
    myDomain.Max = myCurve->LastParameter();
  }
}

//==================================================================================================

ExtremaCC_BSplineCurve::ExtremaCC_BSplineCurve(const occ::handle<Geom_BSplineCurve>& theCurve,
                                               const ExtremaCC::Domain1D&            theDomain)
    : myCurve(theCurve),
      myDomain(theDomain)
{
}

//==================================================================================================

gp_Pnt ExtremaCC_BSplineCurve::Value(double theU) const
{
  return myCurve->Value(theU);
}

//==================================================================================================

void ExtremaCC_BSplineCurve::D1(double theU, gp_Pnt& thePt, gp_Vec& theD1) const
{
  myCurve->D1(theU, thePt, theD1);
}

//==================================================================================================

void ExtremaCC_BSplineCurve::D2(double theU, gp_Pnt& thePt, gp_Vec& theD1, gp_Vec& theD2) const
{
  myCurve->D2(theU, thePt, theD1, theD2);
}
