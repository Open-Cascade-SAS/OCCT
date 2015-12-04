// Created on: 1992-04-06
// Created by: Jacques GOUSSARD
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


#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <IntAna_Curve.hxx>
#include <IntPatch_ALine.hxx>
#include <IntPatch_Point.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IntPatch_ALine,IntPatch_Line)

#define DEBUG 0

IntPatch_ALine::IntPatch_ALine (const IntAna_Curve& C,
				const Standard_Boolean Tang,
				const IntSurf_TypeTrans Trans1,
				const IntSurf_TypeTrans Trans2) :
  IntPatch_Line(Tang,Trans1,Trans2),
  fipt(Standard_False),lapt(Standard_False)

{
  typ = IntPatch_Analytic;
  curv = C;
}


IntPatch_ALine::IntPatch_ALine (const IntAna_Curve& C,
				const Standard_Boolean Tang,
				const IntSurf_Situation Situ1,
				const IntSurf_Situation Situ2) :
  IntPatch_Line(Tang,Situ1,Situ2),
  fipt(Standard_False),lapt(Standard_False)

{
  typ = IntPatch_Analytic;
  curv = C;
}


IntPatch_ALine::IntPatch_ALine (const IntAna_Curve& C,
				const Standard_Boolean Tang) :
  IntPatch_Line(Tang),
  fipt(Standard_False),lapt(Standard_False)

{
  typ = IntPatch_Analytic;
  curv = C;
}

const IntAna_Curve&  IntPatch_ALine::Curve() const { 
  return(curv);
}


#define PCONFUSION 0.00001


