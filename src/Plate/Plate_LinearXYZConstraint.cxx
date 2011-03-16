// File:	Plate_LinearXYZConstraint.cxx
// Created:	Thu Mar 26 15:21:12 1998
// Author:	# Andre LIEUTIER
//		<alr@sgi63>


#include <Plate_LinearXYZConstraint.ixx>


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

