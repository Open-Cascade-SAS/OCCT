// Created on: 1998-11-06
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


#include <FEmTool_ElementsOfRefMatrix.hxx>
#include <FEmTool_LinearFlexion.hxx>
#include <math.hxx>
#include <math_GaussSetIntegration.hxx>
#include <math_IntegerVector.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <PLib.hxx>
#include <PLib_HermitJacobi.hxx>
#include <PLib_JacobiPolynomial.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HArray2OfInteger.hxx>
#include <TColStd_HArray2OfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(FEmTool_LinearFlexion,FEmTool_ElementaryCriterion)

//=======================================================================
//function : FEmTool_LinearFlexion
//purpose  :
//=======================================================================
FEmTool_LinearFlexion::FEmTool_LinearFlexion(const Standard_Integer WorkDegree,
					     const GeomAbs_Shape ConstraintOrder)
                      : RefMatrix(0,WorkDegree,0,WorkDegree)
{
  static Standard_Integer Order = -333, WDeg = 14;
  static math_Vector MatrixElemts(0, ((WDeg+2)*(WDeg+1))/2 -1 );
  
  myOrder = PLib::NivConstr(ConstraintOrder);

  if (myOrder != Order) {
    //Calculating RefMatrix
    if (WorkDegree > WDeg) throw Standard_ConstructionError("Degree too high");
    Order = myOrder;
    Standard_Integer DerOrder = 2;
    Handle(PLib_HermitJacobi) theBase = new PLib_HermitJacobi(WDeg, ConstraintOrder);
    FEmTool_ElementsOfRefMatrix Elem = FEmTool_ElementsOfRefMatrix(theBase, DerOrder);
    Standard_Integer maxDegree = WDeg+1;
    math_IntegerVector anOrder(1,1,Min(4*(maxDegree/2+1),math::GaussPointsMax()));
    math_Vector Lower(1,1,-1.), Upper(1,1,1.); 
    math_GaussSetIntegration anInt(Elem, Lower, Upper, anOrder);
    
    MatrixElemts = anInt.Value();
  }

  Standard_Integer i, j, ii, jj;
  for(ii = i = 0; i <= WorkDegree; i++) {
    RefMatrix(i, i) =  MatrixElemts(ii);
    for(j = i+1, jj = ii+1; j <= WorkDegree; j++, jj++) {
      RefMatrix(j, i) = RefMatrix(i, j) =  MatrixElemts(jj);
    }
    ii += WDeg+1-i;
  }
}

//=======================================================================
//function : DependenceTable
//purpose  :
//=======================================================================
Handle(TColStd_HArray2OfInteger) FEmTool_LinearFlexion::DependenceTable() const
{
  if(myCoeff.IsNull()) throw Standard_DomainError("FEmTool_LinearFlexion::DependenceTable");

  Handle(TColStd_HArray2OfInteger) DepTab = 
    new TColStd_HArray2OfInteger(myCoeff->LowerCol(), myCoeff->UpperCol(),
				 myCoeff->LowerCol(), myCoeff->UpperCol(),0);
  Standard_Integer i;
  for(i = myCoeff->LowerCol(); i <= myCoeff->UpperCol(); i++) DepTab->SetValue(i,i,1);
  
  return DepTab;
}



