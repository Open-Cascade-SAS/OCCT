// File:	AdvApprox_PrefAndRec.cxx
// Created:	Thu Nov 14 13:29:18 1996
// Author:	Philippe MANGIN
//		<pmn@sgi29>


#include <AdvApprox_PrefAndRec.ixx>

#include <Precision.hxx>
#include <Standard_DomainError.hxx>


AdvApprox_PrefAndRec::AdvApprox_PrefAndRec(const TColStd_Array1OfReal& RecCut,
					   const TColStd_Array1OfReal& PrefCut,
					   const Standard_Real Weight):
  myRecCutting(1, RecCut.Length()),
  myPrefCutting(1, PrefCut.Length()),
  myWeight(Weight)
{
  myRecCutting =  RecCut;
  myPrefCutting =  PrefCut;
  if (myWeight <= 1) { Standard_DomainError::Raise("PrefAndRec : Weight is too small");}
}

Standard_Boolean AdvApprox_PrefAndRec::Value(const Standard_Real a,
					     const Standard_Real b,
					     Standard_Real& cuttingvalue) const
{
//  longueur minimum d'un intervalle parametrique : 10*PConfusion()
  Standard_Real lgmin = 10 * Precision::PConfusion();
  Standard_Integer i;
  Standard_Real cut, mil=(a+b)/2, dist;

  cut = mil;

// Recheche d'une decoupe preferentiel
  dist = Abs( (a*myWeight+b)/(1+myWeight) - mil ) ;
  for ( i=1; i<=myPrefCutting.Length(); i++) {
    if ( dist > Abs(mil-myPrefCutting.Value(i))) {
      cut = myPrefCutting.Value(i);
      dist = Abs(mil-cut);
    }
  }

// Recheche d'une decoupe recommende
  dist = Abs((a-b)/2);
  for ( i=1; i<=myRecCutting.Length(); i++) {
    if ((dist-lgmin) > Abs(mil-myRecCutting.Value(i))) {
      cut = myRecCutting.Value(i);
      dist = Abs(mil-cut);
    }
  }
    
// Resultat  
  cuttingvalue = cut;
  return (Abs(cut-a)>=lgmin && Abs(b-cut)>=lgmin);
}
