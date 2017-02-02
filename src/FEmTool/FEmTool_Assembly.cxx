// Created on: 1998-11-17
// Created by: Igor FEOKTISTOV
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


#include <FEmTool_Assembly.hxx>
#include <FEmTool_ListIteratorOfListOfVectors.hxx>
#include <FEmTool_ListOfVectors.hxx>
#include <FEmTool_ProfileMatrix.hxx>
#include <math_Matrix.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_DomainError.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_HArray1OfReal.hxx>

//----------------------------------------------------------------------------
// Purpose - to find min index of global variables and define
//----------------------------------------------------------------------------
static Standard_Integer MinIndex(const Handle(FEmTool_HAssemblyTable)& Table)
{
  Standard_Integer dim, el, nvar, Imin;
  Standard_Integer diml = Table->LowerRow(), dimu = Table->UpperRow(),
                   ell = Table->LowerCol(), elu = Table->UpperCol(), nvarl, nvaru;

  Handle(TColStd_HArray1OfInteger) T = Table->Value(diml, ell);

  nvarl = T->Lower();

  Imin = T->Value(nvarl);

  for(dim = diml; dim <= dimu; dim++)
    for(el = ell; el <= elu; el++) {
      T = Table->Value(dim, el);
      nvarl = T->Lower(); nvaru = T->Upper();
      for(nvar = nvarl; nvar <= nvaru; nvar++) {
	Imin = Min(Imin, T->Value(nvar));
      }
    }
  return Imin;
}

//----------------------------------------------------------------------------
// Purpose - to find max index of global variables 
//----------------------------------------------------------------------------
static Standard_Integer MaxIndex(const Handle(FEmTool_HAssemblyTable)& Table)
{
  Standard_Integer dim, el, nvar, Imax;
  Standard_Integer diml = Table->LowerRow(), dimu = Table->UpperRow(),
                   ell = Table->LowerCol(), elu = Table->UpperCol(), nvarl, nvaru;

  Handle(TColStd_HArray1OfInteger) T = Table->Value(diml, ell);

  nvarl = T->Lower();

  Imax = T->Value(nvarl);

  for(dim = diml; dim <= dimu; dim++)
    for(el = ell; el <= elu; el++) {
      T = Table->Value(dim, el);
      nvarl = T->Lower(); nvaru = T->Upper();
      for(nvar = nvarl; nvar <= nvaru; nvar++) {
	Imax = Max(Imax, T->Value(nvar));
      }
    }
  return Imax;
}



//=======================================================================
//function : FEmTool_Assembly
//purpose  : 
//=======================================================================
FEmTool_Assembly::FEmTool_Assembly(const TColStd_Array2OfInteger& Dependence,
				   const Handle(FEmTool_HAssemblyTable)& Table):
     myDepTable(1, Dependence.ColLength(), 1, Dependence.RowLength()),
     B(MinIndex(Table), MaxIndex(Table))
     
{
  IsSolved = Standard_False;
  myDepTable = Dependence;
  myRefTable = Table;

  TColStd_Array1OfInteger FirstIndexes(1, B.Length()); FirstIndexes.Init(B.Length());

  Standard_Integer dim, el, nvar, Imin, I0 = 1 - B.Lower(), i;

  Standard_Integer diml = Table->LowerRow(), dimu = Table->UpperRow(),
                   ell = Table->LowerCol(), elu = Table->UpperCol(), nvarl, nvaru;

  Handle(TColStd_HArray1OfInteger) T;
  for(dim = diml; dim <= dimu; dim++)
    for(el = ell; el <= elu; el++) {

      T = Table->Value(dim, el);
      nvarl = T->Lower(); nvaru = T->Upper();
      Imin = T->Value(nvarl) + I0;
     
      for(nvar = nvarl; nvar <= nvaru; nvar++) 
	Imin = Min(Imin, T->Value(nvar) + I0);
      
      for(nvar = nvarl; nvar <= nvaru; nvar++) {
	i = T->Value(nvar) + I0;
	FirstIndexes(i) = Min(FirstIndexes(i), Imin);
      }
    }


  H = new FEmTool_ProfileMatrix(FirstIndexes);

  NullifyMatrix();
  NullifyVector();
}

