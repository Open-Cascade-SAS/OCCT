// Created on: 1997-11-04
// Created by: Roman BORISOV / Igor FEOKTISTOV
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


#include <FEmTool_Curve.hxx>
#include <PLib.hxx>
#include <PLib_Base.hxx>
#include <PLib_HermitJacobi.hxx>
#include <PLib_JacobiPolynomial.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(FEmTool_Curve,Standard_Transient)

//=======================================================================
//function : FEmTool_Curve
//purpose  :
//=======================================================================
FEmTool_Curve::FEmTool_Curve(const Standard_Integer Dimension,
                             const Standard_Integer NbElements,
                             const Handle(PLib_Base)& TheBase, 
                             const Standard_Real) : 
       myNbElements(NbElements), myDimension(Dimension), 
       myBase(TheBase), myDegree(1, myNbElements), 
       myCoeff(1, myDimension*myNbElements*(myBase->WorkDegree() + 1)), 
       myPoly(1, myDimension*myNbElements*(myBase->WorkDegree() + 1)), 
       myDeri(1, myDimension*myNbElements*(myBase->WorkDegree())), 
       myDsecn(1, myDimension*myNbElements*(myBase->WorkDegree() - 1)), 
       HasPoly(1, myNbElements), HasDeri(1, myNbElements), 
       HasSecn(1, myNbElements), myLength(1, myNbElements),
       myIndex(0)
{
  myKnots = new TColStd_HArray1OfReal(1, myNbElements + 1);
  myDegree.Init(myBase->WorkDegree());
  HasPoly.Init(0);
  HasDeri.Init(0);
  HasSecn.Init(0);
  myLength.Init(-1);
}

 TColStd_Array1OfReal& FEmTool_Curve::Knots() const
{
  return myKnots->ChangeArray1();
}



//=======================================================================
//function : SetElement
//purpose  :
//=======================================================================
 void FEmTool_Curve::SetElement(const Standard_Integer IndexOfElement,
				const TColStd_Array2OfReal& Coeffs) 
{
  Standard_Integer i, j, degBase, deg;
  if (IndexOfElement > myNbElements || IndexOfElement < 1) throw Standard_OutOfRange();
  degBase = myBase->WorkDegree();
  deg = myDegree(IndexOfElement);
  Standard_Integer iBase = (IndexOfElement - 1)*(degBase + 1)*myDimension, 
                   i1 = iBase-myDimension, i2 = Coeffs.LowerRow() - 1, 
                   j1 = Coeffs.LowerCol() - 1;
  for(i = 1; i <= deg + 1; i++) {
    i1 += myDimension; i2++; 
    for(j = 1; j <= myDimension; j++)
      myCoeff(i1 + j) = Coeffs(i2, j1 + j);
  }

  Standard_Real stenor = (myKnots->Value(IndexOfElement + 1) - myKnots->Value(IndexOfElement)) / 2.,
                mfact;
  Handle(PLib_HermitJacobi) myHermitJacobi = Handle(PLib_HermitJacobi)::DownCast (myBase);

  i1 = iBase;
  i2 = iBase + (myHermitJacobi->NivConstr() + 1) * myDimension;
  for(i = 1; i <= myHermitJacobi->NivConstr(); i++) {
    i1 += myDimension; i2 += myDimension; 
    mfact = Pow(stenor, i);
    for(j = 1; j <= myDimension; j++) {
      myCoeff(i1 + j) *= mfact;
      myCoeff(i2 + j) *= mfact;
    }
  }

  HasPoly(IndexOfElement) = HasDeri(IndexOfElement) = HasSecn(IndexOfElement) = 0;
  myLength(IndexOfElement) = - 1;
}


//=======================================================================
//function : GetElement
//purpose  :
//=======================================================================
 void FEmTool_Curve::GetElement(const Standard_Integer IndexOfElement, TColStd_Array2OfReal& Coeffs) 
{
  Standard_Integer i, j, degBase, deg;
  if (IndexOfElement > myNbElements || IndexOfElement < 1) throw Standard_OutOfRange();
  degBase = myBase->WorkDegree();
  deg = myDegree(IndexOfElement);
  Standard_Integer iBase = (IndexOfElement - 1)*(degBase + 1)*myDimension, 
                   i1 = iBase-myDimension, i2 = Coeffs.LowerRow() - 1, 
                   j1 = Coeffs.LowerCol() - 1;
  for(i = 1; i <= deg + 1; i++) {
    i1 += myDimension; i2++; 
    for(j = 1; j <= myDimension; j++)
      Coeffs(i2, j1 + j) = myCoeff.Value(i1 + j);
  }

  Standard_Real stenor = 2. / (myKnots->Value(IndexOfElement + 1) - myKnots->Value(IndexOfElement)),
                mfact;

  Handle(PLib_HermitJacobi) myHermitJacobi = Handle(PLib_HermitJacobi)::DownCast (myBase);

  i2 = Coeffs.LowerRow();
  Standard_Integer i3 = i2 + myHermitJacobi->NivConstr() + 1;

  for(i = 1; i <= myHermitJacobi->NivConstr(); i++) {
    mfact = Pow(stenor, i);
    for(j = j1+1; j <= myDimension; j++) {
      Coeffs(i2 + i, j) *= mfact;
      Coeffs(i3 + i, j) *= mfact;
    }
  }
}


