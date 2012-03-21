// Created on: 1996-11-14
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
