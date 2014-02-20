// Copyright (c) 1995-1999 Matra Datavision
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

//============================================================================
//======================================================= IntAna2d_Outils.cxx
//============================================================================
#include <IntAna2d_Outils.hxx>
#include <math_DirectPolynomialRoots.hxx>

MyDirectPolynomialRoots::MyDirectPolynomialRoots(const Standard_Real A4,
						 const Standard_Real A3,
						 const Standard_Real A2,
						 const Standard_Real A1,
						 const Standard_Real A0) { 
  //-- cout<<" IntAna2d : A4..A0 "<<A4<<" "<<A3<<" "<<A2<<" "<<A1<<" "<<A0<<" "<<endl;
  nbsol = 0;
  same = Standard_False;
//  Modified by Sergey KHROMOV - Thu Oct 24 13:10:14 2002 Begin
  Standard_Real anAA[5];

  anAA[0] = Abs(A0);
  anAA[1] = Abs(A1);
  anAA[2] = Abs(A2);
  anAA[3] = Abs(A3);
  anAA[4] = Abs(A4);

//   if((Abs(A4)+Abs(A3)+Abs(A2)+Abs(A1)+Abs(A0))<Epsilon(10000.0))  { 
  if((anAA[0]+anAA[1]+anAA[2]+anAA[3]+anAA[4])<Epsilon(10000.0))  { 
//  Modified by Sergey KHROMOV - Thu Oct 24 13:10:15 2002 End
    same = Standard_True;
    return;
  }
  Standard_Integer i,j,nbp;
  for(i=0;i<16;i++) val[i]=RealLast();
  
  Standard_Real tol = Epsilon(100.0);
  math_DirectPolynomialRoots MATH_A43210(A4,A3,A2,A1,A0);
  Standard_Boolean PbPossible = Standard_False;
  Standard_Integer NbsolPolyComplet = 0;
  if(MATH_A43210.IsDone()) { 
    nbp = MATH_A43210.NbSolutions();
    NbsolPolyComplet = nbp;
    for(i=1;i<=nbp;i++) { 
      Standard_Real x = MATH_A43210.Value(i);
      //-- cout<<" IntAna2d : x Pol Complet :"<<x<<endl;
      val[nbsol] = A0 + x*(A1+x*(A2+x*(A3+x*A4)));
      sol[nbsol] = x;
      if(val[nbsol]> tol  ||  val[nbsol]<-tol) {
	PbPossible = Standard_True;
      }
      nbsol++;
    }
    if(nbp & 1) 
      PbPossible = Standard_True;
  }
  else { 
    PbPossible = Standard_True;
  }
  //-- On recherche le plus petit coeff entre A4 et A0 
  if(PbPossible) { 
//  Modified by Sergey KHROMOV - Thu Oct 24 12:45:35 2002 Begin
    Standard_Real anAMin = RealLast();
    Standard_Real anAMax = -1;
    Standard_Real anEps  = RealEpsilon();

    for (i = 0; i < 5; i++) {
      anAMin = Min(anAMin, Max(anAA[i], anEps));
      anAMax = Max(anAMax, Max(anAA[i], anEps));
    }

    anEps = Min(1.e-4, Epsilon(1000.*anAMax/anAMin));
//  Modified by Sergey KHROMOV - Thu Oct 24 15:46:24 2002 End
    math_DirectPolynomialRoots MATH_A4321(A4,A3,A2,A1);
    if(MATH_A4321.IsDone()) { 
      nbp = MATH_A4321.NbSolutions();
      //-- On Ajoute les valeurs au tableau 
      for(i=1;i<=nbp;i++) { 
	Standard_Real x = MATH_A4321.Value(i);
	Standard_Boolean Add = Standard_True;
	for(j=0;j<nbsol;j++) { 
	  Standard_Real t = sol[j]-x;
//  Modified by Sergey KHROMOV - Thu Oct 24 12:04:26 2002 Begin
// 	  if(Abs(t)<tol) { 
	  if(Abs(t) < anEps) { 
//  Modified by Sergey KHROMOV - Thu Oct 24 12:04:47 2002 End
	    Add = Standard_False;
	  }
	}
	if(Add) {
	  val[nbsol] =  A0 + x*(A1+x*(A2+x*(A3+x*A4)));
	  sol[nbsol] = x;
	  nbsol++;
	}
      }
    }
    math_DirectPolynomialRoots MATH_A3210(A3,A2,A1,A0);
    if(MATH_A3210.IsDone()) { 
      nbp = MATH_A3210.NbSolutions();
      //-- On Ajoute les valeurs au tableau 
      for(i=1;i<=nbp;i++) { 
	Standard_Real x = MATH_A3210.Value(i);
	Standard_Boolean Add = Standard_True;
	for(j=0;j<nbsol;j++) { 
	  Standard_Real t = sol[j]-x;
//  Modified by Sergey KHROMOV - Thu Oct 24 12:06:01 2002 Begin
// 	  if(Abs(t)<tol) { 
	  if(Abs(t) < anEps) { 
//  Modified by Sergey KHROMOV - Thu Oct 24 12:06:04 2002 End
	    Add = Standard_False;
	  }
	}
	if(Add) { 
	  val[nbsol] =  A0 + x*(A1+x*(A2+x*(A3+x*A4)));
	  sol[nbsol] = x;
	  nbsol++;
	}
      }
    }
    math_DirectPolynomialRoots MATH_A210(A3,A2,A1);
    if(MATH_A210.IsDone()) { 
      nbp = MATH_A210.NbSolutions();
      //-- On Ajoute les valeurs au tableau 
      for(i=1;i<=nbp;i++) { 
	Standard_Real x = MATH_A210.Value(i);
	Standard_Boolean Add = Standard_True;
	for(j=0;j<nbsol;j++) { 
	  Standard_Real t = sol[j]-x;
//  Modified by Sergey KHROMOV - Thu Oct 24 12:07:04 2002 Begin
// 	  if(Abs(t)<tol) { 
	  if(Abs(t) < anEps) { 
//  Modified by Sergey KHROMOV - Thu Oct 24 12:07:06 2002 End
	    Add = Standard_False;
	  }
	}
	if(Add) {
	  val[nbsol] =  A0 + x*(A1+x*(A2+x*(A3+x*A4)));
	  sol[nbsol] = x;
	  nbsol++;
	}
      }
    }
    //------------------------------------------------------------
    //-- On trie les valeurs par ordre decroissant de val
    //-- for(i=0;i<nbsol;i++) { 
    //--  cout<<" IntAna2d Sol,Val"<<sol[i]<<"  "<<val[i]<<endl;
    //-- }
    Standard_Boolean TriOK = Standard_False;
    do {
      TriOK = Standard_True;
      for(i=1; i<nbsol;i++) { 
	if(Abs(val[i])<Abs(val[i-1])) { 
	  Standard_Real t;
	  t        = val[i];
	  val[i]   = val[i-1];
	  val[i-1] = t;
	  t        = sol[i];
	  sol[i]   = sol[i-1];
	  sol[i-1] = t;
	  TriOK = Standard_False;
	}
      }
    }
    while(!TriOK);
    //-----------------------------------------------------------
    //-- On garde les premieres valeurs
    //-- Au moins autant que le polynome Complet 
    //-- 
    for(nbsol=0; nbsol<NbsolPolyComplet || Abs(val[nbsol])<Epsilon(10000.0); nbsol++) ;
    //-- cout<<" IntAna2d : nbsol:"<<nbsol<<endl;
  }
  if(nbsol==0) { 
    nbsol=-1;
  }
  if(nbsol>4) { 
    same=1;
    nbsol=0;
  }
}


