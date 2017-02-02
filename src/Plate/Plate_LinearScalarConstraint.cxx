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


#include <gp_XYZ.hxx>
#include <Plate_LinearScalarConstraint.hxx>
#include <Plate_PinpointConstraint.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>

Plate_LinearScalarConstraint::Plate_LinearScalarConstraint() {}

Plate_LinearScalarConstraint::Plate_LinearScalarConstraint
   (const Plate_PinpointConstraint& PPC1,const gp_XYZ& coeff)

{
  myPPC = new Plate_HArray1OfPinpointConstraint(1,1);
  myCoef = new TColgp_HArray2OfXYZ(1,1,1,1);

  myPPC->ChangeValue(1) = PPC1;
  myCoef->ChangeValue(1,1) = coeff;
}

Plate_LinearScalarConstraint::Plate_LinearScalarConstraint(const Plate_Array1OfPinpointConstraint& thePPC,const TColgp_Array1OfXYZ& theCoeff)
{
  if(theCoeff.Length()!= thePPC.Length()) throw Standard_DimensionMismatch();
  myPPC = new Plate_HArray1OfPinpointConstraint(1, thePPC.Length());
  myCoef = new TColgp_HArray2OfXYZ(1,1,1, theCoeff.Length());

  myPPC->ChangeArray1() = thePPC;
  for(Standard_Integer i = 1; i<= theCoeff.Length(); i++) {
    myCoef->ChangeValue(1,i) = theCoeff(i+ theCoeff.Lower()-1);
  }
}

Plate_LinearScalarConstraint::Plate_LinearScalarConstraint
   (const Plate_Array1OfPinpointConstraint& thePPC,const TColgp_Array2OfXYZ& theCoeff)
{
  if(theCoeff.RowLength()!= thePPC.Length()) throw Standard_DimensionMismatch();
  myPPC = new Plate_HArray1OfPinpointConstraint(1, thePPC.Length());
  myCoef = new TColgp_HArray2OfXYZ(1, theCoeff.ColLength(),1, theCoeff.RowLength());
  
  myPPC->ChangeArray1() = thePPC;
  myCoef->ChangeArray2() = theCoeff;
}


Plate_LinearScalarConstraint::Plate_LinearScalarConstraint(const Standard_Integer ColLen,const Standard_Integer RowLen)
{
  myPPC = new Plate_HArray1OfPinpointConstraint(1,RowLen);
  myCoef = new TColgp_HArray2OfXYZ(1,ColLen,1,RowLen);
  myCoef->Init(gp_XYZ(0.,0.,0.));
}

 void Plate_LinearScalarConstraint::SetPPC(const Standard_Integer Index,const Plate_PinpointConstraint& Value) 
{
  myPPC->ChangeValue(Index) = Value;
}

 void Plate_LinearScalarConstraint::SetCoeff(const Standard_Integer Row,const Standard_Integer Col,const gp_XYZ& Value) 
{
  myCoef->ChangeValue(Row,Col) = Value;
}

