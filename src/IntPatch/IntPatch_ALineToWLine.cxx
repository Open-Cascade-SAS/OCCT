// Created on: 1993-11-26
// Created by: Modelistation
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
#include <GeomAbs_SurfaceType.hxx>
#include <gp_Cone.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <IntPatch_ALine.hxx>
#include <IntPatch_ALineToWLine.hxx>
#include <IntPatch_WLine.hxx>
#include <IntSurf_LineOn2S.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <IntSurf_Quadric.hxx>
#include <IntSurf_Situation.hxx>
#include <IntSurf_TypeTrans.hxx>
#include <Precision.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>

static 
  gp_Pnt DefineDU(const Handle(IntPatch_ALine)& aline, 
		  const Standard_Real U, 
		  Standard_Real& DU,
		  const Standard_Real CurvDef, 
		  const Standard_Real AngDef);

static 
  Standard_Boolean SameCurve(const Handle(Adaptor2d_HCurve2d)& C1,
			     const Handle(Adaptor2d_HCurve2d)& C2);

static 
  void RecadreMemePeriode(Standard_Real& u1,
			  Standard_Real& u2,
			  const Standard_Real anu1,
			  const Standard_Real anu2);

static
  void CorrectFirstPartOfLine(Handle(IntSurf_LineOn2S)& LinOn2S,
			      const Standard_Real ref_u1,
			      const Standard_Real ref_u2,
			      Standard_Real& new_u1,
			      Standard_Real& new_u2);

//
static
  void RefineParameters(const Handle(IntPatch_ALine)& aALine,
		      const Standard_Real aTb,
		      const Standard_Real aTe,
		      const Standard_Real aTx,
		      const Standard_Integer iDir,
		      const IntSurf_Quadric& aQuadric,
		      const Standard_Real aTol3D,
		      Standard_Real& aUx,
		      Standard_Real& aVx);
static
  Standard_Boolean FindNearParameter(const Handle(IntPatch_ALine)& aALine,
				     const Standard_Real aTx,
				     const Standard_Integer iDir,
				     const Standard_Real aTol3D,
				     Standard_Real& aT1);
static
  Standard_Boolean IsApex(const IntSurf_Quadric& aQuadric,
			  const Standard_Real aVx,
			  const Standard_Real aTol3D);
//

//--  Je ne sais pas faire mieux et ca m'ennerve.  lbr. 
//=======================================================================
//function : IntPatch_ALineToWLine
//purpose  : 
//=======================================================================
  IntPatch_ALineToWLine::IntPatch_ALineToWLine(const IntSurf_Quadric& Quad1,
					       const IntSurf_Quadric& Quad2) 
: 
  quad1(Quad1),
  quad2(Quad2),
  deflectionmax(0.01),
  nbpointsmax(200),
  type(0),
  myTolParam(1.e-12),
  myTolOpenDomain(1.e-9),
  myTolTransition(1.e-8)
{ 
  myTol3D=Precision::Confusion();
}
//=======================================================================
//function : IntPatch_ALineToWLine
//purpose  : 
//=======================================================================
  IntPatch_ALineToWLine::IntPatch_ALineToWLine(const IntSurf_Quadric& Quad1,
					       const IntSurf_Quadric& Quad2,
					       const Standard_Real    Deflection,
					       const Standard_Real    ,
					       const Standard_Integer NbMaxPoints) 
:
  quad1(Quad1),
  quad2(Quad2),
  deflectionmax(Deflection),
  nbpointsmax(NbMaxPoints),
  myTolParam(1.e-12),
  myTolOpenDomain(1.e-9),
  myTolTransition(1.e-8)
{ 
  myTol3D=Precision::Confusion();
  type = 0;
}
//=======================================================================
//function : SetTol3D
//purpose  : 
//=======================================================================
  void IntPatch_ALineToWLine::SetTol3D(const Standard_Real aTol)
{
  myTol3D = aTol;
}
//=======================================================================
//function : Tol3D
//purpose  : 
//=======================================================================
  Standard_Real IntPatch_ALineToWLine::Tol3D()const
{
  return myTol3D;
}
//=======================================================================
//function : SetTolTransition
//purpose  : 
//=======================================================================
  void IntPatch_ALineToWLine::SetTolTransition(const Standard_Real aTol)
{
  myTolTransition = aTol;
}
//=======================================================================
//function : TolTransition
//purpose  : 
//=======================================================================
  Standard_Real IntPatch_ALineToWLine::TolTransition()const
{
  return myTolTransition;
}
//=======================================================================
//function : SetTolParam
//purpose  : 
//=======================================================================
  void IntPatch_ALineToWLine::SetTolParam(const Standard_Real aTolParam)
{
  myTolParam = aTolParam;
}
//=======================================================================
//function : TolParam
//purpose  : 
//=======================================================================
  Standard_Real IntPatch_ALineToWLine::TolParam()const
{
  return myTolParam;
}
//=======================================================================
//function : SetTolOpenDomain
//purpose  : 
//=======================================================================
  void IntPatch_ALineToWLine::SetTolOpenDomain(const Standard_Real aTol)
{
  myTolOpenDomain = aTol;
}
//=======================================================================
//function : TolOpenDomain
//purpose  : 
//=======================================================================
  Standard_Real IntPatch_ALineToWLine::TolOpenDomain()const
{
  return myTolOpenDomain;
}
//=======================================================================
//function : SetConstantParameter
//purpose  : 
//=======================================================================
  void IntPatch_ALineToWLine::SetConstantParameter() const 
{ 
}
//=======================================================================
//function : SetUniformAbscissa
//purpose  : 
//=======================================================================
  void IntPatch_ALineToWLine::SetUniformAbscissa() const 
{ 
}
//=======================================================================
//function : SetUniformDeflection
//purpose  : 
//=======================================================================
  void IntPatch_ALineToWLine::SetUniformDeflection() const 
{ 
}
//=======================================================================
//function : MakeWLine
//purpose  : 
//=======================================================================
  Handle(IntPatch_WLine) IntPatch_ALineToWLine::MakeWLine(const Handle(IntPatch_ALine)& aline) const 
{ 
  Standard_Boolean included;
  Standard_Real f = aline->FirstParameter(included); 
  if(!included) {
    f+=myTolOpenDomain;
  }
  Standard_Real l = aline->LastParameter(included); 
  if(!included) { 
    l-=myTolOpenDomain;
  }
  return(MakeWLine(aline,f,l));
}

