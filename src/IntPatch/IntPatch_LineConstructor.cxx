// Created on: 1996-11-07
// Created by: Laurent BUCHARD
// Copyright (c) 1996-1999 Matra Datavision
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
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <IntPatch_ALine.hxx>
#include <IntPatch_GLine.hxx>
#include <IntPatch_Line.hxx>
#include <IntPatch_LineConstructor.hxx>
#include <IntPatch_RLine.hxx>
#include <IntPatch_WLine.hxx>

#define XPU1009 1

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <IntSurf_Quadric.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <Standard_ConstructionError.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <ElCLib.hxx>
#include <Geom2dInt_TheProjPCurOfGInter.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_IndexedMapOfTransient.hxx>
#include <TColStd_Array1OfTransient.hxx>
#include <TColStd_Array1OfReal.hxx>


//=======================================================================
//function : Recadre
//purpose  : 
//=======================================================================

static void Recadre(const Handle(Adaptor3d_HSurface)& myHS1,
		    const Handle(Adaptor3d_HSurface)& myHS2,
		    Standard_Real& u1,
		    Standard_Real& v1,
		    Standard_Real& u2,
		    Standard_Real& v2) { 
  Standard_Real f,l,lmf;
  GeomAbs_SurfaceType typs1 = myHS1->GetType();
  GeomAbs_SurfaceType typs2 = myHS2->GetType();

  Standard_Boolean myHS1IsUPeriodic,myHS1IsVPeriodic;
  switch (typs1) { 
  case GeomAbs_Cylinder:
  case GeomAbs_Cone:
  case GeomAbs_Sphere: 
    { 
      myHS1IsUPeriodic = Standard_True;
      myHS1IsVPeriodic = Standard_False;
      break;
    }
  case GeomAbs_Torus:
    {
      myHS1IsUPeriodic = myHS1IsVPeriodic = Standard_True;
      break;
    }
  default:
     {
       //-- Le cas de biparametrees periodiques est gere en amont
       myHS1IsUPeriodic = myHS1IsVPeriodic = Standard_False;
       break;
     }
  }

  Standard_Boolean myHS2IsUPeriodic,myHS2IsVPeriodic;
  switch (typs2) { 
  case GeomAbs_Cylinder:
  case GeomAbs_Cone:
  case GeomAbs_Sphere: 
    { 
      myHS2IsUPeriodic = Standard_True;
      myHS2IsVPeriodic = Standard_False;
      break;
    }
  case GeomAbs_Torus:
    {
      myHS2IsUPeriodic = myHS2IsVPeriodic = Standard_True;
      break;
    }
  default:
     {
       //-- Le cas de biparametrees periodiques est gere en amont
       myHS2IsUPeriodic = myHS2IsVPeriodic = Standard_False;
       break;
     }
  }
  if(myHS1IsUPeriodic) {
    lmf = M_PI+M_PI; //-- myHS1->UPeriod();
    f = myHS1->FirstUParameter();
    l = myHS1->LastUParameter();
    while(u1 < f) { u1+=lmf; } 
    while(u1 > l) { u1-=lmf; }
  }
  if(myHS1IsVPeriodic) {
    lmf = M_PI+M_PI; //-- myHS1->VPeriod(); 
    f = myHS1->FirstVParameter();
    l = myHS1->LastVParameter();
    while(v1 < f) { v1+=lmf; } 
    while(v1 > l) { v1-=lmf; }
  }
  if(myHS2IsUPeriodic) { 
    lmf = M_PI+M_PI; //-- myHS2->UPeriod();
    f = myHS2->FirstUParameter();
    l = myHS2->LastUParameter();
    while(u2 < f) { u2+=lmf; } 
    while(u2 > l) { u2-=lmf; }
  }
  if(myHS2IsVPeriodic) { 
    lmf = M_PI+M_PI; //-- myHS2->VPeriod();
    f = myHS2->FirstVParameter();
    l = myHS2->LastVParameter();
    while(v2 < f) { v2+=lmf; } 
    while(v2 > l) { v2-=lmf; }
  }
}

//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================

static void Parameters(const Handle(Adaptor3d_HSurface)& myHS1,
                       const Handle(Adaptor3d_HSurface)& myHS2,
                       const gp_Pnt& Ptref,
                       Standard_Real& U1,
                       Standard_Real& V1,
                       Standard_Real& U2,
                       Standard_Real& V2)
{
  IntSurf_Quadric quad1,quad2;
  GeomAbs_SurfaceType typs = myHS1->Surface().GetType();
  switch (typs) {
  case GeomAbs_Plane:
    quad1.SetValue(myHS1->Surface().Plane());
    break;
  case GeomAbs_Cylinder:
    quad1.SetValue(myHS1->Surface().Cylinder());
    break;
  case GeomAbs_Cone:
    quad1.SetValue(myHS1->Surface().Cone());
    break;
  case GeomAbs_Sphere:
    quad1.SetValue(myHS1->Surface().Sphere());
    break;
  case GeomAbs_Torus:
    quad1.SetValue(myHS1->Surface().Torus());
    break;
  default:
    throw Standard_ConstructionError("IntPatch_IntSS::MakeCurve");
  }
  
  typs = myHS2->Surface().GetType();
  switch (typs) {
  case GeomAbs_Plane:
    quad2.SetValue(myHS2->Surface().Plane());
    break;
  case GeomAbs_Cylinder:
    quad2.SetValue(myHS2->Surface().Cylinder());
    break;
  case GeomAbs_Cone:
    quad2.SetValue(myHS2->Surface().Cone());
    break;
  case GeomAbs_Sphere:
    quad2.SetValue(myHS2->Surface().Sphere());
    break;
  case GeomAbs_Torus:
    quad2.SetValue(myHS2->Surface().Torus());
    break;
  default:
    throw Standard_ConstructionError("IntPatch_IntSS::MakeCurve");
  }
  quad1.Parameters(Ptref,U1,V1);
  quad2.Parameters(Ptref,U2,V2);
}

//=======================================================================
//function : LocalFirstParameter
//purpose  : 
//=======================================================================

static Standard_Real LocalFirstParameter (const Handle(IntPatch_Line)& L)
{
  Standard_Real firstp =0.;
  IntPatch_IType typl = L->ArcType();
  switch (typl) {
  case IntPatch_Analytic:
    {
      Handle(IntPatch_ALine) alin (Handle(IntPatch_ALine)::DownCast (L));
      if (alin->HasFirstPoint()) {
	firstp = alin->FirstPoint().ParameterOnLine();
      }
      else {
	Standard_Boolean included;
	firstp = alin->FirstParameter(included);
	if (!included) {
	  firstp +=Epsilon(firstp);
	}
      }
      return firstp;
    }

  case IntPatch_Restriction:
    {
      Handle(IntPatch_RLine) rlin (Handle(IntPatch_RLine)::DownCast (L));
      if (rlin->HasFirstPoint()) {
	firstp = rlin->FirstPoint().ParameterOnLine();
      }
      else {
	firstp = -Precision::Infinite(); // a voir selon le type de la ligne 2d
      }
      return firstp;
    }
   case IntPatch_Walking:
    {
      
      Handle(IntPatch_WLine) wlin (Handle(IntPatch_WLine)::DownCast (L));
      if (wlin->HasFirstPoint()) {
	firstp = wlin->FirstPoint().ParameterOnLine();
      }
      else {
	firstp = 1.;
      }
      return firstp;
    }
	
  default:
    {
      Handle(IntPatch_GLine) glin (Handle(IntPatch_GLine)::DownCast (L));
      if (glin->HasFirstPoint()) {
	firstp = glin->FirstPoint().ParameterOnLine();
      }
      else {
	switch (typl) {
	case IntPatch_Lin:
	case IntPatch_Parabola:
	case IntPatch_Hyperbola:
	  firstp = -Precision::Infinite();
	  break;

	case IntPatch_Circle:
	case IntPatch_Ellipse:
	  firstp = 0.;
	  break;
	default:
	  {
	  }
	}
      }
      return firstp;
    }
  }
//  return firstp;
}

//=======================================================================
//function : LocalLastParameter
//purpose  : 
//=======================================================================

static Standard_Real LocalLastParameter (const Handle(IntPatch_Line)& L)
{
  Standard_Real lastp =0.;
  IntPatch_IType typl = L->ArcType();
  switch (typl) {
  case IntPatch_Analytic:
    {
      Handle(IntPatch_ALine) alin (Handle(IntPatch_ALine)::DownCast (L));

      if (alin->HasLastPoint()) {
	lastp = alin->LastPoint().ParameterOnLine();
      }
      else {
	Standard_Boolean included;
	lastp = alin->LastParameter(included);
	if (!included) {
	  lastp -=Epsilon(lastp);
	}
      }
      return lastp;
    }

  case IntPatch_Restriction:
    {
      Handle(IntPatch_RLine) rlin (Handle(IntPatch_RLine)::DownCast (L));
      
      if (rlin->HasLastPoint()) {
	lastp = rlin->LastPoint().ParameterOnLine();
      }
      else {
	lastp = Precision::Infinite(); // a voir selon le type de la ligne 2d
      }
      return lastp;
    }
   case IntPatch_Walking:
    {
      Handle(IntPatch_WLine) wlin (Handle(IntPatch_WLine)::DownCast (L));

      if (wlin->HasLastPoint()) {
	lastp = wlin->LastPoint().ParameterOnLine();
      }
      else {
	lastp = wlin->NbPnts();
      }
      return lastp;
    }
	
  default:
    {
      Handle(IntPatch_GLine) glin (Handle(IntPatch_GLine)::DownCast (L));
      
      if (glin->HasLastPoint()) {
	lastp = glin->LastPoint().ParameterOnLine();
      }
      else {
	switch (typl) {
	case IntPatch_Lin:
	case IntPatch_Parabola:
	case IntPatch_Hyperbola:
	  lastp = Precision::Infinite();
	  break;

	case IntPatch_Circle:
	case IntPatch_Ellipse:
	  lastp = M_PI+M_PI;
	  break;
	default:
	  {
	  }
	}
      }
      return lastp;
    }
  }
}


