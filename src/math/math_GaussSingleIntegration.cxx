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
Par Gauss le calcul d'une integrale simple se transforme en sommation des 
valeurs de la fonction donnee aux <Order> points de Gauss affectee des poids
de Gauss.

Les points et poids de Gauss sont stockes dans GaussPoints.cxx.
Les points sont compris entre les valeurs -1 et +1, ce qui necessite un 
changement de variable pour les faire varier dans l'intervalle [Lower, Upper].


On veut calculer Integrale( f(u)* du) entre a et b.


Etapes du calcul:

1- calcul de la fonction au ieme point de Gauss (apres changement de variable).

2- multiplication de cette valeur par le ieme poids de Gauss.

3- sommation de toutes ces valeurs.

4- retour a l'intervalle [Lower, Upper] de notre integrale.

*/

//#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError

//#endif

#include <math.hxx>
#include <math_Function.hxx>
#include <math_GaussSingleIntegration.hxx>
#include <math_Vector.hxx>
#include <StdFail_NotDone.hxx>

math_GaussSingleIntegration::math_GaussSingleIntegration() : Done(Standard_False)  
{
}

math_GaussSingleIntegration::
                 math_GaussSingleIntegration(math_Function& F,
					     const Standard_Real Lower,
					     const Standard_Real Upper,
					     const Standard_Integer Order)
{
  Standard_Integer theOrder = Min(math::GaussPointsMax(), Order);
  Perform(F, Lower, Upper, theOrder);
}

math_GaussSingleIntegration::
                 math_GaussSingleIntegration(math_Function& F,
					     const Standard_Real Lower,
					     const Standard_Real Upper,
					     const Standard_Integer Order,
					     const Standard_Real Tol)
{
  Standard_Integer theOrder = Min(math::GaussPointsMax(), Order);

  const Standard_Integer IterMax = 13;   // Max number of iteration
  Standard_Integer NIter = 1;            // current number of iteration
  Standard_Integer NbInterval = 1;       // current number of subintervals
  Standard_Real    dU,OldLen,Len;

  Perform(F, Lower, Upper, theOrder);
  Len = Val;
  do {
    OldLen = Len;
    Len = 0.;
    NbInterval *= 2;
    dU = (Upper-Lower)/NbInterval;    
    for (Standard_Integer i=1; i<=NbInterval; i++) {
      Perform(F, Lower+(i-1)*dU, Lower+i*dU, theOrder);
      if (!Done) return;
      Len += Val;
    }
    NIter++;
  }    
  while (fabs(OldLen-Len) > Tol && NIter <= IterMax);

  Val = Len;
}

void math_GaussSingleIntegration::Perform(math_Function& F,
					  const Standard_Real Lower,
					  const Standard_Real Upper,
					  const Standard_Integer Order)
{
  Standard_Real xr, xm, dx;
  Standard_Integer j;
  Standard_Real F1, F2;
  Standard_Boolean Ok1;
  math_Vector GaussP(1, Order);
  math_Vector GaussW(1, Order);
  Done = Standard_False;

//Recuperation des points de Gauss dans le fichier GaussPoints.
  math::GaussPoints(Order,GaussP);
  math::GaussWeights(Order,GaussW);

// Calcul de l'integrale aux points de Gauss :

// Changement de variable pour la mise a l'echelle [Lower, Upper] :
  xm = 0.5*(Upper + Lower);
  xr = 0.5*(Upper - Lower);
  Val = 0.;

  Standard_Integer ind = Order/2, ind1 = (Order+1)/2;
  if(ind1 > ind) { // odder case
    Ok1 = F.Value(xm, Val);
    if (!Ok1) return;
    Val *= GaussW(ind1);
  }
// Sommation sur tous les points de Gauss: avec utilisation de la symetrie.
  for (j = 1; j <= ind; j++) {
    dx = xr*GaussP(j);
    Ok1 = F.Value(xm-dx, F1);
    if(!Ok1) return;
    Ok1 = F.Value(xm+dx, F2);
    if(!Ok1) return;
    // Multiplication par les poids de Gauss.
    Standard_Real FT = F1+F2;
    Val += GaussW(j)*FT;  
  }
  // Mise a l'echelle de l'intervalle [Lower, Upper]
  Val *= xr;
  Done = Standard_True;
}

void math_GaussSingleIntegration::Dump(Standard_OStream& o) const {

  o <<"math_GaussSingleIntegration ";
   if (Done) {
     o << " Status = Done \n";
     o << "Integration Value = " << Val<<"\n";
   }
   else {
     o << "Status = not Done \n";
   }
}
