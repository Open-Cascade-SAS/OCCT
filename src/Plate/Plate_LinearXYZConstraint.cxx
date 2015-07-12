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
#include <Standard_OutOfRange.hxx>

Plate_LinearXYZConstraint::Plate_LinearXYZConstraint() {}

Plate_LinearXYZConstraint::Plate_LinearXYZConstraint(const Plate_Array1OfPinpointConstraint& PPC,const TColStd_Array1OfReal& coeff)
{
  if(coeff.Length()!= PPC.Length()) Standard_DimensionMismatch::Raise();
  myPPC = new Plate_HArray1OfPinpointConstraint(1,PPC.Length());
  myCoef = new TColStd_HArray2OfReal(1,1,1,coeff.Length());
  
  myPPC->ChangeArray1() = PPC;
  for(Standard_Integer i = 1; i<= coeff.Length(); i++)
    myCoef->ChangeValue(1,i) = coeff(i+coeff.Lower()-1);
}

Plate_LinearXYZConstraint::Plate_LinearXYZConstraint(const Plate_Array1OfPinpointConstraint& PPC,const TColStd_Array2OfReal& coeff)
{
  if(coeff.RowLength()!= PPC.Length()) Standard_DimensionMismatch::Raise();
  myPPC = new Plate_HArray1OfPinpointConstraint(1,PPC.Length());
  myCoef = new TColStd_HArray2OfReal(1,coeff.ColLength(),1,coeff.RowLength());
  
  myPPC->ChangeArray1() = PPC;
  myCoef->ChangeArray2() = coeff;
}


Plate_LinearXYZConstraint::Plate_LinearXYZConstraint(const Standard_Integer ColLen,const Standard_Integer RowLen)
{
  myPPC = new Plate_HArray1OfPinpointConstraint(1,RowLen);
  myCoef = new TColStd_HArray2OfReal(1,ColLen,1,RowLen);
  myCoef->Init(0.0);
}

void Plate_LinearXYZConstraint::SetPPC(const Standard_Integer Index,const Plate_PinpointConstraint& Value) 
{
  myPPC->ChangeValue(Index) = Value;
}

void Plate_LinearXYZConstraint::SetCoeff(const Standard_Integer Row,const Standard_Integer Col,const Standard_Real Value) 
{
  myCoef->ChangeValue(Row,Col) = Value;
}