// modified by NIZHNY-MKK  Tue Apr  3 15:03:06 2001.BEGIN
//=======================================================================
//function : ComputeParametricTolerance
//purpose  : 
//=======================================================================

static Standard_Real ComputeParametricTolerance(const Standard_Real theTol3d,
						const gp_Vec&       theD1u,
						const gp_Vec&       theD1v) {
  Standard_Real nad1u = theD1u.Magnitude();
  Standard_Real nad1v = theD1v.Magnitude();
  Standard_Real tolu = 0., tolv = 0.;
  if(nad1u > 1e-12) 
    tolu = theTol3d/nad1u; 
  else tolu = 0.1;
  if(nad1v > 1e-12) 
    tolv = theTol3d/nad1v; 
  else tolv = 0.1;
  Standard_Real aTolerance = (tolu > tolv) ? tolu : tolv;
  return aTolerance;
}
// modified by NIZHNY-MKK  Tue Apr  3 15:03:11 2001.END


//=======================================================================
//function : IntPatch_LineConstructor
//purpose  : 
//=======================================================================

IntPatch_LineConstructor::IntPatch_LineConstructor(const Standard_Integer )
{
}

//=======================================================================
//function : AppendSameVertexA
//purpose  : 
//=======================================================================

static Standard_Integer AppendSameVertexA(Handle(IntPatch_ALine)&alig,
				  const Handle(IntPatch_ALine)& L,
				  const Standard_Integer index,
				  Standard_Integer *TabIndex) { 
  Standard_Integer i,a,n;
  a=0;
  n=L->NbVertex();
  const IntPatch_Point& Vtxindex = L->Vertex(index);
  Standard_Real thetol1=Vtxindex.Tolerance();
  for(i=1;i<=n;i++) { 
    if(i!=index) { 
      const IntPatch_Point& Vtxi = L->Vertex(i);
      Standard_Real thetol2=Vtxi.Tolerance();
      if(thetol2<thetol1) 
	thetol2=thetol1;
      Standard_Real d_4=Vtxindex.Value().Distance(Vtxi.Value());
      if(d_4 <= thetol2) { 
	alig->AddVertex(Vtxi);
	a++;
	TabIndex[i]=TabIndex[index];
      }
    }
  }
  return(a);
}

//=======================================================================
//function : AppendSameVertexG
//purpose  : 
//=======================================================================

static Standard_Integer AppendSameVertexG(Handle(IntPatch_GLine)& glig,const Handle(IntPatch_GLine)&L,
				  const Standard_Integer index,
				  const Standard_Real decal,
				  Standard_Integer *TabIndex) { 
  Standard_Integer i,a,n;
  Standard_Real p1,p2,d; //,tol
  Standard_Boolean aajouter;
  a=0;
  n=L->NbVertex();
  const IntPatch_Point& Vtxindex = L->Vertex(index);
  Standard_Real thetol1=Vtxindex.Tolerance();
  for(i=1;i<=n;i++) { 
    if(i!=index) { 
      const IntPatch_Point& Vtxi = L->Vertex(i);
      aajouter=Standard_False;
      Standard_Real thetol2=Vtxi.Tolerance();
      if(thetol2<thetol1) 
	thetol2=thetol1;
      d=Vtxindex.Value().Distance(Vtxi.Value());
      if(d <= thetol2) { 
	aajouter=Standard_True;
      }
      
      
      //-- Le test suivant a ete ajoute le 20 aout 98 (??? mefiance ???) 
      else { 
	p1=Vtxindex.ParameterOnLine();
	p2=Vtxi.ParameterOnLine();
	if(Abs(p1-p2)<Precision::PConfusion()) { 
	  aajouter=Standard_True;
	}
      }
      if(aajouter) { 
	p1= Vtxindex.ParameterOnLine();
	IntPatch_Point aVtx = Vtxi;
	aVtx.SetParameter(p1+decal);
	glig->AddVertex(aVtx);
	a++;
	TabIndex[i]=TabIndex[index];
      }
    }
  }
  return(a);
}

//=======================================================================
//function : AppendSameVertexW
//purpose  : 
//=======================================================================

static Standard_Integer AppendSameVertexW(Handle(IntPatch_WLine)& wlig,
				  const Handle(IntPatch_WLine)&L,
				  const Standard_Integer index,
				  const Standard_Real par,
				  Standard_Integer *TabIndex) { 
  Standard_Integer i,a,n;
  a=0;
  n=L->NbVertex();
  const IntPatch_Point& Vtxindex = L->Vertex(index);
  const gp_Pnt& Pntindex = Vtxindex.Value();
  Standard_Real thetol1=Vtxindex.Tolerance();
  for(i=1;i<=n;i++) { 
    if(i!=index) { 
      IntPatch_Point Vtxi = L->Vertex(i);
      Standard_Real d_2 = Pntindex.Distance(Vtxi.Value());
      Standard_Real thetol2=Vtxi.Tolerance();
      if(thetol2<thetol1) 
	thetol2=thetol1;
      //-- le debugger voit 2 fois la variable d ici. ???? -> d_2
      if(d_2 <= thetol2) { 
	Vtxi.SetParameter(par);
	Standard_Real u1,v1,u2,v2;
	Vtxindex.ParametersOnS1(u1,v1);
	Vtxindex.ParametersOnS2(u2,v2);
	Vtxi.SetParameters(u1,v1,u2,v2);
	Vtxi.SetValue(Pntindex);
	wlig->AddVertex(Vtxi);
	a++;
	TabIndex[i]=TabIndex[index];
      }
    }
  }
  return(a);
}

//=======================================================================
//function : AppendSameVertexR
//purpose  : 
//=======================================================================

static Standard_Integer AppendSameVertexR(Handle(IntPatch_RLine)&rlig,
				  const Handle(IntPatch_RLine)& L,
				  const Standard_Integer index,
				  Standard_Integer *TabIndex) { 
  Standard_Integer i,a,n;
  a=0;
  n=L->NbVertex();
  const IntPatch_Point& Vtxindex = L->Vertex(index);
  Standard_Real thetol1=Vtxindex.Tolerance();
  for(i=1;i<=n;i++) { 
    if(i!=index) { 
      const IntPatch_Point& Vtxi = L->Vertex(i);
      Standard_Real d_3=Vtxindex.Value().Distance(Vtxi.Value());
      Standard_Real thetol2=Vtxi.Tolerance();
      if(thetol2<thetol1) 
	thetol2=thetol1;
      if(d_3<thetol2) { 
	if(Vtxi.ParameterOnLine() != Vtxindex.ParameterOnLine()) { 
	  IntPatch_Point Vtxicop = L->Vertex(i);
	  Vtxicop.SetParameter(Vtxindex.ParameterOnLine());
	  rlig->AddVertex(Vtxicop);
	}
	else { 
	  rlig->AddVertex(Vtxi);
	}
	a++;
	TabIndex[i]=TabIndex[index];
      }
    }
  }
  return(a);
}

//=======================================================================
//function : AddLine
//purpose  : 
//=======================================================================