void FEmTool_Assembly::NullifyMatrix() 
{
  H->Init(0.);
  IsSolved = Standard_False;
}


//=======================================================================
//function : AddMatrix
//purpose  : 
//=======================================================================
void FEmTool_Assembly::AddMatrix(const Standard_Integer Element,
				 const Standard_Integer Dimension1,
				 const Standard_Integer Dimension2,
				 const math_Matrix& Mat)
{

  if(myDepTable(Dimension1, Dimension2) == 0) 
    throw Standard_DomainError("FEmTool_Assembly::AddMatrix");

  const TColStd_Array1OfInteger & T1 = myRefTable->Value(Dimension1,Element)->Array1();
  const TColStd_Array1OfInteger & T2 = myRefTable->Value(Dimension2,Element)->Array1();

  Standard_Integer nvarl = T1.Lower(), nvaru = Min(T1.Upper(), nvarl + Mat.RowNumber() - 1);


  Standard_Integer I, J, I0 = 1 - B.Lower(), i, ii, j,

                   i0 = Mat.LowerRow() - nvarl, j0 = Mat.LowerCol() - nvarl;

  for(i = nvarl; i <= nvaru; i++) {
    I = T1(i) + I0;
    ii = i0+i;
    for(j = nvarl; j <= i; j++) {
      J = T2(j) + I0;
      H->ChangeValue(I, J) += Mat(ii, j0+j);
    }
  }

  IsSolved = Standard_False;
}


//=======================================================================
//function : NullifyVector
//purpose  : 
//=======================================================================
void FEmTool_Assembly::NullifyVector() 
{
  B.Init(0.);
}

//=======================================================================
//function : AddVector
//purpose  : 
//=======================================================================
void FEmTool_Assembly::AddVector(const Standard_Integer Element,
				 const Standard_Integer Dimension,
				 const math_Vector& Vec) 
{
  const TColStd_Array1OfInteger & T = myRefTable->Value(Dimension,Element)->Array1();
  Standard_Integer nvarl = T.Lower(), nvaru = Min(T.Upper(), nvarl + Vec.Length() - 1),
                   i0 = Vec.Lower() - nvarl;

//  Standard_Integer I, i;
  Standard_Integer i;
  
  for(i = nvarl; i <= nvaru; i++) 
    B(T(i)) += Vec(i0 + i);
  
}


