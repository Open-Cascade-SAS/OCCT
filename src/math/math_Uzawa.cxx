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

// lpa le 8/08/91

// Ce programme utilise l algorithme d Uzawa pour resoudre un systeme 
// dans le cas de contraintes. Si ce sont des contraintes d egalite, la 
// resolution est directe.
// Le programme ci-dessous utilise la methode de Crout pour trouver 
// l inverse d une matrice symetrique (Le gain est d environ 30% par
// rapport a Gauss.). Les calculs sur les matrices sont faits avec chaque
// coordonnee car il est plus long d utiliser les methodes deja ecrites 
// de la classe Matrix avec un passage par valeur.

//#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError

//#endif

#include <math_Crout.hxx>
#include <math_Matrix.hxx>
#include <math_Uzawa.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DimensionError.hxx>
#include <StdFail_NotDone.hxx>

math_Uzawa::math_Uzawa(const math_Matrix& Cont, const math_Vector& Secont,
		       const math_Vector& StartingPoint,
		       const Standard_Real EpsLix, const Standard_Real EpsLic,
		       const Standard_Integer NbIterations) :
                       Resul(1, Cont.ColNumber()),
                       Erruza(1, Cont.ColNumber()),
                       Errinit(1, Cont.ColNumber()),
                       Vardua(1, Cont.RowNumber()),
                       CTCinv(1, Cont.RowNumber(),
			      1, Cont.RowNumber()){

 Perform(Cont, Secont, StartingPoint, Cont.RowNumber(), 0, EpsLix,
	 EpsLic, NbIterations);
}

math_Uzawa::math_Uzawa(const math_Matrix& Cont, const math_Vector& Secont,
		       const math_Vector& StartingPoint,
		       const Standard_Integer Nce, const Standard_Integer Nci,
		       const Standard_Real EpsLix, const Standard_Real EpsLic,
		       const Standard_Integer NbIterations) :
                       Resul(1, Cont.ColNumber()),
                       Erruza(1, Cont.ColNumber()),
                       Errinit(1, Cont.ColNumber()),
                       Vardua(1, Cont.RowNumber()),
                       CTCinv(1, Cont.RowNumber(),
			      1, Cont.RowNumber())  {

  Perform(Cont, Secont, StartingPoint, Nce, Nci, EpsLix, EpsLic, NbIterations);

}