//=======================================================================
//function : GetPolynom
//purpose  :
//=======================================================================
 void FEmTool_Curve::GetPolynom(TColStd_Array1OfReal& Coeffs) 

{
  Standard_Integer IndexOfElement, i, di = Coeffs.Lower() - myPoly.Lower();


  for(IndexOfElement = 1; IndexOfElement <= myNbElements; IndexOfElement++)
    if (!HasPoly.Value(IndexOfElement)) Update(IndexOfElement, 0);

  for(i = myPoly.Lower(); i <= myPoly.Upper(); i++)
    Coeffs(di + i) = myPoly(i);

}


//=======================================================================
//function : D0
//purpose  :
//=======================================================================
 void FEmTool_Curve::D0(const Standard_Real U, TColStd_Array1OfReal& Pnt) 
{
  Standard_Integer deg;
  Standard_Real S;

  if (!myIndex || (U<Uf) || (U>Ul) ||
      (myKnots->Value(myIndex)!=Uf) ||
      (myKnots->Value(myIndex+1)!=Ul)) { 
    // Search the span
    if(U <= myKnots->Value(2)) myIndex = 1;
    else {
      for(myIndex = 2; myIndex <= myNbElements; myIndex++)
	if(U >= myKnots->Value(myIndex) && U <= myKnots->Value(myIndex+1)) break;
      if (myIndex > myNbElements) myIndex = myNbElements;
    }
    Uf = myKnots->Value(myIndex);
    Ul = myKnots->Value(myIndex+1);
    Denom = 1. /(Ul-Uf);
    USum = Uf+Ul;
    myPtr = (myIndex - 1)*(myBase->WorkDegree()+ 1)*myDimension + 1;
  }

  deg = myDegree(myIndex);
  if (!HasPoly.Value(myIndex)) Update(myIndex, 0);

//Parameter normalization: S [-1, 1]
  S = (2*U - USum)*Denom;
  PLib::NoDerivativeEvalPolynomial(S, deg, myDimension, deg*myDimension, 
				   myPoly(myPtr), Pnt(Pnt.Lower()));
}


//=======================================================================
//function : D1
//purpose  :
//=======================================================================
 void FEmTool_Curve::D1(const Standard_Real U, TColStd_Array1OfReal& Vec) 
{
  Standard_Integer deg, i;
  Standard_Real S;

  if (!myIndex || (U<Uf) || (U>Ul) ||
      (myKnots->Value(myIndex)!=Uf) ||
      (myKnots->Value(myIndex+1)!=Ul)) { 
    // Search the span
    if(U <= myKnots->Value(2)) myIndex = 1;
    else {
      for(myIndex = 2; myIndex <= myNbElements; myIndex++)
	if(U >= myKnots->Value(myIndex) && U <= myKnots->Value(myIndex+1)) break;
      if (myIndex > myNbElements) myIndex = myNbElements;
    }
    Uf = myKnots->Value(myIndex);
    Ul = myKnots->Value(myIndex+1);
    Denom = 1. /(Ul-Uf);
    USum = Uf+Ul;
    myPtr = (myIndex - 1)*(myBase->WorkDegree()+ 1)*myDimension + 1;
  }

  deg = myDegree(myIndex);
  if (!HasDeri.Value(myIndex)) Update(myIndex, 1);
  
//Parameter normalization: S [-1, 1]
  S = (2*U - USum) * Denom;
  PLib::NoDerivativeEvalPolynomial(S, deg-1, myDimension, (deg-1)*myDimension, 
				   myDeri(1+(myIndex - 1)*myBase->WorkDegree()*
					  myDimension), 
				   Vec(Vec.Lower()));
  
  S = 2*Denom;
  for(i=Vec.Lower(); i<=Vec.Upper(); i++) Vec(i) *= S;
}



