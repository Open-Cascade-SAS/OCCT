// Created on: 1996-12-05
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
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


#include <BlendFunc_Tensor.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_RangeError.hxx>
#include <TColStd_Array1OfReal.hxx>

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

