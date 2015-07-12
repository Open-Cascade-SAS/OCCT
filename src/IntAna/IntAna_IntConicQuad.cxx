// Created on: 1992-07-27
// Created by: Laurent BUCHARD
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#endif

#define CREATE  IntAna_IntConicQuad::IntAna_IntConicQuad
#define PERFORM void IntAna_IntConicQuad::Perform


#include <ElCLib.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <IntAna_QuadQuadGeo.hxx>
#include <IntAna_Quadric.hxx>
#include <IntAna_ResultType.hxx>
#include <math_DirectPolynomialRoots.hxx>
#include <math_TrigonometricFunctionRoots.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

static Standard_Real PIpPI = M_PI + M_PI;
//=============================================================================
//==                                          E m p t y   C o n s t r u c t o r
//== 
CREATE(void) {
  done=Standard_False;
}
//=============================================================================
//==                                                 L i n e  -   Q u a d r i c  
//==
CREATE(const gp_Lin& L,const IntAna_Quadric& Quad) {
  Perform(L,Quad);
}

PERFORM(const gp_Lin& L,const IntAna_Quadric& Quad) {

  Standard_Real Qxx,Qyy,Qzz,Qxy,Qxz,Qyz,Qx,Qy,Qz,QCte;
  done=inquadric=parallel=Standard_False;

  //----------------------------------------------------------------------
  //-- Substitution de x=t Lx + Lx0       ( exprime dans                 )
  //--                 y=t Ly + Ly0       (  le systeme de coordonnees   )
  //--                 z=t Lz + Lz0       (  canonique                   )
  //--
  //-- Dans     Qxx x**2 + Qyy y**2 + Qzz z**2
  //--          + 2 ( Qxy x y  + Qxz x z  + Qyz y z  )
  //--          + 2 ( Qx x + Qy y + Qz z )
  //--          + QCte
  //--
  //-- Done un polynome en t : A2 t**2 + A1 t + A0 avec :
  //----------------------------------------------------------------------


  Standard_Real Lx0,Ly0,Lz0,Lx,Ly,Lz;


  nbpts=0;

  L.Direction().Coord(Lx,Ly,Lz);
  L.Location().Coord(Lx0,Ly0,Lz0);
 
  Quad.Coefficients(Qxx,Qyy,Qzz,Qxy,Qxz,Qyz,Qx,Qy,Qz,QCte);
  
  Standard_Real A0=(QCte + Qxx*Lx0*Lx0 + Qyy*Ly0*Ly0  + Qzz*Lz0*Lz0
	  + 2.0* (  Lx0*( Qx + Qxy*Ly0 + Qxz*Lz0) 
		  + Ly0*( Qy + Qyz*Lz0 )
		  + Qz*Lz0 )
	  );
	  
  
  Standard_Real A1=2.0*( Lx*( Qx + Qxx*Lx0 + Qxy*Ly0 + Qxz*Lz0 )
	      +Ly*( Qy + Qxy*Lx0 + Qyy*Ly0 + Qyz*Lz0 )
	      +Lz*( Qz + Qxz*Lx0 + Qyz*Ly0 + Qzz*Lz0 ));

  Standard_Real A2=(Qxx*Lx*Lx + Qyy*Ly*Ly + Qzz*Lz*Lz
	   + 2.0*( Lx*( Qxy*Ly + Qxz*Lz )
		  + Qyz*Ly*Lz));

  math_DirectPolynomialRoots LinQuadPol(A2,A1,A0);
  
  if( LinQuadPol.IsDone()) {
    done=Standard_True;
    if(LinQuadPol.InfiniteRoots()) {
      inquadric=Standard_True;
    }
    else {
      nbpts= LinQuadPol.NbSolutions();
      
      for(Standard_Integer i=1 ; i<=nbpts; i++) {
	Standard_Real t= LinQuadPol.Value(i);
	paramonc[i-1] = t;
	pnts[i-1]=gp_Pnt( Lx0+Lx*t
			 ,Ly0+Ly*t
			 ,Lz0+Lz*t);
      }
    }
  }
}

//=============================================================================
//==                                            C i r c l e   -   Q u a d r i c  
//==
CREATE(const gp_Circ& C,const IntAna_Quadric& Quad) {
  Perform(C,Quad); 
}