//=======================================================================
//function : D2
//purpose  :
//=======================================================================
 void FEmTool_Curve::D2(const Standard_Real U, TColStd_Array1OfReal& Vec) 
{
  Standard_Integer deg, i;
  Standard_Real S;

  if (!myIndex || (U<Uf) || (U>Ul) ||
      (myKnots->Value(myIndex)!=Uf) ||
      (myKnots->Value(myIndex+1)!=Ul)) { 
    // Search the span
    if(U <= myKnots->Value(2)) myIndex = 1;
    else {
      for(myIndex = 2; myIndex <= myNbElements; myIndex++)
	if(U >= myKnots->Value(myIndex) && U <= myKnots->Value(myIndex+1)) break;
      if (myIndex > myNbElements) myIndex = myNbElements;
    }
    Uf = myKnots->Value(myIndex);
    Ul = myKnots->Value(myIndex+1);
    Denom = 1. /(Ul-Uf);
    USum = Uf+Ul;
    myPtr = (myIndex - 1)*(myBase->WorkDegree()+ 1)*myDimension + 1;
  }

  deg = myDegree(myIndex);
  if (!HasSecn.Value(myIndex)) Update(myIndex, 2); 

//Parameter normalization: S [-1, 1]
  S = (2*U - USum)*Denom;
  PLib::NoDerivativeEvalPolynomial(S, deg-2, myDimension, (deg-2)*myDimension, 
				   myDsecn(1+(myIndex - 1)*
					   (myBase->WorkDegree() - 1)*myDimension), 
				   Vec(Vec.Lower()));

  S = 4*Denom*Denom;
  for(i=Vec.Lower(); i<=Vec.Upper(); i++) Vec(i) *= S;
}



//=======================================================================
//function : Length
//purpose  :
//=======================================================================
 void FEmTool_Curve::Length(const Standard_Real FirstU,
			    const Standard_Real LastU,
			    Standard_Real& Length) 
{
  Standard_Integer Low, High, deg, degBase, i, Ptr;
  if(FirstU > LastU) throw Standard_OutOfRange("FEmTool_Curve::Length");
  
  if(myKnots->Value(1) > FirstU) Low = 1;
  else
    for(Low = 1; Low <= myNbElements; Low++)
      if(FirstU >= myKnots->Value(Low) && FirstU <= myKnots->Value(Low+1)) break;
  if(Low > myNbElements) Low = myNbElements;
  
  if(myKnots->Value(1) > LastU) High = 1;
  else
    for(High = Low; High <= myNbElements; High++)
      if(LastU >= myKnots->Value(High) && LastU <= myKnots->Value(High+1)) break;
  if(myKnots->Value(myNbElements + 1) < LastU) High = myNbElements;
  
  Standard_Real Li;
  degBase = myBase->WorkDegree();
  Length = 0;

  Standard_Real FirstS, LastS;
  FirstS = (2*FirstU - myKnots->Value(Low) - myKnots->Value(Low + 1))/
    (myKnots->Value(Low + 1) - myKnots->Value(Low));
  LastS = (2*LastU - myKnots->Value(High) - myKnots->Value(High + 1))/
    (myKnots->Value(High + 1) - myKnots->Value(High));
  
  if(Low == High) {
    
    Ptr = (Low - 1)*(degBase + 1)*myDimension + 1;
    deg = myDegree(Low);
    
    if(!HasPoly(Low)) Update(Low, 0);
    PLib::EvalLength(deg, myDimension, myPoly(Ptr), 
		     FirstS, LastS, Length);
    return;
  }
  
  deg = myDegree(Low);
  Ptr = (Low - 1)*(degBase + 1)*myDimension + 1;

  if(!HasPoly(Low))  Update(Low, 0);
  if(FirstS < -1.) {
    PLib::EvalLength(deg, myDimension, myPoly(Ptr), FirstS, -1., Li);
    Length += Li;
    if(myLength(Low) < 0.) { 
      PLib::EvalLength(deg, myDimension, myPoly(Ptr), -1., 1.,Li);
      myLength(Low) = Li;
    }
    Length += myLength(Low);
  }
  else {
    PLib::EvalLength(deg, myDimension, myPoly(Ptr), FirstS, 1., Li);
    Length += Li;
  }

  deg = myDegree(High);
  Ptr = (High - 1)*(degBase + 1)*myDimension + 1;

  if(!HasPoly(High)) Update(High, 0);
  if(LastS > 1.) {
    PLib::EvalLength(deg, myDimension, myPoly(Ptr), 1., LastS, Li);
    Length += Li;
    if(myLength(High) < 0.) { 
      PLib::EvalLength(deg, myDimension, myPoly(Ptr), -1., 1., Li);
      myLength(High) = Li;
    }
    Length += myLength(High);
  }
  else {
    PLib::EvalLength(deg, myDimension, myPoly(Ptr), -1., LastS, Li);
    Length += Li;
  }

  
  for(i = Low + 1; i < High; i++) {
    if (myLength.Value(i) < 0) {
      Ptr = (i - 1)*(degBase + 1)*myDimension + 1;
      deg = myDegree(i);
      if(!HasPoly(i)) Update(i, 0);
      PLib::EvalLength(deg, myDimension, myPoly(Ptr), -1., 1., Li);
      myLength(i) = Li; 
    }
    Length += myLength(i);
  }
}

 Standard_Integer FEmTool_Curve::NbElements() const
{
  return myNbElements;
}

 Standard_Integer FEmTool_Curve::Dimension() const
{
  return myDimension;
}

 Handle(PLib_Base) FEmTool_Curve::Base() const
{
  return myBase;
}

 Standard_Integer FEmTool_Curve::Degree(const Standard_Integer IndexOfElement) const
{
  return myDegree.Value(IndexOfElement);
}


