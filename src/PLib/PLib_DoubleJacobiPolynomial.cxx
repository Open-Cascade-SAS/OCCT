// Created on: 1997-05-28
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


#include <math_Vector.hxx>
#include <PLib_DoubleJacobiPolynomial.hxx>
#include <PLib_JacobiPolynomial.hxx>

//=======================================================================
//function : PLib_DoubleJacobiPolynomial
//purpose  : 
//=======================================================================
PLib_DoubleJacobiPolynomial::PLib_DoubleJacobiPolynomial()

{
}

//=======================================================================
//function : PLib_DoubleJacobiPolynomial
//purpose  : 
//=======================================================================

PLib_DoubleJacobiPolynomial::PLib_DoubleJacobiPolynomial(const Handle(PLib_JacobiPolynomial)& JacPolU,
							 const Handle(PLib_JacobiPolynomial)& JacPolV) :
                                                         myJacPolU(JacPolU),
                                                         myJacPolV(JacPolV)
{
  Handle (TColStd_HArray1OfReal) TabMaxU = 
    new TColStd_HArray1OfReal (0,JacPolU->WorkDegree()-2*(JacPolU->NivConstr()+1));
  JacPolU->MaxValue(TabMaxU->ChangeArray1());
  myTabMaxU = TabMaxU;

  Handle (TColStd_HArray1OfReal) TabMaxV = 
    new TColStd_HArray1OfReal (0,JacPolV->WorkDegree()-2*(JacPolV->NivConstr()+1));
  JacPolV->MaxValue(TabMaxV->ChangeArray1());
  myTabMaxV = TabMaxV;
}

//=======================================================================
//function : MaxErrorU
//purpose  : 
//=======================================================================

Standard_Real 
PLib_DoubleJacobiPolynomial::MaxErrorU(const Standard_Integer Dimension, 
				       const Standard_Integer DegreeU, 
				       const Standard_Integer DegreeV, 
				       const Standard_Integer dJacCoeff, 
				       const TColStd_Array1OfReal& JacCoeff) const 
{
  Standard_Integer ii,idim,dJac,MinU,MinV,WorkDegreeU,WorkDegreeV;
  Standard_Real Bid0;

  math_Vector MaxErrDim(1,Dimension,0.);

  MinU = 2*(myJacPolU->NivConstr()+1);
  MinV = 2*(myJacPolV->NivConstr()+1);
  WorkDegreeU = myJacPolU->WorkDegree();
  WorkDegreeV = myJacPolV->WorkDegree();

  Bid0 = myTabMaxV->Value(DegreeV-MinV);
  for (idim=1; idim<=Dimension; idim++) {
    dJac = dJacCoeff + (idim-1)*(WorkDegreeU+1)*(WorkDegreeV+1);
    for (ii=MinU; ii<=DegreeU; ii++) {
      MaxErrDim(idim) += (Abs(JacCoeff(ii + DegreeV*(WorkDegreeU+1) + dJac)) * 
			  myTabMaxU->Value(ii-MinU) * Bid0);
    }
  }
  return (MaxErrDim.Norm());
}

//=======================================================================
//function : MaxErrorV
//purpose  : 
//=======================================================================

Standard_Real 
PLib_DoubleJacobiPolynomial::MaxErrorV(const Standard_Integer Dimension, 
				       const Standard_Integer DegreeU, 
				       const Standard_Integer DegreeV, 
				       const Standard_Integer dJacCoeff, 
				       const TColStd_Array1OfReal& JacCoeff) const 
{
  Standard_Integer jj,idim,dJac,MinU,MinV,WorkDegreeU,WorkDegreeV;
  Standard_Real Bid0;

  math_Vector MaxErrDim(1,Dimension,0.);

  MinU = 2*(myJacPolU->NivConstr()+1);
  MinV = 2*(myJacPolV->NivConstr()+1);
  WorkDegreeU = myJacPolU->WorkDegree();
  WorkDegreeV = myJacPolV->WorkDegree();

  Bid0 = myTabMaxU->Value(DegreeU-MinU);
  for (idim=1; idim<=Dimension; idim++) {
    dJac = dJacCoeff + (idim-1)*(WorkDegreeU+1)*(WorkDegreeV+1);
    for (jj=MinV; jj<=DegreeV; jj++) {
      MaxErrDim(idim) += (Abs(JacCoeff(DegreeU + jj*(WorkDegreeU+1) + dJac)) * 
			  myTabMaxV->Value(jj-MinV) * Bid0);
    }
  }
  return (MaxErrDim.Norm());
}

//=======================================================================
//function : MaxError
//purpose  : 
//=======================================================================