PERFORM(const gp_Circ& C,const IntAna_Quadric& Quad) {
  
  Standard_Real Qxx,Qyy,Qzz,Qxy,Qxz,Qyz,Qx,Qy,Qz,QCte;
  
  //----------------------------------------------------------------------
  //-- Dans le repere liee a C.Position() : 
  //-- xC = R * Cos[t]
  //-- yC = R * Sin[t]
  //-- zC = 0
  //--
  //-- On exprime la quadrique dans ce repere et on substitue 
  //-- xC,yC et zC    a    x,y et z 
  //-- 
  //-- On Obtient un polynome en Cos[t] et Sin[t] de degre 2
  //----------------------------------------------------------------------
  done=inquadric=parallel=Standard_False;  

  Quad.Coefficients(Qxx,Qyy,Qzz,Qxy,Qxz,Qyz,Qx,Qy,Qz,QCte);
  Quad.NewCoefficients(Qxx,Qyy,Qzz,Qxy,Qxz,Qyz,Qx,Qy,Qz,QCte,C.Position());
  
  Standard_Real R=C.Radius();
  Standard_Real RR=R*R;
  
  Standard_Real P_CosCos = RR * Qxx;    //-- Cos Cos
  Standard_Real P_SinSin = RR * Qyy;    //-- Sin Sin
  Standard_Real P_Sin    = R  * Qy;     //-- 2 Sin
  Standard_Real P_Cos    = R  * Qx;     //-- 2 Cos
  Standard_Real P_CosSin = RR * Qxy;    //-- 2 Cos Sin
  Standard_Real P_Cte    = QCte;        //-- 1
  
  math_TrigonometricFunctionRoots CircQuadPol( P_CosCos-P_SinSin
					      ,P_CosSin
					      ,P_Cos+P_Cos
					      ,P_Sin+P_Sin
					      ,P_Cte+P_SinSin
					      ,0.0,PIpPI);
  
  if(CircQuadPol.IsDone()) {
    done=Standard_True;
    if(CircQuadPol.InfiniteRoots()) {
      inquadric=Standard_True;
    }
    else {
      nbpts= CircQuadPol.NbSolutions();
      
      for(Standard_Integer i=1 ; i<=nbpts; i++) {
	Standard_Real t= CircQuadPol.Value(i);
	paramonc[i-1] = t;
	pnts[i-1]     = ElCLib::CircleValue(t,C.Position(),R);
      }
    }
  }
}


//=============================================================================
//==                                                  E l i p s - Q u a d r i c 
//==
CREATE(const gp_Elips& E,const IntAna_Quadric& Quad) {
  Perform(E,Quad);
}

PERFORM(const gp_Elips& E,const IntAna_Quadric& Quad) {
  
  Standard_Real Qxx,Qyy,Qzz,Qxy,Qxz,Qyz,Qx,Qy,Qz,QCte;
  
  done=inquadric=parallel=Standard_False;  
  
  //----------------------------------------------------------------------
  //-- Dans le repere liee a E.Position() : 
  //-- xE = R * Cos[t]
  //-- yE = r * Sin[t]
  //-- zE = 0
  //--
  //-- On exprime la quadrique dans ce repere et on substitue 
  //-- xE,yE et zE    a    x,y et z 
  //-- 
  //-- On Obtient un polynome en Cos[t] et Sin[t] de degre 2
  //----------------------------------------------------------------------
  
  Quad.Coefficients(Qxx,Qyy,Qzz,Qxy,Qxz,Qyz,Qx,Qy,Qz,QCte);
  Quad.NewCoefficients(Qxx,Qyy,Qzz,Qxy,Qxz,Qyz,Qx,Qy,Qz,QCte,E.Position());
  
  Standard_Real R=E.MajorRadius();  
  Standard_Real r=E.MinorRadius();   

  
  Standard_Real P_CosCos = R*R * Qxx;    //-- Cos Cos
  Standard_Real P_SinSin = r*r * Qyy;    //-- Sin Sin
  Standard_Real P_Sin    = r   * Qy;     //-- 2 Sin
  Standard_Real P_Cos    = R   * Qx;     //-- 2 Cos
  Standard_Real P_CosSin = R*r * Qxy;    //-- 2 Cos Sin
  Standard_Real P_Cte    = QCte;         //-- 1
  
  math_TrigonometricFunctionRoots ElipsQuadPol( P_CosCos-P_SinSin
					      ,P_CosSin
					      ,P_Cos+P_Cos
					      ,P_Sin+P_Sin
					      ,P_Cte+P_SinSin
					      ,0.0,PIpPI);
  
  if(ElipsQuadPol.IsDone()) {
    done=Standard_True;
    if(ElipsQuadPol.InfiniteRoots()) {
      inquadric=Standard_True;
    }
    else {
      nbpts= ElipsQuadPol.NbSolutions();
      for(Standard_Integer i=1 ; i<=nbpts; i++) {
	Standard_Real t= ElipsQuadPol.Value(i);
	paramonc[i-1] = t;
	pnts[i-1]     = ElCLib::EllipseValue(t,E.Position(),R,r);
      }
    }
  }
}