void IntPatch_ALine::AddVertex (const IntPatch_Point& VTXj) {
#if 0
  Standard_Integer n = NbVertex();
  if(n>=1) { 
    Standard_Real par = VTXj.ParameterOnLine();

    for(int i=1;  i<=n  ;i++) { 
      const IntPatch_Point& VTXi   = svtx.Value(i);
      if((VTXj.IsOnDomS1()==Standard_False) && (VTXj.IsOnDomS2()==Standard_False)) {
	if((VTXi.IsOnDomS1()==Standard_False) && (VTXi.IsOnDomS2()==Standard_False)) {
	  if(Abs(par-VTXi.ParameterOnLine())<=PCONFUSION) { 
#if DEBUG
	    cout<<" Rejet  IntPatch_ALine::AddVertex   (0) "<<endl;
#endif
	    return;
	  }
	}
      }
    }
    for(i=1;  i<=n  ;i++) { 
      const IntPatch_Point& VTXi   = svtx.Value(i);
      if((VTXj.IsOnDomS1()==Standard_True) && (VTXj.IsOnDomS2()==Standard_False)) {
	if((VTXi.IsOnDomS1()==Standard_True) && (VTXi.IsOnDomS2()==Standard_False)) {
	  if(Abs(VTXi.ParameterOnArc1()-VTXj.ParameterOnArc1())<=PCONFUSION) { 
	    if(VTXi.ArcOnS1() == VTXj.ArcOnS1()) {
#if DEBUG
	      cout<<" Rejet  IntPatch_ALine::AddVertex   (1) "<<endl;
#endif
	      return;
	    }
	  }
	}
      }
    }
    for(i=1;  i<=n  ;i++) { 
      const IntPatch_Point& VTXi   = svtx.Value(i);
      if((VTXj.IsOnDomS2()==Standard_True) && (VTXj.IsOnDomS1()==Standard_False)) {
	if((VTXi.IsOnDomS2()==Standard_True) && (VTXi.IsOnDomS1()==Standard_False)) {
	  if(Abs(VTXi.ParameterOnArc2()-VTXj.ParameterOnArc2())<=PCONFUSION) { 
	    if(VTXi.ArcOnS2() == VTXj.ArcOnS2()) {
#if DEBUG
	      cout<<" Rejet  IntPatch_ALine::AddVertex   (2) "<<endl;
#endif
	      return;
	    }
	  }
	}
      }
    }
    for(i=1;  i<=n  ;i++) { 
      const IntPatch_Point& VTXi   = svtx.Value(i);
      if((VTXj.IsOnDomS2()==Standard_True) && (VTXj.IsOnDomS1()==Standard_True)) {
	if((VTXi.IsOnDomS2()==Standard_True) && (VTXi.IsOnDomS1()==Standard_True)) {
	  if(Abs(VTXi.ParameterOnArc2()-VTXj.ParameterOnArc2())<=PCONFUSION) { 
	    if(Abs(VTXi.ParameterOnArc1()-VTXj.ParameterOnArc1())<=PCONFUSION) { 
	      if(VTXi.ArcOnS2() == VTXj.ArcOnS2()) {
		if(VTXi.ArcOnS1() == VTXj.ArcOnS1()) {
#if DEBUG
		  cout<<" Rejet  IntPatch_ALine::AddVertex   (3) "<<endl;
#endif
		  return;
		}
	      }
	    }
	  }
	}
      }
    }

    //-- Est ce que VTXj present sur 1 et 2  remplace un point VTXi present sur 1 
    for(i=1;  i<=n  ;i++) { 
      const IntPatch_Point& VTXi   = svtx.Value(i);
      if((VTXj.IsOnDomS2()==Standard_True) && (VTXj.IsOnDomS1()==Standard_True)) {
	if((VTXi.IsOnDomS2()==Standard_False) && (VTXi.IsOnDomS1()==Standard_True)) {
	  Standard_Real p = Abs(VTXi.ParameterOnArc1()-VTXj.ParameterOnArc1());
#if DEBUG
	  cout<<" Est ce que VTXj present sur 1 et 2  remplace un point VTXi present sur 1 : "<<p<<endl;
#endif
	  if(p<=PCONFUSION) { 
	    if(VTXi.ArcOnS1() == VTXj.ArcOnS1()) {
#if DEBUG
	      cout<<" Replace  IntPatch_ALine::AddVertex   (1) "<<endl;
#endif
	      Replace(i,VTXj);
	      return;
	    }
	  }
	}
      }
    }
    //-- Est ce que VTXj present sur 1 et 2  remplace un point VTXi present sur 2 
    for(i=1;  i<=n  ;i++) { 
      const IntPatch_Point& VTXi   = svtx.Value(i);
      if((VTXj.IsOnDomS2()==Standard_True) && (VTXj.IsOnDomS1()==Standard_True)) {
	if((VTXi.IsOnDomS2()==Standard_True) && (VTXi.IsOnDomS1()==Standard_False)) {
	  Standard_Real p = Abs(VTXi.ParameterOnArc2()-VTXj.ParameterOnArc2());
#if DEBUG
	  cout<<" Est ce que VTXj present sur 1 et 2  remplace un point VTXi present sur 2 : "<<p<<endl;
#endif
	  if(p<=PCONFUSION) { 
	    if(VTXi.ArcOnS2() == VTXj.ArcOnS2()) {
#if DEBUG
	      cout<<" Replace  IntPatch_ALine::AddVertex   (2) "<<endl;
#endif
	      Replace(i,VTXj);
	      return;
	    }
	  }
	}
      }
    }


    //-- Est ce que VTXi deja present sur 1 et 2  et un point  VTXj present sur 1 
    for(i=1;  i<=n  ;i++) { 
      const IntPatch_Point& VTXi   = svtx.Value(i);
      if((VTXi.IsOnDomS2()==Standard_True) && (VTXi.IsOnDomS1()==Standard_True)) {
	if((VTXj.IsOnDomS2()==Standard_False) && (VTXj.IsOnDomS1()==Standard_True)) {
	  Standard_Real p = Abs(VTXi.ParameterOnArc1()-VTXj.ParameterOnArc1());
	  if(p<=PCONFUSION) { 
	    if(VTXi.ArcOnS1() == VTXj.ArcOnS1()) {
#if DEBUG
	      cout<<" Replace  IntPatch_ALine::AddVertex   (1)  -> RIEN "<<endl;
#endif
	      return;
	    }
	  }
	}
      }
    }
    //-- Est ce que VTXj present sur 1 et 2  remplace un point VTXi present sur 2 
    for(i=1;  i<=n  ;i++) { 
      const IntPatch_Point& VTXi   = svtx.Value(i);
      if((VTXi.IsOnDomS2()==Standard_True) && (VTXi.IsOnDomS1()==Standard_True)) {
	if((VTXj.IsOnDomS2()==Standard_True) && (VTXj.IsOnDomS1()==Standard_False)) {
	  Standard_Real p = Abs(VTXi.ParameterOnArc2()-VTXj.ParameterOnArc2());
	  if(p<=PCONFUSION) { 
	    if(VTXi.ArcOnS2() == VTXj.ArcOnS2()) {
	      return;
	    }
	  }
	}
      }
    }
    svtx.Append(VTXj);
    
  }
  else { 
    svtx.Append(VTXj);
  }
#else
    svtx.Append(VTXj);
#endif
}