Standard_Real 
PLib_DoubleJacobiPolynomial::MaxError(const Standard_Integer Dimension, 
				      const Standard_Integer MinDegreeU, 
				      const Standard_Integer MaxDegreeU, 
				      const Standard_Integer MinDegreeV, 
				      const Standard_Integer MaxDegreeV, 
				      const Standard_Integer dJacCoeff, 
				      const TColStd_Array1OfReal& JacCoeff,
				      const Standard_Real Error) const 
{
  Standard_Integer ii,jj,idim,dJac,MinU,MinV,WorkDegreeU,WorkDegreeV;
  Standard_Real Bid0,Bid1;

  math_Vector MaxErrDim(1,Dimension,0.);

  MinU = 2*(myJacPolU->NivConstr()+1);
  MinV = 2*(myJacPolV->NivConstr()+1);
  WorkDegreeU = myJacPolU->WorkDegree();
  WorkDegreeV = myJacPolV->WorkDegree();

//------------------- Calcul du majorant de l'erreur max ---------------
//----- lorsque sont enleves les coeff. d'indices MinDegreeU a MaxDegreeU ------
//---------------- en U et d'indices MinDegreeV a MaxDegreeV en V --------------

  for (idim=1; idim<=Dimension; idim++) {
    dJac = dJacCoeff + (idim-1)*(WorkDegreeU+1)*(WorkDegreeV+1);
    Bid1 = 0.;
    for (jj=MinDegreeV; jj<=MaxDegreeV; jj++) {
      Bid0 = 0.;
      for (ii=MinDegreeU; ii<=MaxDegreeU; ii++) {
	Bid0 += fabs(JacCoeff(ii + jj*(WorkDegreeU+1) + dJac)) * myTabMaxU->Value(ii-MinU);
      }
      Bid1 += Bid0 * myTabMaxV->Value(jj-MinV);
    }
    MaxErrDim(idim) = Bid1;
  }

//----------------------- Calcul de l' erreur max ----------------------

  math_Vector MaxErr2(1,2);
  MaxErr2(1) = Error;
  MaxErr2(2) = MaxErrDim.Norm();
  return (MaxErr2.Norm());
}

//=======================================================================
//function : ReduceDegree
//purpose  : 
//=======================================================================

void PLib_DoubleJacobiPolynomial::ReduceDegree(const Standard_Integer Dimension, 
					       const Standard_Integer MinDegreeU, 
					       const Standard_Integer MaxDegreeU, 
					       const Standard_Integer MinDegreeV, 
					       const Standard_Integer MaxDegreeV, 
					       const Standard_Integer dJacCoeff, 
					       const TColStd_Array1OfReal& JacCoeff,
					       const Standard_Real EpmsCut,
					       Standard_Real& MaxError,
					       Standard_Integer& NewDegreeU, 
					       Standard_Integer& NewDegreeV) const
{
  Standard_Integer NewU,NewV;
  Standard_Real ErrU,ErrV;

  NewU = MaxDegreeU;
  NewV = MaxDegreeV;
  math_Vector MaxErr2(1,2);

//**********************************************************************
//-------------------- Coupure des coefficients ------------------------
//**********************************************************************

  do {

//------------------- Calcul du majorant de l'erreur max ---------------
//----- lorsque sont enleves les coeff. d'indices MinU a NewU ------
//---------------- en U, le degre en V etant fixe a NewV -----------------
    if (NewV > MinDegreeV) 
      ErrV = MaxErrorU(Dimension,NewU,NewV,dJacCoeff,JacCoeff);
    else {
      ErrV = 2*EpmsCut;
    }

//------------------- Calcul du majorant de l'erreur max ---------------
//----- lorsque sont enleves les coeff. d'indices MinV a NewV ------
//---------------- en V, le degre en U etant fixe a NewU -----------------
    if (NewU > MinDegreeU) 
      ErrU = MaxErrorV(Dimension,NewU,NewV,dJacCoeff,JacCoeff);
    else {
      ErrU = 2*EpmsCut;
    }

//----------------------- Calcul de l' erreur max ----------------------
    MaxErr2(1) = MaxError;
    MaxErr2(2) = ErrU;
    ErrU = MaxErr2.Norm();
    MaxErr2(2) = ErrV;
    ErrV = MaxErr2.Norm();

    if (ErrU > ErrV) {
      if (ErrV < EpmsCut) {
	MaxError = ErrV;
	NewV--;
      }
    }
    else {
      if (ErrU < EpmsCut) {
	MaxError = ErrU;
	NewU--;
      }
    }
  }
  while ((ErrU > ErrV && ErrV <= EpmsCut) || (ErrV >= ErrU && ErrU <= EpmsCut));

//-------------------------- Recuperation des degres -------------------
        
  NewDegreeU = Max(NewU,1);
  NewDegreeV = Max(NewV,1);
} 

