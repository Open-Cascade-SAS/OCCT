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

#define No_Standard_RangeError
#define No_Standard_OutOfRange


#include <AdvApprox_EvaluatorFunction.hxx>
#include <AdvApprox_SimpleApprox.hxx>
#include <math_Vector.hxx>
#include <PLib.hxx>
#include <PLib_JacobiPolynomial.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>

//=======================================================================
//function : AdvApprox_SimpleApprox
//purpose  : 
//=======================================================================
AdvApprox_SimpleApprox::
AdvApprox_SimpleApprox(const Standard_Integer TotalDimension, 
                       const Standard_Integer TotalNumSS, 
                       const GeomAbs_Shape Continuity, 
                       const Standard_Integer WorkDegree, 
                       const Standard_Integer NbGaussPoints, 
                       const Handle(PLib_JacobiPolynomial)& JacobiBase, 
                       const AdvApprox_EvaluatorFunction& Func) :
                       myTotalNumSS(TotalNumSS),
                       myTotalDimension(TotalDimension),
                       myNbGaussPoints(NbGaussPoints),
                       myWorkDegree(WorkDegree),
                       myJacPol(JacobiBase),
                       myEvaluator((Standard_Address)&Func)
{
  
  // the Check of input parameters

  switch (Continuity) {
    case GeomAbs_C0: myNivConstr = 0; break;
    case GeomAbs_C1: myNivConstr = 1; break;
    case GeomAbs_C2: myNivConstr = 2; break;
    default: 
      throw Standard_ConstructionError("Invalid Continuity");
  }

  Standard_Integer DegreeQ = myWorkDegree - 2*(myNivConstr+1);

  // the extraction of the Legendre roots
  myTabPoints = new TColStd_HArray1OfReal(0,NbGaussPoints/2);
  JacobiBase->Points(NbGaussPoints, myTabPoints->ChangeArray1());  

  // the extraction of the Gauss Weights
  myTabWeights = new TColStd_HArray2OfReal(0,NbGaussPoints/2, 0,DegreeQ);
  JacobiBase->Weights(NbGaussPoints, myTabWeights->ChangeArray2());  

  myCoeff       = new TColStd_HArray1OfReal(0,(myWorkDegree+1)*myTotalDimension-1);
  myFirstConstr = new TColStd_HArray2OfReal(1,myTotalDimension, 0,myNivConstr);
  myLastConstr  = new TColStd_HArray2OfReal(1,myTotalDimension, 0,myNivConstr);
  mySomTab      = new TColStd_HArray1OfReal(0,(myNbGaussPoints/2+1)*myTotalDimension-1);
  myDifTab      = new TColStd_HArray1OfReal(0,(myNbGaussPoints/2+1)*myTotalDimension-1);
  done = Standard_False;

}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void AdvApprox_SimpleApprox::Perform(const TColStd_Array1OfInteger& LocalDimension, 
                                     const TColStd_Array1OfReal& LocalTolerancesArray, 
                                     const Standard_Real First, 
                                     const Standard_Real Last, 
                                     const Standard_Integer MaxDegree)

