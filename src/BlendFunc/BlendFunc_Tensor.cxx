// Created on: 1996-12-05
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
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



#include <BlendFunc_Tensor.ixx>

#include <TColStd_Array1OfReal.hxx>
#include <math_Vector.hxx>
#include <math_Matrix.hxx>
//#include <math_StackMemoryManager.hxx>
//#include <math_Memory.hxx>

BlendFunc_Tensor::BlendFunc_Tensor(const Standard_Integer NbRow, 
				   const Standard_Integer NbCol, 
				   const Standard_Integer NbMat) :
                                   Tab(1,NbRow*NbMat*NbCol),
				   nbrow( NbRow ),
				   nbcol( NbCol ),
				   nbmat( NbMat ),
				   nbmtcl( NbMat*NbCol )
{
}

void BlendFunc_Tensor::Init(const Standard_Real InitialValue)
{
// Standard_Integer I, T = nbrow * nbcol *  nbmat;
// for (I=1; I<=T; I++) {Tab(I) = InitialValue;}
  Tab.Init(InitialValue);
}

void BlendFunc_Tensor::Multiply(const math_Vector& Right, 
				math_Matrix& M) const 
{
  Standard_Integer i, j, k;
  Standard_Real Somme;
  for ( i=1; i<=nbrow; i++) {
    for ( j=1; j<=nbcol; j++) {
       Somme = 0;
       for ( k=1; k<=nbmat; k++) {
	 Somme += Value(i,j,k)*Right(k);
       }
       M(i,j) = Somme;
     }
  }
}