//=======================================================================
//function : AverageError
//purpose  : 
//=======================================================================

Standard_Real
PLib_DoubleJacobiPolynomial::AverageError(const Standard_Integer Dimension, 
					      const Standard_Integer DegreeU, 
					      const Standard_Integer DegreeV, 
					      const Standard_Integer dJacCoeff, 
					      const TColStd_Array1OfReal& JacCoeff) const 
{
  Standard_Integer ii,jj,idim,dJac,IDebU,IDebV,MinU,MinV,WorkDegreeU,WorkDegreeV;
  Standard_Real Bid0,Bid1,AverageErr;

//----------------------------- Initialisations ------------------------

  IDebU = 2*(myJacPolU->NivConstr()+1);
  IDebV = 2*(myJacPolV->NivConstr()+1);
  MinU = Max(IDebU,DegreeU);
  MinV = Max(IDebV,DegreeV);
  WorkDegreeU = myJacPolU->WorkDegree();
  WorkDegreeV = myJacPolV->WorkDegree();
  Bid0 = 0.;

//------------------ Calcul du majorant de l'erreur moyenne ------------
//----- lorsque sont enleves les coeff. d'indices DegreeU a WorkDegreeU ------
//---------------- en U et d'indices DegreeV a WorkDegreeV en V --------------

  for (idim=1; idim<=Dimension; idim++) {
    dJac = dJacCoeff + (idim-1)*(WorkDegreeU+1)*(WorkDegreeV+1);
    for (jj=MinV; jj<=WorkDegreeV; jj++) {
      for (ii=IDebU; ii<=WorkDegreeU; ii++) {
	Bid1 = JacCoeff(ii + jj*(WorkDegreeU+1) + dJac);
	Bid0 += Bid1*Bid1;
      }
    }
    for (jj=IDebV; jj<=MinV-1; jj++) {
      for (ii=MinU; ii<=WorkDegreeU; ii++) {
	Bid1 = JacCoeff(ii + jj*(WorkDegreeU+1) + dJac);
	Bid0 += Bid1*Bid1;
      }
    }
  }
  AverageErr = sqrt(Bid0/4);
  return (AverageErr);
}

//=======================================================================
//function : WDoubleJacobiToCoefficients
//purpose  : 
//=======================================================================

void PLib_DoubleJacobiPolynomial::WDoubleJacobiToCoefficients(const Standard_Integer Dimension, 
							      const Standard_Integer DegreeU, 
							      const Standard_Integer DegreeV, 
							      const TColStd_Array1OfReal& JacCoeff, 
							      TColStd_Array1OfReal& Coefficients) const 
{
  Standard_Integer iu,iv,idim,WorkDegreeU,WorkDegreeV;

  Coefficients.Init(0.);

  WorkDegreeU = myJacPolU->WorkDegree();
  WorkDegreeV = myJacPolV->WorkDegree();

  TColStd_Array1OfReal Aux1(0, (DegreeU+1)*(DegreeV+1)*Dimension-1);
  TColStd_Array1OfReal Aux2(0, (DegreeU+1)*(DegreeV+1)*Dimension-1);

  for (iu=0; iu<=DegreeU; iu++) {
    for (iv=0; iv<=DegreeV; iv++) {
      for (idim=1; idim<=Dimension; idim++) {
	Aux1(idim-1 + iv*Dimension + iu*Dimension*(DegreeV+1)) = 
	  JacCoeff(iu + iv*(WorkDegreeU+1) + (idim-1)*(WorkDegreeU+1)*(WorkDegreeV+1));
      }
    }
  }
//   Passage dans canonique en u.
  myJacPolU->ToCoefficients(Dimension*(DegreeV+1),DegreeU,Aux1,Aux2);

//   Permutation des u et des v.
  for (iu=0; iu<=DegreeU; iu++) {
    for (iv=0; iv<=DegreeV; iv++) {
      for (idim=1; idim<=Dimension; idim++) {
	Aux1(idim-1 + iu*Dimension + iv*Dimension*(DegreeU+1)) = 
	  Aux2(idim-1 + iv*Dimension + iu*Dimension*(DegreeV+1));
      }
    }
  }
//   Passage dans canonique en v.
  myJacPolV->ToCoefficients(Dimension*(DegreeU+1),DegreeV,Aux1,Aux2);

//   Permutation des u et des v.
  for (iu=0; iu<=DegreeU; iu++) {
    for (iv=0; iv<=DegreeV; iv++) {
      for (idim=1; idim<=Dimension; idim++) {
	Coefficients(iu + iv*(DegreeU+1) + (idim-1)*(DegreeU+1)*(DegreeV+1)) = 
	  Aux2(idim-1 + iu*Dimension + iv*Dimension*(DegreeU+1));
      }
    }
  }
}

