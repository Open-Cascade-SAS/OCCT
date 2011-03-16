// File:	Plate_LinearScalarConstraint.cxx
// Created:	Thu Mar 26 14:39:21 1998
// Author:	# Andre LIEUTIER
//		<alr@sgi63>


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