void IntPatch_ALine::ComputeVertexParameters(const Standard_Real Tol) { 
  Standard_Boolean   SortIsOK,APointDeleted;
  Standard_Boolean   SortAgain = Standard_True;
  Standard_Integer   nbvtx,i,j;
  Standard_Real ParamMinOnLine,ParamMaxOnLine;
  Standard_Boolean OpenFirst,OpenLast;
  
  ParamMinOnLine = FirstParameter(OpenFirst);
  ParamMaxOnLine = LastParameter(OpenLast);
  

  //----------------------------------------------------------
  //--     F i l t r e   s u r   r e s t r i c t i o n s   --
  //----------------------------------------------------------
  //-- deux vertex sur la meme restriction et seulement 
  //-- sur celle ci ne doivent pas avoir le meme parametre
  //--
  //-- 

  //-- Le tri est necessaire si suppression du first ou du last point 
  nbvtx = NbVertex();


  //-- On verifie qu un vertex a bien toute ses representations : 
  //-- Cas tres rare : point de tangence sur un debut de ligne 
  //-- et la ligne fait 2 * 2 PI de parametrage. 

  for(i=1; i<=nbvtx; i++) { 
    IntPatch_Point& VTX   = svtx.ChangeValue(i); 
    Standard_Real p=VTX.ParameterOnLine();
    Standard_Real pmpimpi=p-M_PI-M_PI;
    if(pmpimpi >= ParamMinOnLine) { 
      gp_Pnt P1 = Value(pmpimpi);
      Standard_Real d1 = P1.Distance(VTX.Value());
      if(d1<Tol) { 
	IntPatch_Point OVTX(VTX);
	OVTX.SetParameter(pmpimpi);
	svtx.Append(OVTX);
      }
    }
    pmpimpi=p+M_PI+M_PI;
    if(pmpimpi <= ParamMaxOnLine) { 
      gp_Pnt P1 = Value(pmpimpi);
      Standard_Real d1 = P1.Distance(VTX.Value());
      if(d1<Tol) { 
	IntPatch_Point OVTX(VTX);
	OVTX.SetParameter(pmpimpi);
	svtx.Append(OVTX);
      }
    }
  }

  
  nbvtx=NbVertex();
  


  if(nbvtx<=0) return;
  do { 
    SortIsOK = Standard_True;
    for(i=2; i<=nbvtx; i++) { 
      if(svtx.Value(i-1).ParameterOnLine()  > svtx.Value(i).ParameterOnLine()) { 
	SortIsOK = Standard_False;
	svtx.Exchange(i-1,i);
	if(fipt) { if(indf==i) indf=i-1; else if(indf==i-1) indf=i; }
	if(lapt) { if(indl==i) indl=i-1; else if(indl==i-1) indl=i; }
      }
    }
  }
  while(!SortIsOK);

  do { 
    APointDeleted = Standard_False;
    for(i=1; (i<=nbvtx) && (APointDeleted==Standard_False) ;i++) { 
      const IntPatch_Point& VTXi   = svtx.Value(i);
      if((VTXi.IsOnDomS1()==Standard_True) && (VTXi.IsOnDomS2()==Standard_False)) { 
	for(j=1; (j<=nbvtx) && (APointDeleted==Standard_False) ;j++) {
	  if(i!=j) { 
	    const IntPatch_Point& VTXj   = svtx.Value(j);
	    if((VTXj.IsOnDomS1()==Standard_True) && (VTXj.IsOnDomS2()==Standard_False)) {
	      if(Abs(VTXi.ParameterOnArc1()-VTXj.ParameterOnArc1())<=PCONFUSION) { 
		if(Abs(VTXi.ParameterOnLine()-VTXj.ParameterOnLine())<=PCONFUSION) {
		  if(VTXi.ArcOnS1() == VTXj.ArcOnS1()) { 
		    svtx.Remove(j);
		    nbvtx--;
		    if(lapt) { if(indl>=j) indl--; } 
		    if(fipt) { if(indf>=j) indf--; } 
		    APointDeleted = Standard_True;
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
  while(APointDeleted == Standard_True);

  do { 
    APointDeleted = Standard_False;
    for(i=1; (i<=nbvtx) && (APointDeleted==Standard_False) ;i++) { 
      const IntPatch_Point& VTXi   = svtx.Value(i);
      if((VTXi.IsOnDomS2()==Standard_True) && (VTXi.IsOnDomS1()==Standard_False)) { 
	for(j=1; (j<=nbvtx) && (APointDeleted==Standard_False) ;j++) {
	  if(i!=j) { 
	    const IntPatch_Point& VTXj   = svtx.Value(j);
	    if((VTXj.IsOnDomS2()==Standard_True) && (VTXj.IsOnDomS1()==Standard_False)) {
	      if(Abs(VTXi.ParameterOnArc2()-VTXj.ParameterOnArc2())<=PCONFUSION) { 
		if(Abs(VTXi.ParameterOnLine()-VTXj.ParameterOnLine())<=PCONFUSION) {
		  if(VTXi.ArcOnS2() == VTXj.ArcOnS2()) { 
		    svtx.Remove(j);
		    nbvtx--;
		    if(lapt) { if(indl>=j) indl--; } 
		    if(fipt) { if(indf>=j) indf--; } 
		    APointDeleted = Standard_True;
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
  while(APointDeleted == Standard_True);
	

  //----------------------------------------------------------
  //-- Tri des vertex et suppression des Vtx superflus
  //-- 
  do { 
    nbvtx     = NbVertex();
    if(SortAgain) { 
      do { 
	SortIsOK = Standard_True;
	for(i=2; i<=nbvtx; i++) { 
	  if(svtx.Value(i-1).ParameterOnLine()  > svtx.Value(i).ParameterOnLine()) { 
	    SortIsOK = Standard_False;
	    svtx.Exchange(i-1,i);
	    if(fipt) { if(indf==i) indf=i-1; else if(indf==i-1) indf=i; }
	    if(lapt) { if(indl==i) indl=i-1; else if(indl==i-1) indl=i; }
	  }
	}
      }
      while(!SortIsOK);
    }
    SortAgain = Standard_False;
    SortIsOK = Standard_True; 
    for(i=2; i<=nbvtx && SortIsOK; i++) { 
      IntPatch_Point& VTX   = svtx.ChangeValue(i);      
      for(j=1; j<i && SortIsOK; j++) {
	IntPatch_Point& VTXM1 = svtx.ChangeValue(j);
	Standard_Boolean kill   = Standard_False;
	Standard_Boolean killm1 = Standard_False;
	if(Abs(VTXM1.ParameterOnLine()-VTX.ParameterOnLine())<PCONFUSION) { 
	  if(VTXM1.IsOnDomS1() && VTX.IsOnDomS1()) {  //-- OnS1    OnS1
	    if(VTXM1.ArcOnS1() == VTX.ArcOnS1()) {    //-- OnS1 == OnS1
	      if(VTXM1.IsOnDomS2()) {                 //-- OnS1 == OnS1  OnS2  
		if(VTX.IsOnDomS2()==Standard_False) {   //-- OnS1 == OnS1  OnS2 PasOnS2
		  kill=Standard_True;   
		}
		else {
		  if(VTXM1.ArcOnS2() == VTX.ArcOnS2()) { //-- OnS1 == OnS1  OnS2 == OnS2
		    kill=Standard_True;
		  }
		}
	      }
	      else {                                  //-- OnS1 == OnS1  PasOnS2  
		if(VTX.IsOnDomS2()) {                 //-- OnS1 == OnS1  PasOnS2  OnS2
		  killm1=Standard_True;
		}
	      }
	    }
	  }
	  else { //-- Pas OnS1  et  OnS1
	    if(VTXM1.IsOnDomS2()==Standard_False && VTX.IsOnDomS2()==Standard_False) { 
	      if(VTXM1.IsOnDomS1() && VTX.IsOnDomS1()==Standard_False) { 
		kill=Standard_True;
	      }
	      else  if(VTX.IsOnDomS1() && VTXM1.IsOnDomS1()==Standard_False) { 
		killm1=Standard_True;
	      }
	    }
	  }
	  
	  if(!(kill || killm1)) {
	    if(VTXM1.IsOnDomS2() && VTX.IsOnDomS2()) {  //-- OnS2    OnS2
	      if(VTXM1.ArcOnS2() == VTX.ArcOnS2()) {    //-- OnS2 == OnS2
		if(VTXM1.IsOnDomS1()) {                 //-- OnS2 == OnS2  OnS1  
		  if(VTX.IsOnDomS1()==Standard_False) {   //-- OnS2 == OnS2  OnS1 PasOnS1
		    kill=Standard_True;   
		  }
		  else {
		    if(VTXM1.ArcOnS1() == VTX.ArcOnS1()) { //-- OnS2 == OnS2  OnS1 == OnS1
		      kill=Standard_True;
		    }
		  }
		}
		else {                                  //-- OnS2 == OnS2  PasOnS1  
		  if(VTX.IsOnDomS1()) {                 //-- OnS2 == OnS2  PasOnS1  OnS1
		    killm1=Standard_True;
		  }
		}
	      }
	    }
	    else { //-- Pas OnS2  et  OnS2
	      if(VTXM1.IsOnDomS1()==Standard_False && VTX.IsOnDomS1()==Standard_False) { 
		if(VTXM1.IsOnDomS2() && VTX.IsOnDomS2()==Standard_False) { 
		  kill=Standard_True;
		}
		else  if(VTX.IsOnDomS2() && VTXM1.IsOnDomS2()==Standard_False) { 
		  killm1=Standard_True;
		}
	      }
	    }
	  }
	  //-- On a j < i 
	  if(kill) { 
	    SortIsOK = Standard_False;
	    if(lapt) { if(indl>i) indl--; else if(indl==i) indl=j; } 
	    if(fipt) { if(indf>i) indf--; else if(indf==i) indf=j; } 
	    svtx.Remove(i);
	    nbvtx--;
	  }
	  else if(killm1) { 
	    SortIsOK = Standard_False;
	    if(lapt) { if(indl>j) indl--;  else if(indl==j) indl=i-1;} 
	    if(fipt) { if(indf>j) indf--;  else if(indf==j) indf=i-1;} 
	    svtx.Remove(j);
	    nbvtx--; 
	  }
	}
      }
    }
  }
  while(!SortIsOK);


  //----------------------------------------------------------
  //--   Traitement des lignes periodiques                  --
  //----------------------------------------------------------
  if(OpenFirst == Standard_False && OpenLast == Standard_False) { 
    nbvtx     = NbVertex();
    
    IntPatch_Point& VTX0   = svtx.ChangeValue(1);
    IntPatch_Point& VTXN   = svtx.ChangeValue(nbvtx);
    if(VTX0.ParameterOnLine() == ParamMinOnLine) { 
      if(VTXN.ParameterOnLine() !=ParamMaxOnLine) { 
	gp_Pnt PN=Value(ParamMaxOnLine);
	Standard_Real d = PN.Distance(VTX0.Value());
	if(d<=Tol) { 
	  IntPatch_Point OVTX(VTX0);
	  OVTX.SetParameter(ParamMaxOnLine);
	  svtx.Append(OVTX);
	}
      }
      else { 
	if(VTXN.ParameterOnLine() == ParamMaxOnLine) { 
	  if(VTX0.ParameterOnLine() !=ParamMinOnLine) { 
	    gp_Pnt P0=Value(ParamMinOnLine);
	    Standard_Real d = P0.Distance(VTX0.Value());
	    if(d<=Tol) { 
	      IntPatch_Point OVTX(VTXN);
	      OVTX.SetParameter(ParamMinOnLine);
	      svtx.Prepend(OVTX);
	    }  
	  }
	}
      }
    }
  }
  //---------------------------------------------------------
  //-- Faut il supprimer le premier et le dernier point 
  //--
  nbvtx     = NbVertex();
  if(nbvtx>1) { 
    IntPatch_Point& VTX0   = svtx.ChangeValue(1);
    if(   (VTX0.IsOnDomS1() == Standard_False)
       && (VTX0.IsOnDomS2() == Standard_False)) { 
      svtx.Remove(1);
      nbvtx--;
      if(lapt) { 
	indl--;
      }
    }
  }
  if(nbvtx>1) { 
    IntPatch_Point& VTX0   = svtx.ChangeValue(nbvtx);
    if(   (VTX0.IsOnDomS1() == Standard_False)
       && (VTX0.IsOnDomS2() == Standard_False)) { 
      svtx.Remove(nbvtx);
      if(lapt) { 
        indl--;
      }
    }
  }

  //-- Si 2 vertex ont le meme parametre   on identifie le p3d
  nbvtx     = NbVertex();
  do { 
    SortIsOK = Standard_True;
    for(i=2; i<=nbvtx; i++) { 
      IntPatch_Point& VTX   = svtx.ChangeValue(i);
      IntPatch_Point& VTXm1 = svtx.ChangeValue(i-1);
      if(Abs(VTX.ParameterOnLine()-VTXm1.ParameterOnLine())<PCONFUSION) { 
	if(VTX.IsOnDomS1() && VTXm1.IsOnDomS1()==Standard_False) { 
	  VTXm1.SetArc(Standard_True,
		       VTX.ArcOnS1(),
		       VTX.ParameterOnArc1(),
		       VTX.TransitionLineArc1(),
		       VTX.TransitionOnS1());
	}
	else if(VTXm1.IsOnDomS1() && VTX.IsOnDomS1()==Standard_False) { 
	  VTX.SetArc(Standard_True,
		     VTXm1.ArcOnS1(),
		     VTXm1.ParameterOnArc1(),
		     VTXm1.TransitionLineArc1(),
		     VTXm1.TransitionOnS1());
	}
	if(VTX.IsVertexOnS1() && VTXm1.IsVertexOnS1()==Standard_False) { 
	  VTXm1.SetVertex(Standard_True, VTX.VertexOnS1());
	  VTXm1.SetArc(Standard_True,
		       VTX.ArcOnS1(),
		       VTX.ParameterOnArc1(),
		       VTX.TransitionLineArc1(),
		       VTX.TransitionOnS1());
	}
	else if(VTXm1.IsVertexOnS1() && VTX.IsVertexOnS1()==Standard_False) { 
	  VTX.SetVertex(Standard_True,	VTXm1.VertexOnS1());
	  VTX.SetArc(Standard_True,
		     VTXm1.ArcOnS1(),
		     VTXm1.ParameterOnArc1(),
		     VTXm1.TransitionLineArc1(),
		     VTXm1.TransitionOnS1());
	}

	if(VTX.IsOnDomS2() && VTXm1.IsOnDomS2()==Standard_False) { 
	  VTXm1.SetArc(Standard_False,
		       VTX.ArcOnS2(),
		       VTX.ParameterOnArc2(),
		       VTX.TransitionLineArc2(),
		       VTX.TransitionOnS2());
	}
	else if(VTXm1.IsOnDomS2() && VTX.IsOnDomS2()==Standard_False) { 
	  VTX.SetArc(Standard_False,
		     VTXm1.ArcOnS2(),
		     VTXm1.ParameterOnArc2(),
		     VTXm1.TransitionLineArc2(),
		     VTXm1.TransitionOnS2());
	}
	if(VTX.IsVertexOnS2() && VTXm1.IsVertexOnS2()==Standard_False) { 
	  VTXm1.SetVertex(Standard_False, VTX.VertexOnS2());
	  VTXm1.SetArc(Standard_False,
		       VTX.ArcOnS2(),
		       VTX.ParameterOnArc2(),
		       VTX.TransitionLineArc2(),
		       VTX.TransitionOnS2());
	}
	else if(VTXm1.IsVertexOnS2() && VTX.IsVertexOnS2()==Standard_False) { 
	  VTX.SetVertex(Standard_False,	VTXm1.VertexOnS2());
	  VTX.SetArc(Standard_False,
		     VTXm1.ArcOnS2(),
		     VTXm1.ParameterOnArc2(),
		     VTXm1.TransitionLineArc2(),
		     VTXm1.TransitionOnS2());
	}
	
	if(VTX.Value().SquareDistance(VTXm1.Value()) > 1e-12) { 
	  IntPatch_Point CopyVtx = VTXm1;
	  VTXm1.SetParameter(VTX.ParameterOnLine());
	  VTXm1.SetValue(VTX.Value(),VTX.Tolerance(),VTX.IsTangencyPoint());
	  Standard_Real u1,v1,u2,v2;
	  VTX.Parameters(u1,v1,u2,v2);
	  VTXm1.SetParameters(u1,v1,u2,v2);
	  if(CopyVtx.IsOnDomS1()) { 
	    VTXm1.SetArc(Standard_True,
			 CopyVtx.ArcOnS1(),
			 CopyVtx.ParameterOnArc1(),
			 CopyVtx.TransitionLineArc1(),
			 CopyVtx.TransitionOnS1());
	  }
	  if(CopyVtx.IsOnDomS2()) { 
	    VTXm1.SetArc(Standard_False,
			 CopyVtx.ArcOnS2(),
			 CopyVtx.ParameterOnArc2(),
			 CopyVtx.TransitionLineArc2(),
			 CopyVtx.TransitionOnS2());
	  }
	  if(CopyVtx.IsVertexOnS1()) { 
	    VTXm1.SetVertex(Standard_True,CopyVtx.VertexOnS1());
	    VTXm1.SetArc(Standard_True,
			 CopyVtx.ArcOnS1(),
			 CopyVtx.ParameterOnArc1(),
			 CopyVtx.TransitionLineArc1(),
			 CopyVtx.TransitionOnS1());
	  }
	  if(CopyVtx.IsVertexOnS2()) { 
	    VTXm1.SetVertex(Standard_False,CopyVtx.VertexOnS2());
	    VTXm1.SetArc(Standard_False,
			 CopyVtx.ArcOnS2(),
			 CopyVtx.ParameterOnArc2(),
			 CopyVtx.TransitionLineArc2(),
			 CopyVtx.TransitionOnS2());
	  }
	  

	  SortIsOK=Standard_False;
	  //-- cout<<" IntPatch_ALine : ComputeVertexParameters : Ajust "<<endl;
	}
      }
    }
  }
  while(!SortIsOK);
      
  /*nbvtx     = NbVertex(); 
  for(Standard_Integer opopo = 1; opopo<=nbvtx; opopo++) { 
    svtx.Value(opopo).Dump();
  }*/
}