//=======================================================================
//function : MakeWLine
//purpose  : 
//=======================================================================
  Handle(IntPatch_WLine) IntPatch_ALineToWLine::MakeWLine(const Handle(IntPatch_ALine)& aline,
						    const Standard_Real _firstparam,
						    const Standard_Real _lastparam) const 
{
  //
  Standard_Real dl, dumin, dumax, U, pv, pu1, pv1, pu2, pv2; 
  Standard_Real firstparam, lastparam;
  Standard_Integer v, nbvtx;
  Standard_Boolean TriOk;
  //
  firstparam = _firstparam;
  lastparam  = _lastparam;
  // Pas Bon ... 
  // nbpointsmax It is the field. ( =200. by default)
  dl = (lastparam - firstparam)/(Standard_Real)(nbpointsmax-1);
  dumin = 0.1*dl;
  dumax = 10 * dl;
  //
  nbvtx = aline->NbVertex();
  //
  TColStd_Array1OfReal paramvertex(1,Max(nbvtx,1)), newparamvertex(1,Max(nbvtx,1));
  //
  for(v = 1; v<=nbvtx; v++) { 
    const IntPatch_Point& aVtx = aline->Vertex(v);
    pv=aVtx.ParameterOnLine();
    paramvertex(v)=pv;
    newparamvertex(v)=-1.;
  }
  //
  //-- Tri et Confusion des vertex proches 
  do { 
    TriOk = Standard_True;
    for(v=2; v<=nbvtx;v++) { 
      if(paramvertex(v) < paramvertex(v-1)) { 
	pv=paramvertex(v);
	paramvertex(v-1)=paramvertex(v);
	paramvertex(v)=pv;
	TriOk = Standard_False;
      }
    }
  }
  while(!TriOk);
  //
  for(v=2; v<=nbvtx;v++) { 
    pv=paramvertex(v);
    pv1=paramvertex(v-1);
    if(pv-pv1 < myTolParam) { 
      paramvertex(v)=pv1;
    }
  }
  //
  Standard_Integer t, nbpwline;
  Standard_Real u1,v1,u2,v2, anu1, anv1, anu2, anv2;
  Standard_Real aL, dl_sur_2;
  gp_Pnt Pnt3d, aPnt3d1;
  IntSurf_PntOn2S POn2S;
  Handle(IntSurf_LineOn2S) LinOn2S;
  //
  LinOn2S = new IntSurf_LineOn2S;

  //// Modified by jgv, 17.09.09 for OCC21255 ////
  Standard_Real refpar = RealLast(), ref_u1 = 0., ref_u2 = 0.;
  if (nbvtx)
    {
      const IntPatch_Point& FirstVertex = aline->Vertex(1);
      refpar = FirstVertex.ParameterOnLine();
      FirstVertex.Parameters(ref_u1, v1, ref_u2, v2);
    }
  ////////////////////////////////////////////////

  //-----------------------------------------------------
  //-- Estimation Grossiere de la longueur de la courbe
  //-- 
  aL=0.;
  dl_sur_2=0.5*dl; 
  Pnt3d = aline->Value(firstparam); 
  for(t=0, U=firstparam+dumin; U<lastparam; t++,U+=dl_sur_2) {
    aPnt3d1 = aline->Value(U);
    aL+=Pnt3d.Distance(aPnt3d1);
    Pnt3d=aPnt3d1;
  }
  //------------------------------------------------------
  aL/=t;
  aL*=2;
  //------------------------------------------------------
  //-- Calcul du premier point 
  //--
  //------------------------------------------------------
  //-- On reajuste firstparam et lastparam au cas ou ces 
  //-- valeurs sont tres proches des parametres de vertex
  if(nbvtx) { 
    Standard_Real pvtxmin, pvtxmax;
    //
    pvtxmin = aline->Vertex(1).ParameterOnLine();
    pvtxmax = pvtxmin;
    //
    for(v=2; v<=nbvtx; v++) { 
      pv=aline->Vertex(v).ParameterOnLine();
      if(pvtxmin>pv){
	pvtxmin = pv;
      }
      if(pvtxmax<pv){
	pvtxmax =pv;
      }
    }
    if(Abs(pvtxmin-firstparam)<myTolOpenDomain) {
      firstparam=pvtxmin;
    }
    if(Abs(pvtxmax-lastparam)<myTolOpenDomain) {
      lastparam=pvtxmax;
    }
  }
  //
  // First Point
  Pnt3d = aline->Value(firstparam);
  quad1.Parameters(Pnt3d,u1,v1);
  //
  RefineParameters(aline, firstparam, lastparam, firstparam, 1, quad1, 10.*myTol3D, u1,v1);
  //
  //
  quad2.Parameters(Pnt3d,u2,v2);
  //
  RefineParameters(aline, firstparam, lastparam, firstparam, 1, quad2, 10.*myTol3D, u2,v2);
  //

  POn2S.SetValue(Pnt3d,u1,v1,u2,v2);
  anu1 = u1;
  anu2 = u2;
  LinOn2S->Add(POn2S);
  nbpwline = 1;
  U = firstparam;
  //-------------------------------------------------------
  //-- On detecte le cas : Point de debut == vertex
  //-- On affecte un parametre bidon qui sera recadre
  //-- dans l insertion des vertex dans la ligne
  //--
  for(v=1;v<=nbvtx;v++) {
    if(newparamvertex(v)<0.) { 
      pv=paramvertex(v);
      if((pv>=U-2.0*myTolOpenDomain) && (pv<=U+2.0*myTolOpenDomain)) { 
	if(pv-U > myTolParam) {
	  newparamvertex(v) = 1.000001;
	}
	else if(U-pv>myTolParam) {
	  newparamvertex(v) = 0.999999;
	}
      }
    }
  }
  //
  Standard_Real DeltaU;
  //
  // the loop
  DeltaU=0.;
  //// Modified by jgv, 17.09.09 for OCC21255 ////
  Standard_Boolean Corrected = Standard_False;
  ////////////////////////////////////////////////
  while(U<lastparam) { 
    Standard_Integer NbCalculD1;
    Standard_Real UPourCalculD1, pvavant, pvapres;
    gp_Vec VPourCalculD1;
    gp_Pnt PPourCalculD1;
    //
    DeltaU=0.;
    NbCalculD1=0;
    UPourCalculD1=U;
    do { 
      if(aline->D1(UPourCalculD1,PPourCalculD1,VPourCalculD1)) { 
	Standard_Real NormV = VPourCalculD1.Magnitude();
	if(NormV > 1.0e-16) {
	  DeltaU = aL/NormV;
	}
      }
      if(DeltaU < dumin) 
	DeltaU = dumin;
      else if (DeltaU > dumax) 
	DeltaU = dumax;
      UPourCalculD1+=dumin;
    }
    while((++NbCalculD1<10)&&(DeltaU==0.));
    //
    //OCC541(apo)->
    // static  //xft 
    Standard_Real CurvDef = deflectionmax, AngDef = CurvDef; 
    if(U+DeltaU < lastparam) {
      DefineDU(aline,U,DeltaU,CurvDef,AngDef); 
    }
    //<-OCC451(apo)
    //
    if(DeltaU==0.){ 
      DeltaU = (dumin+dumax)*0.5;
    }
    //--------------------------------------------------------
    //-- On cherche a placer un minimum de ??? Points entre 
    //-- 2 vertex 
    //--------------------------------------------------------
    pvavant = firstparam;
    pvapres = lastparam;
    for(v=1;v<=nbvtx;v++) {
      pv=paramvertex(v);
      if(pv-U > myTolParam) { 
	if(pvapres>pv) { 
	  pvapres = pv;
	}
      }
      else { 
	if(U-pv > myTolParam) { 
	  if(pvavant<pv) { 
	    pvavant = pv;
	  }
	}
      }
    }
    pvapres-=pvavant;
    if(pvapres < (10.*DeltaU)) { 
      if(pvapres > (10.*dumin)) { 
	DeltaU = pvapres*0.1;
      }
      else { 
	DeltaU = dumin;
      }
    }
    //xf
    if (nbpwline==1 && nbvtx) {
      Standard_Real aUnext;
      //
      aUnext=U+DeltaU;
      pv=paramvertex(1);
      if (aUnext>pv){
	DeltaU=0.5*(pv-firstparam);
      }
    }
    //xt
    //--------------------------------------------------------
    //-- Calcul des nouveaux parametres sur les vertex
    //--
    for(v=1;v<=nbvtx;v++) {
      if(newparamvertex(v)<0.) { 
	pv=paramvertex(v);
	if(pv>=U  && pv<(U+DeltaU) && (U+DeltaU<lastparam) ) {
	  newparamvertex(v) = (Standard_Real)nbpwline + (pv-U)/DeltaU;
	}
      }
    }

    //// Modified by jgv, 17.09.09 for OCC21255 ////
    if (!Corrected && U >= refpar)
      {
	CorrectFirstPartOfLine(LinOn2S, ref_u1, ref_u2, anu1, anu2);
	Corrected = Standard_True;
      }
    ////////////////////////////////////////////////
    U+=DeltaU;
    if(U < lastparam) { 
      nbpwline++;
      Pnt3d = aline->Value(U);
      quad1.Parameters(Pnt3d,u1,v1);
      quad2.Parameters(Pnt3d,u2,v2);
      RecadreMemePeriode(u1,u2,anu1,anu2);
      anu1 = u1;
      anu2 = u2;
      POn2S.SetValue(Pnt3d,u1,v1,u2,v2);
      LinOn2S->Add(POn2S);
    }
  }//while(U<lastparam) 
  
  U-=DeltaU;
  for(v=1;v<=nbvtx;v++) {
    if(newparamvertex(v)<0.) { 
      pv=paramvertex(v);
      if(pv <= lastparam+myTolOpenDomain) {
	if(lastparam-U) { 
	  newparamvertex(v) = (Standard_Real)nbpwline+(pv-U)/(lastparam-U);
	}
	else { 
	  newparamvertex(v) = nbpwline+1;
	}
      }
    }
  }
  //
  Pnt3d = aline->Value(lastparam);
  quad1.Parameters(Pnt3d,u1,v1);
  //
  RefineParameters(aline, firstparam, lastparam, lastparam, -1, quad1, 10.*myTol3D, u1,v1);
  //
  quad2.Parameters(Pnt3d,u2,v2);
  //
  RefineParameters(aline, firstparam, lastparam, lastparam, -1, quad2, 10.*myTol3D, u2,v2);
  //
  RecadreMemePeriode(u1,u2,anu1,anu2);
  POn2S.SetValue(Pnt3d,u1,v1,u2,v2);
  LinOn2S->Add(POn2S);
  nbpwline++;

  //// Modified by jgv, 17.09.09 for OCC21255 ////
  if (!Corrected && 
      (lastparam >= refpar || refpar-lastparam < Precision::Confusion()))
    CorrectFirstPartOfLine(LinOn2S, ref_u1, ref_u2, anu1, anu2);
  ////////////////////////////////////////////////

  //
  //-----------------------------------------------------------------
  //--  Calcul de la transition de la ligne sur les surfaces      ---
  //-----------------------------------------------------------------
  IntSurf_TypeTrans trans1,trans2;
  Standard_Integer indice1;
  Standard_Real dotcross;
  gp_Pnt aPP0, aPP1;
  //
  trans1=IntSurf_Undecided;
  trans2=IntSurf_Undecided;
  //
  indice1 = nbpwline/3;
  if(indice1<=2) {
    indice1 = 2;
  }
  //
  aPP1=LinOn2S->Value(indice1).Value();
  aPP0=LinOn2S->Value(indice1-1).Value();
  //
  gp_Vec tgvalid(aPP0, aPP1);
  gp_Vec aNQ1=quad1.Normale(aPP0);
  gp_Vec aNQ2=quad2.Normale(aPP0);
  //
  dotcross = tgvalid.DotCross(aNQ2, aNQ1);
  if (dotcross > myTolTransition) {
    trans1 = IntSurf_Out;
    trans2 = IntSurf_In;
  }
  else if(dotcross < -myTolTransition) { 
    trans1 = IntSurf_In;
    trans2 = IntSurf_Out;
  } 
  //-----------------------------------------------------------------
  //--              C r e a t i o n  d  e   la   W L i n e        ---
  //-----------------------------------------------------------------
  Handle(IntPatch_WLine) wline;
  //
  if(aline->TransitionOnS1() == IntSurf_Touch)  { 
    Handle(IntPatch_WLine) wlinetemp = new IntPatch_WLine(LinOn2S,
					      aline->IsTangent(),
					      aline->SituationS1(),
					      aline->SituationS2());
    wline = wlinetemp;
  }
  if(aline->TransitionOnS1() == IntSurf_Undecided)  { 
    Handle(IntPatch_WLine) wlinetemp = new IntPatch_WLine(LinOn2S,
					      aline->IsTangent());
    wline = wlinetemp;
  }
  else { 
    Handle(IntPatch_WLine) wlinetemp = new IntPatch_WLine(LinOn2S,
					      aline->IsTangent(),
					      trans1, // aline->TransitionOnS1(),
					      trans2);  //aline->TransitionOnS2());
    wline = wlinetemp; 
  }

  //-----------------------------------------------------------------
  //--         I n s e r t i o n   d  e s  v e r t e x            ---
  //-----------------------------------------------------------------
  TColStd_Array1OfInteger Redir(1,Max(nbvtx,1));
  //
  for(v=1;v<=nbvtx;v++) { 
    Redir(v) = v;
  }
  //
  TriOk=Standard_True;
  Standard_Integer tamp;
  do { 
    TriOk = Standard_True;
    for(v=2; v<=nbvtx;v++) { 
      if(newparamvertex(Redir(v))<newparamvertex(Redir(v-1))) { 
	tamp = Redir(v-1);
	Redir(v-1) = Redir(v);
	Redir(v) = tamp;
	TriOk = Standard_False;
      }
    }
  }
  while(!TriOk);
 
  //-----------------------------------------------------------------
  //-- On detecte le cas ou un Vtx de param 1 ou nbpwline OnArc est double
  //-- Dans ce cas on supprime le vertex qui reference un arc deja reference 
  //-- par un autre vertex 
  //nbvtx = aline->NbVertex();
  Standard_Boolean APointHasBeenRemoved;
  //
  do { 
    Standard_Boolean RemoveVtxo, RemoveVtx;
    Standard_Integer vo, voo;
    Standard_Real ponl, ponlo, aDist13, aDist23;
    //
    APointHasBeenRemoved = Standard_False;
    RemoveVtxo = Standard_False;
    RemoveVtx  = Standard_False;
    //
    for(v=1; v<=nbvtx   && !APointHasBeenRemoved; v++) { 
      //
      if(newparamvertex(v)>=0.) { 
	const IntPatch_Point& Vtx = aline->Vertex(v);
	ponl = Vtx.ParameterOnLine();
	const gp_Pnt& aP=Vtx.Value();
	//
	for(vo=1; vo<=nbvtx && !APointHasBeenRemoved; vo++) { 
	  if(v!=vo) { 
	    if(newparamvertex(vo)>=0.) { 
	      const IntPatch_Point& Vtxo = aline->Vertex(vo);
	      ponlo = Vtxo.ParameterOnLine();
	      const gp_Pnt& aPo=Vtxo.Value();
	      //
	      if(ponl-ponlo<myTolParam && ponlo-ponl<myTolParam) { 
		//
		for(voo=1; voo<=nbvtx && !APointHasBeenRemoved; voo++) { 
		  if(voo!=v && voo!=vo) {
		    if(newparamvertex(voo)>=0.) { 
		      const IntPatch_Point& Vtxoo = aline->Vertex(voo);
		      const gp_Pnt& aPoo=Vtxoo.Value();
		      //
		      aDist13=aP.Distance(aPoo);
		      //
		      if(aDist13<=myTol3D) { 
			//-- 2 vertex de meme param  + un confondu geometriquement
			if((Vtx.IsOnDomS1() == Vtxoo.IsOnDomS1()) &&
			   (Vtx.IsOnDomS2() == Vtxoo.IsOnDomS2()) ) { 
			  
			  if(Vtx.IsOnDomS1()) { 
			    if(Vtx.ParameterOnArc1()==Vtxoo.ParameterOnArc1()) { 
			      if(SameCurve(Vtxoo.ArcOnS1(),Vtx.ArcOnS1())) { //-- param on s1 ?
				RemoveVtx = Standard_True;
			      }
			    }
			  }
			  if(Vtx.IsOnDomS2()) { 
			    if(Vtx.ParameterOnArc2()==Vtxoo.ParameterOnArc2()) { 
			      if(SameCurve(Vtxoo.ArcOnS2(),Vtx.ArcOnS2())) { 
				RemoveVtx = Standard_True;
			      }
			    }
			    else { 
			      RemoveVtx = Standard_False;
			    }
			  }
			}
			//
			if(RemoveVtx==Standard_False) { 
			  //
			  aDist23=aPo.Distance(aPoo);
			  //
			  if(aDist23<=myTol3D) { 
			    //-- 2 vertex de meme param  + un confondu geometriquement
			    if((Vtxo.IsOnDomS1() == Vtxoo.IsOnDomS1())&&
			       (Vtxo.IsOnDomS2() == Vtxoo.IsOnDomS2()) ) { 
			      if(Vtxo.IsOnDomS1()) { 
				if(Vtxo.ParameterOnArc1()==Vtxoo.ParameterOnArc1()) { 
				  if(SameCurve(Vtxoo.ArcOnS1(),Vtxo.ArcOnS1())) { //-- param on s1 ?
				    RemoveVtxo = Standard_True;
				  }
				}
			      }
			      if(Vtxo.IsOnDomS2()) { 
				if(Vtxo.ParameterOnArc2()==Vtxoo.ParameterOnArc2()) { 
				  if(SameCurve(Vtxoo.ArcOnS2(),Vtxo.ArcOnS2())) { 
				    RemoveVtxo = Standard_True;
				  }
				}
				else { 
				  RemoveVtxo = Standard_False;
				}
			      }
			    }
			  }
			} //-- 
			if(RemoveVtx) { 
			  newparamvertex(v) = -1.;
			  APointHasBeenRemoved = Standard_True;
			}
			else if(RemoveVtxo) { 
			  newparamvertex(vo) = -1.;
			  APointHasBeenRemoved = Standard_True;
			}
		      } //-- Pnt = Pntoo 
		    } //-- voo!=v && voo!=vo
		  } //-- Fin boucle sur voo 
		}
	      }
	    }  
	  }
	}
      }
    }//for(v=1; v<=nbvtx   && !APointHasBeenRemoved; v++) 
  }
  while(APointHasBeenRemoved);
  //
  Standard_Integer ParamVtxPrecedent, refpointonwline, aIndV;
  Standard_Real pvtx, approxparamonwline, aDst;
  Standard_Boolean bIsApex1, bIsApex2;
  //
  ParamVtxPrecedent = 0;
  //
  for(v=1; v<=nbvtx; v++) { 
    aIndV=Redir(v);
    pv=paramvertex(v);// parameter from ALine
    pvtx = newparamvertex(aIndV);
    if(pvtx>=0. && (pvtx <= nbpwline+1)) {
      approxparamonwline=newparamvertex(aIndV);
      refpointonwline=1;
      //
      IntPatch_Point NewPoint = aline->Vertex(aIndV);
      //
      Pnt3d = NewPoint.Value();
      quad1.Parameters(Pnt3d, u1, v1);
      quad2.Parameters(Pnt3d, u2, v2);
      //-------------------------------------------------------
      //-- On recadre les parametres des vertex dans la bonne -
      //-- periode en recadrant avec le point le plus proche  -
      //-------------------------------------------------------
      if(approxparamonwline > nbpwline) { 
	refpointonwline = nbpwline-1;
      }
      else if(approxparamonwline < 1) { 
	refpointonwline = 1;
      }
      else { 
	refpointonwline = (Standard_Integer)approxparamonwline;
      }
      //
      //
      const IntSurf_PntOn2S& aP2Sx=LinOn2S->Value(refpointonwline);
      aP2Sx.ParametersOnS1(anu1, anv1);
      aP2Sx.ParametersOnS2(anu2, anv2);
      //
      bIsApex1=IsApex(quad1, v1, myTol3D);
      bIsApex2=IsApex(quad2, v2, myTol3D);
      //
      //if (refpointonwline==1 || refpointonwline==nbpwline) {
      if (bIsApex1 || bIsApex2) {
	if (fabs(pv-firstparam)<myTolParam || fabs(pv-lastparam)<myTolParam) {
	  // aline starts(ends) on vertex 
	  const gp_Pnt& aP1x=aP2Sx.Value();
	  //
	  aDst=aP1x.Distance(Pnt3d);
	  if (aDst<10.*myTol3D) {
	    u1=anu1;
	    v1=anv1;
	    u2=anu2;
	    v2=anv2;
	  }
	}
	//
	else {
	  // aline goes through vertex 
	  if (bIsApex1) {
	    u1=0.;
	  }
	  if (bIsApex2) {
	    u2=0.;
	  }
	}
      }
      //
      //
      if(v==1) { 
	ParamVtxPrecedent=refpointonwline;
	RecadreMemePeriode(u1,u2,anu1,anu2);
	NewPoint.SetParameter(refpointonwline);
	//
	NewPoint.SetParameters(u1,v1,u2,v2);
	wline->AddVertex(NewPoint);
      }
      //
      else { 
	if(ParamVtxPrecedent==refpointonwline) { 
	  //-- 2 vertex renseignent le meme point de la LineOn2S
	  //-- On insere un nv point  =  vtx
	  //-- On decale tous les vtx apres de 1 
	  RecadreMemePeriode(u1,u2,anu1,anu2);
	  POn2S.SetValue(Pnt3d,u1,v1,u2,v2);
	  LinOn2S->InsertBefore(refpointonwline+1, POn2S);
	  nbpwline++;
	  NewPoint.SetParameter(refpointonwline+1);
	  NewPoint.SetParameters(u1,v1,u2,v2);
	  wline->AddVertex(NewPoint);
	  ParamVtxPrecedent = refpointonwline+1;
	  //
	  Standard_Integer vv=v+1;
	  for(; vv<=nbvtx; vv++) { 
	    if(newparamvertex(Redir(vv))!=-1.) { 
	      newparamvertex(Redir(vv))=newparamvertex(Redir(vv))+1.;
	    }
	  }
	}
	//
	else { 
	  RecadreMemePeriode(u1,u2, anu1, anu2);
	  NewPoint.SetParameter(refpointonwline);
	  //
	  NewPoint.SetParameters(u1, v1, u2, v2);
	  wline->AddVertex(NewPoint);
	  ParamVtxPrecedent = refpointonwline;
	}
      }
    }
  }
  //
  pu1=0.;
  pv1=0.;
  pu2=0.;
  pv2=0.;
  //
  switch(quad1.TypeQuadric()) { 
  case GeomAbs_Cylinder:
  case GeomAbs_Cone:
  case GeomAbs_Sphere:
    pu1=M_PI+M_PI;
    break;
  case GeomAbs_Torus:
    pu1=pv1=M_PI+M_PI;
    break;
  default:
    break;
  }
  switch(quad2.TypeQuadric()) { 
  case GeomAbs_Cylinder:
  case GeomAbs_Cone:
  case GeomAbs_Sphere:
    pu2=M_PI+M_PI;
    break;
  case GeomAbs_Torus:
    pu2=pv2=M_PI+M_PI;
    break;
  default:
    break;
  }
  wline->SetPeriod(pu1,pv1,pu2,pv2);
  
  wline->ComputeVertexParameters(myTol3D);
  return(wline);
}
//=======================================================================
//function : DefineDU
//purpose  : 
//=======================================================================
gp_Pnt DefineDU(const Handle(IntPatch_ALine)& aline, 
		const Standard_Real U, 
		Standard_Real& DU,
		const Standard_Real CurvDef, 
		const Standard_Real AngDef)
{
  gp_Pnt P1 = aline->Value(U), P2, P3;
  gp_Vec V13, V12, V23;
  Standard_Real dU = DU/2.0, curvDef, angDef, m1, m2, m3;
  for(;;) { //According to class TangentialDeflection from GCPnts
    P2=aline->Value(U+dU);  P3=aline->Value(U+DU);
    V13 = P3.XYZ().Subtracted(P1.XYZ());  m1 = V13.Magnitude();
    V12 = P2.XYZ().Subtracted(P1.XYZ());  m2 = V12.Magnitude();
    V23 = P3.XYZ().Subtracted(P2.XYZ());  m3 = V23.Magnitude();
    if(m1 < CurvDef || m2 < CurvDef || m3 < CurvDef) break;
    curvDef = Abs(V13.Angle(V12));
    angDef = Abs(V13.Angle(V23));
    if(curvDef < CurvDef && angDef < AngDef) break;
    DU = dU;  dU /= 2.0;  
  }
  return P3;
}
//=======================================================================
//function : SameCurve
//purpose  : 
//=======================================================================
Standard_Boolean SameCurve(const Handle(Adaptor2d_HCurve2d)& C1,const Handle(Adaptor2d_HCurve2d)& C2) 
{ 
  Standard_Real C1f = C1->FirstParameter();
  Standard_Real C2f = C2->FirstParameter();
  if(C1f!=C2f) return(Standard_False);
  Standard_Real C1l = C1->LastParameter();
  Standard_Real C2l = C2->LastParameter();
  if(C1l!=C2l) return(Standard_False);
  Standard_Real u=0.3*C1f+0.7*C1l;
  gp_Pnt2d P1 = C1->Value(u);
  gp_Pnt2d P2 = C2->Value(u);
  if(P1.X()!=P2.X()) return(Standard_False);
  if(P1.Y()!=P2.Y()) return(Standard_False);
  return(Standard_True);
}
//=======================================================================
//function : RecadreMemePeriode
//purpose  : 
//=======================================================================
void RecadreMemePeriode(Standard_Real& u1,
			Standard_Real& u2,
			const Standard_Real anu1,
			const Standard_Real anu2) 
{ 
  //
  while(anu1-u1 > 5.0) {
    u1+=M_PI+M_PI;
  }
  while(u1-anu1 > 5.0) { 
    //
    /*
    if (!bBothCylinders) {//cfe900/H6
      // this check on Cylinder/Cylinder intersection is probably 
      // because of pbs with ALine on it.
      // For other Quadrics there was no pbs found during 
      // grid tests.
      // (it is necessary to see ...IntCyCy(...) for details).
      //
      // In any case the pb does not deal with apex problem. 
      //
      if (u1-M_PI-M_PI<0.) {
	break;
      }
    }
    */
    //
    u1-=M_PI+M_PI;
  }
  while(anu2-u2 > 5.0) { 
    u2+=M_PI+M_PI;
  }
  while(u2-anu2 > 5.0) {
    //
    /*
    if (!bBothCylinders) {//cfe900/H6
      if (u2-M_PI-M_PI<0.) {
	break;
      }
    }
    */
    //
    u2-=M_PI+M_PI;
  }
}