//=======================================================================
//function : Value
//purpose  :
//=======================================================================
Standard_Real FEmTool_LinearFlexion::Value() 
{
  Standard_Integer deg = Min(myCoeff->ColLength() - 1, RefMatrix.UpperRow()), 
                   i, j, j0 = myCoeff->LowerRow(), degH = Min(2*myOrder+1, deg),
                   NbDim = myCoeff->RowLength(), dim;

  TColStd_Array2OfReal NewCoeff( 1, NbDim, 0, deg);

  Standard_Real coeff = (myLast - myFirst)/2., cteh3 = 2./Pow(coeff,3), 
                mfact, Jline;

  Standard_Integer k1;

  Standard_Real J = 0.;

  for(i = 0; i <= degH; i++) {
    k1 = (i <= myOrder)? i : i - myOrder - 1;
    mfact = Pow(coeff,k1);
    for(dim = 1; dim <= NbDim; dim++) 
      NewCoeff(dim, i) = myCoeff->Value(j0 + i, dim) * mfact;
  }

  for(i = degH + 1; i <= deg; i++) {
    for(dim = 1; dim <= NbDim; dim++) 
      NewCoeff(dim, i) = myCoeff->Value(j0 + i, dim);
  }

  for(dim = 1; dim <= NbDim; dim++) {
    for(i = 0; i <= deg; i++) {
      Jline = 0.5 * RefMatrix(i, i) * NewCoeff(dim, i);
      for(j = 0; j < i; j++) 
	Jline += RefMatrix(i, j) * NewCoeff(dim, j);
      J += Jline * NewCoeff(dim, i);
    }    
  }
  
  if(J < 0.) J = 0.;
  return cteh3*J;

}

//=======================================================================
//function : Hessian
//purpose  :
//=======================================================================

void FEmTool_LinearFlexion::Hessian(const Standard_Integer Dimension1,
				    const Standard_Integer Dimension2, math_Matrix& H) 
{
 
  Handle(TColStd_HArray2OfInteger) DepTab = DependenceTable();

  if(Dimension1 < DepTab->LowerRow() || Dimension1 > DepTab->UpperRow() || 
     Dimension2 < DepTab->LowerCol() || Dimension2 > DepTab->UpperCol()) 
    throw Standard_OutOfRange("FEmTool_LinearJerk::Hessian");

  if(DepTab->Value(Dimension1,Dimension2) == 0) 
    throw Standard_DomainError("FEmTool_LinearJerk::Hessian");

  Standard_Integer deg = Min(RefMatrix.UpperRow(), H.RowNumber() - 1), degH = Min(2*myOrder+1, deg);
  
  Standard_Real coeff = (myLast - myFirst)/2., cteh3 = 2./Pow(coeff,3), mfact;
  Standard_Integer k1, k2, i, j;
  
  H.Init(0.);
  
  for(i = 0; i <= degH; i++) {
    k1 = (i <= myOrder)? i : i - myOrder - 1;
    mfact = Pow(coeff,k1)*cteh3;
    // Hermite*Hermite part of matrix
    for(j = i; j <= degH; j++) {
      k2 = (j <= myOrder)? j : j - myOrder - 1;
      H(i, j) = mfact*Pow(coeff, k2)*RefMatrix(i, j);
      if (i != j) H(j, i) = H(i, j);
    }
    // Hermite*Jacobi part of matrix
    for(j = degH + 1; j <= deg; j++) {
      H(i, j) = H(j, i) = mfact*RefMatrix(i, j);
    }
  }
  

  // Jacoby*Jacobi part of matrix
  for(i = degH+1; i <= deg; i++) {
    for(j = i; j <= deg; j++) {
      H(i, j) = cteh3*RefMatrix(i, j);
      if (i != j) H(j, i) = H(i, j);
    }
  } 
}

//=======================================================================
//function : Gradient
//purpose  :
//=======================================================================
void FEmTool_LinearFlexion::Gradient(const Standard_Integer Dimension,math_Vector& G) 
{
  if(Dimension < myCoeff->LowerCol() || Dimension > myCoeff->UpperCol()) 
    throw Standard_OutOfRange("FEmTool_LinearFlexion::Gradient");

  Standard_Integer deg = Min(G.Length() - 1, myCoeff->ColLength() - 1);

  math_Vector X(0,deg);
  math_Matrix H(0,deg,0,deg);
  Standard_Integer i, i1 = myCoeff->LowerRow();
  for(i = 0; i <= deg; i++) X(i) = myCoeff->Value(i1+i, Dimension);

  Hessian(Dimension, Dimension, H);
  
  G.Multiply(H, X);
}