//=======================================================================
//function : Solve
//purpose  : 
//=======================================================================
Standard_Boolean FEmTool_Assembly::Solve() 
{
  IsSolved = H->Decompose();

#ifdef OCCT_DEBUG
  if (!IsSolved) {
    cout << "Solve Echec  H = " << endl;
    H->OutM();
  }
#endif

/* ????
  while(!IsSolved && count < 5) {
    Standard_Integer i;
    for(i = 1; i <= H->RowNumber(); i++) {
      H->ChangeValue(i,i) *= 2.;
    }
    IsSolved = H->Decompose();
    count++;
  }
*/    

// Standard_Integer count = 0;
  if(!G.IsEmpty() && IsSolved) {
    // calculating H-1 Gt
    math_Vector gi(B.Lower(), B.Upper()), qi(B.Lower(), B.Upper());

    if(GHGt.IsNull() || GHGt->RowNumber() != G.Length()) {
      TColStd_Array1OfInteger FirstIndexes(1, G.Length());
//-----------------------------------------------------------------
      TColStd_Array2OfInteger H1(1, NbGlobVar(), 1, NbGlobVar()); H1.Init(1);
      Standard_Integer i, j, k, l, BlockBeg = 1, BlockEnd;
      Standard_Boolean Block, Zero;
      for(i = 2; i <= NbGlobVar(); i++) {
	BlockEnd = i - 1;
	if(!H->IsInProfile(i, BlockEnd)) {
	  // Maybe, begin of block
	  Block = Standard_True;
	  for(j = i + 1; j <= NbGlobVar(); j++) {
	    if(H->IsInProfile(j, BlockEnd)) {
	      Block = Standard_False;
	      break;
	    }
	  }
	  if(Block) {
	    for(j = i; j <= NbGlobVar(); j++) {
	      for(k = BlockBeg; k <= BlockEnd; k++) {
		H1(j, k) = 0; H1(k, j) = 0;
	      } 
	    }
	    BlockBeg = BlockEnd + 1;
	  }
	  else i = j;
	}
      }
      
      FEmTool_ListIteratorOfListOfVectors Iter1;
      FEmTool_ListIteratorOfListOfVectors Iter2;
      for(i = 1; i <= G.Length(); i++) {
	const FEmTool_ListOfVectors& Gi = G.Value(i);
	for(j = 1; j <= i; j++) {
	  const FEmTool_ListOfVectors& Gj = G.Value(j);
	  Zero = Standard_True;
	  for(Iter1.Initialize(Gi); Iter1.More(); Iter1.Next()) {
	    const Handle(TColStd_HArray1OfReal)& a = Iter1.Value();
	    for(k = a->Lower(); k <= a->Upper(); k++) {
	      for(Iter2.Initialize(Gj); Iter2.More(); Iter2.Next()) {
		const Handle(TColStd_HArray1OfReal)& b = Iter2.Value();
		for(l = b->Lower(); l <= b->Upper(); l++) {
		  if(H1(k, l) != 0) {
		    Zero = Standard_False;
		    break;
		  }
		}
		if(!Zero) break;
	      }
	      if(!Zero) break;
	    }
	    if(!Zero) break;
	  }
	  if(!Zero) {
	    FirstIndexes(i) = j;
	    break;
	  }
	}
      }
//-----------------------------------------------------------------------
//      for(i = FirstIndexes.Lower(); i <= FirstIndexes.Upper(); i++)
//	cout << "FirstIndexes(" << i << ") = " << FirstIndexes(i) << endl;
      //      FirstIndexes.Init(1); // temporary GHGt is full matrix
      GHGt = new FEmTool_ProfileMatrix(FirstIndexes);
    }

    GHGt->Init(0.);
    Standard_Integer i, j, k;
    FEmTool_ListIteratorOfListOfVectors Iter;
    
    for(i = 1; i <= G.Length(); i++) {

      const FEmTool_ListOfVectors& L = G.Value(i);
      gi.Init(0.);
      // preparing i-th line of G (or column of Gt) 
      for(Iter.Initialize(L); Iter.More(); Iter.Next()) {

	const Handle(TColStd_HArray1OfReal)& a = Iter.Value();

	for(j = a->Lower(); j <= a->Upper(); j++) gi(j) = a->Value(j); // gi - full line of G 

      }
                        //                                     -1 t
      H->Solve(gi, qi); // solving H*qi = gi, qi is column of H  G


      //                               -1 t
      // Calculation of product M = G H G
      // for each i all elements of i-th column of M are calculated for k >= i
      for(k = i; k <= G.Length(); k++) {

	if(GHGt->IsInProfile(k, i)) {
	  Standard_Real m = 0.; // m = M(k,i)
	
	  const FEmTool_ListOfVectors& aL = G.Value(k);
	
	  for(Iter.Initialize(aL); Iter.More(); Iter.Next()) {

	    const Handle(TColStd_HArray1OfReal)& a = Iter.Value();
	    for(j = a->Lower(); j <= a->Upper(); j++) m += qi(j) * a->Value(j); // scalar product of
	                                             // k-th line of G and i-th column of H-1 Gt 

	  }
	
	  GHGt->ChangeValue(k, i) = m;
	}
      }

    }


    IsSolved = GHGt->Decompose();
/*    count = 0;
    while(!IsSolved && count < 5) {
      for(i = 1; i <= GHGt->RowNumber(); i++) {
	GHGt->ChangeValue(i,i) *= 2.;
      }
      IsSolved = GHGt->Decompose();
      count++;
    }*/

  }

  return IsSolved;

}


