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

// lpa, le 03/09/91


// Implementation de la classe resolvant les equations en cosinus-sinus.
// Equation de la forme a*cos(x)*cos(x)+2*b*cos(x)*sin(x)+c*cos(x)+d*sin(x)+e

//#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError
//#endif

#include <math_TrigonometricFunctionRoots.hxx>
#include <math_DirectPolynomialRoots.hxx>
#include <Standard_OutOfRange.hxx>
#include <math_FunctionWithDerivative.hxx>
#include <math_NewtonFunctionRoot.hxx>
#include <Precision.hxx>


class MyTrigoFunction: public math_FunctionWithDerivative
{
  Standard_Real AA;
  Standard_Real BB;
  Standard_Real CC;
  Standard_Real DD;
  Standard_Real EE;

 public:
  MyTrigoFunction(const Standard_Real A,
                  const Standard_Real B,
                  const Standard_Real C,
                  const Standard_Real D,
                  const Standard_Real E)
  : AA(A), BB(B), CC(C), DD(D), EE(E)
  {
  }

  Standard_Boolean Value(const Standard_Real X, Standard_Real& F);
  Standard_Boolean Derivative(const Standard_Real X, Standard_Real& D);
  Standard_Boolean Values(const Standard_Real X, Standard_Real& F, Standard_Real& D);
};

 Standard_Boolean MyTrigoFunction::Value(const Standard_Real X, Standard_Real& F) {
   Standard_Real CN= cos(X), SN = sin(X);
   //-- F= AA*CN*CN+2*BB*CN*SN+CC*CN+DD*SN+EE;
   F=CN*(AA*CN + (BB+BB)*SN + CC) + DD*SN + EE;
   return Standard_True;
 }

 Standard_Boolean MyTrigoFunction::Derivative(const Standard_Real X, Standard_Real& D) {
   Standard_Real CN= Cos(X), SN = Sin(X);
   //-- D = -2*AA*CN*SN+2*BB*(CN*CN-SN*SN)-CC*SN+DD*CN;
   D = -AA*CN*SN + BB*(CN*CN-SN*SN);
   D+=D;
   D-=CC*SN+DD*CN;
   return Standard_True;
 }

 Standard_Boolean MyTrigoFunction::Values(const Standard_Real X, Standard_Real& F, Standard_Real& D) {
   Standard_Real CN= Cos(X), SN = Sin(X);
   //-- F= AA*CN*CN+2*BB*CN*SN+CC*CN+DD*SN+EE;
   //-- D = -2*AA*CN*SN+2*BB*(CN*CN-SN*SN)-CC*SN+DD*CN;
   Standard_Real AACN = AA*CN;
   Standard_Real BBSN = BB*SN;

   F = AACN*CN + BBSN*(CN+CN) + CC*CN + DD*SN + EE;
   D = -AACN*SN + BB*(CN*CN+SN*SN);
   D+=D;
   D+=-CC*SN+DD*CN;
   return Standard_True;
 }


math_TrigonometricFunctionRoots::math_TrigonometricFunctionRoots
                                (const Standard_Real theD,
                                 const Standard_Real theE,
                                 const Standard_Real theInfBound,
                                 const Standard_Real theSupBound)
: NbSol         (-1),
  Sol           (1, 4),
  InfiniteStatus(Standard_False),
  Done          (Standard_False)
{
  const Standard_Real A(0.0), B(0.0), C(0.0);
  Perform(A, B, C, theD, theE, theInfBound, theSupBound);
}


math_TrigonometricFunctionRoots::math_TrigonometricFunctionRoots
                                (const Standard_Real theC,
                                 const Standard_Real theD,
                                 const Standard_Real theE,
                                 const Standard_Real theInfBound,
                                 const Standard_Real theSupBound)
: NbSol         (-1),
  Sol           (1, 4),
  InfiniteStatus(Standard_False),
  Done          (Standard_False)
{
  const Standard_Real A(0.0), B(0.0);
  Perform(A, B, theC, theD, theE, theInfBound, theSupBound);
}



math_TrigonometricFunctionRoots::math_TrigonometricFunctionRoots
                                (const Standard_Real theA,
                                 const Standard_Real theB,
                                 const Standard_Real theC,
                                 const Standard_Real theD,
                                 const Standard_Real theE,
                                 const Standard_Real theInfBound,
                                 const Standard_Real theSupBound)
: NbSol         (-1),
  Sol           (1, 4),
  InfiniteStatus(Standard_False),
  Done          (Standard_False)
{
  Perform(theA, theB, theC, theD, theE, theInfBound, theSupBound);
}

