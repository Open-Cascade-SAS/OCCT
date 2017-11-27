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

//#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError

//#endif

#include <math_DirectPolynomialRoots.hxx>
#include <math_FunctionRoots.hxx>
#include <math_FunctionWithDerivative.hxx>
#include <math_BracketedRoot.hxx>
#include <Standard_RangeError.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_Array1OfReal.hxx>

#include <stdio.h>
#define ITMAX  100
#define EPS    1e-14
#define EPSEPS 2e-14
#define MAXBIS 100

#ifdef OCCT_DEBUG
static Standard_Boolean myDebug = 0;
static Standard_Integer nbsolve = 0;
#endif

class DerivFunction: public math_Function
{
  math_FunctionWithDerivative *myF;

public:
  DerivFunction(math_FunctionWithDerivative& theF)
    : myF (&theF)
  {}

  virtual Standard_Boolean Value(const Standard_Real theX, Standard_Real& theFval)
  {
    return myF->Derivative(theX, theFval);
  }
};

static void  AppendRoot(TColStd_SequenceOfReal& Sol,
			TColStd_SequenceOfInteger& NbStateSol,
			const Standard_Real X,
			math_FunctionWithDerivative& F,
//			const Standard_Real K,
			const Standard_Real ,
			const Standard_Real dX) { 

  Standard_Integer n=Sol.Length();
  Standard_Real t;
#ifdef OCCT_DEBUG
 if (myDebug) {
   cout << "   Ajout de la solution numero : " << n+1 << endl;
   cout << "   Valeur de la racine :" << X << endl;
 } 
#endif
  
  if(n==0) { 
    Sol.Append(X);
    F.Value(X,t);
    NbStateSol.Append(F.GetStateNumber()); 
  }
  else { 
    Standard_Integer i=1;
    Standard_Integer pl=n+1;
    while(i<=n) { 
      t=Sol.Value(i);
      if(t >= X) { 
	pl=i;
	i=n;
      }
      if(Abs(X-t) <= dX) { 
	pl=0;
	i=n;
      }
      i++;
    } //-- while
    if(pl>n) { 
      Sol.Append(X);
      F.Value(X,t);
      NbStateSol.Append(F.GetStateNumber());
    }
    else if(pl>0) { 
      Sol.InsertBefore(pl,X);
      F.Value(X,t);
      NbStateSol.InsertBefore(pl,F.GetStateNumber());
    }
  }
}

static void  Solve(math_FunctionWithDerivative& F,
		   const Standard_Real K,
		   const Standard_Real x1,
		   const Standard_Real y1,
		   const Standard_Real x2,
		   const Standard_Real y2,
		   const Standard_Real tol,
		   const Standard_Real dX,
		   TColStd_SequenceOfReal& Sol,
		   TColStd_SequenceOfInteger& NbStateSol) { 
#ifdef OCCT_DEBUG
  if (myDebug) {
    cout <<"--> Resolution :" << ++nbsolve << endl;
    cout <<"   x1 =" << x1 << " y1 =" << y1 << endl;
    cout <<"   x2 =" << x2 << " y2 =" << y2 << endl;
  }
#endif

  Standard_Integer iter=0;
  Standard_Real tols2 = 0.5*tol;
  Standard_Real a,b,c,d=0,e=0,fa,fb,fc,p,q,r,s,tol1,xm,min1,min2;
  a=x1;b=c=x2;fa=y1; fb=fc=y2;
  for(iter=1;iter<=ITMAX;iter++) { 
    if((fb>0.0 && fc>0.0) || (fb<0.0 && fc<0.0)) { 
      c=a; fc=fa; e=d=b-a;
    }
    if(Abs(fc)<Abs(fb)) { 
      a=b; b=c; c=a; fa=fb; fb=fc; fc=fa;
    }
    tol1 = EPSEPS*Abs(b)+tols2;
    xm=0.5*(c-b);
    if(Abs(xm)<tol1 || fb==0) { 
      //-- On tente une iteration de newton
      Standard_Real Xp,Yp,Dp;      
      Standard_Integer itern=5;
      Standard_Boolean Ok;
      Xp=b;
      do { 
	Ok = F.Values(Xp,Yp,Dp);
	if(Ok) {
	  Ok=Standard_False;
	  if(Dp>1e-10 || Dp<-1e-10) { 
	    Xp = Xp-(Yp-K)/Dp;
	  }
	  if(Xp<=x2 && Xp>=x1) { 
	    F.Value(Xp,Yp); Yp-=K;
	    if(Abs(Yp)<Abs(fb)) { 
	      b=Xp;
	      fb=Yp; 
	      Ok=Standard_True;
	    }
	  }
	}
      }
      while(Ok && --itern>=0);

      AppendRoot(Sol,NbStateSol,b,F,K,dX);
      return;
    }
    if(Abs(e)>=tol1 && Abs(fa)>Abs(fb)) { 
      s=fb/fa;
      if(a==c) { 
	p=xm*s; p+=p;
	q=1.0-s;
      }
      else { 
	q=fa/fc;
	r=fb/fc;
	p=s*((xm+xm)*q*(q-r)-(b-a)*(r-1.0));
	q=(q-1.0)*(r-1.0)*(s-1.0);
      }
      if(p>0.0) {
	q=-q;
      }
      p=Abs(p);
      min1=3.0*xm*q-Abs(tol1*q);
      min2=Abs(e*q);
      if((p+p)<( (min1<min2) ? min1 : min2)) { 
	e=d;
	d=p/q;
      }
      else { 
	d=xm;
	e=d;
      }
    }
    else { 
      d=xm;
      e=d;
    }
    a=b;
    fa=fb;
    if(Abs(d)>tol1) {
      b+=d;
    }
    else { 
      if(xm>=0) b+=Abs(tol1);
      else      b+=-Abs(tol1);
    }
    F.Value(b,fb);
    fb-=K;
  }
#ifdef OCCT_DEBUG
  cout<<" Non Convergence dans math_FunctionRoots.cxx "<<endl;
#endif
}