static void AddLine(const Handle(IntPatch_Line)& L,
	     const Standard_Integer i,
	     const Standard_Integer j,
//	     const GeomAbs_SurfaceType TypeS1,
	     const GeomAbs_SurfaceType ,
//	     const GeomAbs_SurfaceType TypeS2,
	     const GeomAbs_SurfaceType ,
	     Standard_Integer *TabIndex,
	     IntPatch_SequenceOfLine& slin) { 
  Standard_Integer IndexFirstVertex = 1;
  Standard_Integer IndexLastVertex  = 2;
  if(i==j) { 
    IndexLastVertex  = 1;
  }
  IntPatch_IType typl = L->ArcType();
  switch (typl) {
    case IntPatch_Analytic: { 
      Handle(IntPatch_ALine) ALine (Handle(IntPatch_ALine)::DownCast (L));
      Handle(IntPatch_ALine) alig;
      if(L->TransitionOnS1() == IntSurf_Undecided)
	alig = new IntPatch_ALine(ALine->Curve(),L->IsTangent());
      else if(L->TransitionOnS1() == IntSurf_Touch)
	alig = new IntPatch_ALine(ALine->Curve(),L->IsTangent(),L->SituationS1(),L->SituationS2());
      else
	alig = new IntPatch_ALine(ALine->Curve(),L->IsTangent(),L->TransitionOnS1(),L->TransitionOnS2());
      alig->AddVertex(ALine->Vertex(i));
      IndexLastVertex+=AppendSameVertexA(alig,ALine,i,TabIndex);
      if(i!=j) { 
	alig->AddVertex(ALine->Vertex(j));
	IndexLastVertex+=AppendSameVertexA(alig,ALine,j,TabIndex);
      }
      alig->SetFirstPoint(IndexFirstVertex);
      alig->SetLastPoint(IndexLastVertex);
      slin.Append(alig);
      break;
    }
    case IntPatch_Walking: {  //-- ****************************************
      Handle(IntPatch_WLine) WLine (Handle(IntPatch_WLine)::DownCast (L));
      const Handle(IntSurf_LineOn2S)& Lori = WLine->Curve();
      Handle(IntSurf_LineOn2S) LineOn2S = new IntSurf_LineOn2S();
      Standard_Integer ParamMinOnLine = (Standard_Integer) WLine->Vertex(i).ParameterOnLine();   
      Standard_Integer ParamMaxOnLine = (Standard_Integer) WLine->Vertex(j).ParameterOnLine();
      for(Standard_Integer k=ParamMinOnLine; k<=ParamMaxOnLine; k++) { 
	LineOn2S->Add(Lori->Value(k));
      }
      Handle(IntPatch_WLine) wlig;
      if(L->TransitionOnS1() == IntSurf_Undecided)
	wlig = new IntPatch_WLine(LineOn2S,L->IsTangent());
      else if(L->TransitionOnS1() == IntSurf_Touch)
	wlig = new IntPatch_WLine(LineOn2S,L->IsTangent(),L->SituationS1(),L->SituationS2());
      else
	wlig = new IntPatch_WLine(LineOn2S,L->IsTangent(),L->TransitionOnS1(),L->TransitionOnS2());
      if(WLine->HasArcOnS1()) { 
	wlig->SetArcOnS1(WLine->GetArcOnS1());
      }
      if(WLine->HasArcOnS2()) { 
	wlig->SetArcOnS2(WLine->GetArcOnS2());
      }
      IntPatch_Point Vtx=WLine->Vertex(i);
      Vtx.SetParameter(1);
      wlig->AddVertex(Vtx);
      IndexLastVertex+=AppendSameVertexW(wlig,WLine,i,1,TabIndex);
      if(i!=j) { 
	Vtx=WLine->Vertex(j);
	Vtx.SetParameter(LineOn2S->NbPoints());
	wlig->AddVertex(Vtx);
	IndexLastVertex+=AppendSameVertexW(wlig,WLine,j,LineOn2S->NbPoints(),TabIndex);
      }
      wlig->SetFirstPoint(IndexFirstVertex);
      wlig->SetLastPoint(IndexLastVertex);
      wlig->SetPeriod(WLine->U1Period(),WLine->V1Period(),WLine->U2Period(),WLine->V2Period());
      wlig->ComputeVertexParameters(Precision::Confusion());
      slin.Append(wlig);
      //-- **********************************************************************

      break;
    }
    case IntPatch_Restriction: { 
      Handle(IntPatch_RLine) RLine (Handle(IntPatch_RLine)::DownCast (L));
      IndexLastVertex=2;
      IndexFirstVertex=1;
      Handle(IntPatch_RLine) rlig;
      if(L->TransitionOnS1() == IntSurf_Undecided)
	rlig = new IntPatch_RLine(L->IsTangent());
      else if(L->TransitionOnS1() == IntSurf_Touch)
	rlig = new IntPatch_RLine(L->IsTangent(),L->SituationS1(),L->SituationS2());
      else
	rlig = new IntPatch_RLine(L->IsTangent(),L->TransitionOnS1(),L->TransitionOnS2());
      if(RLine->IsArcOnS1()) { rlig->SetArcOnS1(RLine->ArcOnS1()); } 
      if(RLine->IsArcOnS2()) { rlig->SetArcOnS2(RLine->ArcOnS2()); } 

      rlig->AddVertex(RLine->Vertex(i));     
#if XPU1009
      IndexLastVertex+=AppendSameVertexR(rlig,RLine,i,TabIndex);
#endif
      for(Standard_Integer k=i+1; k<j;k++) { 
	rlig->AddVertex(RLine->Vertex(k));
	IndexLastVertex++;
      }
      if(i!=j) { 
	rlig->AddVertex(RLine->Vertex(j)); 
#if XPU1009	
	IndexLastVertex+=AppendSameVertexR(rlig,RLine,j,TabIndex);
#endif
      }
      rlig->SetFirstPoint(IndexFirstVertex);
      rlig->SetLastPoint(IndexLastVertex);
      rlig->ComputeVertexParameters(Precision::Confusion());
      slin.Append(rlig);
      break;
    }
    case IntPatch_Lin:
    case IntPatch_Parabola:
    case IntPatch_Hyperbola:
    case IntPatch_Circle:
    case IntPatch_Ellipse: { 
      Handle(IntPatch_GLine) GLine (Handle(IntPatch_GLine)::DownCast (L));
      Handle(IntPatch_GLine) glig;
      switch (typl) {
      case IntPatch_Lin:
	if(L->TransitionOnS1() == IntSurf_Undecided)
	  glig = new IntPatch_GLine(GLine->Line(),L->IsTangent());
	else if(L->TransitionOnS1() == IntSurf_Touch)
	  glig = new IntPatch_GLine(GLine->Line(),L->IsTangent(),L->SituationS1(),L->SituationS2());
	else
	  glig = new IntPatch_GLine(GLine->Line(),L->IsTangent(),L->TransitionOnS1(),L->TransitionOnS2());
	break;
      case IntPatch_Parabola:
	if(L->TransitionOnS1() == IntSurf_Undecided)
	  glig = new IntPatch_GLine(GLine->Parabola(),L->IsTangent());
	else if(L->TransitionOnS1() == IntSurf_Touch)
	  glig = new IntPatch_GLine(GLine->Parabola(),L->IsTangent(),L->SituationS1(),L->SituationS2());
	else
	  glig = new IntPatch_GLine(GLine->Parabola(),L->IsTangent(),L->TransitionOnS1(),L->TransitionOnS2());
	break;
      case IntPatch_Hyperbola:
	if(L->TransitionOnS1() == IntSurf_Undecided)
	  glig = new IntPatch_GLine(GLine->Hyperbola(),L->IsTangent());
	else if(L->TransitionOnS1() == IntSurf_Touch)
	  glig = new IntPatch_GLine(GLine->Hyperbola(),L->IsTangent(),L->SituationS1(),L->SituationS2());
	else
	  glig = new IntPatch_GLine(GLine->Hyperbola(),L->IsTangent(),L->TransitionOnS1(),L->TransitionOnS2());
	break;
      case IntPatch_Circle:
	if(L->TransitionOnS1() == IntSurf_Undecided)
	  glig = new IntPatch_GLine(GLine->Circle(),L->IsTangent());
	else if(L->TransitionOnS1() == IntSurf_Touch)
	  glig = new IntPatch_GLine(GLine->Circle(),L->IsTangent(),L->SituationS1(),L->SituationS2());
	else
	  glig = new IntPatch_GLine(GLine->Circle(),L->IsTangent(),L->TransitionOnS1(),L->TransitionOnS2());
	break;
      case IntPatch_Ellipse: default:
	if(L->TransitionOnS1() == IntSurf_Undecided)
	  glig = new IntPatch_GLine(GLine->Ellipse(),L->IsTangent());
	else if(L->TransitionOnS1() == IntSurf_Touch)
	  glig = new IntPatch_GLine(GLine->Ellipse(),L->IsTangent(),L->SituationS1(),L->SituationS2());
	else
	  glig = new IntPatch_GLine(GLine->Ellipse(),L->IsTangent(),L->TransitionOnS1(),L->TransitionOnS2());
	break;
      }
      glig->AddVertex(GLine->Vertex(i));
      IndexLastVertex+=AppendSameVertexG(glig,GLine,i,0,TabIndex);
      if(i!=j) { 
	if ((typl == IntPatch_Circle || typl == IntPatch_Ellipse) && i>j) {
	  IntPatch_Point Vtx=GLine->Vertex(j);
	  Vtx.SetParameter(GLine->Vertex(j).ParameterOnLine()+M_PI+M_PI);
	  glig->AddVertex(Vtx);
	  IndexLastVertex+=AppendSameVertexG(glig,GLine,j,M_PI+M_PI,TabIndex);
	}
	else {
	  glig->AddVertex(GLine->Vertex(j));
	  IndexLastVertex+=AppendSameVertexG(glig,GLine,j,0,TabIndex);
	}
      }
      glig->SetFirstPoint(IndexFirstVertex);
      glig->SetLastPoint(IndexLastVertex);
      slin.Append(glig);
      break;
    }
    default: { 
      throw Standard_ConstructionError("IntPatch_LineConstructor::AddLine");
    }
      break;
    }
}

//=======================================================================
//function : Line
//purpose  : 
//=======================================================================

Handle(IntPatch_Line) IntPatch_LineConstructor::Line(const Standard_Integer l) const { 
  return(slin.Value(l));
}

//=======================================================================
//function : NbLines
//purpose  : 
//=======================================================================

Standard_Integer IntPatch_LineConstructor::NbLines() const { 
  return(slin.Length());
}

//=======================================================================
//function : GetVertexTolerance
//purpose  : 
//=======================================================================

static Standard_Real GetVertexTolerance(const IntPatch_Point& vtx/*,
					const Handle(Adaptor3d_TopolTool)& aDomain1,
					const Handle(Adaptor3d_TopolTool)& aDomain2*/)
{
  Standard_Real tol = vtx.Tolerance();
//    if (aDomain1->Has3d() && vtx.IsVertexOnS1()) {
//      Standard_Real tolv = aDomain1->Tol3d(vtx.VertexOnS1());
//      if (tolv > tol) tol = tolv;
//    }
//    if (aDomain2->Has3d() && vtx.IsVertexOnS2()) {
//      Standard_Real tolv = aDomain2->Tol3d(vtx.VertexOnS2());
//      if (tolv > tol) tol = tolv;
//    }
  return tol;
}

//=======================================================================
//function : IsSegmentSmall
//purpose  : 
//=======================================================================