//=======================================================================
//function : Solution
//purpose  : 
//=======================================================================
void FEmTool_Assembly::Solution(math_Vector& Solution) const
{
  if(!IsSolved) throw StdFail_NotDone("FEmTool_Assembly::Solution");

  if(G.IsEmpty()) H->Solve(B, Solution);
  else {

    math_Vector v1(B.Lower(), B.Upper());
    H->Solve(B, v1);
    
    math_Vector l(1, G.Length()), v2(1, G.Length());
    Standard_Integer i, j;
    FEmTool_ListIteratorOfListOfVectors Iter;
    
    for(i = 1; i <= G.Length(); i++) {
      
      const FEmTool_ListOfVectors& L = G.Value(i);
      Standard_Real m = 0.;
      
      for(Iter.Initialize(L); Iter.More(); Iter.Next()) {
	
	const Handle(TColStd_HArray1OfReal)& a = Iter.Value();
	for(j = a->Lower(); j <= a->Upper(); j++) 
	  m += v1(j) * a->Value(j); // scalar product
	// G v1
      }
      
      v2(i) = m - C.Value(i);
    }
    
    GHGt->Solve(v2, l); // Solving M*l = v2
    
    v1 = B;
    // Calculation v1 = B-Gt*l
    // v1(j) = B(j) - Gt(j,i)*l(i) = B(j) - G(i,j)*l(i)
    // 
      for(i = 1; i <= G.Length(); i++) {
	
	const FEmTool_ListOfVectors& L = G.Value(i);
	
	for(Iter.Initialize(L); Iter.More(); Iter.Next()) {
	  
	  const Handle(TColStd_HArray1OfReal)& a = Iter.Value();
	  for(j = a->Lower(); j <= a->Upper(); j++) v1(j) -= l(i) * a->Value(j);
	  
	}
	
      }
      
    H->Solve(v1, Solution);
  }
  
}

Standard_Integer FEmTool_Assembly::NbGlobVar() const
{

  return B.Length();

}

void FEmTool_Assembly::GetAssemblyTable(Handle(FEmTool_HAssemblyTable)& AssTable) const
{
  AssTable = myRefTable;
}


void FEmTool_Assembly::ResetConstraint() 
{
  G.Clear();
  C.Clear();
}

void FEmTool_Assembly::NullifyConstraint() 
{
  FEmTool_ListIteratorOfListOfVectors Iter;
  Standard_Integer i;

  for(i = 1; i <= G.Length(); i++) {
  
    C.SetValue(i, 0.);

    for(Iter.Initialize(G.Value(i)); Iter.More(); Iter.Next()) 
      Iter.Value()->Init(0.);

  }

}


