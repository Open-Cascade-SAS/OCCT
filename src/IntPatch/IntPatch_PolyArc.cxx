// Created on: 1993-01-27
// Created by: Isabelle GRIGNON
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Adaptor2d_HCurve2d.hxx>
#include <Bnd_Box2d.hxx>
#include <gp_Pnt2d.hxx>
#include <IntPatch_PolyArc.hxx>
#include <Standard_ConstructionError.hxx>

inline void MinMax (const Standard_Real a1, const Standard_Real a2,
		    Standard_Real& amin, Standard_Real& amax)
{
  if (a1 < a2) {
    amin = a1; amax = a2;
  }
  else {
    amin = a2; amax = a1;
  }
}

IntPatch_PolyArc::IntPatch_PolyArc(const Handle(Adaptor2d_HCurve2d)& Line ,
				   const Standard_Integer NbSample,
				   const Standard_Real aPdeb,
				   const Standard_Real aPfin,
				   const Bnd_Box2d& BoxOtherPolygon):
				   brise(1,Max(1,NbSample)),
				   param(1,Max(1,NbSample)),
				   offsetx(0.0),
				   offsety(0.0)
{
  Standard_Real Pdeb = aPdeb;
  Standard_Real Pfin = aPfin;
  gp_Pnt2d p2d;
  
  if (Pdeb == RealFirst() || Pfin == RealLast() || NbSample <= 1) {
    throw Standard_ConstructionError();
  }
  //----------------------------------------------------------------------
  //-- On veut eviter les cas ou  le present polygone est beaucoup plus 
  //-- grand que l objet en second.
  //-- 
  //-- Par exemple lorsque l objet en second est une ligne de cheminement
  //-- qui contient de nombreux segments (>>100), une fleche nulle 
  //-- et ce polygone quelques segments et une fleche qui contient
  //-- toute la ligne de cheminement. 
  //--
  //-- Dans ce cas (tout un polygone compris dans la zone d influence)
  //-- les calculs deviennent tres longs (N2)
  //----------------------------------------------------------------------
  Standard_Integer IndexInf = NbSample+1;
  Standard_Integer IndexSup = 0;
  
  Standard_Real bx0,by0,bxmin,bxmax,bymin,bymax,r,r2;
  
  BoxOtherPolygon.Get(bxmin,bymin,bxmax,bymax);
  r=(bxmax-bxmin)+(bymax-bymin);
  bx0=(bxmax+bxmin)*0.5;
  by0=(bymax+bymin)*0.5;
  
  Standard_Real Pas;
  Standard_Real X,Y,Xs,Ys,Xm,Ym,XXs,YYs;
  
  r*=0.8;
  r2 = r*r*49.;
  Standard_Integer nbloop=0;
  
  do { 
    nbloop++;
    Pas = (Pfin-Pdeb)/(NbSample-1);
    param(1) = Pdeb;
    Line->D0(Pdeb,p2d);
    Xs = p2d.X(); Ys = p2d.Y();
    brise(1).SetCoord(Xs,Ys);
    
    myBox.SetVoid();
    
    myBox.Add(brise(1));
    myError =0.;
    
    for (Standard_Integer i =2; i<=NbSample;i++) {
      param(i) = Pdeb + (i-1)*Pas;
      Line->D0(param(i),p2d);
      X = p2d.X(); Y = p2d.Y();
      brise(i).SetCoord(X,Y);
      XXs = 0.5*(Xs+X);
      YYs = 0.5*(Ys+Y);
      //------------------------------------------------------------
      //-- On recherche le debut et la fin de la zone significative
      //------------------------------------------------------------
      // MSV: (see cda 002 H2) if segment is too large (>>r) we have
      //      a risk to jump through BoxOtherPolygon, therefore we should
      //      check this condition if the first one is failure.
      Standard_Boolean isMidPtInBox = (Abs(bx0-XXs) + Abs(by0-YYs)) < r;
      Standard_Boolean isSegOut = Standard_True;
      if(!isMidPtInBox) {
	Standard_Real d = (X-Xs)*(X-Xs)+(Y-Ys)*(Y-Ys);
	if (d > r2) {
	  Standard_Real xmin,xmax,ymin,ymax;
	  MinMax (Xs,X, xmin,xmax);
	  MinMax (Ys,Y, ymin,ymax);
	  isSegOut = (xmax < bxmin || xmin > bxmax ||
		      ymax < bymin || ymin > bymax);
	}
      }
      if(isMidPtInBox || !isSegOut) { 
	// MSV: take the previous and the next segments too, because of
	//      we check only the middle point (see BUC60946)
	//if(IndexInf>i) IndexInf=i-1;
	//if(IndexSup<i) IndexSup=i;
	if(IndexInf>i) IndexInf=Max(i-2,1);
	if(IndexSup<i) IndexSup=Min(i+1,NbSample);
      }
      
      myBox.Add(brise(i));
      Line->D0(param(i)-Pas*0.5,p2d);
      Xm = p2d.X() - XXs;
      Ym = p2d.Y() - YYs;
      Xm = Sqrt(Xm*Xm+Ym*Ym);
      myError =Max (myError , Xm);
      Xs = X;
      Ys = Y;
    }
    if(IndexInf > IndexSup) { 
      r+=r; 
      r2 = r*r*49.;
      //-- cout<<" Le rayon : "<<r<<" est insuffisant "<<endl;
    }
    else {
      //----------------------------------------------
      //-- Si le nombre de points significatifs est
      //-- insuffisant, on reechantillonne une fois
      //-- encore
      //----------------------------------------------
      if((IndexSup-IndexInf)<(NbSample/2)) { 
	//-- cout<<" --- On remet ca entre les index "<<IndexInf<<" et "<<IndexSup<<endl;
	nbloop = 10;
	//if(IndexInf>1) IndexInf--;
	//if(IndexSup<NbSample) IndexSup++;
	Pdeb = param(IndexInf); 
	Pfin = param(IndexSup);
	//IndexInf = IndexSup+1;
	IndexInf = NbSample+1;
	IndexSup = 0;
      }
    }
  }
  while((IndexInf > IndexSup) && nbloop<=10); 
  myError*=1.2;
  if(myError<0.00000001) 
    myError = 0.00000001;
  myBox.Enlarge(myError);

  ferme = (Line->Value(aPdeb).Distance(Line->Value(aPfin)) <= 1e-7);
}

Standard_Boolean IntPatch_PolyArc::Closed() const { return ferme;}

Standard_Integer IntPatch_PolyArc::NbPoints() const {return brise.Length();}

gp_Pnt2d IntPatch_PolyArc::Point(const Standard_Integer Index ) const 
{ 
  if(offsetx == 0.0 && offsety==0.0) 
    return(brise(Index));
  
  const gp_Pnt2d& P = brise(Index);
  return (gp_Pnt2d(P.X()+offsetx,P.Y()+offsety));
}

Standard_Real IntPatch_PolyArc::Parameter(const Standard_Integer Index ) const
{ return param(Index);}


void IntPatch_PolyArc::SetOffset(const Standard_Real ox,const Standard_Real oy) { 
  Standard_Real xmin,ymin,xmax,ymax,g;
  myBox.Get(xmin,ymin,xmax,ymax);
  g = myBox.GetGap();
  
  myBox.SetVoid();
  
  myBox.Update(xmin-offsetx,ymin-offsety,
	       xmax-offsetx,ymax-offsety);
  offsetx = ox;
  offsety = oy;
  myBox.Update(xmin+offsetx,ymin+offsety,
	       xmax+offsetx,ymax+offsety);
  myBox.SetGap(g);
}
