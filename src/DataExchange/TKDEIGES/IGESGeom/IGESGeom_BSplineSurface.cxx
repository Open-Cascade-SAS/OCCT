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
#include <IGESGeom_BSplineSurface.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGeom_BSplineSurface, IGESData_IGESEntity)

IGESGeom_BSplineSurface::IGESGeom_BSplineSurface() {}

void IGESGeom_BSplineSurface::Init(const int                                       anIndexU,
                                   const int                                       anIndexV,
                                   const int                                       aDegU,
                                   const int                                       aDegV,
                                   const bool                                      aCloseU,
                                   const bool                                      aCloseV,
                                   const bool                                      aPolynom,
                                   const bool                                      aPeriodU,
                                   const bool                                      aPeriodV,
                                   const occ::handle<NCollection_HArray1<double>>& allKnotsU,
                                   const occ::handle<NCollection_HArray1<double>>& allKnotsV,
                                   const occ::handle<NCollection_HArray2<double>>& allWeights,
                                   const occ::handle<NCollection_HArray2<gp_XYZ>>& allPoles,
                                   const double                                    aUmin,
                                   const double                                    aUmax,
                                   const double                                    aVmin,
                                   const double                                    aVmax)
{
  if (allWeights->RowLength() != allPoles->RowLength()
      || allWeights->ColLength() != allPoles->ColLength())
    throw Standard_DimensionMismatch("IGESGeom_BSplineSurface : Init");
  if (allKnotsU->Lower() != -aDegU || allKnotsV->Lower() != -aDegV
      || allKnotsU->Upper() != anIndexU + 1 || allKnotsV->Upper() != anIndexV + 1
      || allWeights->LowerRow() != 0 || allWeights->LowerCol() != 0 || allPoles->LowerRow() != 0
      || allPoles->LowerCol() != 0 || allPoles->UpperRow() != anIndexU
      || allPoles->UpperCol() != anIndexV)
    throw Standard_DimensionMismatch("IGESGeom_BSplineSurface : Init");

  theIndexU    = anIndexU;
  theIndexV    = anIndexV;
  theDegreeU   = aDegU;
  theDegreeV   = aDegV;
  isClosedU    = aCloseU;
  isClosedV    = aCloseV;
  isPolynomial = aPolynom;
  isPeriodicU  = aPeriodU;
  isPeriodicV  = aPeriodV;
  theKnotsU    = allKnotsU;
  theKnotsV    = allKnotsV;
  theWeights   = allWeights;
  thePoles     = allPoles;
  theUmin      = aUmin;
  theUmax      = aUmax;
  theVmin      = aVmin;
  theVmax      = aVmax;
  InitTypeAndForm(128, FormNumber());
  // FormNumber  precises the shape  0-9
}

void IGESGeom_BSplineSurface::SetFormNumber(const int form)
{
  if (form < 0 || form > 9)
    throw Standard_OutOfRange("IGESGeom_BSplineSurface : SetFormNumber");
  InitTypeAndForm(128, form);
}

int IGESGeom_BSplineSurface::UpperIndexU() const
{
  return theIndexU;
}

int IGESGeom_BSplineSurface::UpperIndexV() const
{
  return theIndexV;
}

int IGESGeom_BSplineSurface::DegreeU() const
{
  return theDegreeU;
}

int IGESGeom_BSplineSurface::DegreeV() const
{
  return theDegreeV;
}

bool IGESGeom_BSplineSurface::IsClosedU() const
{
  return isClosedU;
}

bool IGESGeom_BSplineSurface::IsClosedV() const
{
  return isClosedV;
}

bool IGESGeom_BSplineSurface::IsPolynomial(const bool flag) const
{
  if (flag)
    return isPolynomial;
  int    i, j;
  double w0 = theWeights->Value(0, 0);
  /*CR23377
   * Following fix is needed to address Rational surface with non-unitary weights at last index
   * Limit for indices are changed from theIndexV-->theIndexV+1 (=NbPolesV())
   *                                    theIndexU--> theIndexU+1 (=NbPolesU())
   */
  for (j = 0; j < (theIndexV + 1); j++)
    for (i = 0; i < (theIndexU + 1); i++)
      if (std::abs(theWeights->Value(i, j) - w0) > 1.e-10)
        return false;
  return true;
}

bool IGESGeom_BSplineSurface::IsPeriodicU() const
{
  return isPeriodicU;
}

bool IGESGeom_BSplineSurface::IsPeriodicV() const
{
  return isPeriodicV;
}

int IGESGeom_BSplineSurface::NbKnotsU() const
{
  return theKnotsU->Length();
}

int IGESGeom_BSplineSurface::NbKnotsV() const
{
  return theKnotsV->Length();
}

double IGESGeom_BSplineSurface::KnotU(const int anIndex) const
{
  return theKnotsU->Value(anIndex);
}

double IGESGeom_BSplineSurface::KnotV(const int anIndex) const
{
  return theKnotsV->Value(anIndex);
}

int IGESGeom_BSplineSurface::NbPolesU() const
{
  return theIndexU + 1;
}

int IGESGeom_BSplineSurface::NbPolesV() const
{
  return theIndexV + 1;
}

double IGESGeom_BSplineSurface::Weight(const int anIndex1, const int anIndex2) const
{
  return theWeights->Value(anIndex1, anIndex2);
}

gp_Pnt IGESGeom_BSplineSurface::Pole(const int anIndex1, const int anIndex2) const
{
  gp_XYZ tempXYZ = thePoles->Value(anIndex1, anIndex2);
  // Reversal of the order of indices since the poles are
  // stored in the array like that. See ReadOwnParams()
  gp_Pnt Pole(tempXYZ);
  return Pole;
}

gp_Pnt IGESGeom_BSplineSurface::TransformedPole(const int anIndex1, const int anIndex2) const
{
  gp_XYZ tempXYZ = thePoles->Value(anIndex1, anIndex2);
  // Reversal of the order of indices since the poles are
  // stored in the array like that. See ReadOwnParams()
  if (HasTransf())
    Location().Transforms(tempXYZ);
  gp_Pnt Pole(tempXYZ);
  return Pole;
}

double IGESGeom_BSplineSurface::UMin() const
{
  return theUmin;
}

double IGESGeom_BSplineSurface::UMax() const
{
  return theUmax;
}

double IGESGeom_BSplineSurface::VMin() const
{
  return theVmin;
}

double IGESGeom_BSplineSurface::VMax() const
{
  return theVmax;
}
