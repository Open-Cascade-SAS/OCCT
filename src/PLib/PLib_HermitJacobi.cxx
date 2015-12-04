// Created on: 1997-10-22
// Created by: Sergey SOKOLOV
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


#include <NCollection_LocalArray.hxx>
#include <PLib.hxx>
#include <PLib_HermitJacobi.hxx>
#include <PLib_JacobiPolynomial.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HArray1OfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(PLib_HermitJacobi,PLib_Base)

//=======================================================================
//function : PLib_HermitJacobi
//purpose  : 
//=======================================================================
PLib_HermitJacobi::PLib_HermitJacobi(const Standard_Integer WorkDegree,
				     const GeomAbs_Shape ConstraintOrder) :
                                     myH(1,2*(PLib::NivConstr(ConstraintOrder)+1),
					 1,2*(PLib::NivConstr(ConstraintOrder)+1)),
				     myWCoeff(1,2*(PLib::NivConstr(ConstraintOrder)+1)+1)
{
  Standard_Integer NivConstr = PLib::NivConstr(ConstraintOrder);
  PLib::HermiteCoefficients(-1.,1.,NivConstr,NivConstr,myH);

  myJacobi = new PLib_JacobiPolynomial (WorkDegree,ConstraintOrder);
  
  myWCoeff.Init(0.);
  myWCoeff(1) = 1.;
  switch(NivConstr) {
    case 0: myWCoeff(3) = -1.; break;
    case 1: myWCoeff(3) = -2.; myWCoeff(5) = 1.; break;
    case 2: myWCoeff(3) = -3.; myWCoeff(5) = 3.; myWCoeff(7) = -1.; break;
  }
}

//=======================================================================
//function : MaxError
//purpose  : 
//=======================================================================

Standard_Real PLib_HermitJacobi::MaxError(const Standard_Integer Dimension,
					  Standard_Real& HermJacCoeff,
					  const Standard_Integer NewDegree) const
{
  return myJacobi->MaxError(Dimension,HermJacCoeff,NewDegree);
}

//=======================================================================
//function : ReduceDegree
//purpose  : 
//=======================================================================

void PLib_HermitJacobi::ReduceDegree(const Standard_Integer Dimension,
				     const Standard_Integer MaxDegree,
				     const Standard_Real Tol,
				     Standard_Real& HermJacCoeff,
				     Standard_Integer& NewDegree,
				     Standard_Real& MaxError) const
{
  myJacobi->ReduceDegree(Dimension,MaxDegree,Tol,
			 HermJacCoeff,NewDegree,MaxError);
}

//=======================================================================
//function : AverageError
//purpose  : 
//=======================================================================

Standard_Real PLib_HermitJacobi::AverageError(const Standard_Integer Dimension,
					      Standard_Real& HermJacCoeff,
					      const Standard_Integer NewDegree) const
{
  return myJacobi->AverageError(Dimension,HermJacCoeff,NewDegree);
}

//=======================================================================
//function : ToCoefficients
//purpose  : 
//=======================================================================

void PLib_HermitJacobi::ToCoefficients(const Standard_Integer Dimension,
				       const Standard_Integer Degree,
				       const TColStd_Array1OfReal& HermJacCoeff,
				       TColStd_Array1OfReal& Coefficients) const
{
  Standard_Integer i,k,idim,i1,i2;
  Standard_Real h1, h2;
  Standard_Integer NivConstr  = this->NivConstr(),
                   DegreeH    = 2*NivConstr+1;
  Standard_Integer ibegHJC = HermJacCoeff.Lower(), kdim;

  TColStd_Array1OfReal AuxCoeff(0,(Degree+1)*Dimension-1);
  AuxCoeff.Init(0.);

  for (k=0; k<=DegreeH; k++) {
    kdim =  k*Dimension;
    for (i=0; i<=NivConstr; i++) {
      h1 =  myH(i+1, k+1);
      h2 =  myH(i+NivConstr+2,k+1);
      i1 =  ibegHJC + i*Dimension;
      i2 =  ibegHJC + (i+NivConstr+1)*Dimension;
      
      for (idim=0; idim<Dimension; idim++) {
	AuxCoeff(idim + kdim) += HermJacCoeff(i1 + idim) * h1 +
	                         HermJacCoeff(i2 + idim) * h2;
      }
    }
  }
  kdim = (Degree+1)*Dimension;
  for (k=(DegreeH+1)*Dimension; k<kdim; k++) {
    AuxCoeff(k) = HermJacCoeff(ibegHJC + k);
  }
  
  if(Degree > DegreeH) 
    myJacobi->ToCoefficients(Dimension,Degree,AuxCoeff,Coefficients);
  else {
    Standard_Integer ibegC = Coefficients.Lower();
    kdim = (Degree+1)*Dimension;    
    for(k=0; k < kdim; k++)
	Coefficients(ibegC+k) = AuxCoeff(k);
  }
}

//=======================================================================
//function : D0123
//purpose  : common part of D0,D1,D2,D3 (FORTRAN subroutine MPOBAS)
//=======================================================================