//=======================================================================
//function : CorrectFirstPartOfLine
//purpose  : 
//=======================================================================
void CorrectFirstPartOfLine(Handle(IntSurf_LineOn2S)& LinOn2S,
			    const Standard_Real ref_u1,
			    const Standard_Real ref_u2,
			    Standard_Real& new_u1,
			    Standard_Real& new_u2)
{
  Standard_Integer nbp = LinOn2S->NbPoints();
  Standard_Real u1, v1, u2, v2, OffsetOnS1, OffsetOnS2;

  IntSurf_PntOn2S aPoint = LinOn2S->Value(nbp);
  aPoint.Parameters(u1, v1, u2, v2);

  new_u1 = u1;
  new_u2 = u2;
  RecadreMemePeriode(new_u1, new_u2, ref_u1, ref_u2);
  OffsetOnS1 = new_u1 - u1;
  OffsetOnS2 = new_u2 - u2;
  if (Abs(OffsetOnS1) > 1. || Abs(OffsetOnS2) > 1.) //recadre on n*2*PI is done
    {
      Standard_Integer i;
      for (i = 1; i <= nbp; i++)
	{
	  aPoint = LinOn2S->Value(i);
	  aPoint.Parameters(u1, v1, u2, v2);
	  LinOn2S->SetUV( i, Standard_True,  u1 + OffsetOnS1, v1 );
	  LinOn2S->SetUV( i, Standard_False, u2 + OffsetOnS2, v2 );
	}
    }
}

