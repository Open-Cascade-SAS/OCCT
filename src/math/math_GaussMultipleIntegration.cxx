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

/*
Ce programme permet le calcul de l'integrale nieme d'une fonction. La 
resolution peut etre vue comme l'integrale de l'integrale de ....(n fois), ce
qui necessite l'emploi de fonction recursive.

Une integrale simple de Gauss est la somme de la fonction donnee sur les points
de Gauss affectee du poids de Gauss correspondant.

I = somme(F(GaussPt) * GaussW)   sur i = 1 jusqu'a l'ordre d'integration.

Une integrale multiple est une somme recursive sur toutes les variables.

Tout le calcul est effectue dans la methode recursive_iteration appelee n fois.
 (n = nombre de variables)

Cette methode fait une sommation sur toutes les variables, sur tous les ordres
d'integration correspondants de la valeur de la fonction aux points et poids
de Gauss.

*/

//#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError

//#endif

#include <math.hxx>
#include <math_GaussMultipleIntegration.hxx>
#include <math_IntegerVector.hxx>
#include <math_Matrix.hxx>
#include <math_MultipleVarFunction.hxx>
#include <math_Vector.hxx>
#include <StdFail_NotDone.hxx>

class IntegrationFunction {

// Cette classe sert a conserver dans les champs les valeurs de tests de 
// la fonction recursive.(En particulier le pointeur sur la fonction F, 
// heritant d'une classe abstraite, non traite en cdl.)

  math_MultipleVarFunction *Fsav;
  math_IntegerVector Ordsav;
  Standard_Integer NVarsav;
  math_Vector xr;
  math_Vector xm;
  math_Matrix GaussPoint;
  math_Matrix GaussWeight;
  Standard_Real Val;
  Standard_Boolean Done;

 public:
    IntegrationFunction(math_MultipleVarFunction& F, 
			const Standard_Integer maxsav, const Standard_Integer NVar,
			const math_IntegerVector& Ord,
			const math_Vector& Lowsav,
			const math_Vector& Uppsav);

    Standard_Real Value() ;
    Standard_Boolean IsDone() const;
    Standard_Boolean recursive_iteration(Standard_Integer& n, math_IntegerVector& inc);
};

 IntegrationFunction::IntegrationFunction(math_MultipleVarFunction& F,
			const Standard_Integer maxsav, const Standard_Integer NVar,
			const math_IntegerVector& Ord,
			const math_Vector& Lowsav,const  math_Vector& Uppsav):
                        Ordsav(1, NVar),
                        xr(1, NVar),
                        xm(1, NVar),
                        GaussPoint(1, NVar, 1, maxsav),
                        GaussWeight(1, NVar, 1, maxsav)
{

Standard_Integer i, k;
math_IntegerVector inc(1, NVar);
inc.Init(0);
Fsav = &F;
NVarsav = NVar;
Ordsav = Ord;
Done = Standard_False;

//Recuperation des points et poids de Gauss dans le fichier GaussPoints 
  for (i =1; i<= NVarsav; i++) {
    xm(i) = 0.5*(Lowsav(i) + Uppsav(i));
    xr(i) = 0.5*(Uppsav(i) - Lowsav(i));
    math_Vector GP(1,Ordsav(i)), GW(1,Ordsav(i));
    math::GaussPoints(Ordsav(i),GP);
    math::GaussWeights(Ordsav(i),GW);
    for (k = 1; k <= Ordsav(i); k++) {
      GaussPoint(i,k)  = GP(k);  //kieme point et poids de  
      GaussWeight(i,k) = GW(k);//Gauss de la variable i.
    }
  }
  Val = 0.0;
  Standard_Integer Iterdeb = 1;
  Standard_Boolean recur = recursive_iteration(Iterdeb, inc);
  if (recur) {
//On ramene l'integration a la bonne echelle.
     for ( i = 1; i <= NVarsav; i++) {
       Val *= xr(i);
     }
     Done = Standard_True;
  }
}

Standard_Real IntegrationFunction::Value() {
  return Val;
}

Standard_Boolean IntegrationFunction::IsDone() const{
  return Done;
}

Standard_Boolean IntegrationFunction::recursive_iteration(Standard_Integer& n,
				   math_IntegerVector& inc) {

// Termination criterium :
// Calcul de la valeur de la fonction aux points de Gauss fixes:
	int local;
   if (n == (NVarsav+1)) {
     math_Vector dx(1, NVarsav);
     Standard_Integer j ;
     for ( j = 1; j <= NVarsav; j++) {
       dx(j) = xr(j)* GaussPoint(j, inc(j));
     }
     Standard_Real F1;
     Standard_Boolean Ok = Fsav->Value(xm + dx, F1);
     if (!Ok) {return Standard_False;}; 
     Standard_Real Interm = 1;
     for ( j = 1; j <= NVarsav; j++) {
       Interm *= GaussWeight(j, inc(j));
     }
     Val += Interm*F1;
     return Standard_True;
   }

   // Calcul recursif, pour chaque variable n de la valeur de la fonction aux
   // Ordsav(n) points de Gauss.

   Standard_Boolean OK=Standard_False;
   for (inc(n) = 1; inc(n) <= Ordsav(n); inc(n)++) {
	local = n + 1;
     OK = recursive_iteration(local, inc);
   }
   return OK;
}

math_GaussMultipleIntegration::
                math_GaussMultipleIntegration(math_MultipleVarFunction& F,
					      const math_Vector& Lower,
					      const math_Vector& Upper,
					      const math_IntegerVector& Order)
{
  Standard_Integer MaxOrder = math::GaussPointsMax();

  Standard_Integer i,  max =0;
  Standard_Integer NVar = F.NbVariables();  
  math_IntegerVector Ord(1, NVar);
  math_Vector Lowsav(1, NVar);
  math_Vector Uppsav(1, NVar);
  Lowsav = Lower;
  Uppsav = Upper;
//  Ord = Order;
  Done = Standard_False;
  for (i = 1; i <= NVar; i++) {
    if (Order(i) > MaxOrder) {
      Ord(i) = MaxOrder;
    }
    else {
      Ord(i) = Order(i);
    }
    if(Ord(i) >= max) {max = Ord(i);}
  }

//Calcul de l'integrale aux points de Gauss.
// Il s agit d une somme multiple sur le domaine [Lower, Upper].

  IntegrationFunction Func(F, max, NVar, Ord, Lowsav, Uppsav);
  if (Func.IsDone()) {
    Val = Func.Value();
    Done = Standard_True;
  }
}

void math_GaussMultipleIntegration::Dump(Standard_OStream& o) const {

  o <<"math_GaussMultipleIntegration ";
   if (Done) {
     o << " Status = Done \n";
     o << " Integration value = " << Val <<"\n";
   }
   else {
     o << "Status = not Done \n";
   }
}