//=============================================================================
//==                                                  P a r a b - Q u a d r i c 
//==
CREATE(const gp_Parab& P,const IntAna_Quadric& Quad) {
  Perform(P,Quad);
}

PERFORM(const gp_Parab& P,const IntAna_Quadric& Quad) {
  
  Standard_Real Qxx,Qyy,Qzz,Qxy,Qxz,Qyz,Qx,Qy,Qz,QCte;
  
  done=inquadric=parallel=Standard_False;

  //----------------------------------------------------------------------
  //-- Dans le repere liee a P.Position() : 
  //-- xP = y*y / (2 p)
  //-- yP = y
  //-- zP = 0
  //--
  //-- On exprime la quadrique dans ce repere et on substitue 
  //-- xP,yP et zP    a    x,y et z 
  //-- 
  //-- On Obtient un polynome en y de degre 4
  //----------------------------------------------------------------------
  
  Quad.Coefficients(Qxx,Qyy,Qzz,Qxy,Qxz,Qyz,Qx,Qy,Qz,QCte);
  Quad.NewCoefficients(Qxx,Qyy,Qzz,Qxy,Qxz,Qyz,Qx,Qy,Qz,QCte,P.Position());
  
  Standard_Real f=P.Focal();
  Standard_Real Un_Sur_2p = 0.25 / f;

  Standard_Real A4 = Qxx * Un_Sur_2p * Un_Sur_2p;
  Standard_Real A3 = (Qxy+Qxy) * Un_Sur_2p;
  Standard_Real A2 = Qyy + (Qx+Qx) * Un_Sur_2p;
  Standard_Real A1 = Qy+Qy;
  Standard_Real A0 = QCte;
  
  math_DirectPolynomialRoots ParabQuadPol(A4,A3,A2,A1,A0);
  
  if( ParabQuadPol.IsDone()) {
    done=Standard_True;
    if(ParabQuadPol.InfiniteRoots()) {
      inquadric=Standard_True;
    }
    else {
      nbpts= ParabQuadPol.NbSolutions();
      
      for(Standard_Integer i=1 ; i<=nbpts; i++) {
	Standard_Real t= ParabQuadPol.Value(i);
	paramonc[i-1] = t;
	pnts[i-1]     = ElCLib::ParabolaValue(t,P.Position(),f);
      }
    }
  }  
}

//=============================================================================
//==                                                    H y p r - Q u a d r i c 
//==
CREATE(const gp_Hypr& H,const IntAna_Quadric& Quad) {
  Perform(H,Quad);
}