void math_Uzawa::Perform(const math_Matrix& Cont, const math_Vector& Secont,
	                 const math_Vector& StartingPoint,
		         const Standard_Integer Nce, const Standard_Integer Nci,
		         const Standard_Real EpsLix, const Standard_Real EpsLic,
		         const Standard_Integer NbIterations)  {

  Standard_Integer i,j, k;
  Standard_Real scale;
  Standard_Real Normat, Normli, Xian, Xmax=0, Xmuian;
  Standard_Real Rho, Err, Err1, ErrMax=0, Coef = 1./Sqrt(2.);
  Standard_Integer Nlig = Cont.RowNumber();
  Standard_Integer Ncol = Cont.ColNumber();

  Standard_DimensionError_Raise_if((Secont.Length() != Nlig) ||
				   ((Nce+Nci) != Nlig), " ");

  // Calcul du vecteur Cont*X0 - D:  (erreur initiale)
  //==================================================

    for (i = 1; i<= Nlig; i++) {
      Errinit(i) = Cont(i,1)*StartingPoint(1)-Secont(i);
      for (j = 2; j <= Ncol; j++) {
	Errinit(i) += Cont(i,j)*StartingPoint(j);
      }
    }

  if (Nci == 0) {                          // cas de resolution directe
    NbIter = 1;                            //==========================
    // Calcul de Cont*T(Cont)
      for (i = 1; i <= Nlig; i++) {
	for (j =1; j <= i; j++) {              // a utiliser avec Crout.
//      for (j = 1; j <= Nlig; j++) {        // a utiliser pour Gauss.
           CTCinv(i,j)= Cont(i,1)*Cont(j,1);
	   for (k = 2; k <= Ncol; k++) {
	     CTCinv(i,j) += Cont(i,k)*Cont(j,k);
	   }
	 }
       }
    // Calcul de l inverse de CTCinv :
    //================================
//      CTCinv = CTCinv.Inverse();           // utilisation de Gauss.
    math_Crout inv(CTCinv);                  // utilisation de Crout.
    CTCinv = inv.Inverse();
    for (i =1; i <= Nlig; i++) {
      scale = CTCinv(i,1)*Errinit(1);
      for (j = 2; j <= i; j++) {
	scale += CTCinv(i,j)*Errinit(j);
      }
      for (j =i+1; j <= Nlig; j++) {
	scale += CTCinv(j,i)*Errinit(j);
      }
      Vardua(i) = scale;
    }
    for (i = 1; i <= Ncol; i++) {
      Erruza(i) = -Cont(1,i)*Vardua(1);
      for (j = 2; j <= Nlig; j++) {
	Erruza(i) -= Cont(j,i)*Vardua(j);
      }
    }
    // restitution des valeurs calculees:
    //===================================
    Resul = StartingPoint + Erruza;
    Done = Standard_True;
    return;
  } // Fin de la resolution directe.
    //==============================

  else {  
    // Initialisation des variables duales.
    //=====================================
    for (i = 1; i <= Nlig; i++) {
      if (i <= Nce) {
	Vardua(i) = 0.0;
      }
      else {
	Vardua(i) = 1.;
      }
    }
    
    // Calcul du coefficient Rho:
    //===========================
    Normat = 0.0;
    for (i = 1; i <= Nlig; i++) {
      Normli = Cont(i,1)*Cont(i,1);
      for (j = 2; j <= Ncol; j++) {
	Normli += Cont(i,j)*Cont(i,j);
      }
      Normat += Normli;
    }
    Rho = Coef/Normat;
    
    // Boucle des iterations de la methode d Uzawa.
    //=============================================
    for (NbIter = 1; NbIter <= NbIterations; NbIter++) {
      for (i = 1; i <= Ncol; i++) {
	Xian = Erruza(i);
	Erruza(i) = -Cont(1,i)*Vardua(1);
	for(j =2; j <= Nlig; j++) {
	  Erruza(i) -= Cont(j,i)*Vardua(j);
	}
	if (NbIter > 1) {                      
	  if (i == 1) {
	    Xmax = Abs(Erruza(i) - Xian);
	  }
	  Xmax = Max(Xmax, Abs(Erruza(i)-Xian));
	}
      }

      // Calcul de Xmu a l iteration NbIter et evaluation de l erreur sur 
      // la verification des contraintes.
      //=================================================================
      for (i = 1; i <= Nlig; i++) {
	Err  = Cont(i,1)*Erruza(1) + Errinit(i);
	for (j = 2; j <= Ncol; j++) {
	  Err += Cont(i,j)*Erruza(j);
	}
	if (i <= Nce) {
	  Vardua(i) += Rho * Err;
	  Err1 = Abs(Rho*Err);
	}
	else {
	  Xmuian = Vardua(i);
	  Vardua(i) = Max(0.0, Vardua(i)+ Rho*Err);
	  Err1 = Abs(Vardua(i) - Xmuian);
	}
	if (i == 1) {
	  ErrMax = Err1;
	}
	ErrMax = Max(ErrMax, Err1);
      }

      if (NbIter > 1) {                   
	if (Xmax <= EpsLix) {
	  if (ErrMax <= EpsLic) {
//	    cout <<"Convergence atteinte dans Uzawa"<<endl;
	    Done = Standard_True;
	  }
	  else {
//	    cout <<"convergence non atteinte pour le probleme dual"<<endl;
	    Done = Standard_False;
	    return;
	  }
	  // Restitution des valeurs calculees
          //==================================
	  Resul = StartingPoint + Erruza;
	  Done = Standard_True;
	  return;
	}
      }

    } // fin de la boucle d iterations.
      Done = Standard_False;
  }
}


void math_Uzawa::Duale(math_Vector& V) const
{
  V = Vardua;
}

void math_Uzawa::Dump(Standard_OStream& o) const {

  o << "math_Uzawa";
  if(Done) {
    o << " Status = Done \n";
    o << " Number of iterations = " << NbIter << endl;
    o << " The solution vector is: " << Resul << endl;
  }
  else {
    o << " Status = not Done \n";
  }
}