static Standard_Boolean IsSegmentSmall(const Handle(IntPatch_WLine)& WLine,
				       const Standard_Integer ivFirst,
				       const Standard_Integer ivLast/*,
				       const Standard_Real TolArc*/)
{
  const IntPatch_Point& vtxF = WLine->Vertex(ivFirst);
  const IntPatch_Point& vtxL = WLine->Vertex(ivLast);
  Standard_Integer ipF = (Standard_Integer) vtxF.ParameterOnLine();
  Standard_Integer ipL = (Standard_Integer) vtxL.ParameterOnLine();
  if (ipF >= ipL) return Standard_True;

  Standard_Real tolF = GetVertexTolerance(vtxF);
  Standard_Real tolL = GetVertexTolerance(vtxL);
  Standard_Real tol = Max (tolF, tolL);

  Standard_Real len = 0.;
  gp_Pnt p1 = WLine->Point(ipF).Value();
  for (Standard_Integer i=ipF+1; i <= ipL; i++) {
    const gp_Pnt& p2 = WLine->Point(i).Value();
    len += p1.Distance(p2);
    if (len > tol) break;
    p1 = p2;
  }
  return len <= tol;
}

//=======================================================================
//function : TestWLineIsARLine
//purpose  : 
//=======================================================================

static Standard_Boolean TestWLineIsARLine(const IntPatch_SequenceOfLine& slinref,
					  const Handle(IntPatch_WLine)& wlin,
					  const Standard_Real tol2d) {
  int nbpnt=wlin->NbPnts();
  int indicepnt=nbpnt/2;
  if(indicepnt<1) return(Standard_False);
  const IntSurf_PntOn2S& POn2S=wlin->Point(indicepnt);
  const IntSurf_PntOn2S& POn2S1=wlin->Point(indicepnt+1);
  Standard_Integer lastl=slinref.Length();
  for(int i=1;i<=lastl;i++) { 
    if(slinref.Value(i)->ArcType()==IntPatch_Restriction) { 
      Handle(IntPatch_RLine)& rlin = *((Handle(IntPatch_RLine) *)&(slinref(i)));
      for (Standard_Integer is=0; is<2; is++) {
        Standard_Boolean onFirst = is==0;
        if((onFirst && rlin->IsArcOnS1()) || (!onFirst && rlin->IsArcOnS2())) {
          Handle(Adaptor2d_HCurve2d) arc;
          Standard_Real u,v,u1,v1;
          if (onFirst) {
            arc = rlin->ArcOnS1();
            POn2S.ParametersOnS1(u,v);
            POn2S1.ParametersOnS1(u1,v1);
          }
          else {
            arc = rlin->ArcOnS2();
            POn2S.ParametersOnS2(u,v);
            POn2S1.ParametersOnS2(u1,v1);
          }
          if (indicepnt == 1) {
            u = (u+u1)*0.5;
            v = (v+v1)*0.5;
          }
          const Adaptor2d_Curve2d& C2d=arc->Curve2d();
          gp_Pnt2d PObt,P2d(u,v);
          Standard_Real par= Geom2dInt_TheProjPCurOfGInter::FindParameter(C2d,P2d,1e-7);
          PObt=C2d.Value(par);
          if(PObt.Distance(P2d) < tol2d) {
            return Standard_True;
          }
        }
      }
    }
  }
  return Standard_False;
}

//=======================================================================
//function : TestIfWLineIsRestriction
//purpose  : 
//=======================================================================

static Standard_Boolean TestIfWLineIsRestriction(const IntPatch_SequenceOfLine& slinref,
					  const Handle(IntPatch_WLine)& wlin,
					  const Handle(Adaptor3d_HSurface)& S1,
					  const Handle(Adaptor3d_TopolTool)&D1,
					  const Handle(Adaptor3d_HSurface)& S2,
					  const Handle(Adaptor3d_TopolTool)&D2,
					  Standard_Real TolArc) { 

  Standard_Integer NbPnts = wlin->NbPnts();
  Standard_Integer allon1=0,allon2=0,i;
  Standard_Real tol2d1=0., tol2d2=0.;
  for(i=1;i<=NbPnts;i++) { 
    const IntSurf_PntOn2S& Pmid = wlin->Point(i);
    Standard_Real u1,v1,u2,v2;
    Pmid.Parameters(u1,v1,u2,v2);
    //-- Estimation d un majorant de Toluv a partir de Tol
    gp_Pnt ap;
    gp_Vec ad1u,ad1v;
    Standard_Real tol;
    //------------------------------------------
    S1->D1(u1,v1,ap,ad1u,ad1v);
    tol = ComputeParametricTolerance(TolArc,ad1u,ad1v);
    if (tol > tol2d1) tol2d1 = tol;
    //--
    if(allon1+1 == i && D1->IsThePointOn(gp_Pnt2d(u1,v1),tol)) {
      allon1++;
    }
    //------------------------------------------
    S2->D1(u2,v2,ap,ad1u,ad1v);
    tol = ComputeParametricTolerance(TolArc,ad1u,ad1v);
    if (tol > tol2d2) tol2d2 = tol;
    //--
    if(allon2+1 == i && D2->IsThePointOn(gp_Pnt2d(u2,v2),tol)) { 
      allon2++;
    }
    if(allon1!=i && allon2!=i) 
      break;
  }
  if(allon1==NbPnts || allon2==NbPnts) {
#ifdef OCCT_DEBUG
    cout<<" IntPatch_LineConstructor.gxx :  CC**ONS"<<(allon1==NbPnts?1:2)<<"** Traitement WLIne + ARC CLASS "<<endl;
#endif
    Standard_Real tol2d = Max(tol2d1,tol2d2);
    return TestWLineIsARLine(slinref,wlin,tol2d);
  }
  return Standard_False;
}

//=======================================================================
//function : ProjectOnArc
//purpose  : 
//=======================================================================

static Standard_Boolean ProjectOnArc(const Standard_Real u,
				     const Standard_Real v,
				     const Handle(Adaptor2d_HCurve2d)& arc,
				     const Handle(Adaptor3d_HSurface)& surf,
				     const Standard_Real TolArc,
				     Standard_Real& par,
				     Standard_Real& dist)
{
  gp_Pnt aPbid;
  gp_Vec ad1u, ad1v;
  surf->D1(u,v,aPbid,ad1u,ad1v);
  Standard_Real tol2d = ComputeParametricTolerance(TolArc,ad1u,ad1v);
  const Adaptor2d_Curve2d& C2d=arc->Curve2d();
  gp_Pnt2d aP(u,v),aPprj;
  par=Geom2dInt_TheProjPCurOfGInter::FindParameter(C2d,aP,1e-7);
  aPprj=C2d.Value(par);
  dist = aPprj.Distance(aP);
  return dist <= tol2d;
}

//=======================================================================
//function : TestWLineToRLine
//purpose  : 
//=======================================================================

