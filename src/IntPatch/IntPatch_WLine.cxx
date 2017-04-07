// Created on: 1991-05-27
// Created by: Isabelle GRIGNON
// Copyright (c) 1991-1999 Matra Datavision
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
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntPatch_Point.hxx>
#include <IntPatch_WLine.hxx>
#include <IntSurf_LineOn2S.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IntPatch_WLine,IntPatch_PointLine)

#define DEBUG 0
#define DEBUGV 0

#include <IntSurf_PntOn2S.hxx>
#include <Precision.hxx>
#include <stdio.h>


IntPatch_WLine::IntPatch_WLine (const Handle(IntSurf_LineOn2S)& Line,
                                const Standard_Boolean Tang,
                                const IntSurf_TypeTrans Trans1,
                                const IntSurf_TypeTrans Trans2) :
  IntPatch_PointLine(Tang,Trans1,Trans2),fipt(Standard_False),lapt(Standard_False),
  hasArcOnS1(Standard_False),hasArcOnS2(Standard_False),
  myIsPurgerAllowed(Standard_True)
{
  typ = IntPatch_Walking;
  curv = Line;
  Buv1.SetWhole();
  Buv2.SetWhole();
  Bxyz.SetWhole();
  u1period=v1period=u2period=v2period=0.0;
}


IntPatch_WLine::IntPatch_WLine (const Handle(IntSurf_LineOn2S)& Line,
                                const Standard_Boolean Tang,
                                const IntSurf_Situation Situ1,
                                const IntSurf_Situation Situ2) :
  IntPatch_PointLine(Tang,Situ1,Situ2),fipt(Standard_False),lapt(Standard_False),
  hasArcOnS1(Standard_False),hasArcOnS2(Standard_False),
  myIsPurgerAllowed(Standard_True)
{
  typ = IntPatch_Walking;
  curv = Line;
  Buv1.SetWhole();
  Buv2.SetWhole();
  Bxyz.SetWhole();
  u1period=v1period=u2period=v2period=0.0;
}


IntPatch_WLine::IntPatch_WLine (const Handle(IntSurf_LineOn2S)& Line,
                                const Standard_Boolean Tang) :
  IntPatch_PointLine(Tang),fipt(Standard_False),lapt(Standard_False),
  hasArcOnS1(Standard_False),hasArcOnS2(Standard_False),
  myIsPurgerAllowed(Standard_True)
{
  typ = IntPatch_Walking;
  curv = Line;
  Buv1.SetWhole();
  Buv2.SetWhole();
  Bxyz.SetWhole();
  u1period=v1period=u2period=v2period=0.0;
}


void IntPatch_WLine::SetPoint(const Standard_Integer Index,
                              const IntPatch_Point& thepoint)
{
  curv->Value(Index,thepoint.PntOn2S());
}


Handle(IntSurf_LineOn2S) IntPatch_WLine::Curve() const
{ 
  return(curv);
}

static void RecadreMemePeriode(Standard_Real& u1,Standard_Real& v1,
			       Standard_Real& u2,Standard_Real& v2,
			       const Standard_Real anu1,const Standard_Real anv1,
			       const Standard_Real anu2,const Standard_Real anv2,
			       const Standard_Real U1Period,const Standard_Real V1Period,
			       const Standard_Real U2Period,const Standard_Real V2Period) { 
  if(U1Period) { 
    while(anu1-u1 > 0.8*U1Period) { u1+=U1Period; }
    while(u1-anu1 > 0.8*U1Period) { u1-=U1Period; }
  }
  if(U2Period) { 
    while(anu2-u2 > 0.8*U2Period) { u2+=U2Period; }
    while(u2-anu2 > 0.8*U2Period) { u2-=U2Period; }
  }
  if(V1Period) { 
    while(anv1-v1 > 0.8*V1Period) { v1+=V1Period; }
    while(v1-anv1 > 0.8*V1Period) { v1-=V1Period; }
  }
  if(V2Period) { 
    while(anv2-v2 > 0.8*V2Period) { v2+=V2Period; }
    while(v2-anv2 > 0.8*V2Period) { v2-=V2Period; }
  }

}

static void RecadreMemePeriode(IntSurf_PntOn2S& POn2S,const IntSurf_PntOn2S& RefPOn2S,
			       const Standard_Real up1,
			       const Standard_Real vp1,
			       const Standard_Real up2,
			       const Standard_Real vp2) { 
  Standard_Real u1,v1,u2,v2,pu1,pv1,pu2,pv2;
  POn2S.Parameters(u1,v1,u2,v2);
  RefPOn2S.Parameters(pu1,pv1,pu2,pv2);
  RecadreMemePeriode(u1,v1,u2,v2,pu1,pv1,pu2,pv2,up1,vp1,up2,vp2);
  POn2S.SetValue(u1,v1,u2,v2);
}

static Standard_Boolean CompareVertexAndPoint(const gp_Pnt& V, const gp_Pnt& P, const Standard_Real& Tol) { 
  const Standard_Real aSQDist = V.SquareDistance(P);
  const Standard_Real aSQTol = Tol*Tol;
  return (aSQDist <= aSQTol);
}