//
//=======================================================================
//function : IsApex
//purpose  : 
//=======================================================================
Standard_Boolean IsApex(const IntSurf_Quadric& aQuadric,
			const Standard_Real aVx,
			const Standard_Real aTol3D)
			
{
  Standard_Boolean bFlag;
  Standard_Real aHalfPi, aEpsilon;
  GeomAbs_SurfaceType aType;
  //
  bFlag=Standard_False;
  //
  aType=aQuadric.TypeQuadric();
  if (!(aType==GeomAbs_Cone || aType==GeomAbs_Sphere)) {
    return bFlag;
  }
  //
  aEpsilon=Epsilon(10.);//1.77e-15
  //
  // apex on the Sphere
  if(aType==GeomAbs_Sphere) {
    aHalfPi=0.5*M_PI;
    if (fabs(aVx-aHalfPi)<aEpsilon) {
      bFlag=!bFlag;
    }
    else if (fabs(aVx+aHalfPi)<aEpsilon){
      bFlag=!bFlag;
    }
  }
  //
  // apex on the Cone
  else if(aType==GeomAbs_Cone) {
    Standard_Real aDst;
    gp_Pnt aPap, aPx;
    gp_Cone aCone;
    //
    aCone=aQuadric.Cone();
    aPap=aCone.Apex();
    aPx=aQuadric.Value(0.,aVx);
    //
    aDst=aPx.Distance(aPap);
    if(aDst<aTol3D) {
      bFlag=!bFlag;
    }
  }
  return bFlag;
}   
//=======================================================================
//function : RefineParameters
//purpose  : 
//=======================================================================
void RefineParameters(const Handle(IntPatch_ALine)& aALine,
		      const Standard_Real aTb,
		      const Standard_Real aTe,
		      const Standard_Real aTx,
		      const Standard_Integer iDir,
		      const IntSurf_Quadric& aQuadric,
		      const Standard_Real aTol3D,
		      Standard_Real& aUx,
		      Standard_Real& aVx)
{
  GeomAbs_SurfaceType aType;
  //
  aType=aQuadric.TypeQuadric();
  if (!(aType==GeomAbs_Cone || aType==GeomAbs_Sphere)) {
    return;
  }
  //
  Standard_Boolean bIsDone, bIsEmpty, bParallel, bFound;
  Standard_Integer aNbPoints = 0;
  Standard_Real aHalfPi, aEpsilon, aLimV, dT, aT1, aT2 = 0., aEpsT;
  Standard_Real aU1, aV1, aU2, aV2;
  gp_Pnt aP1, aP2, aPx;
  gp_Pnt2d  aP2D1, aP2D2, aPLim(0., 0.);
  gp_Vec2d aVLim(1., 0.);
  gp_Lin2d aLLim;
  IntAna2d_AnaIntersection aAI;
  //
  aEpsilon=Epsilon(10.);//1.77e-15
  aEpsT=0.0001;
  aLLim.SetDirection(aVLim);
  //
  // apex on the Cone
  if(aType==GeomAbs_Cone) {
    Standard_Real aDst;
    gp_Pnt aPap;
    gp_Cone aCone;
    //
    aCone=aQuadric.Cone();
    aPap=aCone.Apex();
    //aPx=aQuadric.Value(0.,aVx);
    aPx=aALine->Value(aTx);
    //
    aDst=aPx.Distance(aPap);
    if(aDst>aTol3D) {// nothing to do
      return;
    }
    //
    aPLim.SetY(aVx);
    aLLim.SetLocation(aPLim);
    //
  }
  //
  // apex on the Sphere
  if(aType==GeomAbs_Sphere) {
    aHalfPi=0.5*M_PI;
    //
    if (fabs(aVx-aHalfPi)<aEpsilon) {
      aLimV=aHalfPi;
    }
    else if (fabs(aVx+aHalfPi)<aEpsilon){
      aLimV=-aHalfPi;
    }
    else {
      //Check: aUx must be 0 or 2*pi
      if(fabs(aUx) < aEpsilon || fabs(aUx - 2.*M_PI) < aEpsilon) {
	//aUx = 0 or 2*pi, but may be it must be 2*pi or 0?
	bFound=FindNearParameter(aALine, aTx, iDir, aTol3D, aT1);
	if(!bFound) {
	  dT=aEpsT*(aTe-aTb);
	  if (iDir<0) {
	    dT=-dT;
	  }
	  aT1=aTx+dT;
	}

	aP1=aALine->Value(aT1);
	aQuadric.Parameters(aP1, aU1, aV1);

	if(fabs(aU1) > fabs(aU1 - 2.*M_PI)) {
	  aUx = 2.*M_PI;
	}
	else {
	  aUx = 0.;
	}
      }
	
      return;
    }
    //
    aPLim.SetY(aLimV);
    aLLim.SetLocation(aPLim);
  }
  //
  // aT1, aT2
  //
  // Try to find aT1, aT2 taking into acc 3D Tolerance
  bFound=FindNearParameter(aALine, aTx, iDir, aTol3D, aT1);
  if (bFound) {
    bFound=FindNearParameter(aALine, aT1, iDir, aTol3D, aT2);
  }
  if (!bFound) {
    // Assign aT1, aT2 by some values
    dT=aEpsT*(aTe-aTb);
    if (iDir<0) {
      dT=-dT;
    }
    aT1=aTx+dT;
    aT2=aT1+dT;
  }
  //
  aP1=aALine->Value(aT1);
  aQuadric.Parameters(aP1, aU1, aV1);
  aP2D1.SetCoord(aU1, aV1);
  //
  aP2=aALine->Value(aT2);
  aQuadric.Parameters(aP2, aU2, aV2);
  aP2D2.SetCoord(aU2, aV2);
  //
  gp_Vec2d aV12(aP2D1, aP2D2);

  if(aV12.SquareMagnitude() <= aEpsilon) {
    return;
  }
  gp_Lin2d aL12(aP2D1, aV12);
  //
  aAI.Perform(aLLim, aL12);
  bIsDone=aAI.IsDone();
  if (!bIsDone) {
    return;
  }
  bIsEmpty=aAI.IsEmpty();
  if (bIsEmpty) {
    return;
  }
  aNbPoints=aAI.NbPoints();
  if (!aNbPoints) {
    return;
  }
  bParallel=aAI.ParallelElements();
  if (bParallel) {
    return;
  }
  const IntAna2d_IntPoint& aIAPnt=aAI.Point(1);
  const gp_Pnt2d& aP2D=aIAPnt.Value();
  aUx=aP2D.X();
}
//=======================================================================
//function : FindNearParameter
//purpose  : 
//=======================================================================
Standard_Boolean FindNearParameter(const Handle(IntPatch_ALine)& aALine,
				   const Standard_Real aTx,
				   const Standard_Integer iDir,
				   const Standard_Real aTol3D,
				   Standard_Real& aT1)
{
  Standard_Boolean bFound;
  Standard_Real aX, aY, aZ;
  gp_Pnt aPx, aP1;
  gp_Vec aVx;
  //
  aT1=0.;
  //
  bFound=aALine->D1(aTx, aPx, aVx);
  if(!bFound){
    return bFound;
  }
  gp_Dir aDx(aVx);
  if (iDir<0) {
    aDx.Reverse();
  }
  aX=aPx.X()+aDx.X()*aTol3D;
  aY=aPx.Y()+aDx.Y()*aTol3D;
  aZ=aPx.Z()+aDx.Z()*aTol3D;
  aP1.SetCoord(aX, aY, aZ);
  //
  bFound=aALine->FindParameter(aP1, aT1);
  //
  return bFound;
}
		      