#define NEWSEQ 1 

#define MATH_FUNCTIONROOTS_NEWCODE // Nv Traitement
//#define MATH_FUNCTIONROOTS_OLDCODE // Ancien
//#define MATH_FUNCTIONROOTS_CHECK // Check

math_FunctionRoots::math_FunctionRoots(math_FunctionWithDerivative& F,
				       const Standard_Real A,
				       const Standard_Real B,
				       const Standard_Integer NbSample,
				       const Standard_Real _EpsX,
				       const Standard_Real EpsF,
				       const Standard_Real EpsNull,
				       const Standard_Real K ) 
{ 
#ifdef OCCT_DEBUG
  if (myDebug) {
    cout << "---- Debut de math_FunctionRoots ----" << endl;
    nbsolve = 0;
  }
#endif
 
#if NEWSEQ
  TColStd_SequenceOfReal StaticSol;
#endif
  Sol.Clear();
  NbStateSol.Clear();
  #ifdef MATH_FUNCTIONROOTS_NEWCODE
    { 
    Done = Standard_True;
    Standard_Real X0=A;
    Standard_Real XN=B;
    Standard_Integer N=NbSample;    
    //-- ------------------------------------------------------------
    //-- Verifications de bas niveau 
    if(B<A) {
      X0=B;
      XN=A;
    }
    N*=2;
    if(N < 20) { 
      N=20; 
    }
    //--  On teste si EpsX est trop petit (ie : U+Nn*EpsX == U ) 
    Standard_Real EpsX   = _EpsX;
    Standard_Real DeltaU = Abs(X0)+Abs(XN);
    Standard_Real NEpsX  = 0.0000000001 * DeltaU;
    if(EpsX < NEpsX) { 
      EpsX = NEpsX; 
    }
    
    //-- recherche d un intervalle ou F(xi) et F(xj) sont de signes differents
    //-- A .............................................................. B
    //-- X0   X1   X2 ........................................  Xn-1      Xn
    Standard_Integer i;
    Standard_Real X=X0;
    Standard_Boolean Ok;
    double dx = (XN-X0)/N;
    TColStd_Array1OfReal ptrval(0, N);
    Standard_Integer Nvalid = -1;
    Standard_Real aux = 0;
    for(i=0; i<=N ; i++,X+=dx) { 
      if( X > XN) X=XN;
      Ok=F.Value(X,aux);
      if(Ok) ptrval(++Nvalid) = aux - K;
//      ptrval(i)-=K;
    }
    //-- Toute la fonction est nulle ? 

    if( Nvalid < N ) {
      Done = Standard_False;
      return;
    }

    AllNull=Standard_True;
//    for(i=0;AllNull && i<=N;i++) { 
    for(i=0;AllNull && i<=N;i++) { 
      if(ptrval(i)>EpsNull || ptrval(i)<-EpsNull) { 
	AllNull=Standard_False;
      }
    }
    if(AllNull) { 
      //-- tous les points echantillons sont dans la tolerance 
      
    }
    else { 
      //-- Il y a des points hors tolerance 
      //-- on detecte les changements de signes STRICTS 
      Standard_Integer ip1;
//      Standard_Boolean chgtsign=Standard_False;
      Standard_Real tol = EpsX;
      Standard_Real X2;
      for(i=0,ip1=1,X=X0;i<N; i++,ip1++,X+=dx) { 
        X2=X+dx;
        if(X2 > XN) X2 = XN;
	if(ptrval(i)<0.0) { 
	  if(ptrval(ip1)>0.0) { 
	    //-- --------------------------------------------------
	    //-- changement de signe dans Xi Xi+1
	    Solve(F,K,X,ptrval(i),X2,ptrval(ip1),tol,NEpsX,Sol,NbStateSol);
	  }
	}
	else { 
	  if(ptrval(ip1)<0.0) { 
	    //-- --------------------------------------------------
	    //-- changement de signe dans Xi Xi+1
	    Solve(F,K,X,ptrval(i),X2,ptrval(ip1),tol,NEpsX,Sol,NbStateSol);
	  }
	}
      }
      //-- On detecte les cas ou la fct s annule sur des Xi et est 
      //-- non nulle au voisinage de Xi
      //--
      //-- On prend 2 points u0,u1 au voisinage de Xi
      //-- Si (F(u0)-K)*(F(u1)-K) <0   on lance une recherche 
      //-- Sinon si (F(u0)-K)*(F(u1)-K) !=0 on insere le point X
      for(i=0; i<=N; i++) { 
	if(ptrval(i)==0) { 
//	  Standard_Real Val,Deriv;
	  X=X0+i*dx;
          if (X>XN) X=XN;
	  Standard_Real u0,u1;
	  u0=dx*0.5; 	  u1=X+u0; 	  u0+=X;
	  if(u0<X0)  u0=X0;
	  if(u0>XN)  u0=XN;
	  if(u1<X0)  u1=X0;
	  if(u1>XN)  u1=XN;

	  Standard_Real y0,y1;
	  F.Value(u0,y0); y0-=K;
	  F.Value(u1,y1); y1-=K;
	  if(y0*y1 < 0.0) { 
	    Solve(F,K,u0,y0,u1,y1,tol,NEpsX,Sol,NbStateSol);
	  }
	  else {
	    if(y0!=0.0 || y1!=0.0) { 
	      AppendRoot(Sol,NbStateSol,X,F,K,NEpsX);
	    }
	  }
	}
      }
      //-- --------------------------------------------------------------------------------
      //-- Il faut traiter differement le cas des points en bout : 
      if(ptrval(0)<=EpsF && ptrval(0)>=-EpsF) { 
	AppendRoot(Sol,NbStateSol,X0,F,K,NEpsX);
      }
      if(ptrval(N)<=EpsF && ptrval(N)>=-EpsF) { 
	AppendRoot(Sol,NbStateSol,XN,F,K,NEpsX);      
      }

      //-- --------------------------------------------------------------------------------
      //-- --------------------------------------------------------------------------------
      //-- On detecte les zones ou on a sur les points echantillons un minimum avec f(x)>0
      //--                                                          un maximum avec f(x)<0
      //-- On reprend une discretisation plus fine au voisinage de ces extremums
      //--
      //-- Recherche d un minima positif
      Standard_Real xm,ym,dym,xm1,xp1;
      Standard_Real majdx = 5.0*dx;
      Standard_Boolean Rediscr;
//      Standard_Real ptrvalbis[MAXBIS];
      Standard_Integer im1=0;
      ip1=2;
      for(i=1,xm=X0+dx; i<N; xm+=dx,i++,im1++,ip1++) { 
	Rediscr = Standard_False;
        if (xm > XN) xm=XN;
	if(ptrval(i)>0.0) { 
	  if((ptrval(im1)>ptrval(i)) && (ptrval(ip1)>ptrval(i))) { 
	    //-- Peut on traverser l axe Ox 
	    //-- -------------- Estimation a partir de Xim1
            xm1=xm-dx;
            if(xm1 < X0) xm1=X0;
	    F.Values(xm1,ym,dym); ym-=K;  
	    if(dym<-1e-10 || dym>1e-10) {  // normalement dym < 0 
	      Standard_Real t = ym / dym; //-- t=xm-x* = (ym-0)/dym
	      if(t<majdx && t > -majdx) { 
		Rediscr = Standard_True;
	      }
	    }
	    //-- -------------- Estimation a partir de Xip1
	    if(Rediscr==Standard_False) {
              xp1=xm+dx;
              if(xp1 > XN ) xp1=XN;
	      F.Values(xp1,ym,dym); ym-=K;
	      if(dym<-1e-10 || dym>1e-10) {  // normalement dym > 0 
		Standard_Real t = ym / dym; //-- t=xm-x* = (ym-0)/dym
		if(t<majdx && t > -majdx) { 
		  Rediscr = Standard_True;
		}
	      }
	    }
	  }
	}
	else if(ptrval(i)<0.0) { 
	  if((ptrval(im1)<ptrval(i)) && (ptrval(ip1)<ptrval(i))) { 
	    //-- Peut on traverser l axe Ox 
	    //-- -------------- Estimation a partir de Xim1
            xm1=xm-dx;
            if(xm1 < X0) xm1=X0;
	    F.Values(xm1,ym,dym); ym-=K;
	    if(dym>1e-10 || dym<-1e-10) {  // normalement dym > 0 
	      Standard_Real t = ym / dym; //-- t=xm-x* = (ym-0)/dym
	      if(t<majdx && t > -majdx) { 
		Rediscr = Standard_True;
	      }
	    }
	    //-- -------------- Estimation a partir de Xim1
	    if(Rediscr==Standard_False) { 
               xm1=xm-dx;
               if(xm1 < X0) xm1=X0;
	      F.Values(xm1,ym,dym); ym-=K;
	      if(dym>1e-10 || dym<-1e-10) {  // normalement dym < 0 
		Standard_Real t = ym / dym; //-- t=xm-x* = (ym-0)/dym
		if(t<majdx && t > -majdx) { 
		  Rediscr = Standard_True;
		}
	      }
	    }
	  }
	}
	if(Rediscr) {
          Standard_Real x0 = xm - dx;
          Standard_Real x3 = xm + dx;
          if (x0 < X0) x0 = X0;
          if (x3 > XN) x3 = XN;
          Standard_Real aSolX1 = 0., aSolX2 = 0.;
          Standard_Real aVal1 = 0., aVal2 = 0.;
          Standard_Real aDer1 = 0., aDer2 = 0.;
          Standard_Boolean isSol1 = Standard_False;
          Standard_Boolean isSol2 = Standard_False;
          //-- ----------------------------------------------------
          //-- Find minimum of the function |F| between x0 and x3
          //-- by searching for the zero of the function derivative
          DerivFunction aDerF(F);
          math_BracketedRoot aBR(aDerF, x0, x3, _EpsX);
          if (aBR.IsDone())
          {
            aSolX1 = aBR.Root();
            F.Value(aSolX1, aVal1);
            aVal1 = Abs(aVal1);
            if (aVal1 < EpsF)
            {
              isSol1 = Standard_True;
              aDer1 = aBR.Value();
            }
          }

          //-- --------------------------------------------------
          //-- On recherche un extrema entre x0 et x3
          //-- x1 et x2 sont tels que x0<x1<x2<x3 
          //-- et |f(x0)| > |f(x1)|   et |f(x3)| > |f(x2)|
          //--
          //-- En entree : a=xm-dx  b=xm c=xm+dx
          Standard_Real x1, x2, f0, f3;
          Standard_Real R = 0.61803399;
          Standard_Real C = 1.0 - R;
          Standard_Real tolCR = NEpsX*10.0;
          f0 = ptrval(im1);
          f3 = ptrval(ip1);
          Standard_Boolean recherche_minimum = (f0 > 0.0);

          if (Abs(x3 - xm) > Abs(x0 - xm)) { x1 = xm; x2 = xm + C*(x3 - xm); }
          else                        { x2 = xm; x1 = xm - C*(xm - x0); }
          Standard_Real f1, f2;
          F.Value(x1, f1); f1 -= K;
          F.Value(x2, f2); f2 -= K;
          //-- printf("\n *************** RECHERCHE MINIMUM **********\n");
          Standard_Real tolX = 0.001 * NEpsX;
          while (Abs(x3 - x0) > tolCR*(Abs(x1) + Abs(x2)) && (Abs(x1 - x2) > tolX)) {
            //-- printf("\n (%10.5g,%10.5g) (%10.5g,%10.5g) (%10.5g,%10.5g) (%10.5g,%10.5g) ", 
            //--    x0,f0,x1,f1,x2,f2,x3,f3);
            if (recherche_minimum) {
              if (f2 < f1) {
                x0 = x1; x1 = x2; x2 = R*x1 + C*x3;
                f0 = f1; f1 = f2; F.Value(x2, f2); f2 -= K;
              }
              else {
                x3 = x2; x2 = x1; x1 = R*x2 + C*x0;
                f3 = f2; f2 = f1; F.Value(x1, f1); f1 -= K;
              }
            }
            else {
              if (f2 > f1) {
                x0 = x1; x1 = x2; x2 = R*x1 + C*x3;
                f0 = f1; f1 = f2; F.Value(x2, f2); f2 -= K;
              }
              else {
                x3 = x2; x2 = x1; x1 = R*x2 + C*x0;
                f3 = f2; f2 = f1; F.Value(x1, f1); f1 -= K;
              }
            }
            //-- On ne fait pas que chercher des extremas. Il faut verifier 
            //-- si on ne tombe pas sur une racine 
            if (f1*f0 < 0.0) {
              //-- printf("\n Recherche entre  (%10.5g,%10.5g) (%10.5g,%10.5g) ",x0,f0,x1,f1);
              Solve(F, K, x0, f0, x1, f1, tol, NEpsX, Sol, NbStateSol);
            }
            if (f2*f3 < 0.0)  {
              //-- printf("\n Recherche entre  (%10.5g,%10.5g) (%10.5g,%10.5g) ",x2,f2,x3,f3);
              Solve(F, K, x2, f2, x3, f3, tol, NEpsX, Sol, NbStateSol);
            }
          }
          if ((recherche_minimum && f1<f2) || (!recherche_minimum && f1>f2)) {
            //-- x1,f(x1) minimum
            if (Abs(f1) < EpsF) {
              isSol2 = Standard_True;
              aSolX2 = x1;
              aVal2 = Abs(f1);
            }
          }
          else {
            //-- x2.f(x2) minimum
            if (Abs(f2) < EpsF) {
              isSol2 = Standard_True;
              aSolX2 = x2;
              aVal2 = Abs(f2);
            }
          }
          // Choose the best solution between aSolX1, aSolX2
          if (isSol1 && isSol2)
          {
            if (aVal2 - aVal1 > EpsF)
              AppendRoot(Sol, NbStateSol, aSolX1, F, K, NEpsX);
            else if (aVal1 - aVal2 > EpsF)
              AppendRoot(Sol, NbStateSol, aSolX2, F, K, NEpsX);
            else
            {
              aDer1 = Abs(aDer1);
              F.Derivative(aSolX2, aDer2);
              aDer2 = Abs(aDer2);
              if (aDer1 < aDer2)
                AppendRoot(Sol, NbStateSol, aSolX1, F, K, NEpsX);
              else
                AppendRoot(Sol, NbStateSol, aSolX2, F, K, NEpsX);
            }
          }
          else if (isSol1)
            AppendRoot(Sol, NbStateSol, aSolX1, F, K, NEpsX);
          else if(isSol2)
            AppendRoot(Sol, NbStateSol, aSolX2, F, K, NEpsX);
        } //-- Recherche d un extrema    
      } //-- for     
    }      
    
#if NEWSEQ
    #ifdef MATH_FUNCTIONROOTS_CHECK 
    { 
      StaticSol.Clear();
      Standard_Integer n=Sol.Length();
      for(Standard_Integer ii=1;ii<=n;ii++) { 
	StaticSol.Append(Sol.Value(ii));
      }
      Sol.Clear();
      NbStateSol.Clear();
    }
#endif
#endif
#endif
  }
#ifdef MATH_FUNCTIONROOTS_OLDCODE
{ 
    //-- ********************************************************************************
    //--                              ANCIEN TRAITEMENT 
    //-- ********************************************************************************


  // calculate all the real roots of a function within the range 
  // A..B. whitout condition on A and B
  // a solution X is found when
  //   abs(Xi - Xi-1) <= EpsX and abs(F(Xi)-K) <= Epsf.
  // The function is considered as null between A and B if
  // abs(F-K) <= EpsNull within this range.
  Standard_Real EpsX = _EpsX; //-- Cas ou le parametre va de 100000000 a 1000000001
                              //-- Il ne faut pas EpsX = 0.000...001  car dans ce cas
                              //-- U + Nn*EpsX     ==     U 
  Standard_Real Lowr,Upp; 
  Standard_Real Increment;
  Standard_Real Null2;
  Standard_Real FLowr,FUpp,DFLowr,DFUpp;
  Standard_Real U,Xu;
  Standard_Real Fxu,DFxu,FFxu,DFFxu;
  Standard_Real Fyu,DFyu,FFyu,DFFyu;
  Standard_Boolean Finish;
  Standard_Real FFi;
  Standard_Integer Nbiter = 30;
  Standard_Integer Iter;
  Standard_Real Ambda,T;
  Standard_Real AA,BB,CC;
  Standard_Integer Nn;
  Standard_Real Alfa1=0,Alfa2;
  Standard_Real OldDF = RealLast();
  Standard_Real Standard_Underflow = 1e-32 ; //-- RealSmall();
  Standard_Boolean Ok;
  
  Done = Standard_False;
  
  StdFail_NotDone_Raise_if(NbSample <= 0, " ");
  
  // initialisation
  
  if (A > B) {
    Lowr=B;
    Upp=A;
  }
  else {
    Lowr=A;
    Upp=B;
  }
  
  Increment = (Upp-Lowr)/NbSample; 
  StdFail_NotDone_Raise_if(Increment < EpsX, " ");
  Done = Standard_True;
  //--  On teste si EpsX est trop petit (ie : U+Nn*EpsX == U ) 
  Standard_Real DeltaU = Abs(Upp)+Abs(Lowr);
  Standard_Real NEpsX  = 0.0000000001 * DeltaU;
  if(EpsX < NEpsX) { 
    EpsX = NEpsX; 
    //-- cout<<" \n EpsX Init = "<<_EpsX<<" devient : (deltaU : "<<DeltaU<<" )   EpsX = "<<EpsX<<endl;
  }
  //-- 
  Null2 = EpsNull*EpsNull;
  
  Ok = F.Values(Lowr,FLowr,DFLowr);

  if(!Ok) {
    Done = Standard_False;
    return;
  }

  FLowr = FLowr - K;

  Ok = F.Values(Upp,FUpp,DFUpp);

  if(!Ok) {
    Done = Standard_False;
    return;
  }

  FUpp = FUpp - K;
  
  // Calcul sur U
  
  U = Lowr-EpsX;
  Fyu = FLowr-EpsX*DFLowr;  // extrapolation lineaire
  DFyu = DFLowr;
  FFyu = Fyu*Fyu;
  DFFyu = Fyu*DFyu; DFFyu+=DFFyu;
  AllNull = ( FFyu <= Null2 ); 
  
  while ( U < Upp) {
    
    Xu = U;
    Fxu = Fyu;
    DFxu = DFyu;
    FFxu = FFyu;
    DFFxu = DFFyu;
    
    U = Xu + Increment;
    if (U <= Lowr) {
      Fyu = FLowr + (U-Lowr)*DFLowr;
      DFyu = DFLowr;
    }
    else if (U >= Upp) { 
      Fyu = FUpp + (U-Upp)*DFUpp;
      DFyu = DFUpp;
    }
    else { 
      Ok = F.Values(U,Fyu,DFyu);

      if(!Ok) {
	Done = Standard_False;
	return;
      }

      Fyu = Fyu - K;
    }
    FFyu = Fyu*Fyu;
    DFFyu = Fyu*DFyu; DFFyu+=DFFyu; //-- DFFyu = 2.*Fyu*DFyu;
    
    if ( !AllNull || ( FFyu > Null2 && U <= Upp) ){
      
      if (AllNull) {     //rechercher les vraix zeros depuis le debut
	
	AllNull = Standard_False;
	Xu = Lowr-EpsX;
	Fxu = FLowr-EpsX*DFLowr;  
	DFxu = DFLowr;
	FFxu = Fxu*Fxu;
	DFFxu = Fxu*DFxu; DFFxu+=DFFxu;  //-- DFFxu = 2.*Fxu*DFxu;
	U = Xu + Increment;             
	Ok = F.Values(U,Fyu,DFyu);

	if(!Ok) {
	  Done = Standard_False;
	  return;
	}

	Fyu = Fyu - K;
	FFyu = Fyu*Fyu;
	DFFyu = Fyu*DFyu; DFFyu+=DFFyu;//-- DFFyu = 2.*Fyu*DFyu;
      }
      Standard_Real FxuFyu=Fxu*Fyu;
      
      if (  (DFFyu > 0. && DFFxu <= 0.)
	  || (DFFyu < 0. && FFyu >= FFxu && DFFxu <= 0.)
	  || (DFFyu > 0. && FFyu <= FFxu && DFFxu >= 0.)
	  || (FxuFyu <= 0.) ) {
	// recherche d 1 minimun possible
	Finish = Standard_False;
	Ambda = Increment;
	T = 0.;
	Iter=0;
	FFi=0.;  
	
	if (FxuFyu >0.) {
	  // chercher si f peut s annuler pour eviter 
	  //  des iterations inutiles
	  if ( Fxu*(Fxu + 2.*DFxu*Increment) > 0. &&
	      Fyu*(Fyu - 2.*DFyu*Increment) > 0. ) {
	    
	    Finish = Standard_True;  
	    FFi = Min ( FFxu , FFyu);  //pour ne pas recalculer yu
	  }
	  else if ((DFFxu <= Standard_Underflow && -DFFxu <= Standard_Underflow) || 
		   (FFxu <= Standard_Underflow  && -FFxu  <= Standard_Underflow)) {
	    
	    Finish = Standard_True;
	    FFxu = 0.0;
	    FFi = FFyu;   // pour recalculer yu
	  }
	  else if ((DFFyu <= Standard_Underflow && -DFFyu <= Standard_Underflow) || 
		   (FFyu  <= Standard_Underflow && -FFyu  <= Standard_Underflow)) {
	    
	    Finish = Standard_True;
	    FFyu =0.0;
	    FFi = FFxu;   // pour recalculer U
	  }
	}
	else if (FFxu <= Standard_Underflow && -FFxu <= Standard_Underflow) {
	  
	  Finish = Standard_True;
	  FFxu = 0.0;
	  FFi = FFyu;
	}
	else if (FFyu <= Standard_Underflow && -FFyu <= Standard_Underflow) {
	  
	  Finish = Standard_True;
	  FFyu =0.0;
	  FFi = FFxu;
	} 
	while (!Finish) {
	  
	  // calcul des 2 solutions annulant la derivee de l interpolation cubique
	  //    Ambda*t=(U-Xu)  F(t)=aa*t*t*t/3+bb*t*t+cc*t+d
	  //    df=aa*t*t+2*bb*t+cc
	  
	  AA = 3.*(Ambda*(DFFxu+DFFyu)+2.*(FFxu-FFyu));
	  BB = -2*(Ambda*(DFFyu+2.*DFFxu)+3.*(FFxu-FFyu));
	  CC = Ambda*DFFxu;

	  if(Abs(AA)<1e-14 && Abs(BB)<1e-14 && Abs(CC)<1e-14) { 
	    AA=BB=CC=0;
	  }
	  

	  math_DirectPolynomialRoots Solv(AA,BB,CC);
	  if ( !Solv.InfiniteRoots() ) {
	    Nn=Solv.NbSolutions();
	    if (Nn <= 0) {
	      if (Fxu*Fyu < 0.) { Alfa1 = 0.5;}  
	      else Finish = Standard_True; 
	    }
	    else {
	      Alfa1 = Solv.Value(1);
	      if (Nn == 2) {
		Alfa2 = Solv.Value(2);
		if (Alfa1 > Alfa2){
		  AA = Alfa1;
		  Alfa1 = Alfa2;
		  Alfa2 = AA;
		}
		if (Alfa1 > 1. || Alfa2 < 0.){
		  // resolution par dichotomie
		  if (Fxu*Fyu < 0.) Alfa1 = 0.5;
		  else  Finish = Standard_True;
		}
		else if ( Alfa1 < 0. || 
			 ( DFFxu > 0. && DFFyu >= 0.) ) {
		  // si 2 derivees >0 
		  //(cas changement de signe de la distance signee sans
		  // changement de signe de la derivee:
		  //cas de 'presque'tangence avec 2 
		  // solutions proches) ,on prend la plus grane racine
		  if (Alfa2 > 1.) {
		    if (Fxu*Fyu < 0.) Alfa1 = 0.5;
		    else Finish = Standard_True;
		  }
		  else Alfa1 = Alfa2;
		}
	      }
	    }
	  } 
	  else if (Fxu*Fyu < -1e-14) Alfa1 = 0.5;
	  //-- else if (Fxu*Fyu < 0.) Alfa1 = 0.5;
	  else  Finish = Standard_True;
	  
	  if (!Finish) {
	    // petits tests pour diminuer le nombre d iterations
	    if (Alfa1 <= EpsX ) {
	      Alfa1+=Alfa1;
	    }
	    else if (Alfa1 >= (1.-EpsX) ) {
	      Alfa1 = Alfa1+Alfa1-1.; 
	    }
	    Alfa1 = Ambda*(1. - Alfa1);
	    U = U + T - Alfa1;
	    if ( U <= Lowr ) {
	      AA = FLowr + (U - Lowr)*DFLowr;
	      BB = DFLowr;
	    }
	    else if ( U >= Upp ) {
	      AA = FUpp + (U - Upp)*DFUpp;
	      BB = DFUpp;
	    }
	    else {
	      Ok = F.Values(U,AA,BB);

	      if(!Ok) {
		Done = Standard_False;
		return;
	      }

	      AA = AA - K;
	    }
	    FFi = AA*AA;
	    CC = AA*BB; CC+=CC;
	    if (  ( ( CC < 0. && FFi < FFxu ) || DFFxu > 0.)
		&& AA*Fxu > 0. ) {
	      FFxu = FFi;
	      DFFxu = CC;
	      Fxu = AA;
	      DFxu = BB;
	      T = Alfa1;
	      if (Alfa1 > Ambda*0.5) {
		// remarque (1)
		// determination d 1 autre borne pour diviser 
		//le nouvel intervalle par 2 au -
		Xu = U + Alfa1*0.5;
		if ( Xu <= Lowr ) {
		  AA = FLowr + (Xu - Lowr)*DFLowr;
		  BB = DFLowr;
		}
		else if ( Xu >= Upp ) {
		  AA = FUpp + (Xu - Upp)*DFUpp;
		  BB = DFUpp;
		}
		else {
		  Ok = F.Values(Xu,AA,BB);

		  if(!Ok) {
		    Done = Standard_False;
		    return;
		  }

		  AA = AA -K;
		}
		FFi = AA*AA;
		CC = AA*BB; CC+=CC;
		if  ( (( CC >= 0. || FFi >= FFxu ) && DFFxu <0.)
		     || Fxu * AA < 0. ) {
		  Fyu = AA;
		  DFyu = BB;
		  FFyu = FFi;
		  DFFyu = CC;
		  T = Alfa1*0.5;
		  Ambda = Alfa1*0.5;
		}
		else if ( AA*Fyu < 0. && AA*Fxu >0.) {
		  // changement de signe sur l intervalle u,U
		  Fxu = AA;
		  DFxu = BB;
		  FFxu = FFi;
		  DFFxu = CC;
		  FFi = Min(FFxu,FFyu);
		  T = Alfa1*0.5;
		  Ambda = Alfa1*0.5;
		  U = Xu;
		}
		else { Ambda = Alfa1;}      
	      }
	      else { Ambda = Alfa1;}
	    }
	    else {
	      Fyu = AA;
	      DFyu = BB;
	      FFyu = FFi;
	      DFFyu = CC;
	      if ( (Ambda-Alfa1) > Ambda*0.5 ) {
		// meme remarque (1)
		Xu = U - (Ambda-Alfa1)*0.5;
		if ( Xu <= Lowr ) {
		  AA = FLowr + (Xu - Lowr)*DFLowr;
		  BB = DFLowr;
		}
		else if ( Xu >= Upp ) {
		  AA = FUpp + (Xu - Upp)*DFUpp;
		  BB = DFUpp;
		}
		else {
		  Ok = F.Values(Xu,AA,BB);

		  if(!Ok) {
		    Done = Standard_False;
		    return;
		  }

		  AA = AA - K;
		}
		FFi = AA*AA;
		CC = AA*BB; CC+=CC;
		if ( AA*Fyu <=0. && AA*Fxu > 0.) {
		  FFxu = FFi;
		  DFFxu = CC;
		  Fxu = AA;
		  DFxu = BB;
		  Ambda = ( Ambda - Alfa1 )*0.5;
		  T = 0.;
		}
		else if ( AA*Fxu < 0. && AA*Fyu > 0.) {
		  FFyu = FFi;
		  DFFyu = CC;
		  Fyu = AA;
		  DFyu = BB;
		  Ambda = ( Ambda - Alfa1 )*0.5;
		  T = 0.;
		  FFi = Min(FFxu , FFyu);
		  U = Xu;
		}
		else {
		  T =0.;
		  Ambda = Ambda - Alfa1;
		}           
	      }
	      else {
		T =0.;
		Ambda = Ambda - Alfa1;
	      }
	    }
	    // tests d arrets                    
	    if (Abs(FFxu) <= Standard_Underflow ||  
		(Abs(DFFxu) <= Standard_Underflow && Fxu*Fyu > 0.)
		){
	      Finish = Standard_True;
	      if (Abs(FFxu) <= Standard_Underflow ) {FFxu =0.0;}
	      FFi = FFyu;
	    }
	    else if (Abs(FFyu) <= Standard_Underflow ||  
		     (Abs(DFFyu) <= Standard_Underflow && Fxu*Fyu > 0.)
		     ){
	      Finish = Standard_True;
	      if (Abs(FFyu) <= Standard_Underflow ) {FFyu =0.0;}
	      FFi = FFxu;
	    }
	    else { 
	      Iter = Iter + 1;
	      Finish = Iter >= Nbiter || (Ambda <= EpsX &&
					  ( Fxu*Fyu >= 0. || FFi <= EpsF*EpsF));
	    }
	  }                          
	}  // fin interpolation cubique
	
	// restitution du meilleur resultat
	
	if ( FFxu < FFi ) { U = U + T -Ambda;}
	else if ( FFyu < FFi ) { U = U + T;}
	
	if ( U >= (Lowr -EpsX) && U <= (Upp + EpsX) ) {
	  U = Max( Lowr , Min( U , Upp));
	  Ok = F.Value(U,FFi);
	  FFi = FFi - K;
	  if ( Abs(FFi) < EpsF ) {
	    //coherence
	    if (Abs(Fxu) <= Standard_Underflow) { AA = DFxu;}
	    else if (Abs(Fyu) <= Standard_Underflow) { AA = DFyu;}
	    else if (Fxu*Fyu > 0.) { AA = 0.;} 
	    else { AA = Fyu-Fxu;}
	    if (!Sol.IsEmpty()) {
	      if (Abs(Sol.Last() - U) > 5.*EpsX 
		  || (OldDF != RealLast() && OldDF*AA < 0.) ) {
		Sol.Append(U);
		NbStateSol.Append(F.GetStateNumber());
	      }
	    }
	    else {
	      Sol.Append(U);
	      NbStateSol.Append(F.GetStateNumber()); 
	    }
	    OldDF = AA ;
	  }     
	}
	DFFyu = 0.;
	Fyu = 0.;
	Nn = 1;
	while ( Nn < 1000000 && DFFyu <= 0.) {
	  // on repart d 1 pouyem plus loin
	  U = U + Nn*EpsX;
	  if ( U <= Lowr ) {
	    AA = FLowr + (U - Lowr)*DFLowr;
	    BB = DFLowr;
	  }
	  else if ( U >= Upp ) {
	    AA = FUpp + (U - Upp)*DFUpp;
	    BB = DFUpp;
	  }
	  else {
	    Ok = F.Values(U,AA,BB);
	    AA = AA - K;
	  }
	  if ( AA*Fyu < 0.) {
	    U = U - Nn*EpsX;
	    Nn = 1000001;
	  }
	  else {
	    Fyu = AA;
	    DFyu = BB;
	    FFyu = AA*AA;
	    DFFyu = 2.*DFyu*Fyu;
	    Nn = 2*Nn;
	  }
	} 
      }
    }
  }
#if NEWSEQ
  #ifdef MATH_FUNCTIONROOTS_CHECK
  { 
    Standard_Integer n1=StaticSol.Length();
    Standard_Integer n2=Sol.Length();
    if(n1!=n2) { 
      printf("\n mathFunctionRoots : n1=%d  n2=%d EpsF=%g EpsX=%g\n",n1,n2,EpsF,NEpsX);
      for(Standard_Integer x1=1; x1<=n1;x1++) { 
	Standard_Real v; F.Value(StaticSol(x1),v); v-=K;
	printf(" (%+13.8g:%+13.8g) ",StaticSol(x1),v);
      }
      printf("\n");
      for(Standard_Integer x2=1; x2<=n2; x2++) { 
	Standard_Real v; F.Value(Sol(x2),v); v-=K;
	printf(" (%+13.8g:%+13.8g) ",Sol(x2),v);
      }
      printf("\n");
    }
    Standard_Integer n=n1; 
    if(n1>n2) n=n2;
    for(Standard_Integer i=1;i<=n;i++) { 
      Standard_Real t = Sol(i)-StaticSol(i);
      if(Abs(t)>NEpsX) { 
	printf("\n mathFunctionRoots : i:%d/%d  delta: %g",i,n,t);
      }
    }
  }
#endif
#endif
}
#endif
}


void math_FunctionRoots::Dump(Standard_OStream& o) const 
{
  
  o << "math_FunctionRoots ";
  if(Done) {
    o << " Status = Done \n";
    o << " Number of solutions = " << Sol.Length() << endl;
    for (Standard_Integer i = 1; i <= Sol.Length(); i++) {
      o << " Solution Number " << i << "= " << Sol.Value(i) << endl;
    }
  }
  else {
    o<< " Status = not Done \n";
  }
}