PERFORM(const gp_Hypr& H,const IntAna_Quadric& Quad) {
  
  Standard_Real Qxx,Qyy,Qzz,Qxy,Qxz,Qyz,Qx,Qy,Qz,QCte;

  done=inquadric=parallel=Standard_False;  

  //----------------------------------------------------------------------
  //-- Dans le repere liee a P.Position() : 
  //-- xH = R Ch[t]
  //-- yH = r Sh[t]
  //-- zH = 0
  //--
  //-- On exprime la quadrique dans ce repere et on substitue 
  //-- xP,yP et zP    a    x,y et z 
  //-- 
  //-- On Obtient un polynome en Exp[t] de degre 4
  //----------------------------------------------------------------------
  
  Quad.Coefficients(Qxx,Qyy,Qzz,Qxy,Qxz,Qyz,Qx,Qy,Qz,QCte);
  Quad.NewCoefficients(Qxx,Qyy,Qzz,Qxy,Qxz,Qyz,Qx,Qy,Qz,QCte,H.Position());
  
  Standard_Real R=H.MajorRadius();
  Standard_Real r=H.MinorRadius();

  Standard_Real RR=R*R;
  Standard_Real rr=r*r;
  Standard_Real Rr=R*r;

  Standard_Real A4 = RR * Qxx + Rr* ( Qxy + Qxy ) + rr * Qyy;
  Standard_Real A3 = 4.0* ( R*Qx + r*Qy );
  Standard_Real A2 = 2.0* ( (QCte+QCte) + Qxx*RR - Qyy*rr );
  Standard_Real A1 = 4.0* ( R*Qx - r*Qy);
  Standard_Real A0 = Qxx*RR - Rr*(Qxy+Qxy) + Qyy*rr; 
  
  math_DirectPolynomialRoots HyperQuadPol(A4,A3,A2,A1,A0);
  
  if( HyperQuadPol.IsDone()) {
    done=Standard_True;
    if(HyperQuadPol.InfiniteRoots()) {
      inquadric=Standard_True;
    }
    else {
      nbpts= HyperQuadPol.NbSolutions();
      Standard_Integer bonnessolutions = 0;
      for(Standard_Integer i=1 ; i<=nbpts; i++) {
	Standard_Real t= HyperQuadPol.Value(i);
	if(t>=RealEpsilon()) {
	  Standard_Real Lnt = Log(t);
	  paramonc[bonnessolutions] = Lnt;
	  pnts[bonnessolutions]     = ElCLib::HyperbolaValue(Lnt,H.Position(),R,r);
	  bonnessolutions++;
	}
      }
      nbpts=bonnessolutions;
    }
  } 
}
//=============================================================================




IntAna_IntConicQuad::IntAna_IntConicQuad (const gp_Lin& L, const gp_Pln& P,
                                          const Standard_Real Tolang,
                                          const Standard_Real Tol,
                                          const Standard_Real Len) {
  Perform(L,P,Tolang,Tol,Len);
}


IntAna_IntConicQuad::IntAna_IntConicQuad (const gp_Circ& C, const gp_Pln& P,
					  const Standard_Real Tolang,
					  const Standard_Real Tol) {
  Perform(C,P,Tolang,Tol);
}


IntAna_IntConicQuad::IntAna_IntConicQuad (const gp_Elips& E, const gp_Pln& P,
					  const Standard_Real Tolang,
					  const Standard_Real Tol) {
  Perform(E,P,Tolang,Tol);
}


IntAna_IntConicQuad::IntAna_IntConicQuad (const gp_Parab& Pb, const gp_Pln& P,
					  const Standard_Real Tolang){
  Perform(Pb,P,Tolang);
}


IntAna_IntConicQuad::IntAna_IntConicQuad (const gp_Hypr& H, const gp_Pln& P,
					  const Standard_Real Tolang){
  Perform(H,P,Tolang);
}


void IntAna_IntConicQuad::Perform (const gp_Lin& L, const gp_Pln& P,
                                   const Standard_Real Tolang,
                                   const Standard_Real Tol,
                                   const Standard_Real Len) {
  
  // Tolang represente la tolerance angulaire a partir de laquelle on considere
  // que l angle entre 2 vecteurs est nul. On raisonnera sur le cosinus de cet
  // angle, (on a Cos(t) equivalent a t au voisinage de Pi/2).
  
  done=Standard_False;
  
  Standard_Real A,B,C,D;
  Standard_Real Al,Bl,Cl;
  Standard_Real Dis,Direc;
  
  P.Coefficients(A,B,C,D);
  gp_Pnt Orig(L.Location());
  L.Direction().Coord(Al,Bl,Cl);

  Direc=A*Al+B*Bl+C*Cl;
  Dis = A*Orig.X() + B*Orig.Y() + C*Orig.Z() + D;
  //
  parallel=Standard_False;
  if (Abs(Direc) < Tolang) {
    parallel=Standard_True;
    if (Len!=0 && Direc!=0) {
      //check the distance from bounding point of the line to the plane
      gp_Pnt aP1, aP2;
      //
      aP1.SetCoord(Orig.X()-Dis*A, Orig.Y()-Dis*B, Orig.Z()-Dis*C);
      aP2.SetCoord(aP1.X()+Len*Al, aP1.Y()+Len*Bl, aP1.Z()+Len*Cl);
      if (P.Distance(aP2) > Tol) {
        parallel=Standard_False;
      } 
    }
  }
  if (parallel) {
    if (Abs(Dis) < Tolang) {
      inquadric=Standard_True;
    }
    else {
      inquadric=Standard_False;
    }
  }
  else {
    parallel=Standard_False;
    inquadric=Standard_False;
    nbpts = 1;
    paramonc [0] = - Dis/Direc;
    pnts[0].SetCoord(Orig.X()+paramonc[0]*Al,
                     Orig.Y()+paramonc[0]*Bl,
                     Orig.Z()+paramonc[0]*Cl);
  }
  done=Standard_True;
}