//=======================================================================
//function : SetDegree
//purpose  :
//=======================================================================
 void FEmTool_Curve::SetDegree(const Standard_Integer IndexOfElement,
			       const Standard_Integer Degree)
{
  if (Degree <= myBase->WorkDegree()) {
    myDegree(IndexOfElement) = Degree;
    HasPoly(IndexOfElement) = HasDeri(IndexOfElement) = HasSecn(IndexOfElement) = 0;
    myLength(IndexOfElement) = -1;
  }
  else if(Degree > myBase->WorkDegree()) throw Standard_OutOfRange("FEmTool_Curve::SetDegree");
}



//=======================================================================
//function : ReduceDegree
//purpose  :
//=======================================================================
 void FEmTool_Curve::ReduceDegree(const Standard_Integer IndexOfElement,
				  const Standard_Real Tol,
				  Standard_Integer& NewDegree,
				  Standard_Real& MaxError)
{
  Standard_Integer deg = myDegree(IndexOfElement);
  
  Standard_Integer Ptr = (IndexOfElement - 1)*
    (myBase->WorkDegree() + 1)*myDimension + 1;
  
  myBase->ReduceDegree(myDimension, deg, Tol, myCoeff.ChangeValue(Ptr), NewDegree, MaxError);
  Handle(PLib_HermitJacobi) myHermitJacobi = Handle(PLib_HermitJacobi)::DownCast (myBase);
  
  NewDegree = Max(NewDegree, 2 * myHermitJacobi->NivConstr() + 1);
  
  if (NewDegree < deg) {
    myDegree(IndexOfElement) = NewDegree;
    HasPoly(IndexOfElement) = HasDeri(IndexOfElement) = HasSecn(IndexOfElement) = 0;
    myLength(IndexOfElement) = -1;
  }
}

//=======================================================================
//function : Update
//purpose  :
//=======================================================================
 void FEmTool_Curve::Update(const Standard_Integer Index,
			    const Standard_Integer Order)
{
  Standard_Integer  degBase = myBase->WorkDegree(), deg = myDegree(Index);

  if (!HasPoly(Index)) {
    Standard_Integer Ptr = (Index - 1)*(degBase + 1)*myDimension + 1;

    TColStd_Array1OfReal Coeff(myPoly.ChangeValue(Ptr), 0, myDimension*(deg + 1) - 1);
    TColStd_Array1OfReal BaseCoeff(myCoeff.ChangeValue(Ptr), 0, myDimension*(deg + 1) - 1);

    myBase->ToCoefficients(myDimension, deg, BaseCoeff, Coeff);
    HasPoly(Index) = 1;
  }
 
  if (Order >=1) {
    Standard_Integer i1 = (Index - 1)*(degBase)*myDimension - myDimension, 
                     i2 = (Index - 1)*(degBase + 1)*myDimension;
    Standard_Integer i,j;
    if (!HasDeri.Value(Index)) {
      for(i = 1; i <= deg; i++) {
	i1 += myDimension; i2 += myDimension;
	for(j = 1; j<= myDimension; j++)
	  myDeri(i1 + j) = i*myPoly.Value(i2 + j);
      }
      HasDeri(Index) = 1;
    }
    if ((Order>=2) &&!HasSecn.Value(Index)) {
      i1 = (Index - 1)*(degBase-1)*myDimension - myDimension, 
      i2 = (Index - 1)*(degBase)*myDimension;
      for(i = 1; i < deg; i++) {
	i1 += myDimension; i2 += myDimension;
	for(j = 1; j<= myDimension; j++)
	  myDsecn(i1 + j) = i*myDeri.Value(i2 + j);
      }
      HasSecn(Index) = 1;
    }  
  }
}