void IntPatch_WLine::SetPeriod(const Standard_Real pu1,
			       const Standard_Real pv1,
			       const Standard_Real pu2,
			       const Standard_Real pv2) { 
  u1period=pu1; v1period=pv1; u2period=pu2; v2period=pv2;
}
Standard_Real IntPatch_WLine::U1Period() const {    return(u1period); }
Standard_Real IntPatch_WLine::V1Period() const {    return(v1period); }
Standard_Real IntPatch_WLine::U2Period() const {    return(u2period); }
Standard_Real IntPatch_WLine::V2Period() const {    return(v2period); }


//------------------------------------------------------------------------
//--  En Entree : Une ligne de cheminement     +    Une Liste de Vetex
//--
//--   LineOn2S   :       1------2-------3-------4-----5---- ----nbp
//-- 
//--   Vertex     :   a     b c          d    e      f
//--
//--
//--  En Sortie 
//--
//--                  1--2-3-4--5--------6----7--8--9--10--------
//--
//--  avec   a de parametre 1 
//--         b              3
//--
//--   etc ...
//--
//--
//-- !!!!!!!!!!!!!!! On considere que deux vertex ne peuvent pas etre 
//-- !!!!!!!!!!!!!!!  a une distance inferieure a Tol
//------------------------------------------------------------------------
//--
//-- On Teste si la LineOn2S contient des points confondus. 
//-- Dans ce cas, on remove ces points.
//--
//------------------------------------------------------------------------

Standard_Boolean SameVtxRst(const IntPatch_Point& vtx1,const IntPatch_Point& vtx2) { 
  if(vtx1.IsOnDomS1()) { 
    if(vtx2.IsOnDomS1()) { 
      if(vtx1.ArcOnS1() == vtx2.ArcOnS1()) { 
	if(vtx1.ParameterOnArc1() == vtx2.ParameterOnArc1()) { 
	  
	}
	else { 
	  return(Standard_False);
	}
      }
      else { 
	return(Standard_False); 
      }
    }
    else { 
      return(Standard_False);
    }
  }
  else { 
    if(vtx2.IsOnDomS1()) { 
      return(Standard_False);
    }
  }
  if(vtx1.IsOnDomS2()) { 
    if(vtx2.IsOnDomS2()) { 
      if(vtx1.ArcOnS2() == vtx2.ArcOnS2()) { 
	if(vtx1.ParameterOnArc2() == vtx2.ParameterOnArc2()) { 
	  
	}
	else { 
	  return(Standard_False);
	}
      }
      else { 
	return(Standard_False); 
      }
    }
    else { 
      return(Standard_False);
    }
  }
  else {
    if(vtx2.IsOnDomS2()) {
      return(Standard_False);
    }
  }
  return(Standard_True);
}


static Standard_Boolean CompareVerticesOnSurf(const IntPatch_Point& vtx1,
					      const IntPatch_Point& vtx2,
					      const Standard_Boolean onFirst)
{
  Standard_Real u1,v1,u2,v2, tolU, tolV;
  if (onFirst) {
    vtx1.ParametersOnS1(u1,v1);
    vtx2.ParametersOnS1(u2,v2);
  }
  else {
    vtx1.ParametersOnS2(u1,v1);
    vtx2.ParametersOnS2(u2,v2);
  }
  tolU = Precision::PConfusion();
  tolV = Precision::PConfusion();
  return (Abs(u1-u2) <= tolU && Abs(v1-v2) <= tolV);
}

inline Standard_Boolean CompareVerticesOnS1(const IntPatch_Point& vtx1, const IntPatch_Point& vtx2)
{return CompareVerticesOnSurf (vtx1, vtx2, Standard_True);}

inline Standard_Boolean CompareVerticesOnS2(const IntPatch_Point& vtx1, const IntPatch_Point& vtx2)
{return CompareVerticesOnSurf (vtx1, vtx2, Standard_False);}


