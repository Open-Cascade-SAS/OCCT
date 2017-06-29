// Copyright (c) 1997-1999 Matra Datavision
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


#include <math.hxx>
#include <math_Vector.hxx>
#include <PLib.hxx>
#include <PLib_JacobiPolynomial.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Type.hxx>
#include <TColStd_Array2OfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(PLib_JacobiPolynomial,PLib_Base)

#include "PLib_JacobiPolynomial_Data.pxx"

// The possible values for NbGaussPoints
const Standard_Integer NDEG8=8,   NDEG10=10, NDEG15=15, NDEG20=20, NDEG25=25, 
                       NDEG30=30, NDEG40=40, NDEG50=50, NDEG61=61;

const Standard_Integer UNDEFINED=-999;

//=======================================================================
//function : PLib_JacobiPolynomial
//purpose  : 
//=======================================================================

 PLib_JacobiPolynomial::PLib_JacobiPolynomial (const Standard_Integer WorkDegree, 
                                               const GeomAbs_Shape ConstraintOrder)
{
  myWorkDegree = WorkDegree;
    
  switch (ConstraintOrder) {
    case GeomAbs_C0: myNivConstr = 0; break;
    case GeomAbs_C1: myNivConstr = 1; break;
    case GeomAbs_C2: myNivConstr = 2; break;
    default: 
      throw Standard_ConstructionError("Invalid ConstraintOrder");
  }
  myDegree = myWorkDegree - 2*(myNivConstr+1);
  if (myDegree > 30)
    throw Standard_ConstructionError("Invalid Degree");
}

//=======================================================================
//function : Points
//purpose  : 
//=======================================================================

void PLib_JacobiPolynomial::Points(const Standard_Integer NbGaussPoints, 
                                   TColStd_Array1OfReal& TabPoints) const 
{
  if ((NbGaussPoints != NDEG8  && NbGaussPoints != NDEG10 &&  
      NbGaussPoints != NDEG15 && NbGaussPoints != NDEG20 && 
      NbGaussPoints != NDEG25 && NbGaussPoints != NDEG30 && 
      NbGaussPoints != NDEG40 && NbGaussPoints != NDEG50 && 
      NbGaussPoints != NDEG61) || 
      NbGaussPoints <= myDegree)
    throw Standard_ConstructionError("Invalid NbGaussPoints");

  math_Vector DecreasingPoints(1,NbGaussPoints);

  math::GaussPoints(NbGaussPoints,DecreasingPoints);

// TabPoints consist of only positive increasing values
  for (Standard_Integer i=1; i<=NbGaussPoints/2; i++) 
    TabPoints(i) = DecreasingPoints(NbGaussPoints/2-i+1);
  if (NbGaussPoints % 2 == 1)
    TabPoints(0) = 0.;
  else
    TabPoints(0) = UNDEFINED;
}

//=======================================================================
//function : Weights
//purpose  : 
//=======================================================================

void PLib_JacobiPolynomial::Weights(const Standard_Integer NbGaussPoints, 
                                       TColStd_Array2OfReal& TabWeights) const 
{

  Standard_Integer i,j;
  Standard_Real const *pdb=NULL;     // the current pointer to WeightsDB
  switch (myNivConstr) {
    case 0: pdb = WeightsDB_C0; break;
    case 1: pdb = WeightsDB_C1; break;
    case 2: pdb = WeightsDB_C2; break;
  }
  Standard_Integer infdg = 2*(myNivConstr+1);
  if (NbGaussPoints > NDEG8)  pdb += (NDEG8 *(NDEG8 -infdg)/2);
  if (NbGaussPoints > NDEG10) pdb += (NDEG10*(NDEG10-infdg)/2);
  if (NbGaussPoints > NDEG15) pdb += (((NDEG15-1)/2)*(NDEG15-infdg));
  if (NbGaussPoints > NDEG20) pdb += (NDEG20*(NDEG20-infdg)/2);
  if (NbGaussPoints > NDEG25) pdb += (((NDEG25-1)/2)*(NDEG25-infdg));
  if (NbGaussPoints > NDEG30) pdb += (NDEG30*(NDEG30-infdg)/2);
  if (NbGaussPoints > NDEG40) pdb += (NDEG40*(NDEG40-infdg)/2);
  if (NbGaussPoints > NDEG50) pdb += (NDEG50*(NDEG50-infdg)/2);
 
// the copy of TabWeightsDB into TabWeights
  for (j=0; j<=myDegree; j++) {
    for (i=1; i<=NbGaussPoints/2; i++) {
      TabWeights.SetValue(i,j,*pdb++);
    }
  } 

  if (NbGaussPoints % 2 == 1) {
    // NbGaussPoints is odd - the values addition for 0.
    Standard_Real const *pdb0=NULL;  // the current pointer to WeightsDB0
    switch (myNivConstr) {
      case 0: pdb0 = WeightsDB0_C0; break;
      case 1: pdb0 = WeightsDB0_C1; break;
      case 2: pdb0 = WeightsDB0_C2; break;
    }

    if (NbGaussPoints > NDEG15) pdb0 += ((NDEG15-1-infdg)/2 + 1);
    if (NbGaussPoints > NDEG25) pdb0 += ((NDEG25-1-infdg)/2 + 1);

    // the copy of TabWeightsDB0 into TabWeights
    for (j=0; j<=myDegree; j+=2) 
      TabWeights.SetValue(0,j,*pdb0++);
    for (j=1; j<=myDegree; j+=2) 
      TabWeights.SetValue(0,j,0.);
  }
  else {
    for (j=0; j<=myDegree; j++) {
      TabWeights.SetValue(0,j,UNDEFINED);
    }
  }
}

