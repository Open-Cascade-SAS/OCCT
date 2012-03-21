// Created on: 1997-10-31
// Created by: Roman BORISOV
// Copyright (c) 1997-1999 Matra Datavision
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


#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError

#include <FEmTool_ProfileMatrix.ixx>
#include <gp.hxx>


//=======================================================================
//function : :FEmTool_ProfileMatrix
//purpose  :
//=======================================================================
FEmTool_ProfileMatrix::FEmTool_ProfileMatrix(const TColStd_Array1OfInteger& FirstIndexes) 
                       : profile(1, 2, 1, FirstIndexes.Length())
{
  Standard_Integer i, j, k, l;
  profile(1, 1) = 0;
  profile(2, 1) = 1;
  for(i = 2; i <= FirstIndexes.Length(); i++) {
    profile(1, i) = i - FirstIndexes(i);
    profile(2, i) = profile(2, i-1) + profile(1, i) + 1;
  }
  NextCoeff = new TColStd_HArray1OfInteger(1, profile(2, FirstIndexes.Length()));
 
  for(i = 1, k = 1; i <= FirstIndexes.Length(); i++)
    for(j = FirstIndexes(i); j <= i; j++) {
      for(l = i+1; l <= FirstIndexes.Length() && j < FirstIndexes(l); l++);

      if(l > FirstIndexes.Length()) NextCoeff->SetValue(k, 0);
      else NextCoeff->SetValue(k, l);
      k++;
    }
  

  ProfileMatrix = new TColStd_HArray1OfReal(1, profile(2, FirstIndexes.Length()));
  SMatrix = new TColStd_HArray1OfReal(1, profile(2, FirstIndexes.Length()));
  IsDecomp = Standard_False;
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================

 void FEmTool_ProfileMatrix::Init(const Standard_Real Value) 
{
  ProfileMatrix->Init(Value);
  IsDecomp = Standard_False;
}

//=======================================================================
//function : ChangeValue
//purpose  : 
//=======================================================================

 Standard_Real& FEmTool_ProfileMatrix::ChangeValue(const Standard_Integer I,
						   const Standard_Integer J)
{
  Standard_Integer Ind;
  Ind = I-J;
  if (Ind < 0) {
    Ind = -Ind;
    Standard_OutOfRange_Raise_if(Ind>profile(1, J),
				  "FEmTool_ProfileMatrix::ChangeValue");
    Ind = profile(2, J) - Ind;
  }
  else {
    Standard_OutOfRange_Raise_if(Ind>profile(1, I),
				  "FEmTool_ProfileMatrix::ChangeValue");
    Ind = profile(2, I)-Ind;
  }
  return  ProfileMatrix->ChangeValue(Ind);
}

//=======================================================================
//function : Decompose
//purpose  : Choleski's decomposition
//=======================================================================
 Standard_Boolean FEmTool_ProfileMatrix::Decompose() 
{
  Standard_Integer i, j, k, ik, jk, DiagAddr, CurrAddr, Kmin, Kj;
  Standard_Real Sum, a, Eps = 1.e-32;

  SMatrix->Init(0.);
  Standard_Real * SMA =  &SMatrix->ChangeValue(1);
  SMA--;
  const Standard_Real * PM = &ProfileMatrix->Value(1);
  PM--;

  for(j = 1; j <= RowNumber(); j++) {
    DiagAddr = profile(2, j);
    Kj = j - profile(1, j);
    Sum = 0;
    for(k = DiagAddr - profile(1, j); k < DiagAddr; k++)
      Sum += SMA[k] * SMA[k];

    a = PM[DiagAddr] - Sum;
    if(a < Eps) {
      return IsDecomp = Standard_False;// Matrix is not positive defined
    }
    a = Sqrt(a);
    SMA[DiagAddr] = a;
  
    CurrAddr = DiagAddr;
    while ((i = NextCoeff->Value(CurrAddr)) > 0) {
      CurrAddr = profile(2, i) - (i - j);
    
      // Computation of Sum of S  .S  for k = 1,..,j-1
      //                        ik  jk
      Sum = 0;
      Kmin = Max((i - profile(1, i)), Kj);
      ik = profile(2, i) - i + Kmin;
      jk = DiagAddr - j + Kmin;
      for(k = Kmin; k <j; k++,ik++,jk++) {
	Sum += SMA[ik]*SMA[jk];
      }  
      SMA[CurrAddr] = (PM[CurrAddr] - Sum)/a;
    }
  }
  return IsDecomp = Standard_True;
}

//=======================================================================
//function : Solve
//purpose  : Resolution of the system S*t(S)X = B
//=======================================================================
 void FEmTool_ProfileMatrix::Solve(const math_Vector& B,math_Vector& X) const
{
  if (!IsDecomp) StdFail_NotDone::Raise("Decomposition must be done");
 
  Standard_Integer i, j, jj,DiagAddr, CurrAddr;
  Standard_Real Sum;

  Standard_Real * x = &X(X.Lower());
  x--;
  Standard_Real * b = &B(B.Lower());
  b--;
  const Standard_Real * SMA = &SMatrix->Value(1);
  SMA --;
  const Standard_Integer * NC = &NextCoeff->Value(1);
  NC--;

// Resolution of Sw = B;
  for(i = 1; i <= RowNumber(); i++) {
    DiagAddr = profile(2, i);
    Sum = 0;
    for(j = i - profile(1, i), jj = DiagAddr - (i - j); 
	j < i; j++, jj++) 
      Sum += SMA[jj]* x[j];
    x[i] = (b[i] - Sum)/SMA[DiagAddr];
  }

// Resolution of t(S)X = w;
  for(i = ColNumber(); i >= 1; i--) {
    DiagAddr = profile(2, i);
    j = NC[DiagAddr];
    Sum = 0;
    while(j > 0) {
      CurrAddr = profile(2, j) - (j-i);
      Sum += SMA[CurrAddr]*x[j];
      j = NC[CurrAddr];
    }
   x[i] = (x[i] - Sum)/SMA[DiagAddr]; 
  }
}

 Standard_Boolean FEmTool_ProfileMatrix::Prepare() 
{
  Standard_NotImplemented::Raise("FEmTool_ProfileMatrix::Prepare");
  return Standard_False;  
}

// void FEmTool_ProfileMatrix::Solve(const math_Vector& B,const math_Vector& Init,math_Vector& X,math_Vector& Residual,const Standard_Real Tolerance,const Standard_Integer NbIterations) const
 void FEmTool_ProfileMatrix::Solve(const math_Vector& ,const math_Vector& ,math_Vector& ,math_Vector& ,const Standard_Real ,const Standard_Integer ) const
{
  Standard_NotImplemented::Raise("FEmTool_ProfileMatrix::Solve");
}


//=======================================================================
//function : Multiplied
//purpose  : MX = H*X
//=======================================================================
 void FEmTool_ProfileMatrix::Multiplied(const math_Vector& X,math_Vector& MX) const
{
  Standard_Integer i, j, jj, DiagAddr, CurrAddr;
  Standard_Real * m = &MX(MX.Lower());
  m--;
  Standard_Real * x = &X(X.Lower());
  x--;
  const Standard_Real * PM = &ProfileMatrix->Value(1);
  PM--;
  const Standard_Integer * NC = &NextCoeff->Value(1);
  NC--;

  for(i = 1; i <= RowNumber(); i++) {
    DiagAddr = profile(2, i);
    m[i] = 0;
    for(j = i - profile(1, i), jj = DiagAddr - (i - j); 
	j <= i; j++, jj++)
      m[i] += PM[jj]*x[j];

    CurrAddr = DiagAddr;
    for(j = NC[CurrAddr]; j > 0; j = NC[CurrAddr]) {
      CurrAddr = profile(2, j) - (j-i);
      m[i] += PM[CurrAddr]*x[j];
    }
  }
}

 Standard_Integer FEmTool_ProfileMatrix::RowNumber() const
{
  return profile.RowLength();
} 

 Standard_Integer FEmTool_ProfileMatrix::ColNumber() const
{
  return profile.RowLength();
}

Standard_Boolean FEmTool_ProfileMatrix::IsInProfile(const Standard_Integer i, 
						    const Standard_Integer j) const
{
  if (j <= i) {
    if ((i - j) <= profile(1, i)) return Standard_True;
    else return Standard_False;
  }
  else if ((j - i) <= profile(1, j)) return Standard_True;

  return Standard_False;
}

 void FEmTool_ProfileMatrix::OutM() const
{
  Standard_Integer i, j;
  cout<<"Matrix A"<<endl;
  for(i = 1; i <= RowNumber(); i++) {
    for(j = 1; j < i - profile(1, i); j++) 
      cout<<"0 ";

    for(j = profile(2, i) - profile(1, i); j <= profile(2, i); j++) 
      cout<<ProfileMatrix->Value(j)<<" ";
    cout<<endl;
  }

  cout<<"NextCoeff"<<endl;
  for(i = 1; i <= profile(2, RowNumber()); i++)
    cout<<NextCoeff->Value(i)<<" ";
  cout<<endl;
}

 void FEmTool_ProfileMatrix::OutS() const
{
  Standard_Integer i, j;
  cout<<"Matrix S"<<endl;
  for(i = 1; i <= RowNumber(); i++) {
    for(j = 1; j < i - profile(1, i); j++) 
      cout<<"0 ";

    for(j = profile(2, i) - profile(1, i); j <= profile(2, i); j++) 
      cout<<SMatrix->Value(j)<<" ";
    cout<<endl;
  }
}