void IntPatch_WLine::ComputeVertexParameters( const Standard_Real RTol)
{
  // MSV Oct 15, 2001: use tolerance of vertex instead of RTol where 
  //                   it is possible

  Standard_Integer i,j,k,nbvtx,nbponline;
  Standard_Integer indicevertexonline;
  Standard_Real    indicevertex;

  Standard_Boolean APointDeleted = Standard_False;
  //----------------------------------------------------------
  //--     F i l t r e   s u r   r e s t r i c t i o n s   --
  //----------------------------------------------------------
  //-- deux vertex sur la meme restriction et seulement 
  //-- sur celle ci ne doivent pas avoir le meme parametre
  //--
  Standard_Real Tol=RTol;
  nbvtx = NbVertex();

#if DEBUGV  
  cout<<"\n----------- avant ComputeVertexParameters -------------"<<endl;
  for(i=1;i<=nbvtx;i++) { 
    Vertex(i).Dump();
    Standard_Real  polr = Vertex(i).ParameterOnLine();
    Standard_Real pol = (Standard_Integer)polr;
    if(pol>=1 && pol<=nbvtx) { 
      cout<<"----> IntSurf_PntOn2S : "<<polr<<"  Pnt ("<<Vertex(pol).Value().X()
	<<","<<Vertex(pol).Value().Y()
	  <<","<<Vertex(pol).Value().Z()<<")"<<endl;
    }
  }
  cout<<"\n----------------------------------------------------------"<<endl;
#endif  

  
  //-- ----------------------------------------------------------------------
  //-- Traitement des aretes de couture : On duplique les points situes
  //-- sur des restrictions differentes
  //-- 
  //-- Phase Creation de nouveaux points sur S1 
  Standard_Boolean encoreunefois;
  do { 
    nbvtx=NbVertex();
    encoreunefois=Standard_False;
    for(i=1; i<=nbvtx && encoreunefois==Standard_False; i++) { 
      IntPatch_Point& VTXi   = svtx.ChangeValue(i);
      for(j=1; j<=nbvtx && encoreunefois==Standard_False; j++) { 
	if(i!=j) { 
	  IntPatch_Point& VTXj   = svtx.ChangeValue(j);
	  if(VTXi.ParameterOnLine() != VTXj.ParameterOnLine()) { 
	    Standard_Real d = VTXi.Value().Distance(VTXj.Value());
	    Standard_Real toli = VTXi.Tolerance();
	    Standard_Real tolj = VTXj.Tolerance();
	    Standard_Real maxtol = Max(toli,tolj);
	    // MSV Oct 30, 2001: compare in 2D space also;
	    //                   increase tolerances
	    if (d < maxtol ||
	       CompareVerticesOnS1(VTXi,VTXj) || CompareVerticesOnS2(VTXi,VTXj)) { 
	      //-- Creation Vtx (REF:S1(i)  S2(j))    (On Garde S1(i))
	      Standard_Real newtoli = Max (toli, tolj+d*1.01);
	      Standard_Real newtolj = Max (tolj, toli+d*1.01);
	      Standard_Boolean acreer=Standard_False;
	      if(VTXi.IsOnDomS1()) { 
		if(VTXj.IsOnDomS1()) { 
		  if(VTXj.ArcOnS1() != VTXi.ArcOnS1()) { 
		    acreer=Standard_True;
		  }
		}
		else { 
		  acreer=Standard_True;
		}
	      }
	      if(acreer) { 
		IntPatch_Point vtx;
		vtx = VTXj;
		vtx.SetArc(Standard_True,
			   VTXi.ArcOnS1(),
			   VTXi.ParameterOnArc1(),
			   VTXi.TransitionLineArc1(),
			   VTXi.TransitionOnS1());
		for(k=1; encoreunefois==Standard_False && k<=nbvtx; k++) { 
		  const IntPatch_Point& VTXk   = svtx.Value(k);
		  if(SameVtxRst(VTXk,vtx)) { 
		    encoreunefois=Standard_True;
		  }
		}
		if(encoreunefois==Standard_False) {
		  VTXi.SetTolerance(newtoli);
		  VTXj.SetTolerance(newtolj);
		  vtx.SetTolerance(newtolj);
		  svtx.Append(vtx);
		  encoreunefois=Standard_True;
		}
		else { 
		  encoreunefois=Standard_False;
		}
	      }
	      //-- -----------------------------------------------------
	      //-- Creation Vtx (REF:S2(i)  S1(j))    (On Garde S2(i))
	      acreer=Standard_False;
	      if(VTXi.IsOnDomS2()) { 
		if(VTXj.IsOnDomS2()) { 
		  if(VTXj.ArcOnS2() != VTXi.ArcOnS2()) { 
		    acreer=Standard_True;
		  }
		}
		else { 
		  acreer=Standard_True;
		}
	      }
	      if(acreer) { 
		IntPatch_Point vtx;
		vtx = VTXj;
		vtx.SetArc(Standard_False,
			   VTXi.ArcOnS2(),
			   VTXi.ParameterOnArc2(),
			   VTXi.TransitionLineArc2(),
			   VTXi.TransitionOnS2());
		for(k=1; encoreunefois==Standard_False && k<=nbvtx; k++) { 
		  const IntPatch_Point& VTXk   = svtx.Value(k);
		  if(SameVtxRst(VTXk,vtx)) { 
		    encoreunefois=Standard_True;
		  }
		}
		if(encoreunefois==Standard_False) {
		  VTXi.SetTolerance(newtoli);
		  VTXj.SetTolerance(newtolj);
		  vtx.SetTolerance(newtolj);
		  svtx.Append(vtx);
		  encoreunefois=Standard_True;
		}
		else { 
		  encoreunefois=Standard_False;
		}
	      }	    
	    }
	  }
	}
      }
    }
  }
  while(encoreunefois);
  
  

  //-- ----------------------------------------------------------------------



  do { 
    APointDeleted = Standard_False;
    for(i=1; (i<=nbvtx) && (APointDeleted==Standard_False) ;i++) { 
      const IntPatch_Point& VTXi   = svtx.Value(i);
      if(VTXi.Tolerance() > Tol) 	Tol = VTXi.Tolerance(); //-- 9 oct 97 
      if((VTXi.IsOnDomS1()==Standard_True) && (VTXi.IsOnDomS2()==Standard_False)) { 
	for(j=1; (j<=nbvtx) && (APointDeleted==Standard_False) ;j++) {
	  if(i!=j) { 
	    const IntPatch_Point& VTXj   = svtx.Value(j);
	    if((VTXj.IsOnDomS1()==Standard_True) && (VTXj.IsOnDomS2()==Standard_False)) {
	      if(VTXi.ParameterOnLine() == VTXj.ParameterOnLine()) { 
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
	      if(VTXi.ParameterOnLine() == VTXj.ParameterOnLine()) { 
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
  while(APointDeleted == Standard_True);
	
  nbvtx     = NbVertex();
  nbponline = NbPnts();

  //----------------------------------------------------
  //-- On trie les Vertex 
  Standard_Boolean SortIsOK;
  do { 
    SortIsOK = Standard_True;
    for(i=2; i<=nbvtx; i++) { 
      if(svtx.Value(i-1).ParameterOnLine()  > svtx.Value(i).ParameterOnLine()) { 
	SortIsOK = Standard_False;
	svtx.Exchange(i-1,i);
      }
    }
  }
  while(!SortIsOK);
  
  //----------------------------------------------------
  //-- On detecte les points confondus dans la LineOn2S
  Standard_Real dmini = Precision::Confusion();
  dmini*=dmini;
  for(i=2; (i<=nbponline) && (nbponline > 2); i++) { 
    const IntSurf_PntOn2S& aPnt1=curv->Value(i-1);
    const IntSurf_PntOn2S& aPnt2=curv->Value(i);
    Standard_Real d = (aPnt1.Value()).SquareDistance((aPnt2.Value()));
    if(d < dmini) { 
      curv->RemovePoint(i);
      nbponline--;
      //----------------------------------------------
      //-- On recadre les Vertex si besoin 
      //-- 
      for(j=1; j<=nbvtx; j++) { 
        indicevertex = svtx.Value(j).ParameterOnLine();
        if(indicevertex >= i) {
          svtx.ChangeValue(j).SetParameter(indicevertex-1.0);
        }
      }
      //modified by NIZNHY-PKV Mon Feb 11 09:28:02 2002 f
      i--;
      //modified by NIZNHY-PKV Mon Feb 11 09:28:04 2002 t
    }
  }
  //----------------------------------------------------
  for(i=1; i<=nbvtx; i++) {
    const gp_Pnt& P    = svtx.Value(i).Value();
    Standard_Real vTol = svtx.Value(i).Tolerance();

    indicevertex = svtx.Value(i).ParameterOnLine();
    indicevertexonline = (Standard_Integer)indicevertex;
    //--------------------------------------------------
    //-- On Compare le vertex avec les points de la ligne
    //-- d indice   indicevertexOnLine-1
    //--            indicevertexOnLine
    //--            indicevertexOnLine+1
    //--------------------------------------------------
    if(indicevertexonline<1) { 
      if(CompareVertexAndPoint(P,curv->Value(1).Value(),vTol)) {
	//-------------------------------------------------------
	//-- On remplace le point cheminement(1) par vertex(i)
	//-- et   vertex(i) prend pour parametre 1
	//-------------------------------------------------------
	
	IntSurf_PntOn2S POn2S = svtx.Value(i).PntOn2S();
	RecadreMemePeriode(POn2S,curv->Value(1),U1Period(),V1Period(),U2Period(),V2Period());
	curv->Value(1,POn2S);

	//--curv->Value(1,svtx.Value(i).PntOn2S());
	svtx.ChangeValue(i).SetParameter(1.0);
      }
      else { 
	//-------------------------------------------------------
	//-- On insere le point de cheminement Vertex(i)
	//-- On recadre les parametres des autres vertex
	//-------------------------------------------------------
	IntSurf_PntOn2S POn2S = svtx.Value(i).PntOn2S();
	RecadreMemePeriode(POn2S,curv->Value(1),U1Period(),V1Period(),U2Period(),V2Period());
	curv->InsertBefore(1,POn2S);

	//-- curv->InsertBefore(1,svtx.Value(i).PntOn2S());
	svtx.ChangeValue(i).SetParameter(1.0);
	nbponline++;
	for(j=1;j<=nbvtx;j++) {
	  if(j!=1) { 
	    Standard_Real t = svtx.Value(j).ParameterOnLine();
	    if(t>1.0) { 
	      svtx.ChangeValue(j).SetParameter(t+1.0);
	    }
	  }
	}
      }
    } //--- fin : if(indicevertexonline<1) 
    else { 
      //---------------------------------------------------------
      //-- vertex(i)   ==   cheminement (indicevertexonline-1)
      //-- vertex(i)   ==   cheminement (indicevertexonline)
      //-- vertex(i)   ==   cheminement (indicevertexonline+1)
      //---------------------------------------------------------
      Standard_Boolean Substitution = Standard_False;
      //-- for(k=indicevertexonline+1; !Substitution && k>=indicevertexonline-1;k--) {   avant le 9 oct 97 
      for(k=indicevertexonline+1; k>=indicevertexonline-1;k--) { 
	if(k>0 && k<=nbponline) { 
	  if(CompareVertexAndPoint(P,curv->Value(k).Value(),vTol)) {
	    //-------------------------------------------------------
	    //-- On remplace le point cheminement(k) 
	    //-- par vertex(i)  et vertex(i) prend pour parametre k
	    //-------------------------------------------------------
	    IntSurf_PntOn2S POn2S = svtx.Value(i).PntOn2S();
	    RecadreMemePeriode(POn2S,curv->Value(k),U1Period(),V1Period(),U2Period(),V2Period());
	    curv->Value(k,POn2S);
	    Standard_Real mu1,mv1,mu2,mv2;
	    POn2S.Parameters(mu1,mv1,mu2,mv2);
	    svtx.ChangeValue(i).SetParameter(k);
	    svtx.ChangeValue(i).SetParameters(mu1,mv1,mu2,mv2);
	    Substitution = Standard_True;
	  }
	}
      }

      //Remove duplicating points
      if (Substitution)
      {
        Standard_Integer ind_point;
        for(ind_point = 2; (ind_point <= nbponline && nbponline > 1); ind_point++) { 
          Standard_Real d = (curv->Value(ind_point-1).Value()).SquareDistance((curv->Value(ind_point).Value()));
          if(d < dmini) { 
            curv->RemovePoint(ind_point);
            nbponline--;
            //----------------------------------------------
            //-- On recadre les Vertex si besoin 
            //-- 
            for(j=1; j<=nbvtx; j++) { 
              indicevertex = svtx.Value(j).ParameterOnLine();
              if(indicevertex >= ind_point) {
                svtx.ChangeValue(j).SetParameter(indicevertex-1.0);
              }
            }
            //modified by NIZNHY-PKV Mon Feb 11 09:28:02 2002 f
            ind_point--;
            //modified by NIZNHY-PKV Mon Feb 11 09:28:04 2002 t
          }
        }
      }
      
      //--static int deb6nov98=1;    Ne resout rien (a part partiellement BUC60409)
      //--if(deb6nov98) { 
      //--Substitution=Standard_True;
      //-- }
      
      if(Substitution==Standard_False) { 
	//-------------------------------------------------------
	//-- On insere le point de cheminement Vertex(i)
	//-- On recadre les parametres des autres vertex
	//-------------------------------------------------------
	IntSurf_PntOn2S POn2S = svtx.Value(i).PntOn2S();
	if(indicevertexonline >= nbponline) { 
	  RecadreMemePeriode(POn2S,curv->Value(nbponline),U1Period(),V1Period(),U2Period(),V2Period());
	  curv->Add(POn2S);
	}
	else { 
	  RecadreMemePeriode(POn2S,curv->Value(indicevertexonline+1),U1Period(),V1Period(),U2Period(),V2Period());
	  curv->InsertBefore(indicevertexonline+1,POn2S);
	}
	//-- curv->InsertBefore(indicevertexonline+1,svtx.Value(i).PntOn2S());
	svtx.ChangeValue(i).SetParameter(indicevertexonline+1);
	nbponline++;
	for(j=1;j<=nbvtx;j++) { 
	  if(j!=i) { 
	    Standard_Real t = svtx.Value(j).ParameterOnLine();
	    if(t>(Standard_Real)indicevertexonline) { 
	      svtx.ChangeValue(j).SetParameter(t+1.0);
	    }
	  }
	}
      } //-- Substitution
    } //-- indicevertexonline>=1

  } //-- boucle i sur vertex

  
  
  
  do { 	
    APointDeleted = Standard_False;
    for(i=1; i<=nbvtx && (APointDeleted == Standard_False); i++) { 
      const IntPatch_Point& VTX   = svtx.Value(i);      
      for(j=1; j<=nbvtx && (APointDeleted == Standard_False) ; j++) { 
	if(i!=j) { 
	  const IntPatch_Point& VTXM1 = svtx.Value(j);
	  
	  Standard_Boolean kill   = Standard_False;
	  Standard_Boolean killm1 = Standard_False;
	  if(VTXM1.ParameterOnLine() == VTX.ParameterOnLine()) { 
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
	    }
	    if(kill) { 
	      APointDeleted = Standard_True;
	      svtx.Remove(i);
	      nbvtx--;
	    }
	    else if(killm1) { 
	      APointDeleted = Standard_True;
	      svtx.Remove(j);
	      nbvtx--; 
	    }
	  }
	}
      }
    }
  }
  while(APointDeleted == Standard_True);
  
  do { 	
    SortIsOK = Standard_True;
    for(i=2; i<=nbvtx && SortIsOK; i++) {
      const IntPatch_Point& Pim1=svtx.Value(i-1);
      const IntPatch_Point& Pii  =svtx.Value(i);
      if(Pim1.ParameterOnLine()==Pii.ParameterOnLine()) { 
	if(   (Pii.IsOnDomS1() == Standard_False)
	   && (Pii.IsOnDomS2() == Standard_False)) { 
	  SortIsOK = Standard_False;
	  svtx.Remove(i);
	  nbvtx--;
	}
	else {
	  if(   (Pim1.IsOnDomS1() == Standard_False)
	     && (Pim1.IsOnDomS2() == Standard_False)) { 
	    SortIsOK = Standard_False;
	    svtx.Remove(i-1);
	    nbvtx--;
	  }	  
	}
      }
    }
  }
  while(!SortIsOK);
  //-- ----------------------------------------------------------------------------
  //-- On ajoute les vertex de debut et de fin de ligne s il ne sont pas presents.
  //-- 
  //-- Existe t il un vertex de debut de ligne, de fin .
  //--
  //-- Si Besoin : il faudra dedoubler les points de debut et de fin sur les periodiques ??????



  
  Standard_Boolean bFirst = Standard_False;
  Standard_Boolean bLast  = Standard_False;
  nbponline = NbPnts();
  for(i=1;i<=nbvtx;i++) { 
    Standard_Real pol = svtx.Value(i).ParameterOnLine();
    if(pol==1.0)       {
      bFirst = fipt = Standard_True;
      indf   = i;
    }
    if(pol==nbponline) {
      bLast = lapt = Standard_True;
      indl  = i;
    }
  }
  if(bFirst == Standard_False) { 
    Standard_Real pu1,pv1,pu2,pv2;
    Standard_Boolean vtxfound = Standard_False;
    IntPatch_Point vtx;
    curv->Value(1).Parameters(pu1,pv1,pu2,pv2);
    for(i=1;
	(vtxfound==Standard_False) && (i<=nbvtx);i++) { 
      const IntPatch_Point&  V = svtx.Value(i);
      //jgv: to avoid loops
      //Standard_Real vTol = V.Tolerance();
      if(CompareVertexAndPoint(V.Value(), curv->Value(1).Value(), Precision::Confusion()/*vTol*/)) { 
	vtx = V;
	vtx.SetParameters(pu1,pv1,pu2,pv2);
	vtxfound = Standard_True;
      }
    }
    if(vtxfound == Standard_False)  { 
      vtx.SetValue(curv->Value(1).Value(),Tol,Standard_False);
      vtx.SetParameters(pu1,pv1,pu2,pv2);
    }
    vtx.SetParameter(1);
    svtx.Prepend(vtx);    nbvtx++;
    fipt = Standard_True;
    indf = 1;
  }
  if(bLast == Standard_False) { 
    Standard_Real pu1,pv1,pu2,pv2;
    Standard_Boolean vtxfound = Standard_False;    
    IntPatch_Point vtx;
    curv->Value(nbponline).Parameters(pu1,pv1,pu2,pv2);   
    for(i=1;
	(vtxfound==Standard_False) && (i<=nbvtx);i++) { 
      const IntPatch_Point&  V = svtx.Value(i);
      //jgv: to avoid loops
      //Standard_Real vTol = V.Tolerance();
      if(CompareVertexAndPoint(V.Value(), curv->Value(nbponline).Value(), Precision::Confusion()/*vTol*/)) { 
	vtx = V;
	vtx.SetParameters(pu1,pv1,pu2,pv2);
	vtxfound = Standard_True;
      }
    }
    if(vtxfound == Standard_False)  {     
      vtx.SetValue(curv->Value(nbponline).Value(),Tol,Standard_False);
      vtx.SetParameters(pu1,pv1,pu2,pv2);
    }
    vtx.SetParameter(nbponline);
    svtx.Append(vtx); nbvtx++;
    lapt = Standard_True;
    indl = nbvtx; 
  }
  



  //--------------------------------------------------------------
  //-- ** Detection de points trouves sur une meme restriction 
  //--    avec la meme transition et avec des params on line 
  //--    voisins.   
  //-- ** Dans ce cas (-> donnerait un baillemenmt) on supprime 
  //--    le point 'intermediaire'. 
  //-- ** (exemple Vtx(1)  .....  Vtx(204) Vtx(205))
  //--    on supprime le Vtx(204)
  //-- ** (exemple Vtx(1) Vtx(2)  .....  Vtx(205))
  //--    on supprime le Vtx(2)
  //-- ** (exemple Vtx(1)  ...  Vtx(100) Vtx(101) ... Vtx(205))
  //--    on supprime le Vtx(100)  (Vtx(100)et101 sur m restr)

  //--------------------------------------------------------------
  nbvtx = NbVertex();
  do { 
    APointDeleted = Standard_False;
    for(i=1; (i<=nbvtx) && (APointDeleted==Standard_False) ;i++) { 
      const IntPatch_Point& VTXi   = svtx.Value(i);
      if((VTXi.IsOnDomS1()==Standard_True) && (VTXi.IsOnDomS2()==Standard_False)) { 
	for(j=1; (j<=nbvtx) && (APointDeleted==Standard_False) ;j++) {
	  if(i!=j) { 
	    const IntPatch_Point& VTXj   = svtx.Value(j);
	    if((VTXj.IsOnDomS1()==Standard_True) && (VTXj.IsOnDomS2()==Standard_False)) {
	      if(   (VTXi.ParameterOnLine() == VTXj.ParameterOnLine()+1)
		 || (VTXi.ParameterOnLine() == VTXj.ParameterOnLine()-1)) { 
		if(VTXi.ArcOnS1() == VTXj.ArcOnS1()) { 
		  IntSurf_Transition t1 = VTXi.TransitionLineArc1();
		  IntSurf_Transition t2 = VTXj.TransitionLineArc1();
		  if(t1.TransitionType()==t2.TransitionType()) { 
		    if((VTXj.ParameterOnLine()!=1) && (VTXj.ParameterOnLine()!=NbPnts())) {   
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
  }
  
  //-- meme traitement sur les restrictions du second shape

  while(APointDeleted == Standard_True);
  nbvtx = NbVertex();
  do { 
    APointDeleted = Standard_False;
    for(i=1; (i<=nbvtx) && (APointDeleted==Standard_False) ;i++) { 
      const IntPatch_Point& VTXi   = svtx.Value(i);
      if((VTXi.IsOnDomS1()==Standard_False) && (VTXi.IsOnDomS2()==Standard_True)) { 
	for(j=1; (j<=nbvtx) && (APointDeleted==Standard_False) ;j++) {
	  if(i!=j) { 
	    const IntPatch_Point& VTXj   = svtx.Value(j);
	    if((VTXj.IsOnDomS1()==Standard_False) && (VTXj.IsOnDomS2()==Standard_True)) {
	      if(   (VTXi.ParameterOnLine() == VTXj.ParameterOnLine()+1)
		 || (VTXi.ParameterOnLine() == VTXj.ParameterOnLine()-1)) { 
		if(VTXi.ArcOnS2() == VTXj.ArcOnS2()) { 
		  IntSurf_Transition t1 = VTXi.TransitionLineArc2();
		  IntSurf_Transition t2 = VTXj.TransitionLineArc2();
		  if(t1.TransitionType()==t2.TransitionType()) { 
		    if((VTXj.ParameterOnLine()!=1) && (VTXj.ParameterOnLine()!=NbPnts())) {   
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
  }
  while(APointDeleted == Standard_True);
  //--------------------------------------------------------------

  //--------------------------------------------------------------
  //-- dans le cas de lignes periodiques du type : 
  //--    Un point sur restriction R1 de param p1  -> P3d1   Vtx1
  //--    Un point sur restriction R2 de param p2  -> P3d1   Vtx2
  //--    
  //--    Un point sur restriction R1 de param p1  -> P3d1   Vtx3
  //--    pas de point sur R2 
  //--
  //-- On doit dans ce cas creer un nouveau Vtx4 = Vtx3 sur la 
  //-- restriction R2
  //--
  //-- Ce cas se produit qd on a corrige un baillement avec le filtre
  //-- precedent
  //-- 
  



  

  nbvtx = NbVertex();
  do { 
    SortIsOK = Standard_True;
    for(i=2; i<=nbvtx; i++) { 
      if(svtx.Value(i-1).ParameterOnLine()  > svtx.Value(i).ParameterOnLine()) { 
	SortIsOK = Standard_False;
	svtx.Exchange(i-1,i);
      }
    }
  }
  while(!SortIsOK);
  
  //-- Dump();

#if DEBUGV  
  cout<<"\n----------- apres ComputeVertexParameters -------------"<<endl;
  for(i=1;i<=nbvtx;i++) { 
    Vertex(i).Dump();
    Standard_Real  polr = Vertex(i).ParameterOnLine();
    Standard_Real pol = (Standard_Integer)polr;
    if(pol>=1 && pol<=nbvtx) { 
      cout<<"----> IntSurf_PntOn2S : "<<polr<<"  Pnt ("<<Vertex(pol).Value().X()
	<<","<<Vertex(pol).Value().Y()
	  <<","<<Vertex(pol).Value().Z()<<")"<<endl;
    }
  }
  cout<<"\n----------------------------------------------------------"<<endl;
#endif  


}



Standard_Boolean IntPatch_WLine::IsOutSurf1Box(const gp_Pnt2d& P1uv)  { 
  if(Buv1.IsWhole()) {
    Standard_Integer n=NbPnts();
    Standard_Real pu1,pu2,pv1,pv2;
    Buv1.SetVoid();
    for(Standard_Integer i=1;i<=n;i++) { 
      curv->Value(i).Parameters(pu1,pv1,pu2,pv2);
      Buv1.Add(gp_Pnt2d(pu1,pv1));
    }
    Buv1.Get(pu1,pv1,pu2,pv2);
    pu2-=pu1;
    pv2-=pv1;
    if(pu2>pv2) { 
      Buv1.Enlarge(pu2*0.01);
    }
    else { 
      Buv1.Enlarge(pv2*0.01);
    }
  }
  Standard_Boolean out=Buv1.IsOut(P1uv);
  return(out);
}

Standard_Boolean IntPatch_WLine::IsOutSurf2Box(const gp_Pnt2d& P2uv)  { 
  if(Buv2.IsWhole()) { 
    Standard_Integer n=NbPnts();
    Standard_Real pu1,pu2,pv1,pv2;
    Buv2.SetVoid();
    for(Standard_Integer i=1;i<=n;i++) { 
      curv->Value(i).Parameters(pu1,pv1,pu2,pv2);
      Buv2.Add(gp_Pnt2d(pu2,pv2));
    }
    Buv2.Get(pu1,pv1,pu2,pv2);
    pu2-=pu1;
    pv2-=pv1;
    if(pu2>pv2) { 
      Buv2.Enlarge(pu2*0.01);
    }
    else { 
      Buv2.Enlarge(pv2*0.01);
    }    
  }
  Standard_Boolean out=Buv2.IsOut(P2uv);
  return(out);
}

Standard_Boolean IntPatch_WLine::IsOutBox(const gp_Pnt& Pxyz)  { 
  if(Bxyz.IsWhole()) { 
    Standard_Integer n=NbPnts();
    Bxyz.SetVoid();
    for(Standard_Integer i=1;i<=n;i++) { 
      gp_Pnt P=curv->Value(i).Value();
      Bxyz.Add(P);
    }
    Standard_Real x0,y0,z0,x1,y1,z1;
    Bxyz.Get(x0,y0,z0,x1,y1,z1);
    x1-=x0; y1-=y0; z1-=z0;
    if(x1>y1) {
      if(x1>z1) {
	Bxyz.Enlarge(x1*0.01);
      }
      else { 
	Bxyz.Enlarge(z1*0.01);
      }
    }
    else { 
      if(y1>z1) { 
	Bxyz.Enlarge(y1*0.01);
      }
      else { 
	Bxyz.Enlarge(z1*0.01);
      }
    }
  }
  Standard_Boolean out=Bxyz.IsOut(Pxyz);
  return(out);
}


Standard_Boolean IntPatch_WLine::HasArcOnS1() const  {
  return(hasArcOnS1);
}

void IntPatch_WLine::SetArcOnS1(const Handle(Adaptor2d_HCurve2d)& A) { 
  hasArcOnS1=Standard_True;
  theArcOnS1=A;
}

const Handle(Adaptor2d_HCurve2d)& IntPatch_WLine::GetArcOnS1() const  { 
  return(theArcOnS1);
}

Standard_Boolean IntPatch_WLine::HasArcOnS2() const  {
  return(hasArcOnS2);
}

void IntPatch_WLine::SetArcOnS2(const Handle(Adaptor2d_HCurve2d)& A) { 
  hasArcOnS2=Standard_True;
  theArcOnS2=A;
}

const Handle(Adaptor2d_HCurve2d)& IntPatch_WLine::GetArcOnS2() const  { 
  return(theArcOnS2);
}


void IntPatch_WLine::Dump(const Standard_Integer theMode) const
{ 
  cout<<" ----------- D u m p    I n t P a t c h  _  W L i n e  -(begin)------"<<endl;
  const Standard_Integer aNbPoints = NbPnts();
  const Standard_Integer aNbVertex = NbVertex();

  switch(theMode)
  {
  case 0:
    printf("Num    [X  Y  Z]     [U1  V1]   [U2  V2]\n");
    for(Standard_Integer i=1; i<=aNbPoints; i++)
    {
      Standard_Real u1,v1,u2,v2;
      Point(i).Parameters(u1,v1,u2,v2);
      printf("%4d  [%+10.20f %+10.20f %+10.20f]  [%+10.20f %+10.20f]  [%+10.20f %+10.20f]\n",
              i,Point(i).Value().X(),Point(i).Value().Y(),Point(i).Value().Z(),
              u1,v1,u2,v2);
    }
    
    for(Standard_Integer i=1;i<=aNbVertex;i++)
    {
      Vertex(i).Dump();
      Standard_Real  polr = Vertex(i).ParameterOnLine();
      Standard_Integer pol = static_cast<Standard_Integer>(polr);

      if(pol>=1 && pol<=aNbVertex)
      {
        cout<<"----> IntSurf_PntOn2S : "<<
                      polr <<", Pnt (" << Vertex(pol).Value().X() << "," <<
                                          Vertex(pol).Value().Y() << "," <<
                                          Vertex(pol).Value().Z() <<")" <<endl;
      }
    }

    break;
  case 1:
    for(Standard_Integer i = 1; i <= aNbPoints; i++)
    {
      Standard_Real u1,v1,u2,v2;
      Point(i).Parameters(u1,v1,u2,v2);
      printf("point p%d %+10.20f %+10.20f %+10.20f\n",
              i,Point(i).Value().X(),Point(i).Value().Y(),Point(i).Value().Z());
    }

    break;
  case 2:
    for(Standard_Integer i = 1; i <= aNbPoints; i++)
    {
      Standard_Real u1,v1,u2,v2;
      Point(i).Parameters(u1,v1,u2,v2);
      printf("point p%d %+10.20f %+10.20f\n", i, u1, v1);
    }

    break;
  default:
    for(Standard_Integer i = 1; i <= aNbPoints; i++)
    {
      Standard_Real u1,v1,u2,v2;
      Point(i).Parameters(u1,v1,u2,v2);
      printf("point p%d %+10.20f %+10.20f\n", i, u2, v2);
    }

    break;
  }
  cout<<"\n--------------------------------------------------- (end) -------"<<endl;  
}