//=======================================================================
//function : MaxValue
//purpose  : 
//=======================================================================

void PLib_JacobiPolynomial::MaxValue(TColStd_Array1OfReal& TabMax) const 
{
  Standard_Real const *pdb=NULL;  // the pointer to MaxValues
  switch (myNivConstr) {
      case 0: pdb = MaxValuesDB_C0; break;
      case 1: pdb = MaxValuesDB_C1; break;
      case 2: pdb = MaxValuesDB_C2; break;
    }
  for (Standard_Integer i=TabMax.Lower(); i <= TabMax.Upper(); i++) {
    TabMax.SetValue(i,*pdb++);
  }
}

//=======================================================================
//function : MaxError
//purpose  : 
//=======================================================================

Standard_Real PLib_JacobiPolynomial::MaxError(const Standard_Integer Dimension,
                                                 Standard_Real& JacCoeff, 
                                                 const Standard_Integer NewDegree) const 
{
  Standard_Integer i,idim,ibeg,icut;

  math_Vector MaxErrDim(1,Dimension,0.);

  TColStd_Array1OfReal TabMax(0, myDegree+1);
  MaxValue(TabMax);

  ibeg = 2*(myNivConstr+1);
  icut = Max (ibeg, NewDegree+1);
  Standard_Real * JacArray = &JacCoeff;
  for (idim=1; idim<=Dimension; idim++) {
    for (i=icut; i<=myWorkDegree; i++) {
      MaxErrDim(idim) += Abs(JacArray[i*Dimension+idim-1]) * TabMax(i-ibeg);
    }
  }
  Standard_Real MaxErr = MaxErrDim.Norm();
  return (MaxErr);
}

//=======================================================================
//function : ReduceDegree
//purpose  : 
//=======================================================================

void PLib_JacobiPolynomial::ReduceDegree(const Standard_Integer Dimension,
                                            const Standard_Integer MaxDegree,
                                            const Standard_Real Tol,
                                            Standard_Real& JacCoeff,
                                            Standard_Integer& NewDegree,
                                            Standard_Real& MaxError) const
{
  Standard_Integer i,idim,icut, ia = 2*(myNivConstr+1)-1;
  Standard_Real Bid,Eps1,Error;

  math_Vector MaxErrDim(1,Dimension,0.);

  NewDegree = ia;
  MaxError = 0.;
  Error = 0.;
  icut=ia+1;

  TColStd_Array1OfReal TabMax(0, myDegree+1);
  MaxValue(TabMax);
  Standard_Real * JacArray = &JacCoeff;
  for (i=myWorkDegree; i>=icut; i--) {
    for (idim=1; idim<=Dimension; idim++) {
      MaxErrDim(idim) += Abs(JacArray[i*Dimension+idim-1]) * TabMax(i-icut);
    }
    Error = MaxErrDim.Norm();
    if (Error > Tol && i <= MaxDegree) {
      NewDegree = i;
      break;
    }
    else
      MaxError = Error;
  }
  if (NewDegree==ia) {
    Eps1=0.000000001;
    NewDegree = 0;
    for (i=ia; i>=1; i--) {
      Bid = 0.;
      for (idim=1; idim<=Dimension; idim++) {
        Bid += Abs(JacArray[i*Dimension+idim-1]);
      }
      if (Bid > Eps1) {
        NewDegree = i; 
        break;
      }
    }
  }
}

//=======================================================================
//function : AverageError
//purpose  : 
//=======================================================================

Standard_Real PLib_JacobiPolynomial::AverageError(const Standard_Integer Dimension,
                                                     Standard_Real& JacCoeff,
                                                     const Standard_Integer NewDegree) 
                                                     const
{
  Standard_Integer i,idim, icut = Max (2*(myNivConstr+1)+1, NewDegree+1);
  Standard_Real BidJ, AverageErr = 0.;
  Standard_Real * JacArray = &JacCoeff;
  for (idim=1; idim<=Dimension; idim++) {
    for (i=icut; i<=myDegree; i++) {
      BidJ = JacArray[i*Dimension+idim-1];
      AverageErr += BidJ*BidJ;
    }
  }
  AverageErr = sqrt(AverageErr/2);
  return (AverageErr);
}