MyDirectPolynomialRoots::MyDirectPolynomialRoots(const Standard_Real A2,
						 const Standard_Real A1,
						 const Standard_Real A0) { 
  //-- cout<<" IntAna2d : A2..A0 "<<A2<<" "<<A1<<" "<<A0<<" "<<endl;
  nbsol=0;
  if((Abs(A2)+Abs(A1)+Abs(A0))<Epsilon(10000.0))  { 
    same = Standard_True;
    return;
  }  
  math_DirectPolynomialRoots MATH_A210(A2,A1,A0);
  same = Standard_False;
  if(MATH_A210.IsDone()) { 
    for(Standard_Integer i=1;i<=MATH_A210.NbSolutions(); i++) { 
      Standard_Real x = MATH_A210.Value(i);
      val[nbsol] = A0 + x*(A1+x*A2);
      sol[nbsol] = x;
      //-- cout<<" IntAna2d : x Pol Complet :"<<x<<"  Val:"<<val[nbsol]<<endl;
      nbsol++;
    }
  }
  else { 
    nbsol = -1;
  }
}

Standard_Boolean Points_Confondus(const Standard_Real x1,const Standard_Real y1
				  ,const Standard_Real x2,const Standard_Real y2) {
  if(Abs(x1-x2)<Epsilon(x1)) {
    if(Abs(y1-y2)<Epsilon(y1)) {
      return(Standard_True);
    }
  }
  return(Standard_False);
}