void IntAna_IntConicQuad::Perform (const gp_Circ& C, const gp_Pln& P,
				  const Standard_Real Tolang,
				  const Standard_Real Tol)
{
  
  done=Standard_False;
  
  gp_Pln Plconic(gp_Ax3(C.Position()));
  IntAna_QuadQuadGeo IntP(Plconic,P,Tolang,Tol);
  if (!IntP.IsDone()) {return;}
  if (IntP.TypeInter() == IntAna_Empty) {
    parallel=Standard_True;
    Standard_Real distmax = P.Distance(C.Location()) + C.Radius()*Tolang;
    if (distmax < Tol) {
      inquadric = Standard_True;
    }
    else {
      inquadric = Standard_False;
    }
    done=Standard_True;
  }
  else     if(IntP.TypeInter() == IntAna_Same) { 
    inquadric = Standard_True;
    done = Standard_True;
  }
  else {
    inquadric=Standard_False;
    parallel=Standard_False;
    gp_Lin Ligsol(IntP.Line(1));
    
    gp_Vec V0(Plconic.Location(),Ligsol.Location());
    gp_Vec Axex(Plconic.Position().XDirection());
    gp_Vec Axey(Plconic.Position().YDirection());
    
    gp_Pnt2d Orig(Axex.Dot(V0),Axey.Dot(V0));
    gp_Vec2d Dire(Axex.Dot(Ligsol.Direction()),
		  Axey.Dot(Ligsol.Direction()));
    
    gp_Lin2d Ligs(Orig,Dire);
    gp_Pnt2d Pnt2dBid(0.0,0.0);
    gp_Dir2d Dir2dBid(1.0,0.0);
    gp_Ax2d Ax2dBid(Pnt2dBid,Dir2dBid);
    gp_Circ2d Cir(Ax2dBid,C.Radius());
    
    IntAna2d_AnaIntersection Int2d(Ligs,Cir);
    
    if (!Int2d.IsDone()) {return;}
    
    nbpts=Int2d.NbPoints();
    for (Standard_Integer i=1; i<=nbpts; i++) {
      
      gp_Pnt2d resul(Int2d.Point(i).Value());
      Standard_Real X= resul.X();
      Standard_Real Y= resul.Y();
      pnts[i-1].SetCoord(Plconic.Location().X() + X*Axex.X() + Y*Axey.X(),
			 Plconic.Location().Y() + X*Axex.Y() + Y*Axey.Y(),
			 Plconic.Location().Z() + X*Axex.Z() + Y*Axey.Z());
      paramonc[i-1]=Int2d.Point(i).ParamOnSecond();
    }
    done=Standard_True;
  }
}


void IntAna_IntConicQuad::Perform (const gp_Elips& E, const gp_Pln& Pln,
				   const Standard_Real,
				   const Standard_Real){
  Perform(E,Pln);
}


void IntAna_IntConicQuad::Perform (const gp_Parab& P, const gp_Pln& Pln,
				   const Standard_Real){
  Perform(P,Pln);
}


void IntAna_IntConicQuad::Perform (const gp_Hypr& H, const gp_Pln& Pln,
				   const Standard_Real){
  Perform(H,Pln);
}