//=======================================================================
//function :ToCoefficients
//purpose  : 
//=======================================================================

void PLib_JacobiPolynomial::ToCoefficients(const Standard_Integer Dimension,
					   const Standard_Integer Degree,
					   const TColStd_Array1OfReal& JacCoeff,
					   TColStd_Array1OfReal& Coefficients) const
{
  const Standard_Integer MAXM=31;
  Standard_Integer i,iptt,j,idim, ii, jj;
  Standard_Real const *pTr=NULL;  // the pointer to TransMatrix
  Standard_Real Bid;
  Standard_Integer ibegJC=JacCoeff.Lower(), ibegC=Coefficients.Lower();

  switch (myNivConstr) {
    case 0: pTr = &TransMatrix_C0[0][0]; break;
    case 1: pTr = &TransMatrix_C1[0][0]; break;
    case 2: pTr = &TransMatrix_C2[0][0]; break;
  }
// the conversation for even elements of JacCoeff
  for (i=0; i<=Degree/2; i++) {
    iptt = i*MAXM-(i+1)*i/2;
    for (idim=1; idim<=Dimension; idim++) {
      Bid = 0.;
      for (j=i; j<=Degree/2; j++) {
        Bid += (*(pTr+iptt+j)) * JacCoeff(2*j*Dimension+idim-1);
      }
      Coefficients.SetValue(2*i*Dimension+idim-1, Bid);
    }
  }
  
  if (Degree == 0) return;

// the conversation for odd elements of JacCoeff
  pTr += MAXM*(MAXM+1)/2;
  for (i=0; i<=(Degree-1)/2; i++) {
    iptt = i*MAXM-(i+1)*i/2;
    ii = ibegC+(2*i+1)*Dimension;
    for (idim=1; idim<=Dimension; idim++, ii++) {
      Bid = 0.;
      jj = ibegJC+(2*i+1)*Dimension+idim-1;
      for (j=i; j<=(Degree-1)/2; j++, jj+=2*Dimension) {
        Bid += (*(pTr+iptt+j)) * JacCoeff(jj);
      }
      Coefficients(ii) = Bid;
    }
  }
}

//=======================================================================
//function : D0123
//purpose  : common part of D0,D1,D2,D3 (FORTRAN subroutine MPOJAC)
//=======================================================================

