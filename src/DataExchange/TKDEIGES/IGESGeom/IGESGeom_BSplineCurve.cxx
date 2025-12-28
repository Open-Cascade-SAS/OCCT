// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <gp_GTrsf.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <IGESGeom_BSplineCurve.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGeom_BSplineCurve, IGESData_IGESEntity)

IGESGeom_BSplineCurve::IGESGeom_BSplineCurve() {}

void IGESGeom_BSplineCurve::Init(const int               anIndex,
                                 const int               aDegree,
                                 const bool               aPlanar,
                                 const bool               aClosed,
                                 const bool               aPolynom,
                                 const bool               aPeriodic,
                                 const occ::handle<NCollection_HArray1<double>>& allKnots,
                                 const occ::handle<NCollection_HArray1<double>>& allWeights,
                                 const occ::handle<NCollection_HArray1<gp_XYZ>>&   allPoles,
                                 const double                  aUmin,
                                 const double                  aUmax,
                                 const gp_XYZ&                        aNorm)
{
  if (!allPoles.IsNull())
  {
    if (allKnots->Lower() != -aDegree || allKnots->Upper() != anIndex + 1 || allPoles->Lower() != 0)
    {
      throw Standard_DimensionMismatch("IGESGeom_BSplineCurve : Init");
    }
    if (!allWeights.IsNull())
    {
      if (allPoles->Length() != allWeights->Length() || allWeights->Upper() != anIndex
          || allWeights->Lower() != 0)
      {
        throw Standard_DimensionMismatch("IGESGeom_BSplineCurve : Init");
      }
    }
  }

  theIndex     = anIndex;
  theDegree    = aDegree;
  isPlanar     = aPlanar;
  isClosed     = aClosed;
  isPolynomial = aPolynom;
  isPeriodic   = aPeriodic;
  theKnots     = allKnots;
  theWeights   = allWeights;
  thePoles     = allPoles;
  theUmin      = aUmin;
  theUmax      = aUmax;
  theNorm      = aNorm;
  InitTypeAndForm(126, FormNumber());
  // FormNumber  precises the shape  0-5
}

void IGESGeom_BSplineCurve::SetFormNumber(const int form)
{
  if (form < 0 || form > 5)
    throw Standard_OutOfRange("IGESGeom_BSplineCurve : SetFormNumber");
  InitTypeAndForm(126, form);
}

int IGESGeom_BSplineCurve::UpperIndex() const
{
  return theIndex;
}

int IGESGeom_BSplineCurve::Degree() const
{
  return theDegree;
}

bool IGESGeom_BSplineCurve::IsPlanar() const
{
  return isPlanar;
}

bool IGESGeom_BSplineCurve::IsClosed() const
{
  return isClosed;
}

bool IGESGeom_BSplineCurve::IsPolynomial(const bool flag) const
{
  if (flag || theWeights.IsNull())
    return isPolynomial;
  int i, i1 = theWeights->Lower(), i2 = theWeights->Upper();
  double    w0 = theWeights->Value(i1);
  for (i = i1 + 1; i <= i2; i++)
    if (std::abs(theWeights->Value(i) - w0) > 1.e-10)
      return false;
  return true;
}

bool IGESGeom_BSplineCurve::IsPeriodic() const
{
  return isPeriodic;
}

int IGESGeom_BSplineCurve::NbKnots() const
{
  return (theKnots.IsNull() ? 0 : theKnots->Length());
}

double IGESGeom_BSplineCurve::Knot(const int anIndex) const
{
  return theKnots->Value(anIndex);
}

int IGESGeom_BSplineCurve::NbPoles() const
{
  return (thePoles.IsNull() ? 0 : thePoles->Length());
}

double IGESGeom_BSplineCurve::Weight(const int anIndex) const
{
  return theWeights->Value(anIndex);
}

gp_Pnt IGESGeom_BSplineCurve::Pole(const int anIndex) const
{
  gp_XYZ tempXYZ = thePoles->Value(anIndex);
  gp_Pnt Pole(tempXYZ);
  return Pole;
}

gp_Pnt IGESGeom_BSplineCurve::TransformedPole(const int anIndex) const
{
  gp_XYZ tempXYZ = thePoles->Value(anIndex);
  if (HasTransf())
    Location().Transforms(tempXYZ);
  gp_Pnt Pole(tempXYZ);
  return Pole;
}

double IGESGeom_BSplineCurve::UMin() const
{
  return theUmin;
}

double IGESGeom_BSplineCurve::UMax() const
{
  return theUmax;
}

gp_XYZ IGESGeom_BSplineCurve::Normal() const
{
  return theNorm;
}
