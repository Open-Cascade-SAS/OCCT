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

#include <IGESGeom_SplineSurface.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGeom_SplineSurface, IGESData_IGESEntity)

IGESGeom_SplineSurface::IGESGeom_SplineSurface() {}

void IGESGeom_SplineSurface::Init(
  const int                                                                         aBoundaryType,
  const int                                                                         aPatchType,
  const occ::handle<NCollection_HArray1<double>>&                                   allUBreakPoints,
  const occ::handle<NCollection_HArray1<double>>&                                   allVBreakPoints,
  const occ::handle<NCollection_HArray2<occ::handle<NCollection_HArray1<double>>>>& allXCoeffs,
  const occ::handle<NCollection_HArray2<occ::handle<NCollection_HArray1<double>>>>& allYCoeffs,
  const occ::handle<NCollection_HArray2<occ::handle<NCollection_HArray1<double>>>>& allZCoeffs)
{
  int i, j;
  if (allUBreakPoints->Lower() != 1 || allVBreakPoints->Lower() != 1)
    throw Standard_DimensionMismatch(
      "IGESGeom_SplineSurface: Lower Indices of BreakPoints in Init");

  int nbUSegs = allUBreakPoints->Length() - 1;
  int nbVSegs = allVBreakPoints->Length() - 1;

  int len = allXCoeffs->RowLength();
  if ((len != allYCoeffs->RowLength()) || (len != allZCoeffs->RowLength()))
    throw Standard_DimensionMismatch("IGESGeom_SplineSurface: Row Length of HArray2s in Init");
  if (allXCoeffs->LowerCol() != 1 || allXCoeffs->LowerRow() != 1 || allYCoeffs->LowerCol() != 1
      || allYCoeffs->LowerRow() != 1 || allZCoeffs->LowerCol() != 1 || allZCoeffs->LowerRow() != 1)
    throw Standard_DimensionMismatch(
      "IGESGeom_SplineSurface: Lower Col-Row Indices of HArray2s in Init");

  len = allXCoeffs->ColLength();
  if ((len != allYCoeffs->ColLength()) || (len != allZCoeffs->ColLength()))
    throw Standard_DimensionMismatch("IGESGeom_SplineSurface: Column Length of HArray2s in Init");

  occ::handle<NCollection_HArray1<double>> temp1;
  occ::handle<NCollection_HArray1<double>> temp2;
  occ::handle<NCollection_HArray1<double>> temp3;
  for (i = 1; i <= nbUSegs; i++)
    for (j = 1; j <= nbVSegs; j++)
    {
      temp1 = allXCoeffs->Value(i, j);
      temp2 = allYCoeffs->Value(i, j);
      temp3 = allZCoeffs->Value(i, j);
      if ((temp1.IsNull() || temp1->Length() != 16) || (temp2.IsNull() || temp2->Length() != 16)
          || (temp3.IsNull() || temp3->Length() != 16))
        throw Standard_DimensionMismatch(
          "IGESGeom_SplineSurface: Lengths of elements of HArray2s in Init");
    }

  theBoundaryType = aBoundaryType;
  thePatchType    = aPatchType;
  theUBreakPoints = allUBreakPoints;
  theVBreakPoints = allVBreakPoints;
  theXCoeffs      = allXCoeffs;
  theYCoeffs      = allYCoeffs;
  theZCoeffs      = allZCoeffs;
  InitTypeAndForm(114, 0);
}

int IGESGeom_SplineSurface::NbUSegments() const
{
  return (theUBreakPoints->Length() - 1);
}

int IGESGeom_SplineSurface::NbVSegments() const
{
  return (theVBreakPoints->Length() - 1);
}

int IGESGeom_SplineSurface::BoundaryType() const
{
  return theBoundaryType;
}

int IGESGeom_SplineSurface::PatchType() const
{
  return thePatchType;
}

double IGESGeom_SplineSurface::UBreakPoint(const int Index) const
{
  return theUBreakPoints->Value(Index);
}

double IGESGeom_SplineSurface::VBreakPoint(const int Index) const
{
  return theVBreakPoints->Value(Index);
}

occ::handle<NCollection_HArray1<double>> IGESGeom_SplineSurface::XPolynomial(const int Index1,
                                                                             const int Index2) const
{
  return (theXCoeffs->Value(Index1, Index2));
}

occ::handle<NCollection_HArray1<double>> IGESGeom_SplineSurface::YPolynomial(const int Index1,
                                                                             const int Index2) const
{
  return (theYCoeffs->Value(Index1, Index2));
}

occ::handle<NCollection_HArray1<double>> IGESGeom_SplineSurface::ZPolynomial(const int Index1,
                                                                             const int Index2) const
{
  return (theZCoeffs->Value(Index1, Index2));
}

void IGESGeom_SplineSurface::Polynomials(
  occ::handle<NCollection_HArray2<occ::handle<NCollection_HArray1<double>>>>& allXCoeffs,
  occ::handle<NCollection_HArray2<occ::handle<NCollection_HArray1<double>>>>& allYCoeffs,
  occ::handle<NCollection_HArray2<occ::handle<NCollection_HArray1<double>>>>& allZCoeffs) const
{
  allXCoeffs = theXCoeffs;
  allYCoeffs = theYCoeffs;
  allZCoeffs = theZCoeffs;
}