void PLib_JacobiPolynomial::D0123(const Standard_Integer NDeriv,
				  const Standard_Real U,
				  TColStd_Array1OfReal& BasisValue,
				  TColStd_Array1OfReal& BasisD1,
				  TColStd_Array1OfReal& BasisD2,
				  TColStd_Array1OfReal& BasisD3)
{
  Standard_Integer i,j, HermitNivConstr = 2*(myNivConstr+1);
  Standard_Real Aux1,Aux2;

  if (myTNorm.IsNull()) {

    // Inizialization of myTNorm,myCofA,myCofB,myDenom

    myTNorm = new TColStd_HArray1OfReal(0,myDegree);
    for (i=0; i<=myDegree; i++) {
      Aux2 = 1.;
      for (j=1; j<=HermitNivConstr; j++) {
	Aux2 *= ((Standard_Real)(i+HermitNivConstr+j)/(Standard_Real)(i+j));
      }
      myTNorm->SetValue(i, Sqrt (Aux2 * (2*i+2*HermitNivConstr+1) / 
                                (Pow (2,2*HermitNivConstr+1))));
    }

    if(myDegree >= 2) {
      myCofA  = new TColStd_HArray1OfReal(0,myDegree);
      myCofB  = new TColStd_HArray1OfReal(0,myDegree);
      myDenom = new TColStd_HArray1OfReal(0,myDegree);
      for (i=2; i<=myDegree; i++) {
	Aux1 = HermitNivConstr+i-1;
	Aux2 = 2 * Aux1;
	myCofA ->SetValue(i, Aux2*(Aux2+1)*(Aux2+2));
	myCofB ->SetValue(i, -2. *(Aux2+2) * Aux1* Aux1);
	myDenom->SetValue(i, 1./(2. * i * ( i-1 + 2*HermitNivConstr+1) * Aux2));
      }
    }
  }

//  --- Positionements triviaux -----
  Standard_Integer ibeg0 = BasisValue.Lower();
  Standard_Integer ibeg1 = BasisD1.Lower();
  Standard_Integer ibeg2 = BasisD2.Lower();
  Standard_Integer ibeg3 = BasisD3.Lower();
  Standard_Integer i0, i1, i2, i3;


  if (myDegree == 0) {
     BasisValue(ibeg0+0) = 1.;
     if (NDeriv >= 1) {
       BasisD1(ibeg1+0) = 0.;
       if (NDeriv >= 2) {
	 BasisD2(ibeg2+0) = 0.;
	 if (NDeriv == 3) 
	   BasisD3(ibeg3+0) = 0.;
       }
     }
  }
  else {
    BasisValue(ibeg0+0) = 1.;
    Aux1 = HermitNivConstr+1;
    BasisValue(ibeg0+1) = Aux1 * U;
    if (NDeriv >= 1) {
      BasisD1(ibeg1+0) = 0.;
      BasisD1(ibeg1+1) = Aux1;
      if (NDeriv >= 2) {
	BasisD2(ibeg2+0) = 0.;
	BasisD2(ibeg2+1) = 0.;
	if (NDeriv == 3) {
	  BasisD3(ibeg3+0) = 0.;
	  BasisD3(ibeg3+1) = 0.;
	}
      }
    }
  }


//  --- Positionement par reccurence
  if (myDegree > 1) {
    if (NDeriv == 0) {   
      Standard_Real * BV = &BasisValue(ibeg0);
      Standard_Real * CofA = &myCofA->ChangeValue(0);
      Standard_Real * CofB = &myCofB->ChangeValue(0);
      Standard_Real * Denom = &myDenom->ChangeValue(0);   
      for (i=2; i<=myDegree; i++) {
	BV[i]  = (CofA[i]*U*BV[i-1] + CofB[i]*BV[i-2])*Denom[i];
      }
    }

    else {
      Standard_Real CofA, CofB, Denom;
      for (i=2; i<=myDegree; i++) {
	i0=i+ibeg0; 
	i1=i+ibeg1;
	CofA = myCofA->Value(i);
	CofB = myCofB->Value(i);
	Denom = myDenom->Value(i);

	BasisValue(i0) = (CofA * U * BasisValue(i0-1) + 
			  CofB * BasisValue(i0-2)) * Denom;
	BasisD1(i1)    = (CofA * (U * BasisD1(i1-1)  + BasisValue(i0-1)) +
			  CofB * BasisD1(i1-2)) * Denom;
	if (NDeriv >= 2) {
	  i2=i+ibeg2;
	  BasisD2(i2) = ( CofA * (U*BasisD2(i2-1) + 2*BasisD1(i1-1)) + 
			 CofB*BasisD2(i2-2)) * Denom;
	  if (NDeriv == 3) {
	    i3=i+ibeg3;
	    BasisD3(i3) = (CofA * (U*BasisD3(i3-1) + 3*BasisD2(i2-1))   + 
			 CofB*BasisD3(i3-2)) * Denom;
	  }
	}
      }
    }
  }

// Normalization
  if (NDeriv == 0) {
    Standard_Real * BV = &BasisValue(ibeg0);
    Standard_Real * TNorm =  &myTNorm->ChangeValue(0);
    for (i=0; i<=myDegree; i++) 
      BV[i] *= TNorm[i];
  }
  else {
    Standard_Real TNorm;
    for (i=0; i<=myDegree; i++) {
      TNorm = myTNorm->Value(i);
      BasisValue(i+ibeg0) *= TNorm;
      BasisD1(i+ibeg1)    *= TNorm;
      if (NDeriv >= 2) {
	BasisD2(i+ibeg2) *= TNorm;
	if (NDeriv >= 3) BasisD3(i+ibeg3) *= TNorm;
      }
    }
  }
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void PLib_JacobiPolynomial::D0(const Standard_Real U,
			       TColStd_Array1OfReal& BasisValue) 
{
  D0123(0,U,BasisValue,BasisValue,BasisValue,BasisValue);
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void PLib_JacobiPolynomial::D1(const Standard_Real U,
			       TColStd_Array1OfReal& BasisValue,
			       TColStd_Array1OfReal& BasisD1) 
{
  D0123(1,U,BasisValue,BasisD1,BasisD1,BasisD1);
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void PLib_JacobiPolynomial::D2(const Standard_Real U,
			       TColStd_Array1OfReal& BasisValue,
			       TColStd_Array1OfReal& BasisD1,
			       TColStd_Array1OfReal& BasisD2) 
{
  D0123(2,U,BasisValue,BasisD1,BasisD2,BasisD2);
}

//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void PLib_JacobiPolynomial::D3(const Standard_Real U,
			       TColStd_Array1OfReal& BasisValue,
			       TColStd_Array1OfReal& BasisD1,
			       TColStd_Array1OfReal& BasisD2,
			       TColStd_Array1OfReal& BasisD3) 
{
  D0123(3,U,BasisValue,BasisD1,BasisD2,BasisD3);
}