{
// ======= the computation of Pp(t) = Rr(t) + W(t)*Qq(t) =======

  done = Standard_False;
  Standard_Integer i,idim,k,numss;

  Standard_Integer Dimension = myTotalDimension;
  AdvApprox_EvaluatorFunction& Evaluator = 
    *(AdvApprox_EvaluatorFunction*)myEvaluator;

  // ===== the computation of Rr(t) (the first part of Pp) ======

  Standard_Integer DegreeR = 2*myNivConstr+1;
  Standard_Integer DegreeQ = myWorkDegree - 2*(myNivConstr+1);

  Standard_Real FirstLast[2];
  FirstLast[0] = First;
  FirstLast[1] = Last;

  math_Vector Result(1,myTotalDimension);
  Standard_Integer ErrorCode,derive,i_idim;
  Standard_Real Fact=(Last-First)/2;
  Standard_Real *pResult = (Standard_Real*) &Result.Value(1);
  Standard_Real param;  

  for (param = First, derive = myNivConstr; 
       derive >= 0 ; derive--) {
    Evaluator(&Dimension, FirstLast, &param, &derive, pResult, &ErrorCode);
    if (ErrorCode != 0) 
      return; // Evaluation error
    if (derive>=1) Result *= Fact;
    if (derive==2) Result *= Fact;
    for (idim=1; idim<=myTotalDimension; idim++) {
      myFirstConstr->SetValue (idim,derive,Result(idim));
    }
  }

  for (param = Last, derive = myNivConstr; 
       derive >= 0 ; derive--) { 
    Evaluator(&Dimension, FirstLast, &param, &derive, pResult, &ErrorCode); 
   if (ErrorCode != 0) 
      return; // Evaluation error
    if (derive>=1) Result *= Fact;
    if (derive==2) Result *= Fact;
    for (idim=1; idim<=myTotalDimension; idim++) {
      myLastConstr->SetValue (idim,derive,Result(idim));
    }
  }

  PLib::HermiteInterpolate(myTotalDimension, -1., 1., myNivConstr, myNivConstr,
                           myFirstConstr->Array2(), myLastConstr->Array2(), 
                           myCoeff->ChangeArray1());

  // ===== the computation of the coefficients of Qq(t) (the second part of Pp) ======

  math_Vector Fti (1,myTotalDimension);
  math_Vector Rpti (1,myTotalDimension);
  math_Vector Rmti (1,myTotalDimension);
  Standard_Real *pFti  = (Standard_Real*) &Fti.Value(1);
  Standard_Real* Coef1 = (Standard_Real*) &(myCoeff->ChangeArray1().Value(0));

  derive = 0;
  Standard_Real ti, tip, tin, alin = (Last-First)/2, blin = (Last+First)/2.;

  i_idim = myTotalDimension;
  for (i=1; i<=myNbGaussPoints/2; i++) {
    ti = myTabPoints->Value(i);
    tip = alin*ti+blin;
    Evaluator(&Dimension, FirstLast, &tip, &derive, pFti, &ErrorCode);
    if (ErrorCode != 0) 
      return; // Evaluation error
    for (idim=1; idim<=myTotalDimension; idim++) {
      mySomTab->SetValue(i_idim,Fti(idim));
      myDifTab->SetValue(i_idim,Fti(idim));
      i_idim++;
    }
  }
  i_idim = myTotalDimension;
  for (i=1; i<=myNbGaussPoints/2; i++) {
    ti = myTabPoints->Value(i);
    tin = -alin*ti+blin;
    Evaluator(&Dimension, FirstLast, &tin, &derive, pFti, &ErrorCode);
    if (ErrorCode != 0) 
      return; // Evaluation error
    PLib::EvalPolynomial(ti, derive, DegreeR, myTotalDimension,
			 Coef1[0], Rpti(1));
    ti = -ti;
    PLib::EvalPolynomial(ti, derive, DegreeR, myTotalDimension,
			 Coef1[0], Rmti(1));

    for (idim=1; idim<=myTotalDimension; idim++) {
      mySomTab->SetValue(i_idim, mySomTab->Value(i_idim) + 
			 Fti(idim)- Rpti(idim) - Rmti(idim));
      myDifTab->SetValue(i_idim, myDifTab->Value(i_idim) - 
			 Fti(idim)- Rpti(idim) + Rmti(idim));
      i_idim++;
    }
  }


  // for odd NbGaussPoints - the computation of [ F(0) - R(0) ]
  if (myNbGaussPoints % 2 == 1) {
    ti = myTabPoints->Value(0);
    tip = blin;
    Evaluator(&Dimension, FirstLast, &tip, &derive, pFti, &ErrorCode);
    if (ErrorCode != 0) 
      return; // Evaluation error
    PLib::EvalPolynomial(ti, derive, DegreeR, myTotalDimension,
			 Coef1[0], Rpti(1));
    for (idim=1; idim<=myTotalDimension; idim++) {
      mySomTab->SetValue(idim-1, Fti(idim) - Rpti(idim));
      myDifTab->SetValue(idim-1, Fti(idim) - Rpti(idim));
    }
  }
  
  // the computation of Qq(t)
  Standard_Real Sum = 0.;
  for (k=0; k<=DegreeQ; k+=2) {
    for (idim=1; idim<=myTotalDimension; idim++) {
      Sum=0.;
      for (i=1; i<=myNbGaussPoints/2; i++) {
        Sum += myTabWeights->Value(i,k) * mySomTab->Value(i*myTotalDimension+idim-1);
      }
      myCoeff->SetValue((k+DegreeR+1)*myTotalDimension+idim-1,Sum);
    }
  }
  for (k=1; k<=DegreeQ; k+=2) {
    for (idim=1; idim<=myTotalDimension; idim++) {
      Sum=0.;
      for (i=1; i<=myNbGaussPoints/2; i++) {
        Sum += myTabWeights->Value(i,k) * myDifTab->Value(i*myTotalDimension+idim-1);
      }
      myCoeff->SetValue((k+DegreeR+1)*myTotalDimension+idim-1,Sum);
    }
  }
  if (myNbGaussPoints % 2 == 1) {
    for (idim=1; idim<=myTotalDimension; idim++) {
      for (k=0; k<=DegreeQ; k+=2) {
        Sum += myTabWeights->Value(0,k) * mySomTab->Value(0*myTotalDimension+idim-1);
        myCoeff->SetValue((k+DegreeR+1)*myTotalDimension+idim-1,Sum);
      }
    }
  }
//  for (i=0; i<(WorkDegree+1)*TotalDimension; i++)
//    cout << "  Coeff(" << i << ") = " << Coeff(i) << endl;
  // the computing of NewDegree
  TColStd_Array1OfReal JacCoeff(0, myTotalDimension*(myWorkDegree+1)-1);
 
  Standard_Real MaxErr,AverageErr;
  Standard_Integer Dim, RangSS, RangCoeff, RangJacCoeff, RangDim, NewDegree, NewDegreeMax = 0;

  myMaxError =  new TColStd_HArray1OfReal (1,myTotalNumSS);
  myAverageError = new TColStd_HArray1OfReal (1,myTotalNumSS);
  RangSS =0;
  RangJacCoeff = 0 ;
  for (numss=1; numss<=myTotalNumSS; numss++) {
    Dim=LocalDimension(numss);
    RangCoeff = 0;
    RangDim = 0;
    for (k=0; k<=myWorkDegree; k++) {
      for (idim=1; idim<=Dim; idim++) {
	JacCoeff(RangJacCoeff+RangDim+idim-1) = 
	  myCoeff->Value(RangCoeff+ RangSS +idim-1);
      }
      RangDim =RangDim + Dim ;
      RangCoeff = RangCoeff+myTotalDimension;
    }

    Standard_Real* JacSS = (Standard_Real*) &JacCoeff.Value(RangJacCoeff) ;
    myJacPol->ReduceDegree(Dim,MaxDegree,LocalTolerancesArray(numss), 
			   JacSS [0], NewDegree,MaxErr); 
    if (NewDegree > NewDegreeMax) NewDegreeMax = NewDegree;
    RangSS = RangSS + Dim;
    RangJacCoeff = RangJacCoeff + (myWorkDegree+1)* Dim;
  } 
  // the computing of MaxError and AverageError
  RangSS =0;
  RangJacCoeff = 0 ;
  for (numss=1; numss<=myTotalNumSS; numss++) {
    Dim=LocalDimension(numss);

    Standard_Real* JacSS = (Standard_Real*) &JacCoeff.Value(RangJacCoeff) ;
    MaxErr=myJacPol->MaxError(LocalDimension(numss),JacSS [0],NewDegreeMax);
    myMaxError->SetValue(numss,MaxErr);
    AverageErr=myJacPol->AverageError(LocalDimension(numss),JacSS [0],NewDegreeMax);
    myAverageError->SetValue(numss,AverageErr);
    RangSS = RangSS + Dim;
    RangJacCoeff = RangJacCoeff + (myWorkDegree+1)* Dim;
  }

  myDegree = NewDegreeMax;
  done = Standard_True;
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean AdvApprox_SimpleApprox::IsDone() const 
{
  return done;
}

//=======================================================================
//function : Degree
//purpose  : 
//=======================================================================

Standard_Integer AdvApprox_SimpleApprox::Degree() const 
{
  return myDegree;
}

//=======================================================================
//function : Coefficients
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal) AdvApprox_SimpleApprox::Coefficients() const 
{
  return myCoeff;
}

