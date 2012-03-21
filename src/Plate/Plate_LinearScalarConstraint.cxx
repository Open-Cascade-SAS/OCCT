// Created on: 1998-03-26
// Created by: # Andre LIEUTIER
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <Plate_LinearScalarConstraint.ixx>
#include <Standard_DimensionMismatch.hxx>


Plate_LinearScalarConstraint::Plate_LinearScalarConstraint() {}

Plate_LinearScalarConstraint::Plate_LinearScalarConstraint
   (const Plate_PinpointConstraint& PPC1,const gp_XYZ& coeff)

{
  myPPC = new Plate_HArray1OfPinpointConstraint(1,1);
  myCoef = new TColgp_HArray2OfXYZ(1,1,1,1);

  myPPC->ChangeValue(1) = PPC1;
  myCoef->ChangeValue(1,1) = coeff;
}

Plate_LinearScalarConstraint::Plate_LinearScalarConstraint(const Plate_Array1OfPinpointConstraint& PPC,const TColgp_Array1OfXYZ& coeff)
{
  if(coeff.Length()!= PPC.Length()) Standard_DimensionMismatch::Raise();
  myPPC = new Plate_HArray1OfPinpointConstraint(1,PPC.Length());
  myCoef = new TColgp_HArray2OfXYZ(1,1,1,coeff.Length());

  myPPC->ChangeArray1() = PPC;
  for(Standard_Integer i = 1; i<= coeff.Length(); i++) {
    myCoef->ChangeValue(1,i) = coeff(i+coeff.Lower()-1);
  }
}

Plate_LinearScalarConstraint::Plate_LinearScalarConstraint
   (const Plate_Array1OfPinpointConstraint& PPC,const TColgp_Array2OfXYZ& coeff)
{
  if(coeff.RowLength()!= PPC.Length()) Standard_DimensionMismatch::Raise();
  myPPC = new Plate_HArray1OfPinpointConstraint(1,PPC.Length());
  myCoef = new TColgp_HArray2OfXYZ(1,coeff.ColLength(),1,coeff.RowLength());
  
  myPPC->ChangeArray1() = PPC;
  myCoef->ChangeArray2() = coeff;
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

