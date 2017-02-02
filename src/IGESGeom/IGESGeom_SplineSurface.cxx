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
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGeom_SplineSurface,IGESData_IGESEntity)

IGESGeom_SplineSurface::IGESGeom_SplineSurface ()    {  }


    void IGESGeom_SplineSurface::Init
  (const Standard_Integer aBoundaryType, 
   const Standard_Integer aPatchType,
   const Handle(TColStd_HArray1OfReal)& allUBreakPoints,
   const Handle(TColStd_HArray1OfReal)& allVBreakPoints,
   const Handle(IGESBasic_HArray2OfHArray1OfReal)& allXCoeffs,
   const Handle(IGESBasic_HArray2OfHArray1OfReal)& allYCoeffs,
   const Handle(IGESBasic_HArray2OfHArray1OfReal)& allZCoeffs)
{
  Standard_Integer i,j;
  if (allUBreakPoints->Lower() != 1 || allVBreakPoints->Lower() != 1)
    throw Standard_DimensionMismatch("IGESGeom_SplineSurface: Lower Indices of BreakPoints in Init");

  Standard_Integer nbUSegs = allUBreakPoints->Length() - 1;
  Standard_Integer nbVSegs = allVBreakPoints->Length() - 1;

  Standard_Integer len = allXCoeffs->RowLength();
  if ((len != allYCoeffs->RowLength()) || (len != allZCoeffs->RowLength()))
    throw Standard_DimensionMismatch("IGESGeom_SplineSurface: Row Length of HArray2s in Init");
  if (allXCoeffs->LowerCol() != 1 || allXCoeffs->LowerRow() != 1 ||
      allYCoeffs->LowerCol() != 1 || allYCoeffs->LowerRow() != 1 ||
      allZCoeffs->LowerCol() != 1 || allZCoeffs->LowerRow() != 1 )
    throw Standard_DimensionMismatch("IGESGeom_SplineSurface: Lower Col-Row Indices of HArray2s in Init");

  len = allXCoeffs->ColLength();
  if ((len != allYCoeffs->ColLength()) || (len != allZCoeffs->ColLength()))
    throw Standard_DimensionMismatch("IGESGeom_SplineSurface: Column Length of HArray2s in Init");

  Handle(TColStd_HArray1OfReal) temp1;
  Handle(TColStd_HArray1OfReal) temp2;
  Handle(TColStd_HArray1OfReal) temp3;
  for (i = 1; i <= nbUSegs; i++)
    for (j = 1; j <= nbVSegs; j++)
      {
        temp1 = allXCoeffs->Value(i,j);
        temp2 = allYCoeffs->Value(i,j);
        temp3 = allZCoeffs->Value(i,j);
        if ((temp1.IsNull() || temp1->Length() != 16) || 
            (temp2.IsNull() || temp2->Length() != 16) || 
            (temp3.IsNull() || temp3->Length() != 16))
	  throw Standard_DimensionMismatch("IGESGeom_SplineSurface: Lengths of elements of HArray2s in Init");
      }


  theBoundaryType = aBoundaryType;
  thePatchType = aPatchType;
  theUBreakPoints = allUBreakPoints;
  theVBreakPoints = allVBreakPoints;
  theXCoeffs = allXCoeffs;
  theYCoeffs = allYCoeffs;
  theZCoeffs = allZCoeffs;
  InitTypeAndForm(114,0);
}

    Standard_Integer IGESGeom_SplineSurface::NbUSegments () const
{
  return (theUBreakPoints->Length() - 1);
}

    Standard_Integer IGESGeom_SplineSurface::NbVSegments () const
{
  return (theVBreakPoints->Length() - 1);
}

    Standard_Integer IGESGeom_SplineSurface::BoundaryType () const
{
  return theBoundaryType;
}

    Standard_Integer IGESGeom_SplineSurface::PatchType () const
{
  return thePatchType;
}

    Standard_Real IGESGeom_SplineSurface::UBreakPoint
  (const Standard_Integer Index) const
{
  return theUBreakPoints->Value(Index);
}

    Standard_Real IGESGeom_SplineSurface::VBreakPoint
  (const Standard_Integer Index) const
{
  return theVBreakPoints->Value(Index);
}

    Handle(TColStd_HArray1OfReal) IGESGeom_SplineSurface::XPolynomial
  (const Standard_Integer Index1, const Standard_Integer Index2) const
{
  return(theXCoeffs->Value(Index1,Index2));
}

    Handle(TColStd_HArray1OfReal) IGESGeom_SplineSurface::YPolynomial
  (const Standard_Integer Index1, const Standard_Integer Index2) const
{
  return(theYCoeffs->Value(Index1,Index2));
}

    Handle(TColStd_HArray1OfReal) IGESGeom_SplineSurface::ZPolynomial
  (const Standard_Integer Index1, const Standard_Integer Index2) const
{
  return(theZCoeffs->Value(Index1,Index2));
}

    void  IGESGeom_SplineSurface::Polynomials
  (Handle(IGESBasic_HArray2OfHArray1OfReal)& allXCoeffs,
   Handle(IGESBasic_HArray2OfHArray1OfReal)& allYCoeffs,
   Handle(IGESBasic_HArray2OfHArray1OfReal)& allZCoeffs) const
{
  allXCoeffs = theXCoeffs;
  allYCoeffs = theYCoeffs;
  allZCoeffs = theZCoeffs;
}
