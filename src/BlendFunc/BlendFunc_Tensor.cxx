// File:	BlendFunc_Tensor.cxx
// Created:	Thu Dec  5 09:52:07 1996
// Author:	Philippe MANGIN
//		<pmn@sgi29>


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