static void TestWLineToRLine(const IntPatch_SequenceOfLine& slinref,
		      IntPatch_SequenceOfLine& slin,
		      const Handle(Adaptor3d_HSurface)& mySurf1,
		      const Handle(Adaptor3d_TopolTool)& myDom1,
		      const Handle(Adaptor3d_HSurface)& mySurf2,
		      const Handle(Adaptor3d_TopolTool)& myDom2,
		      const Standard_Real TolArc) { 

  Standard_Integer lastwline=slin.Length();
  Handle(IntPatch_WLine)& WLine = *((Handle(IntPatch_WLine) *)& (slin.Value(lastwline)));

  Standard_Integer nbvtx=WLine->NbVertex();
  if (nbvtx < 2) return;
  Standard_Integer ParamMinOnLine = (Standard_Integer) WLine->Vertex(1).ParameterOnLine();
  Standard_Integer ParamMaxOnLine = (Standard_Integer) WLine->Vertex(nbvtx).ParameterOnLine();
  if (ParamMinOnLine >= ParamMaxOnLine) return;
  Standard_Integer midInd = (ParamMaxOnLine + ParamMinOnLine) / 2;

  TColStd_SequenceOfInteger indicesV1,indicesV2;
  Standard_Integer iv;
  for (iv=1; iv <= nbvtx; iv++) {
    Standard_Integer plin = (Standard_Integer) WLine->Vertex(iv).ParameterOnLine();
    if (plin == ParamMinOnLine) indicesV1.Append(iv);
    else if (plin == ParamMaxOnLine) indicesV2.Append(iv);
  }

  Standard_Boolean isRLine = Standard_False;

  typedef void (IntSurf_PntOn2S::* PiParOnS)(Standard_Real&,Standard_Real&) const;
  typedef Standard_Boolean (IntPatch_Point::* PQuery)() const;
  typedef const Handle(Adaptor2d_HCurve2d)& (IntPatch_Point::* PArcOnS)() const;
  typedef Standard_Real (IntPatch_Point::* PParOnArc)() const;

  // cycle for both surfaces
  Standard_Integer is;
  for (is=0; is<2; is++) {
    Standard_Boolean onFirst = is==0;
    if(( onFirst && WLine->HasArcOnS1()) ||
       (!onFirst && WLine->HasArcOnS2())) {
      PiParOnS piParOnS;
      PQuery pIsOnDomS;
      PArcOnS pArcOnS;
      PParOnArc pParOnArc;
      Handle(Adaptor3d_HSurface) surf;
      Handle(Adaptor3d_TopolTool) aDomain;
      if (onFirst) {
	piParOnS = &IntSurf_PntOn2S::ParametersOnS1;
	pIsOnDomS = &IntPatch_Point::IsOnDomS1;
	pArcOnS = &IntPatch_Point::ArcOnS1;
	pParOnArc = &IntPatch_Point::ParameterOnArc1;
	surf = mySurf1;
	aDomain = myDom1;
      }
      else {
	piParOnS = &IntSurf_PntOn2S::ParametersOnS2;
	pIsOnDomS = &IntPatch_Point::IsOnDomS2;
	pArcOnS = &IntPatch_Point::ArcOnS2;
	pParOnArc = &IntPatch_Point::ParameterOnArc2;
	surf = mySurf2;
	aDomain = myDom2;
      }

      // resolve arcs for vertices not having a link to an arc
      Standard_Real utst,vtst;
      TColStd_Array1OfReal paramsResolved(1,nbvtx);
      TColStd_Array1OfTransient arcsResolved(1,nbvtx);
      arcsResolved.Init(Handle(Adaptor2d_HCurve2d)());
      for (iv=1; iv <= nbvtx; iv++) {
	if (!(WLine->Vertex(iv).*pIsOnDomS)()) {
	  Standard_Integer ip = (Standard_Integer) WLine->Vertex(iv).ParameterOnLine();
	  (WLine->Point(ip).*piParOnS)(utst,vtst);
	  Standard_Real distmin=RealLast();
	  for (aDomain->Init(); aDomain->More(); aDomain->Next()) {
	    const Handle(Adaptor2d_HCurve2d)& arc = aDomain->Value();
	    Standard_Real par,dist;
	    if (ProjectOnArc(utst,vtst,arc,surf,TolArc,par,dist) && dist < distmin) {
	      arcsResolved(iv) = arc;
	      paramsResolved(iv) = par;
	      distmin = dist;
	    }
	  }
	}
      }

      // prepare list of common arcs for both ends of wline
      TColStd_IndexedMapOfTransient mapArcsV1,mapArcs;
      Standard_Integer i;
      for (i=1; i <= indicesV1.Length(); i++) {
	iv = indicesV1(i);
	Handle(Adaptor2d_HCurve2d) arc;
	if ((WLine->Vertex(iv).*pIsOnDomS)()) arc = (WLine->Vertex(iv).*pArcOnS)();
	else arc = Handle(Adaptor2d_HCurve2d)::DownCast (arcsResolved(iv));
	if (!arc.IsNull()) mapArcsV1.Add(arc);
      }
      for (i=1; i <= indicesV2.Length(); i++) {
	iv = indicesV2(i);
	Handle(Adaptor2d_HCurve2d) arc;
	if ((WLine->Vertex(iv).*pIsOnDomS)()) arc = (WLine->Vertex(iv).*pArcOnS)();
	else arc = Handle(Adaptor2d_HCurve2d)::DownCast (arcsResolved(iv));
	if (!arc.IsNull() && mapArcsV1.Contains(arc)) mapArcs.Add(arc);
      }

      // for each common arc
      for (Standard_Integer ia=1; ia <= mapArcs.Extent(); ia++) {
	const Handle(Adaptor2d_HCurve2d) arc (Handle(Adaptor2d_HCurve2d)::DownCast (mapArcs(ia)));
	// get end vertices of wline linked with this arc
	Standard_Integer iv1=0,iv2=0;
	for (i=1; i <= indicesV1.Length() && iv1==0; i++) {
	  iv = indicesV1(i);
	  Handle(Adaptor2d_HCurve2d) arc1;
	  if ((WLine->Vertex(iv).*pIsOnDomS)()) arc1 = (WLine->Vertex(iv).*pArcOnS)();
	  else arc1 = Handle(Adaptor2d_HCurve2d)::DownCast (arcsResolved(iv));
	  if (!arc1.IsNull() && arc1 == arc) iv1 = iv;
	}
	for (i=1; i <= indicesV2.Length() && iv2==0; i++) {
	  iv = indicesV2(i);
	  Handle(Adaptor2d_HCurve2d) arc1;
	  if ((WLine->Vertex(iv).*pIsOnDomS)()) arc1 = (WLine->Vertex(iv).*pArcOnS)();
	  else arc1 = Handle(Adaptor2d_HCurve2d)::DownCast (arcsResolved(iv));
	  if (!arc1.IsNull() && arc1 == arc) iv2 = iv;
	}
	if (!iv1 || !iv2) {
#ifdef OCCT_DEBUG
	  cout<<" Pb getting vertices linked with arc"<<endl;
#endif
	  continue;
	}
	Standard_Real par1 = (arcsResolved(iv1).IsNull()
			      ? (WLine->Vertex(iv1).*pParOnArc)()
			      : paramsResolved(iv1));
	Standard_Real par2 = (arcsResolved(iv2).IsNull()
			      ? (WLine->Vertex(iv2).*pParOnArc)()
			      : paramsResolved(iv2));
#ifdef OCCT_DEBUG
	cout<<"****** Parameters on arc on S"<<is+1<<": "<<par1<<" "<<par2<<endl;
#endif

	// check that the middle point is on arc
	(WLine->Point(midInd).*piParOnS)(utst,vtst);
	if (midInd == ParamMinOnLine) {
	  Standard_Real utst1=0.0,vtst1=0.0;
	  (WLine->Point(midInd+1).*piParOnS)(utst1,vtst1);
	  utst = (utst+utst1)*0.5;
	  vtst = (vtst+vtst1)*0.5;
	}
	Standard_Real par,dist;
	if (!ProjectOnArc(utst,vtst,arc,surf,TolArc,par,dist)) {
#ifdef OCCT_DEBUG
	  cout<<" Pb en projection ds IntPatch_LineConstructor"<<endl;
#endif
	  continue;
	}

	//-- codage de la WLine en RLine 
	Handle(IntPatch_RLine) rlig = new IntPatch_RLine(Standard_True,IntSurf_Unknown,IntSurf_Unknown);
	if (onFirst) rlig->SetArcOnS1(arc);
	else         rlig->SetArcOnS2(arc);

	Handle(IntSurf_LineOn2S) LineOn2S = new IntSurf_LineOn2S();
	const Handle(IntSurf_LineOn2S)& Lori = WLine->Curve();
	Standard_Integer ivmin,ivmax;
	Standard_Real parmin, parmax;
	Standard_Boolean reverse = Standard_False;
	TColStd_SequenceOfInteger *pIndVmin, *pIndVmax;
	if (par1<par2) {
	  for(i=ParamMinOnLine; i<=ParamMaxOnLine; i++) { 
	    LineOn2S->Add(Lori->Value(i));
	  }
	  ivmin = iv1; ivmax = iv2;
	  parmin = par1; parmax = par2;
	  pIndVmin = &indicesV1; pIndVmax = &indicesV2;
	}
	else { 
	  for(i=ParamMaxOnLine; i>=ParamMinOnLine; i--) {
	    LineOn2S->Add(Lori->Value(i));
	  }
	  ivmin = iv2; ivmax = iv1;
	  parmin = par2; parmax = par1;
	  pIndVmin = &indicesV2; pIndVmax = &indicesV1;
	  reverse = Standard_True;
	}
	rlig->Add(LineOn2S);
	IntSurf_Transition TransitionUndecided;
	IntPatch_Point VtxFirst = WLine->Vertex(ivmin);
	VtxFirst.SetParameter(parmin);
	if (!arcsResolved(ivmin).IsNull())
	  VtxFirst.SetArc(onFirst,arc,parmin,TransitionUndecided,TransitionUndecided);
	if (reverse)
	  VtxFirst.ReverseTransition();  //-- inversion des transitions
	rlig->AddVertex(VtxFirst);
	for (i=1; i <= pIndVmin->Length(); i++) {
	  iv = pIndVmin->Value(i);
	  if (iv != ivmin) {
	    IntPatch_Point Vtx=WLine->Vertex(iv);
	    Vtx.SetParameter(parmin);
	    if (!arcsResolved(iv).IsNull())
	      Vtx.SetArc(onFirst,arc,parmin,TransitionUndecided,TransitionUndecided);
	    if (reverse)
	      Vtx.ReverseTransition();
	    rlig->AddVertex(Vtx);
	  }
	}
	for (i=1; i <= pIndVmax->Length(); i++) {
	  iv = pIndVmax->Value(i);
	  if (iv != ivmax) {
	    IntPatch_Point Vtx=WLine->Vertex(iv);
	    Vtx.SetParameter(parmax);
	    if (!arcsResolved(iv).IsNull())
	      Vtx.SetArc(onFirst,arc,parmax,TransitionUndecided,TransitionUndecided);
	    if (reverse)
	      Vtx.ReverseTransition();
	    rlig->AddVertex(Vtx);
	  }
	}
	IntPatch_Point VtxLast=WLine->Vertex(ivmax);
	VtxLast.SetParameter(parmax);
	if (!arcsResolved(ivmax).IsNull())
	  VtxLast.SetArc(onFirst,arc,parmax,TransitionUndecided,TransitionUndecided);
	if (reverse)
	  VtxLast.ReverseTransition();
	rlig->AddVertex(VtxLast);
	rlig->SetFirstPoint(1);
	rlig->SetLastPoint(indicesV1.Length()+indicesV2.Length());
	slin.Append(rlig);
	isRLine = Standard_True;
      }
    }
  }

  if(isRLine ||
     TestIfWLineIsRestriction(slinref,WLine,
			      mySurf1,myDom1,
			      mySurf2,myDom2,
			      TolArc)) {
    slin.Remove(lastwline);
  }
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void IntPatch_LineConstructor::Perform(const IntPatch_SequenceOfLine& slinref,
				       const Handle(IntPatch_Line)& L,
				       const Handle(Adaptor3d_HSurface)& mySurf1,
				       const Handle(Adaptor3d_TopolTool)& myDom1,
				       const Handle(Adaptor3d_HSurface)& mySurf2,
				       const Handle(Adaptor3d_TopolTool)& myDom2,
				       const Standard_Real TolArc)  {

  Standard_Integer i=1,nbvtx;
  Standard_Real firstp,lastp;
  Standard_Real Tol = Precision::PConfusion()*100.; // JMB le 13 Jan 2000. Report de la correction du PRO19653
  GeomAbs_SurfaceType typs1 = mySurf1->GetType();
  GeomAbs_SurfaceType typs2 = mySurf2->GetType();

  IntPatch_IType typl = L->ArcType();
  if(typl == IntPatch_Analytic) { 
    Standard_Real u1,v1,u2,v2;
    Handle(IntPatch_ALine) ALine (Handle(IntPatch_ALine)::DownCast (L));
    slin.Clear();
    nbvtx = ALine->NbVertex();
    //-- -------------------------------------------------------------------
    Standard_Integer *TabIndex=new Standard_Integer [nbvtx+2];
    Standard_Integer numline=0;
    for(i=1;i<=nbvtx;i++) { 
    //for(Standard_Integer i=1;i<=nbvtx;i++) { 
      TabIndex[i]=0; 
    }
    //-- -------------------------------------------------------------------
    for(i=1;i<nbvtx;i++) { 
      const IntPatch_Point& ALine_Vertex_i  =ALine->Vertex(i);
      const IntPatch_Point& ALine_Vertex_ip1=ALine->Vertex(i+1);
      firstp = ALine_Vertex_i.ParameterOnLine();
      lastp =  ALine_Vertex_ip1.ParameterOnLine();
      if(firstp!=lastp) { 
	Standard_Real pmid = (firstp+lastp)*0.5;
	gp_Pnt Pmid = ALine->Value(pmid);
	Parameters(mySurf1,mySurf2,Pmid,u1,v1,u2,v2);
	Recadre(mySurf1,mySurf2,u1,v1,u2,v2);
	TopAbs_State in1,in2;
	in1 = myDom1->Classify(gp_Pnt2d(u1,v1),Tol,Standard_False);
	in2 = (in1!=TopAbs_OUT)? myDom2->Classify(gp_Pnt2d(u2,v2),Tol,Standard_False) : TopAbs_OUT;
	if(in1 == TopAbs_OUT || in2 == TopAbs_OUT) { 
	}
	else { 
	  //-- cout<<"Analytic   : firtsp="<<firstp<<" lastp="<<lastp<<" Vtx:"<<i<<","<<i+1<<endl;	  
	  TabIndex[i]=TabIndex[i+1]=++numline;
	  AddLine(L,i,i+1,typs1,typs2,TabIndex,slin);
	}
      }
    }
    //-- -------------------------------------------------------------------
    //-- On recherche les vertex interference Edge Edge Externe 
    //-- Ces vertex ne figurent sur aucune ligne et sont Restriction 
    //-- sur les 2 edges
    for(i=1;i<=nbvtx;i++) { 
      if(TabIndex[i]==0) { 
	const IntPatch_Point& ALine_Vertex_i  =ALine->Vertex(i);
	if(ALine_Vertex_i.IsOnDomS1() && ALine_Vertex_i.IsOnDomS2()) { 
	  TabIndex[i]=++numline;
	  AddLine(L,i,i,typs1,typs2,TabIndex,slin);	  
	}
      }
    }
    delete [] TabIndex;
    //-- -------------------------------------------------------------------
    return;
  }
  else if(typl == IntPatch_Walking) { 
    Standard_Real u1,v1,u2,v2;
    Handle(IntPatch_WLine) WLine (Handle(IntPatch_WLine)::DownCast (L));
    slin.Clear();
    nbvtx = WLine->NbVertex();
    //-- -------------------------------------------------------------------
    Standard_Integer *TabIndex=new Standard_Integer [nbvtx+2];
    Standard_Integer numline=0;
    for(i=1;i<=nbvtx;i++)  {
    //for(Standard_Integer i=1;i<=nbvtx;i++)  {
      TabIndex[i]=0;
    }
    //-- -------------------------------------------------------------------
    for(i=1;i<nbvtx;i++) { 
      const IntPatch_Point& WLineVertex_i   =  WLine->Vertex(i);
      const IntPatch_Point& WLineVertex_ip1 =  WLine->Vertex(i+1);
      firstp = WLineVertex_i.ParameterOnLine();
      lastp =  WLineVertex_ip1.ParameterOnLine();
      if(firstp!=lastp && !IsSegmentSmall(WLine,i,i+1/*,TolArc*/)) {  
	Standard_Integer pmid;
	pmid = (Standard_Integer)((firstp+lastp)/2);
	Standard_Integer int_lastp = (Standard_Integer)lastp;
	Standard_Integer int_firstp = (Standard_Integer)firstp;
	if(pmid==int_lastp) pmid=int_firstp;
	const IntSurf_PntOn2S& Pmid = WLine->Point(pmid);
	Pmid.Parameters(u1,v1,u2,v2);
	Recadre(mySurf1,mySurf2,u1,v1,u2,v2);
	
	// modified by NIZHNY-MKK  Tue Apr  3 15:03:40 2001.BEGIN
	//------------------------------------------	
	gp_Pnt ap;
	gp_Vec ad1u,ad1v;
	mySurf1->D1(u1,v1,ap,ad1u,ad1v);
	Standard_Real aTolerance = ComputeParametricTolerance(TolArc, ad1u, ad1v);
	//------------------------------------------

	//TopAbs_State in1 = myDom1->Classify(gp_Pnt2d(u1,v1),Tol,Standard_False);
	TopAbs_State in1 = myDom1->Classify(gp_Pnt2d(u1,v1), aTolerance, Standard_False);
	//TopAbs_State in2 = (in1!=TopAbs_OUT)? myDom2->Classify(gp_Pnt2d(u2,v2),Tol,Standard_False) : TopAbs_OUT;
	TopAbs_State in2 = TopAbs_OUT;
	if (in1!=TopAbs_OUT) {
	  //------------------------------------------
	  mySurf2->D1(u2,v2,ap,ad1u,ad1v);
	  aTolerance = ComputeParametricTolerance(TolArc, ad1u, ad1v);
	  //------------------------------------------
	  in2 = myDom2->Classify(gp_Pnt2d(u2,v2), aTolerance, Standard_False);
	}
	// modified by NIZHNY-MKK  Tue Apr  3 15:06:31 2001.END
	
	// modified by NIZHNY-OFV  Wed Jun 13 17:31:23 2001
	// --purpose: If on a face (lastp-firstp) == 1,
	//            sometimes it could mean a bad parametrisation of WLine.
	//            In this case we try to classify the "virtual" WLine point:
	//            the geometrical point between two vertexes. This emulates
	//            situation when (lastp-firstp) != 1.
	if(Abs(int_lastp-int_firstp) == 1)
	  {
	    Standard_Real vFu1,vFv1,vFu2,vFv2,vLu1,vLv1,vLu2,vLv2;
	    const IntSurf_PntOn2S& vF = WLineVertex_i. PntOn2S();
	    const IntSurf_PntOn2S& vL = WLineVertex_ip1. PntOn2S();
	    vF.Parameters(vFu1,vFv1,vFu2,vFv2);
	    Recadre(mySurf1,mySurf2,vFu1,vFv1,vFu2,vFv2);
	    vL.Parameters(vLu1,vLv1,vLu2,vLv2);
	    Recadre(mySurf1,mySurf2,vLu1,vLv1,vLu2,vLv2);
	    if(in1 != TopAbs_IN)
	      {
		Standard_Real du,dv;
		gp_Pnt2d pvF(vFu1,vFv1);
		gp_Pnt2d pvL(vLu1,vLv1);
		gp_Pnt2d pPm(u1,v1);
		Standard_Real dpvFpPm = pvF.Distance(pPm);
		Standard_Real dpvLpPm = pvL.Distance(pPm);
		if(dpvFpPm > dpvLpPm)
		  {
		    du = (vFu1 + u1) * 0.5;
		    dv = (vFv1 + v1) * 0.5;
		  }
		else
		  {
		    du = (vLu1 + u1) * 0.5;
		    dv = (vLv1 + v1) * 0.5;
		  }
		mySurf1->D1(du,dv,ap,ad1u,ad1v);
		aTolerance = ComputeParametricTolerance(TolArc, ad1u, ad1v);
		in1 = myDom1->Classify(gp_Pnt2d(du,dv), aTolerance, Standard_False);
	      }
	    if(in2 != TopAbs_IN)
	      {
		Standard_Real du,dv;
		gp_Pnt2d pvF(vFu2,vFv2);
		gp_Pnt2d pvL(vLu2,vLv2);
		gp_Pnt2d pPm(u2,v2);
		Standard_Real dpvFpPm = pvF.Distance(pPm);
		Standard_Real dpvLpPm = pvL.Distance(pPm);
		if(dpvFpPm > dpvLpPm)
		  {
		    du = (vFu2 + u2) * 0.5;
		    dv = (vFv2 + v2) * 0.5;
		  }
		else
		  {
		    du = (vLu2 + u2) * 0.5;
		    dv = (vLv2 + v2) * 0.5;
		  }
		mySurf2->D1(du,dv,ap,ad1u,ad1v);
		aTolerance = ComputeParametricTolerance(TolArc, ad1u, ad1v);
		in2 = myDom2->Classify(gp_Pnt2d(du,dv), aTolerance, Standard_False);
	      }
	  } //end of if(Abs(int_lastp-int_firstp) == 1)

	if (in1 != TopAbs_OUT && in2 != TopAbs_OUT)
	  {
	    Standard_Boolean   LignetropPetite=Standard_False;
	    Standard_Real u1a,v1a,u2a,v2a;
	    const IntSurf_PntOn2S& Pmid1 = WLine->Point((Standard_Integer)firstp);
	    Pmid1.Parameters(u1a,v1a,u2a,v2a);
	    Recadre(mySurf1,mySurf2,u1a,v1a,u2a,v2a);
	    
	    const IntSurf_PntOn2S& Pmid2 = WLine->Point((Standard_Integer)lastp);
	    Standard_Real u1b,v1b,u2b,v2b;
	    Pmid2.Parameters(u1b,v1b,u2b,v2b);
	    Recadre(mySurf1,mySurf2,u1b,v1b,u2b,v2b);
	    
	    Standard_Real dd12_u=Abs(u1a-u1b); 
	    Standard_Real dd12_v=Abs(v1a-v1b); 
	    if(dd12_u+dd12_v < 1e-12) { 
	      dd12_u=Abs(u1-u1b);
	      dd12_v=Abs(v1-v1b);
	      if(dd12_u+dd12_v < 1e-12) { 
		LignetropPetite=Standard_True;
	      }
	    }
	    if(LignetropPetite==Standard_False) { 	    
	      //-- cout<<"WLine      : firtsp="<<firstp<<" lastp="<<lastp<<" Vtx:"<<i<<","<<i+1<<endl;
	      TabIndex[i]=TabIndex[i+1]=++numline;
	      AddLine(L,i,i+1,typs1,typs2,TabIndex,slin);
	      TestWLineToRLine(slinref,slin,mySurf1,myDom1,mySurf2,myDom2,TolArc); //-- on teste la derniere entree de slin
	    }
	  } //end of if (in1 != TopAbs_OUT && in2 != TopAbs_OUT)
      } //end of if(firstp!=lastp && !IsSegmentSmall(WLine,i,i+1/*,TolArc*/))
    } //end of for(i=1;i<nbvtx;i++)

    //-- -------------------------------------------------------------------
    //-- On recherche les vertex interference Edge Edge Externe 
    //-- Ces vertex ne figurent sur aucune ligne et sont Restriction 
    //-- sur les 2 edges
    for(i=1;i<=nbvtx;i++) { 
      if(TabIndex[i]==0) { 
	const IntPatch_Point& WLine_Vertex_i  =WLine->Vertex(i);
	if(WLine_Vertex_i.IsOnDomS1() && WLine_Vertex_i.IsOnDomS2()) { 
	  TabIndex[i]=++numline;
	  AddLine(L,i,i,typs1,typs2,TabIndex,slin);	  
	}
      }
    }
    delete [] TabIndex;
    //-- -------------------------------------------------------------------
    return;
  }
  else if (typl != IntPatch_Restriction) { // JAG 01.07.96
    Standard_Real u1,v1,u2,v2;
    Handle(IntPatch_GLine) GLine (Handle(IntPatch_GLine)::DownCast (L));
    slin.Clear();
    nbvtx = GLine->NbVertex();
    //-- -------------------------------------------------------------------
    Standard_Integer *TabIndex=new Standard_Integer [nbvtx+2];
    Standard_Integer numline=0;
//    for(Standard_Integer i=1;i<=nbvtx;i++) { 
    for(i=1;i<=nbvtx;i++) { 
     TabIndex[i]=0;
    }
    //-- -------------------------------------------------------------------
    Standard_Boolean intrvtested = Standard_False;
    for(i=1;i<nbvtx;i++) { 
      firstp = GLine->Vertex(i).ParameterOnLine();
      lastp =  GLine->Vertex(i+1).ParameterOnLine();
      if(Abs(firstp-lastp)>Precision::PConfusion()) {
	intrvtested = Standard_True;
	Standard_Real pmid = (firstp+lastp)*0.5;
	gp_Pnt Pmid;
	if (typl == IntPatch_Lin) {
	  Pmid = ElCLib::Value(pmid,GLine->Line());
	}
	else if (typl == IntPatch_Circle) {
	  Pmid = ElCLib::Value(pmid,GLine->Circle());
	}
	else if (typl == IntPatch_Ellipse) {
	  Pmid = ElCLib::Value(pmid,GLine->Ellipse());
	}
	else if (typl == IntPatch_Hyperbola) {
	  Pmid = ElCLib::Value(pmid,GLine->Hyperbola());
	}
	else if (typl == IntPatch_Parabola) {
	  Pmid = ElCLib::Value(pmid,GLine->Parabola());
	}
	Parameters(mySurf1,mySurf2,Pmid,u1,v1,u2,v2);
	Recadre(mySurf1,mySurf2,u1,v1,u2,v2);

	gp_Vec Du,Dv;
	gp_Pnt P;
	myDom1->Init();
	if (myDom2->More()) {
	  mySurf1->D1(u1,v1,P,Du,Dv);
	  Tol = ComputeParametricTolerance( myDom1->Tol3d(myDom1->Value()) ,Du,Dv);
	}
	TopAbs_State in1 = myDom1->Classify(gp_Pnt2d(u1,v1),Tol,Standard_False);
	
	myDom2->Init();
	if (in1 != TopAbs_OUT  &&  myDom2->More() ) {
	  mySurf2->D1(u2,v2,P,Du,Dv);
	  Tol = ComputeParametricTolerance( myDom2->Tol3d(myDom2->Value()) ,Du,Dv);
	}
	TopAbs_State in2 = (in1!=TopAbs_OUT)? myDom2->Classify(gp_Pnt2d(u2,v2),Tol,Standard_False) : TopAbs_OUT;
	// modified by NIZHNY-OFV  Wed May 30 17:04:08 2001.BEGIN
	// --purpose: section algo with infinite prism works now!!!
	if(in1 == TopAbs_UNKNOWN) in1 = TopAbs_OUT;
	if(in2 == TopAbs_UNKNOWN) in2 = TopAbs_OUT;
	// modified by NIZHNY-OFV  Wed May 30 17:05:47 2001.END
	if(in1 == TopAbs_OUT || in2 == TopAbs_OUT) { 
	}
	else { 
	  //-- cout<<"GLine      : firtsp="<<firstp<<" lastp="<<lastp<<" Vtx:"<<i<<","<<i+1<<endl;
	  TabIndex[i]=TabIndex[i+1]=++numline;
	  AddLine(L,i,i+1,typs1,typs2,TabIndex,slin); 
	}
      }
    }
    if(typl == IntPatch_Circle || typl == IntPatch_Ellipse) { 
      firstp = GLine->Vertex(nbvtx).ParameterOnLine();
      lastp  = M_PI + M_PI + GLine->Vertex(1).ParameterOnLine();
      Standard_Real cadrinf = LocalFirstParameter(L);
      Standard_Real cadrsup = LocalLastParameter(L);
      Standard_Real acadr = (firstp+lastp)*0.5;
      while(acadr < cadrinf) { acadr+=M_PI+M_PI; }
      while(acadr > cadrsup) { acadr-=M_PI+M_PI; } 
      if(acadr>=cadrinf && acadr<=cadrsup) { 
	if(Abs(firstp-lastp)>Precision::PConfusion()) {
	  intrvtested = Standard_True;
	  Standard_Real pmid = (firstp+lastp)*0.5;
	  gp_Pnt Pmid;
	  if (typl == IntPatch_Circle) {
	    Pmid = ElCLib::Value(pmid,GLine->Circle());
	  }
	  else {
	    Pmid = ElCLib::Value(pmid,GLine->Ellipse());
	  }
	  Parameters(mySurf1,mySurf2,Pmid,u1,v1,u2,v2);
	  Recadre(mySurf1,mySurf2,u1,v1,u2,v2);
	  TopAbs_State in1 = myDom1->Classify(gp_Pnt2d(u1,v1),Tol,Standard_False);
	  TopAbs_State in2 = (in1!=TopAbs_OUT)?  myDom2->Classify(gp_Pnt2d(u2,v2),Tol,Standard_False) : TopAbs_OUT;
	  // modified by NIZHNY-OFV  Wed May 30 17:04:08 2001.BEGIN
	  // --purpose: section algo with infinite prism works now!!!
	  if(in1 == TopAbs_UNKNOWN) in1 = TopAbs_OUT;
	  if(in2 == TopAbs_UNKNOWN) in2 = TopAbs_OUT;
	  // modified by NIZHNY-OFV  Wed May 30 17:05:47 2001.END
	  if(in1 == TopAbs_OUT || in2 == TopAbs_OUT) { 
	  }
	  else { 
	    //-- cout<<"GLine  bis : firtsp="<<firstp<<" lastp="<<lastp<<" Vtx:"<<i<<","<<i+1<<endl;
	    TabIndex[nbvtx]=TabIndex[1]=++numline;
	    AddLine(L,nbvtx,1,typs1,typs2,TabIndex,slin); 
	  }
	}
      }
    }      
    if (!intrvtested) {
      // on garde a priori. Il faudrait un point 2d sur chaque
      // surface pour prendre la decision. Sera fait dans 
      // l`appelant
      //if(nbvtx) { 
      //	TabIndex[nbvtx]=TabIndex[1]=++numline;
      //	AddLine(L,1,nbvtx,typs1,typs2,TabIndex,slin);
      //}
    }
    //-- -------------------------------------------------------------------
    //-- On recherche les vertex interference Edge Edge Externe 
    //-- Ces vertex ne figurent sur aucune ligne et sont Restriction 
    //-- sur les 2 edges
    for(i=1;i<=nbvtx;i++) { 
      if(TabIndex[i]==0) { 
	const IntPatch_Point& GLine_Vertex_i  =GLine->Vertex(i);
	if(GLine_Vertex_i.IsOnDomS1() && GLine_Vertex_i.IsOnDomS2()) { 
	  TabIndex[i]=++numline;
	  AddLine(L,i,i,typs1,typs2,TabIndex,slin);	  
	}
      }
    }
    delete [] TabIndex;
    //-- -------------------------------------------------------------------    
    return;
  }
  else {  //-- Restriction
    Handle(IntPatch_RLine) RLine (Handle(IntPatch_RLine)::DownCast (L));
    slin.Clear();
    Standard_Integer NbVtx    = RLine->NbVertex();
    Standard_Boolean RestOnS1 = RLine->IsArcOnS1();
    Standard_Boolean RestOnS2 = RLine->IsArcOnS2();
    //-- -------------------------------------------------------------------
    Standard_Integer *TabIndex=new Standard_Integer [NbVtx+2];
    //Standard_Integer numline=0;
    for(i=1; i<=NbVtx; i++) { 
      TabIndex[i]=0;
    }
    //-- -------------------------------------------------------------------    
    for(i=1; i<NbVtx; i++) { 
      const IntPatch_Point&  Vtx1=RLine->Vertex(i);
      const IntPatch_Point&  Vtx2=RLine->Vertex(i+1);
      if(RestOnS1 && RestOnS2) { 
	AddLine(L,i,i+1,typs1,typs2,TabIndex,slin); 
      }
      else if(RestOnS1) { //-- On na classifie pas sur 1 
	Standard_Real u0 = Vtx1.ParameterOnLine();
	Standard_Real u1 = Vtx2.ParameterOnLine();
	if(Abs(u1-u0)>Precision::PConfusion()) { 
	  Standard_Real u  = (999.0*u0+u1)*0.001;
	  
	  gp_Pnt P0=Vtx1.Value();
	  gp_Pnt2d Px2d=RLine->ArcOnS1()->Value(u);
	  gp_Pnt   Px = mySurf1->Value(Px2d.X(),Px2d.Y());
	  gp_Vec P0Px=gp_Vec(P0,Px);
	  
	  Standard_Real U1,V1,U2,V2;
	  Vtx1.PntOn2S().Parameters(U1,V1,U2,V2);

	  gp_Vec D1u,D1v;
	  gp_Pnt P;
	  mySurf2->D1(U2,V2,P,D1u,D1v);
	  myDom2->Init();
	  if (myDom2->More())
	    Tol = ComputeParametricTolerance( myDom2->Tol3d(myDom2->Value()), D1u,D1v);

	  //-- le 23 mars 1999 
	  TopAbs_State bornin = myDom2->Classify(gp_Pnt2d(U2,V2),Tol,Standard_False);
	  if(bornin!=TopAbs_OUT) { 
	    Standard_Real U1t,V1t,U2t,V2t;
	    Vtx2.PntOn2S().Parameters(U1t,V1t,U2t,V2t);
	    bornin = myDom2->Classify(gp_Pnt2d(U2t,V2t),Tol,Standard_False);
	  }
	  if (bornin==TopAbs_OUT) continue;
	  
	  
	  //-- Attention , on faisait  une estimatoin de deltau et deltav 
	  //-- Maintenant : 
	  //-- POPx . D1u = deltau * D1u.D1u  + deltav * D1u.D1v
	  //-- POPx . D1v = deltau * D1u.D1v  + deltav * D1v.D1v
	  //--
	  //-- deltau=
	  Standard_Real D1uD1v,TgD1u,TgD1v,D1uD1u,D1vD1v,DIS;
	  //Standard_Real DeltaU,DeltaV;
	  D1uD1u = D1u.Dot(D1u);
	  D1vD1v = D1v.Dot(D1v);
	  D1uD1v = D1u.Dot(D1v);
	  TgD1u = P0Px.Dot(D1u);
	  TgD1v = P0Px.Dot(D1v);
	  DIS  = D1uD1u * D1vD1v - D1uD1v * D1uD1v;
	  
	  Standard_Real deltau=1e-10;
	  Standard_Real deltav=1e-10;
	  if(DIS<-1e-10 || DIS>1e-10) {
	    deltau=(TgD1u*D1vD1v-TgD1v*D1uD1v)/DIS;
	    deltav=(TgD1v*D1uD1u-TgD1u*D1uD1v)/DIS;
	  }

	  U2+=deltau;
	  V2+=deltav;
	  if(bornin!=TopAbs_OUT) { 
	    TopAbs_State in2 = myDom2->Classify(gp_Pnt2d(U2,V2),Tol,Standard_False);
	    deltau*=0.05;
	    deltav*=0.05;
	    if(in2==TopAbs_OUT) { 
	      in2 = myDom2->Classify(gp_Pnt2d(U2+deltau,V2),Tol,Standard_False);
	    }
	    if(in2==TopAbs_OUT) { 
	      in2 = myDom2->Classify(gp_Pnt2d(U2-deltau,V2),Tol,Standard_False);
	    }
	    if(in2==TopAbs_OUT) { 
	      in2 = myDom2->Classify(gp_Pnt2d(U2,V2+deltav),Tol,Standard_False);
	    }
	    if(in2==TopAbs_OUT) { 
	      in2 = myDom2->Classify(gp_Pnt2d(U2,V2-deltav),Tol,Standard_False);
	    }
	    
	    if(in2!=TopAbs_OUT) { 
	      //-- cout<<"RLine ons1 : u0    ="<<u0<<" u1   ="<<u1<<" Vtx:"<<i<<","<<i+1<<endl;
	      AddLine(L,i,i+1,typs1,typs2,TabIndex,slin); 
	    }
	  }
	}
      }
      else {
	Standard_Real u0 = Vtx1.ParameterOnLine();
	Standard_Real u1 = Vtx2.ParameterOnLine();
	if(Abs(u1-u0)>Precision::PConfusion()) { 
	  Standard_Real u  = (999.0*u0+u1)*0.001;
	  
	  gp_Pnt P0=Vtx1.Value();
	  gp_Pnt2d Px2d=RLine->ArcOnS2()->Value(u);
	  gp_Pnt   Px = mySurf2->Value(Px2d.X(),Px2d.Y());
	  gp_Vec P0Px=gp_Vec(P0,Px);
	  
	  Standard_Real U1,V1,U2,V2;
	  Vtx1.PntOn2S().Parameters(U1,V1,U2,V2);
	  
	  gp_Vec D1u,D1v;
	  gp_Pnt P;
	  mySurf1->D1(U1,V1,P,D1u,D1v);
	  myDom1->Init();
	  if (myDom2->More())
	    Tol = ComputeParametricTolerance( myDom1->Tol3d(myDom1->Value()) ,D1u,D1v);
	  
	  //-- le 23 mars 1999 
	  TopAbs_State bornin = myDom1->Classify(gp_Pnt2d(U1,V1),Tol,Standard_False);
	  if(bornin!=TopAbs_OUT) { 
	    Standard_Real U1t,V1t,U2t,V2t;
	    Vtx2.PntOn2S().Parameters(U1t,V1t,U2t,V2t);
	    bornin = myDom1->Classify(gp_Pnt2d(U1t,V1t),Tol,Standard_False);
	  }
	  if (bornin==TopAbs_OUT) continue;
	  
	  
	  //-- Attention , on faisait  une estimatoin de deltau et deltav 
	  //-- Maintenant : 
	  //-- POPx . D1u = deltau * D1u.D1u  + deltav * D1u.D1v
	  //-- POPx . D1v = deltau * D1u.D1v  + deltav * D1v.D1v
	  //--
	  //-- deltau=
	  Standard_Real D1uD1v,TgD1u,TgD1v,D1uD1u,D1vD1v,DIS;
	  //Standard_Real DeltaU,DeltaV;
	  D1uD1u = D1u.Dot(D1u);
	  D1vD1v = D1v.Dot(D1v);
	  D1uD1v = D1u.Dot(D1v);
	  TgD1u = P0Px.Dot(D1u);
	  TgD1v = P0Px.Dot(D1v);
	  DIS  = D1uD1u * D1vD1v - D1uD1v * D1uD1v;
	  
	  Standard_Real deltau=1e-10;
	  Standard_Real deltav=1e-10;
	  if(DIS<-1e-10 || DIS>1e-10) {
	    deltau=(TgD1u*D1vD1v-TgD1v*D1uD1v)/DIS;
	    deltav=(TgD1v*D1uD1u-TgD1u*D1uD1v)/DIS;
	  }
	  
	  U1+=deltau;
	  V1+=deltav;
	  
	  if(bornin!=TopAbs_OUT) { 
	    TopAbs_State in2 = myDom1->Classify(gp_Pnt2d(U1,V1),Tol,Standard_False);	  
	    deltau*=0.05;
	    deltav*=0.05;
	    if(in2==TopAbs_OUT) { 
	      in2 = myDom1->Classify(gp_Pnt2d(U1+deltau,V1),Tol,Standard_False);
	    }
	    if(in2==TopAbs_OUT) { 
	      in2 = myDom1->Classify(gp_Pnt2d(U1-deltau,V1),Tol,Standard_False);
	    }
	    if(in2==TopAbs_OUT) { 
	      in2 = myDom1->Classify(gp_Pnt2d(U1,V1+deltav),Tol,Standard_False);
	    }
	    if(in2==TopAbs_OUT) { 
	      in2 = myDom1->Classify(gp_Pnt2d(U1,V1-deltav),Tol,Standard_False);
	    }
	    
	    if(in2!=TopAbs_OUT) { 
	      //-- cout<<"RLine ons2 : u0    ="<<u0<<" u1   ="<<u1<<" Vtx:"<<i<<","<<i+1<<endl;
	      
	      AddLine(L,i,i+1,typs1,typs2,TabIndex,slin); 
	    }
	  }
	}
      }
    }
    delete [] TabIndex;
  }
}