//=======================================================================
//function : AddConstraint
//purpose  : 
//=======================================================================
void FEmTool_Assembly::AddConstraint(const Standard_Integer IndexofConstraint,
				     const Standard_Integer Element,
				     const Standard_Integer Dimension,
				     const math_Vector& LinearForm,
				     const Standard_Real Value) 
{
  while(G.Length() < IndexofConstraint) {
    // Add new lines in G
    FEmTool_ListOfVectors L;
    G.Append(L);
    C.Append(0.);
  }

  FEmTool_ListOfVectors& L = G.ChangeValue(IndexofConstraint);
  
  Handle(TColStd_HArray1OfInteger) Indexes = myRefTable->Value(Dimension,Element);
  Standard_Integer i, Imax = 0, Imin = NbGlobVar();

  for(i = Indexes->Lower(); i <= Indexes->Upper(); i++) {
    Imin = Min(Imin, Indexes->Value(i));
    Imax = Max(Imax, Indexes->Value(i));
  }

  Handle(TColStd_HArray1OfReal) Coeff;

  if(L.IsEmpty()) {
    Coeff = new TColStd_HArray1OfReal(Imin,Imax);
    Coeff->Init(0.);
    L.Append(Coeff);
  }
  else {
    FEmTool_ListIteratorOfListOfVectors Iter(L);
    Standard_Real  s1 = 0, s2 = 0;
    Handle(TColStd_HArray1OfReal) Aux1, Aux2;
    for(i=1; Iter.More(); Iter.Next(), i++) {
      if(Imin >= Iter.Value()->Lower()) {
	s1 = i;
	Aux1 = Iter.Value();
	if(Imax <= Iter.Value()->Upper()) {
	  s2 = s1;
	  Coeff = Iter.Value();
	  break;
	}
      }

      if(Imax <= Iter.Value()->Upper()) {
	s2 = i;
	Aux2 = Iter.Value();
      }
    }

    if(s1 != s2) {
      if(s1 == 0) {
	if(Imax < Aux2->Lower()) {
	  // inserting before first segment
	  Coeff = new TColStd_HArray1OfReal(Imin,Imax);
	  Coeff->Init(0.);
	  L.Prepend(Coeff);
	}
	else {
	  // merge new and first segment
	  Coeff = new TColStd_HArray1OfReal(Imin, Aux2->Upper());
	  for(i = Imin; i <= Aux2->Lower() - 1; i++) Coeff->SetValue(i, 0.);
	  for(i = Aux2->Lower(); i <= Aux2->Upper(); i++) Coeff->SetValue(i, Aux2->Value(i));
	  L.First() = Coeff;
	}
      }
      else if(s2 == 0) {
	if(Imin > Aux1->Upper()) {
	  // append new
	  Coeff = new TColStd_HArray1OfReal(Imin,Imax);
	  Coeff->Init(0.);
	  L.Append(Coeff);
	}
	else {
	  // merge new and last segment
	  Coeff = new TColStd_HArray1OfReal(Aux1->Lower(), Imax);
	  for(i = Aux1->Lower(); i <= Aux1->Upper(); i++) Coeff->SetValue(i, Aux1->Value(i));
	  for(i = Aux1->Upper() + 1; i <= Imax; i++) Coeff->SetValue(i, 0.);
	  L.Last() = Coeff;
	}
      }
      else if(Imin <= Aux1->Upper() && Imax < Aux2->Lower()) {
	// merge s1 and new
	Coeff = new TColStd_HArray1OfReal(Aux1->Lower(), Imax);
	for(i = Aux1->Lower(); i <= Aux1->Upper(); i++) Coeff->SetValue(i, Aux1->Value(i));
	for(i = Aux1->Upper() + 1; i <= Imax; i++) Coeff->SetValue(i, 0.);
	Iter.Initialize(L);
	for(i = 1; i < s1; Iter.Next(), i++);
	Iter.Value() = Coeff;
      }
      else if(Imin > Aux1->Upper() && Imax >= Aux2->Lower()) { 
	// merge new and first segment
	Coeff = new TColStd_HArray1OfReal(Imin, Aux2->Upper());
	for(i = Imin; i <= Aux2->Lower() - 1; i++) Coeff->SetValue(i, 0.);
	for(i = Aux2->Lower(); i <= Aux2->Upper(); i++) Coeff->SetValue(i, Aux2->Value(i));
	Iter.Initialize(L);
	for(i = 1; i < s2; Iter.Next(), i++);
	Iter.Value() = Coeff;
     }
      else if(Imin > Aux1->Upper() && Imax < Aux2->Lower()) {
	// inserting new between s1 and s2
	Coeff = new TColStd_HArray1OfReal(Imin,Imax);
	Coeff->Init(0.);
	Iter.Initialize(L);
	for(i = 1; i < s1; Iter.Next(), i++);
	L.InsertAfter(Coeff,Iter);
      }
      else {
	// merge s1, new, s2 and remove s2 
	Coeff = new TColStd_HArray1OfReal(Aux1->Lower(), Aux2->Upper());
	for(i = Aux1->Lower(); i <= Aux1->Upper(); i++) Coeff->SetValue(i, Aux1->Value(i));
	for(i = Aux1->Upper() + 1; i <= Aux2->Lower() - 1; i++) Coeff->SetValue(i, 0.);
	for(i = Aux2->Lower(); i <= Aux2->Upper(); i++) Coeff->SetValue(i, Aux2->Value(i));
	Iter.Initialize(L);
	for(i = 1; i < s1; Iter.Next(), i++);
	Iter.Value() = Coeff;
	Iter.Next();
	L.Remove(Iter);
      }
    }
  }

  // adding 
  Standard_Integer j = LinearForm.Lower();
  for(i = Indexes->Lower(); i <= Indexes->Upper(); i++, j++) {
    Coeff->ChangeValue(Indexes->Value(i)) += LinearForm(j);
  }
  
  C.ChangeValue(IndexofConstraint) += Value;



}