void math_TrigonometricFunctionRoots::Perform(const Standard_Real A, 
					      const Standard_Real B,
					      const Standard_Real C, 
					      const Standard_Real D,
					      const Standard_Real E, 
					      const Standard_Real InfBound, 
					      const Standard_Real SupBound) {

  Standard_Integer i, j=0, k, l, NZer=0, Nit = 10;
  Standard_Real Depi, Delta, Mod, AA, BB, CC, MyBorneInf;
  Standard_Real Teta, X;
  Standard_Real Eps, Tol1 = 1.e-15;
  TColStd_Array1OfReal ko(1,5), Zer(1,4);
  Standard_Boolean Flag4;
  InfiniteStatus = Standard_False;
  Done = Standard_True;

  Eps = 1.5e-12;

  Depi = M_PI+M_PI;
  if (InfBound <= RealFirst() && SupBound >= RealLast()) {
    MyBorneInf = 0.0;
    Delta = Depi;
    Mod = 0.0;
  }
  else if (SupBound >= RealLast()) {
    MyBorneInf = InfBound;
    Delta = Depi;
    Mod = MyBorneInf/Depi;
  }
  else if (InfBound <= RealFirst()) {
    MyBorneInf = SupBound - Depi;
    Delta = Depi;
    Mod = MyBorneInf/Depi;
  }
  else {
    MyBorneInf = InfBound;
    Delta = SupBound-InfBound;
    Mod = InfBound/Depi; 
    if ((SupBound-InfBound) > Depi) { Delta = Depi;}
  }

  if ((Abs(A) <= Eps) && (Abs(B) <= Eps)) {
    if (Abs(C) <= Eps) {
      if (Abs(D) <= Eps) {
        if (Abs(E) <= Eps) {
          InfiniteStatus = Standard_True;   // infinite de solutions.
          return;
        }
        else {
          NbSol = 0;
          return;
        }
      }
      else { 
        // Equation du type d*sin(x) + e = 0
        // =================================	
        NbSol = 0;
        AA = -E/D;
        if (Abs(AA) > 1.) {
          return;
        }

        Zer(1) = ASin(AA);
        Zer(2) = M_PI - Zer(1);
        NZer = 2;
        for (i = 1; i <= NZer; i++) {
          if (Zer(i) <= -Eps) {
            Zer(i) = Depi - Abs(Zer(i));
          }
          // On rend les solutions entre InfBound et SupBound:
          // =================================================
          Zer(i) += IntegerPart(Mod)*Depi;
          X = Zer(i)-MyBorneInf;
          if ((X > (-Epsilon(Delta))) && (X < Delta+ Epsilon(Delta))) {
            NbSol++;
            Sol(NbSol) = Zer(i);
          }
        }
      }
      return;
    }
    else if (Abs(D) <= Eps)  {

      // Equation du premier degre de la forme c*cos(x) + e = 0
      // ======================================================	  
      NbSol = 0;
      AA = -E/C;
      if (Abs(AA) >1.) {
        return;
      }
      Zer(1) = ACos(AA);
      Zer(2) = -Zer(1);
      NZer = 2;

      for (i = 1; i <= NZer; i++) {
        if (Zer(i) <= -Eps) {
          Zer(i) = Depi - Abs(Zer(i));
        }
        // On rend les solutions entre InfBound et SupBound:
        // =================================================
        Zer(i) += IntegerPart(Mod)*2.*M_PI;
        X = Zer(i)-MyBorneInf;
        if ((X >= (-Epsilon(Delta))) && (X <= Delta+ Epsilon(Delta))) {
          NbSol++;
          Sol(NbSol) = Zer(i);
        }
      }
      return;
    }
    else {

      // Equation du second degre:
      // =========================
      AA = E - C;
      BB = 2.0*D;
      CC = E + C;

      math_DirectPolynomialRoots Resol(AA, BB, CC);
      if (!Resol.IsDone()) {
        Done = Standard_False;
        return;
      }
      else if(!Resol.InfiniteRoots()) {
        NZer = Resol.NbSolutions();
        for (i = 1; i <= NZer; i++) {
          Zer(i) = Resol.Value(i);
        }
      }
      else if (Resol.InfiniteRoots()) {
        InfiniteStatus = Standard_True;
        return;
      }
    }
  }
  else {
    // Two additional analytical cases.
    if ((Abs(A) <= Eps) && 
        (Abs(E) <= Eps))
    {
      if (Abs(C) <= Eps)
      {
        // 2 * B * sin * cos + D * sin = 0
        NZer = 2;
        Zer(1) = 0.0;
        Zer(2) = M_PI;

        AA = -D/(B * 2);
        if (Abs(AA) <= 1.0 + Precision::PConfusion())
        {
          NZer = 4;
          if (AA >= 1.0)
          {
            Zer(3)= 0.0;
            Zer(4)= 0.0;
          }
          else if (AA <= -1.0)
          {
            Zer(3)= M_PI;
            Zer(4)= M_PI;
          }
          else
          {
            Zer(3)= ACos(AA);
            Zer(4) = Depi - Zer(3);
          }
        }

        NbSol = 0;
        for (i = 1; i <= NZer; i++) 
        {
          if (Zer(i) <= MyBorneInf - Eps)
          {
            Zer(i) += Depi;
          }
          // On rend les solutions entre InfBound et SupBound:
          // =================================================
          Zer(i) += IntegerPart(Mod)*2.*M_PI;
          X = Zer(i)-MyBorneInf;
          if ((X >= (-Precision::PConfusion())) && 
              (X <= Delta + Precision::PConfusion()))
          {
            if (Zer(i) < InfBound)
              Zer(i) = InfBound;
            if (Zer(i) > SupBound)
              Zer(i) = SupBound;
            NbSol++;
            Sol(NbSol) = Zer(i);
          }
        }
        return;
      }
      if (Abs(D) <= Eps)
      {
        // 2 * B * sin * cos + C * cos = 0
        NZer = 2;
        Zer(1) = M_PI / 2.0;
        Zer(2) = M_PI * 3.0 / 2.0;

        AA = -C/(B * 2);
        if (Abs(AA) <= 1.0 + Precision::PConfusion())
        {
          NZer = 4;
          if (AA >= 1.0)
          {
            Zer(3) = M_PI / 2.0;
            Zer(4) = M_PI / 2.0;
          }
          else if (AA <= -1.0)
          {
            
            Zer(3) = M_PI * 3.0 / 2.0;
            Zer(4) = M_PI * 3.0 / 2.0;
          }
          else
          {
            Zer(3)= ASin(AA);
            Zer(4) = M_PI - Zer(3);
          }
        }

        NbSol = 0;
        for (i = 1; i <= NZer; i++) 
        {
          if (Zer(i) <= MyBorneInf - Eps)
          {
            Zer(i) += Depi;
          }
          // On rend les solutions entre InfBound et SupBound:
          // =================================================
          Zer(i) += IntegerPart(Mod)*2.*M_PI;
          X = Zer(i)-MyBorneInf;
          if ((X >= (-Precision::PConfusion())) && 
              (X <= Delta + Precision::PConfusion()))
          {
            if (Zer(i) < InfBound)
              Zer(i) = InfBound;
            if (Zer(i) > SupBound)
              Zer(i) = SupBound;
            NbSol++;
            Sol(NbSol) = Zer(i);
          }
        }
        return;
      }
    }

// Equation du 4 ieme degre
// ========================
    ko(1) = A-C+E;
    ko(2) = 2.0*D-4.0*B;
    ko(3) = 2.0*E-2.0*A;
    ko(4) = 4.0*B+2.0*D;
    ko(5) = A+C+E;
    Standard_Boolean bko;
    Standard_Integer nbko=0;
    do { 
      bko=Standard_False;
      math_DirectPolynomialRoots Resol4(ko(1), ko(2), ko(3), ko(4), ko(5));
      if (!Resol4.IsDone()) {
	Done = Standard_False;
	return;
      }
      else if (!Resol4.InfiniteRoots()) {
	NZer = Resol4.NbSolutions();
	for (i = 1; i <= NZer; i++) {
	  Zer(i) = Resol4.Value(i);
	}
      }
      else if (Resol4.InfiniteRoots()) {
	InfiniteStatus = Standard_True;
	return;
      }
      Standard_Boolean triok;
      do { 
	triok=Standard_True;
	for(i=1;i<NZer;i++) { 
	  if(Zer(i)>Zer(i+1)) { 
	    Standard_Real t=Zer(i);
	    Zer(i)=Zer(i+1);
	    Zer(i+1)=t;
	    triok=Standard_False;
	  }
	}
      }
      while(triok==Standard_False);
      
      for(i=1;i<NZer;i++) { 
	if(Abs(Zer(i+1)-Zer(i))<Eps) { 
	  //-- est ce une racine double ou une erreur numerique ? 
	  Standard_Real qw=Zer(i+1);
	  Standard_Real va=ko(4)+qw*(2.0*ko(3)+qw*(3.0*ko(2)+qw*(4.0*ko(1))));
	  //-- cout<<"   Val Double ("<<qw<<")=("<<va<<")"<<endl;
	  if(Abs(va)>Eps) { 
	    bko=Standard_True;
	    nbko++;
#ifdef OCCT_DEBUG
	    //if(nbko==1) { 
	    //  cout<<"Pb ds math_TrigonometricFunctionRoots CC="
	    //	<<A<<" CS="<<B<<" C="<<C<<" S="<<D<<" Cte="<<E<<endl;
	    //}
#endif
	    break;
	  }
	}
      }
      if(bko) { 
	//-- Si il y a un coeff petit, on divise
	//-- 
	
	ko(1)*=0.0001;
	ko(2)*=0.0001;
	ko(3)*=0.0001;
	ko(4)*=0.0001;
	ko(5)*=0.0001;
	
      }	
    }
    while(bko);
  }

  // Verification des solutions par rapport aux bornes:
  // ==================================================
  Standard_Real SupmInfs100 = (SupBound-InfBound)*0.01;
  NbSol = 0;
  for (i = 1; i <= NZer; i++) {
    Teta = atan(Zer(i)); Teta+=Teta;
    if (Zer(i) <= (-Eps)) {
      Teta = Depi-Abs(Teta);
    }
    Teta += IntegerPart(Mod)*Depi;
    if (Teta-MyBorneInf < 0) Teta += Depi;  

    X = Teta -MyBorneInf;
    if ((X >= (-Epsilon(Delta))) && (X <= Delta+ Epsilon(Delta))) {
      X = Teta;

      // Appel de Newton:
      //OCC541(apo):  Standard_Real TetaNewton=0;  
      Standard_Real TetaNewton = Teta;  
      MyTrigoFunction MyF(A, B, C, D, E);
      math_NewtonFunctionRoot Resol(MyF, X, Tol1, Eps, Nit);
      if (Resol.IsDone()) {
	TetaNewton = Resol.Root();
      }
      //-- lbr le 7 mars 97 (newton converge tres tres loin de la solution initilale)
      Standard_Real DeltaNewton = TetaNewton-Teta;
      if((DeltaNewton > SupmInfs100) || (DeltaNewton < -SupmInfs100)) { 
	//-- cout<<"\n Newton X0="<<Teta<<" -> "<<TetaNewton<<endl;
      }
      else { 
	Teta=TetaNewton;
      }
      
      Flag4 = Standard_False;
      
      for(k = 1; k <= NbSol; k++) {
	//On met les valeurs par ordre croissant:
	if (Teta < Sol(k)) {
	  for (l = k; l <= NbSol; l++) {
	    j = NbSol-l+k;
	    Sol(j+1) = Sol(j);
	  }
	  Sol(k) = Teta;
	  NbSol++;
	  Flag4 = Standard_True;
	  break;
	}
      }
      if (!Flag4) {
	NbSol++;
	Sol(NbSol) = Teta;
      }
    }
  }  
  // Cas particulier de  PI:
  if(NbSol<4) { 
    Standard_Integer startIndex = NbSol + 1;
    for( Standard_Integer solIt = startIndex; solIt <= 4; solIt++) {
      Teta = M_PI + IntegerPart(Mod)*2.0*M_PI;;
      X = Teta - MyBorneInf;
      if ((X >= (-Epsilon(Delta))) && (X <= Delta + Epsilon(Delta))) {
	if (Abs(A-C+E) <= Eps) {
	  Flag4 = Standard_False;
	  for (k = 1; k <= NbSol; k++) {
	    j = k;
	    if (Teta < Sol(k)) {
	      Flag4 = Standard_True;
	      break;
	    }
	    if ((solIt == startIndex) && (Abs(Teta-Sol(k)) <= Eps)) {
	      return;
	    }
	  }

	  if (!Flag4) {
	    NbSol++;
	    Sol(NbSol) = Teta;
	  }
	  else {
	    for (k = j; k <= NbSol; k++) {
	      i = NbSol-k+j;
	      Sol(i+1) = Sol(i);
	    }
	    Sol(j) = Teta;
	    NbSol++;
	  }
	}
      }
    }
  }
}


void math_TrigonometricFunctionRoots::Dump(Standard_OStream& o) const
{
  o << " math_TrigonometricFunctionRoots: \n";
  if (!Done) {
    o << "Not Done \n";
  }
  else if (InfiniteStatus) {
    o << " There is an infinity of roots\n";
  }
  else if (!InfiniteStatus) {
    o << " Number of solutions = " << NbSol <<"\n";
    for (Standard_Integer i = 1; i <= NbSol; i++) {
      o << " Value number " << i << "= " << Sol(i) << "\n";
    }
  }
}