void PLib_HermitJacobi::D0123(const Standard_Integer NDeriv,
			      const Standard_Real U,
			      TColStd_Array1OfReal& BasisValue,
			      TColStd_Array1OfReal& BasisD1,
			      TColStd_Array1OfReal& BasisD2,
			      TColStd_Array1OfReal& BasisD3)
{
  NCollection_LocalArray<Standard_Real> jac0 (4 * 20);
  NCollection_LocalArray<Standard_Real> jac1 (4 * 20);
  NCollection_LocalArray<Standard_Real> jac2 (4 * 20);
  NCollection_LocalArray<Standard_Real> jac3 (4 * 20);
  NCollection_LocalArray<Standard_Real> wvalues (4);

  Standard_Integer i, j;
  Standard_Integer NivConstr  = this->NivConstr(),
                   WorkDegree = this->WorkDegree(),
                   DegreeH    = 2*NivConstr+1;
  Standard_Integer ibeg0 = BasisValue.Lower(), 
                   ibeg1 = BasisD1.Lower(),
                   ibeg2 = BasisD2.Lower(),
                   ibeg3 = BasisD3.Lower();
  Standard_Integer JacDegree = WorkDegree-DegreeH-1;
  Standard_Real W0;

  TColStd_Array1OfReal JacValue0(jac0[0], 0, Max(0,JacDegree)); 
  TColStd_Array1OfReal WValues(wvalues[0],0,NDeriv); 
  WValues.Init(0.);

// Evaluation des polynomes d'hermite
  math_Matrix HermitValues(0,DegreeH, 0, NDeriv, 0.);
  if(NDeriv == 0)
    for (i=0; i<=DegreeH; i++) {
      PLib::NoDerivativeEvalPolynomial(U,DegreeH,1, DegreeH,
				       myH(i+1,1), HermitValues(i,0));
    }
  else
    for (i=0; i<=DegreeH; i++) {
      PLib::EvalPolynomial(U,NDeriv,DegreeH,1,
			   myH(i+1,1), HermitValues(i,0));
    }

// Evaluation des polynomes de Jaccobi
  if(JacDegree >= 0) {

    switch (NDeriv) {
    case 0 :
      myJacobi->D0(U,JacValue0);
      break;
    case 1 :
      {
	TColStd_Array1OfReal JacValue1(jac1[0], 0, JacDegree);
	myJacobi->D1(U,JacValue0,JacValue1);
	break;
      }
    case 2 :
      {      
	TColStd_Array1OfReal JacValue1(jac1[0], 0, JacDegree);      
	TColStd_Array1OfReal JacValue2(jac2[0], 0, JacDegree);
	myJacobi->D2(U,JacValue0,JacValue1,JacValue2);      
	break;
      }
    case 3 :
      {
	TColStd_Array1OfReal JacValue1(jac1[0], 0, JacDegree);      
	TColStd_Array1OfReal JacValue2(jac2[0], 0, JacDegree);  
	TColStd_Array1OfReal JacValue3(jac3[0], 0, JacDegree);
	myJacobi->D3(U,JacValue0,JacValue1,JacValue2,JacValue3);
      }
    }

// Evaluation de W(t)
    if(NDeriv == 0)
      PLib::NoDerivativeEvalPolynomial(U,DegreeH+1,1,DegreeH+1,myWCoeff(1), WValues(0));
    else
      PLib::EvalPolynomial(U,NDeriv,DegreeH+1,1,myWCoeff(1), WValues(0));
  }

// Evaluation a l'ordre 0
  for (i=0; i<=DegreeH; i++) {
    BasisValue(ibeg0+i) = HermitValues(i,0);
  }
  W0 = WValues(0);
  for (i=DegreeH+1, j=0; i<=WorkDegree; i++, j++) {
    BasisValue(ibeg0+i) = W0 * jac0[j];
  }

// Evaluation a l'ordre 1
  if (NDeriv >= 1) {
    Standard_Real W1=WValues(1);
    for (i=0; i<=DegreeH; i++) {
      BasisD1(ibeg1+i) = HermitValues(i,1);
    }
    for (i=DegreeH+1, j=0; i<=WorkDegree; i++, j++) {
      BasisD1(ibeg1+i) = W0 * jac1[j] +
	                 W1 * jac0[j];
    }
// Evaluation a l'ordre 2
    if (NDeriv >= 2) {
      Standard_Real W2=WValues(2);
      for (i=0; i<=DegreeH; i++) {
	BasisD2(ibeg2+i) = HermitValues(i,2);
      }
      for (i=DegreeH+1, j=0; i<=WorkDegree; i++, j++) {
	BasisD2(ibeg2+i) =  
	  W0 * jac2[j] + 2 * W1 * jac1[j] + W2 * jac0[j];
      }

// Evaluation a l'ordre 3
      if (NDeriv == 3) {
	 Standard_Real W3 = WValues(3);
	for (i=0; i<=DegreeH; i++) {
	  BasisD3(ibeg3+i) = HermitValues(i,3);
	}
	for (i=DegreeH+1, j=0; i<=WorkDegree; i++,j++) {
	  BasisD3(ibeg3+i) = W0*jac3[j] + W3*jac0[j] 
                           + 3*(W1*jac2[j] + W2*jac1[j]);
	}
      }
    }
  }
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void PLib_HermitJacobi::D0(const Standard_Real U, TColStd_Array1OfReal& BasisValue) 
{
  D0123(0,U,BasisValue,BasisValue,BasisValue,BasisValue);
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void PLib_HermitJacobi::D1(const Standard_Real U,
			   TColStd_Array1OfReal& BasisValue, TColStd_Array1OfReal& BasisD1)  
{
  D0123(1,U,BasisValue,BasisD1,BasisD1,BasisD1);
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void PLib_HermitJacobi::D2(const Standard_Real U, TColStd_Array1OfReal& BasisValue,
			   TColStd_Array1OfReal& BasisD1,TColStd_Array1OfReal& BasisD2) 
{
  D0123(2,U,BasisValue,BasisD1,BasisD2,BasisD2);
}

//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void PLib_HermitJacobi::D3(const Standard_Real U,
			   TColStd_Array1OfReal& BasisValue, TColStd_Array1OfReal& BasisD1,
			   TColStd_Array1OfReal& BasisD2,    TColStd_Array1OfReal& BasisD3) 
{
  D0123(3,U,BasisValue,BasisD1,BasisD2,BasisD3);
}
