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

#include <IGESGeom_SplineCurve.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGeom_SplineCurve, IGESData_IGESEntity)

IGESGeom_SplineCurve::IGESGeom_SplineCurve() {}

void IGESGeom_SplineCurve::Init(const int                                       aType,
                                const int                                       aDegree,
                                const int                                       nbDimensions,
                                const occ::handle<NCollection_HArray1<double>>& allBreakPoints,
                                const occ::handle<NCollection_HArray2<double>>& allXPolynomials,
                                const occ::handle<NCollection_HArray2<double>>& allYPolynomials,
                                const occ::handle<NCollection_HArray2<double>>& allZPolynomials,
                                const occ::handle<NCollection_HArray1<double>>& allXvalues,
                                const occ::handle<NCollection_HArray1<double>>& allYvalues,
                                const occ::handle<NCollection_HArray1<double>>& allZvalues)
{
  int len = allXPolynomials->ColLength();
  if ((len != allYPolynomials->ColLength()) || (len != allZPolynomials->ColLength()))
    throw Standard_DimensionMismatch("IGESGeom_SplineCurve : Column Length of HArray2s in Init");
  if (allBreakPoints->Lower() != 1 || allXvalues->Lower() != 1 || allYvalues->Lower() != 1
      || allZvalues->Lower() != 1 || allXPolynomials->LowerCol() != 1
      || allXPolynomials->LowerRow() != 1 || allYPolynomials->LowerCol() != 1
      || allYPolynomials->LowerRow() != 1 || allZPolynomials->LowerCol() != 1
      || allZPolynomials->LowerRow() != 1)
    throw Standard_DimensionMismatch("IGESGeom_SplineCurve : Lower Indices of HArrays in Init");
  len = allXPolynomials->RowLength();
  if ((len != allYPolynomials->RowLength()) || (len != allZPolynomials->RowLength()))
    throw Standard_DimensionMismatch("IGESGeom_SplineCurve : Row Length of HArray2s in Init");

  len = allXvalues->Length();
  if ((len != allYvalues->Length()) || (len != allZvalues->Length()))
    throw Standard_DimensionMismatch("IGESGeom_SplineCurve : Length of HArray1s in Init");

  theType              = aType;
  theDegree            = aDegree;
  theNbDimensions      = nbDimensions;
  theBreakPoints       = allBreakPoints;
  theXCoordsPolynomial = allXPolynomials;
  theYCoordsPolynomial = allYPolynomials;
  theZCoordsPolynomial = allZPolynomials;
  theXvalues           = allXvalues;
  theYvalues           = allYvalues;
  theZvalues           = allZvalues;
  InitTypeAndForm(112, 0);
}

int IGESGeom_SplineCurve::SplineType() const
{
  return theType;
}

int IGESGeom_SplineCurve::Degree() const
{
  return theDegree;
}

int IGESGeom_SplineCurve::NbDimensions() const
{
  return theNbDimensions;
}

int IGESGeom_SplineCurve::NbSegments() const
{
  if (theBreakPoints.IsNull())
    return 0;
  else
    return ((theBreakPoints->Length()) - 1);
}

double IGESGeom_SplineCurve::BreakPoint(const int Index) const
{
  return theBreakPoints->Value(Index);
}

void IGESGeom_SplineCurve::XCoordPolynomial(const int Index,
                                            double&   AX,
                                            double&   BX,
                                            double&   CX,
                                            double&   DX) const
{
  AX = theXCoordsPolynomial->Value(Index, 1);
  BX = theXCoordsPolynomial->Value(Index, 2);
  CX = theXCoordsPolynomial->Value(Index, 3);
  DX = theXCoordsPolynomial->Value(Index, 4);
}

void IGESGeom_SplineCurve::YCoordPolynomial(const int Index,
                                            double&   AY,
                                            double&   BY,
                                            double&   CY,
                                            double&   DY) const
{
  AY = theYCoordsPolynomial->Value(Index, 1);
  BY = theYCoordsPolynomial->Value(Index, 2);
  CY = theYCoordsPolynomial->Value(Index, 3);
  DY = theYCoordsPolynomial->Value(Index, 4);
}

void IGESGeom_SplineCurve::ZCoordPolynomial(const int Index,
                                            double&   AZ,
                                            double&   BZ,
                                            double&   CZ,
                                            double&   DZ) const
{
  AZ = theZCoordsPolynomial->Value(Index, 1);
  BZ = theZCoordsPolynomial->Value(Index, 2);
  CZ = theZCoordsPolynomial->Value(Index, 3);
  DZ = theZCoordsPolynomial->Value(Index, 4);
}

void IGESGeom_SplineCurve::XValues(double& TPX0, double& TPX1, double& TPX2, double& TPX3) const
{
  TPX0 = theXvalues->Value(1);
  TPX1 = theXvalues->Value(2);
  TPX2 = theXvalues->Value(3);
  TPX3 = theXvalues->Value(4);
}

void IGESGeom_SplineCurve::YValues(double& TPY0, double& TPY1, double& TPY2, double& TPY3) const
{
  TPY0 = theYvalues->Value(1);
  TPY1 = theYvalues->Value(2);
  TPY2 = theYvalues->Value(3);
  TPY3 = theYvalues->Value(4);
}

void IGESGeom_SplineCurve::ZValues(double& TPZ0, double& TPZ1, double& TPZ2, double& TPZ3) const
{
  TPZ0 = theZvalues->Value(1);
  TPZ1 = theZvalues->Value(2);
  TPZ2 = theZvalues->Value(3);
  TPZ3 = theZvalues->Value(4);
}