//=======================================================================
//function : FirstConstr
//purpose  : 
//=======================================================================

Handle(TColStd_HArray2OfReal) AdvApprox_SimpleApprox::FirstConstr() const 
{
  return myFirstConstr;
}

//=======================================================================
//function : LastConstr
//purpose  : 
//=======================================================================

Handle(TColStd_HArray2OfReal) AdvApprox_SimpleApprox::LastConstr() const 
{
  return myLastConstr;
}

//=======================================================================
//function : SomTab
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal) AdvApprox_SimpleApprox::SomTab() const 
{
  return mySomTab;
}

//=======================================================================
//function : DifTab
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal) AdvApprox_SimpleApprox::DifTab() const 
{
  return myDifTab;
}

//=======================================================================
//function : MaxError
//purpose  : 
//=======================================================================

Standard_Real AdvApprox_SimpleApprox::MaxError(const Standard_Integer Index) const 
{
  return myMaxError->Value(Index);
}

//=======================================================================
//function : AverageError
//purpose  : 
//=======================================================================

Standard_Real AdvApprox_SimpleApprox::AverageError(const Standard_Integer Index) const 
{
  return myAverageError->Value(Index);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void AdvApprox_SimpleApprox::Dump(Standard_OStream& o) const
{
  Standard_Integer ii;
  o << "Dump of SimpleApprox " << endl;
  for (ii=1; ii <= myTotalNumSS; ii++) {
    o << "Error   " << MaxError(ii) << endl;
  }
}


