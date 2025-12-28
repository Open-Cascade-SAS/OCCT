// Created on: 1998-03-26
// Created by: # Andre LIEUTIER
// Copyright (c) 1998-1999 Matra Datavision
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

#include <Plate_LinearXYZConstraint.hxx>
#include <Plate_PinpointConstraint.hxx>
#include <Standard_DimensionMismatch.hxx>

Plate_LinearXYZConstraint::Plate_LinearXYZConstraint() = default;

Plate_LinearXYZConstraint::Plate_LinearXYZConstraint(
  const NCollection_Array1<Plate_PinpointConstraint>& thePPC,
  const NCollection_Array1<double>&                   theCoeff)
{
  if (theCoeff.Length() != thePPC.Length())
    throw Standard_DimensionMismatch();
  myPPC  = new NCollection_HArray1<Plate_PinpointConstraint>(1, thePPC.Length());
  myCoef = new NCollection_HArray2<double>(1, 1, 1, theCoeff.Length());

  myPPC->ChangeArray1() = thePPC;
  for (int i = 1; i <= theCoeff.Length(); i++)
    myCoef->ChangeValue(1, i) = theCoeff(i + theCoeff.Lower() - 1);
}

Plate_LinearXYZConstraint::Plate_LinearXYZConstraint(
  const NCollection_Array1<Plate_PinpointConstraint>& thePPC,
  const NCollection_Array2<double>&                   theCoeff)
{
  if (theCoeff.RowLength() != thePPC.Length())
    throw Standard_DimensionMismatch();
  myPPC  = new NCollection_HArray1<Plate_PinpointConstraint>(1, thePPC.Length());
  myCoef = new NCollection_HArray2<double>(1, theCoeff.ColLength(), 1, theCoeff.RowLength());

  myPPC->ChangeArray1()  = thePPC;
  myCoef->ChangeArray2() = theCoeff;
}

Plate_LinearXYZConstraint::Plate_LinearXYZConstraint(const int ColLen, const int RowLen)
{
  myPPC  = new NCollection_HArray1<Plate_PinpointConstraint>(1, RowLen);
  myCoef = new NCollection_HArray2<double>(1, ColLen, 1, RowLen);
  myCoef->Init(0.0);
}

void Plate_LinearXYZConstraint::SetPPC(const int Index, const Plate_PinpointConstraint& Value)
{
  myPPC->ChangeValue(Index) = Value;
}

void Plate_LinearXYZConstraint::SetCoeff(const int Row, const int Col, const double Value)
{
  myCoef->ChangeValue(Row, Col) = Value;
}