//-----------------------------------------------------------------------------
//--- Les points confondus sont supprimes
//--- Le nombre de points est mis a jour

void Traitement_Points_Confondus(Standard_Integer& nb_pts,
				 IntAna2d_IntPoint* pts) {
  Standard_Integer i,j;
  for(i=nb_pts;i>1;i--) {  
    Standard_Boolean Non_Egalite=Standard_True;
    for(j=i-1;(j>0) && Non_Egalite;j--) { 
      //                                        <--- Deja Teste --->
      //             | 1  |2  |  | J |  |I-1| I |I+1|          |NPTS|
      //             | 1  |2  |  | J |  |I-1|XXX|I+1|          |NPTS|
      //             | 1  |2  |  | J |  |I-1|I+1|I+2|     |NPTS|
      if(Points_Confondus(pts[i-1].Value().X(),
			  pts[i-1].Value().Y(),
			  pts[j-1].Value().X(),
			  pts[j-1].Value().Y())) {
	Standard_Integer k;
	Non_Egalite=Standard_False;
	for(k=i;k<nb_pts;k++) {
	  Standard_Real Xk,Yk,Uk;
	  Xk=pts[k].Value().X();
	  Yk=pts[k].Value().Y();
	  Uk=pts[k].ParamOnFirst();
	  pts[k-1].SetValue(Xk,Yk,Uk);
	}
	nb_pts--;
      }
    }
  }
}

//-----------------------------------------------------------------------------
void Coord_Ancien_Repere(Standard_Real& x1,Standard_Real& y1,const gp_Ax2d Dir1) {
  Standard_Real t11,t12,t21,t22,t13,t23;
  Standard_Real x0,y0;  

  // x1 et y1 Sont les Coordonnees dans le repere lie a Dir1
  // On Renvoie ces Coordonnees dans le repere "absolu"

  Dir1.Direction().Coord(t11,t21);
  Dir1.Location().Coord(t13,t23);

  t22=t11;
  t12=-t21;

  x0= t11*x1 + t12*y1 + t13;
  y0= t21*x1 + t22*y1 + t23;

  x1=x0;
  y1=y0;
}



#if 0      

//-- A Placer dans les ressources de la classe Conic   ??
//-----------------------------------------------------------------------------
//--- Calcul des Coefficients A,..F dans le repere lie a  Dir1
//--- A Partir des Coefficients dans le repere "Absolu"

void Coeff_Nouveau_Repere(Standard_Real& A,Standard_Real& B,Standard_Real& C
			  ,Standard_Real& D,Standard_Real& E,Standard_Real& F
			  ,const gp_Ax2d Dir1)  {
  Standard_Real t11,t12,t13;                  // x = t11 X + t12 Y + t13
  Standard_Real t21,t22,t23;                  // y = t21 X + t22 Y + t23
  Standard_Real A1,B1,C1,D1,E1,F1;            

  // On a P0(x,y)=A x x + B y y + ... + F =0    (x et y ds le repere "Absolu")
  // et on cherche P1(X(x,y),Y(x,y))=P0(x,y)
  // Avec P1(X,Y)= A1 X X + B1 Y Y + 2 C1 X Y + 2 D1 X + 2 E1 Y + F1
  //             = A  x x + B  y y + 2 C  x y + 2 D  x + 2 E  y + f

  Dir1.Direction().Coord(t11,t21);
  Dir1.Location().Coord(t13,t23);

  t22=t11;
  t12=-t21;

  A1=(t11*(A*t11 + 2*C*t21) + B*t21*t21);
  B1=(t12*(A*t12 + 2*C*t22) + B*t22*t22);
  C1=(t12*(A*t11 + C*t21) + t22*(C*t11 + B*t21));
  D1=(t11*(D + A*t13) + t21*(E + C*t13) + t23*(C*t11 + B*t21));
  E1=(t12*(D + A*t13) + t22*(E + C*t13) + t23*(C*t12 + B*t22));
  F1=F + t13*(2.0*D + A*t13) + t23*(2.0*E + 2.0*C*t13 + B*t23);
  
  A=A1; B=B1; C=C1; D=D1; E=E1; F=F1;
}
#endif










