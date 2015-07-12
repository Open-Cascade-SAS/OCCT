// Created on: 1994-01-20
// Created by: Isabelle GRIGNON
// Copyright (c) 1994-1999 Matra Datavision
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
#include <AppBlend_Approx.hxx>
#include <Blend_CurvPointFuncInv.hxx>
#include <Blend_FuncInv.hxx>
#include <Blend_Function.hxx>
#include <Blend_RstRstFunction.hxx>
#include <Blend_SurfCurvFuncInv.hxx>
#include <Blend_SurfPointFuncInv.hxx>
#include <Blend_SurfRstFunction.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_HCurve.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepBlend_Line.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <ChFi3d_Builder.hxx>
#include <ChFi3d_Builder_0.hxx>
#include <ChFiDS_CommonPoint.hxx>
#include <ChFiDS_HElSpine.hxx>
#include <ChFiDS_Spine.hxx>
#include <ChFiDS_Stripe.hxx>
#include <ChFiDS_SurfData.hxx>
#include <ChFiKPart_RstMap.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom2dHatch_Hatcher.hxx>
#include <Geom2dHatch_Intersector.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <gp.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <HatchGen_Domain.hxx>
#include <HatchGen_PointOnElement.hxx>
#include <HatchGen_PointOnHatching.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TopExp.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Surface.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean ChFi3d_GettraceDRAWFIL();
extern void ChFi3d_CheckSurfData(const TopOpeBRepDS_DataStructure& DStr,
				 const Handle(ChFiDS_SurfData)& Data);
#endif
//=======================================================================
//function : CompTra
//purpose  : Calculate the Transition from start point.
//=======================================================================

static TopAbs_Orientation CompTra (const TopAbs_Orientation O1,
				   const TopAbs_Orientation O2,
				   const Standard_Boolean isfirst)
{
  if(isfirst) return TopAbs::Reverse(TopAbs::Compose(O1,O2));
  else return TopAbs::Compose(O1,O2);
}


//=======================================================================
//function : CompCommonpoint
//purpose  : Fill the commonpoint in case of a vertex.
//=======================================================================

static void CompCommonPoint (ChFiDS_CommonPoint& FilPoint, 
			     const TopoDS_Edge& arc,
			     const HatchGen_PointOnElement& PE,
			     const TopAbs_Orientation Or)
{
  TopAbs_Orientation pos = PE.Position();
  TopoDS_Vertex V;
  if ( pos == TopAbs_FORWARD ) {
    V = TopExp::FirstVertex(arc);
  }
  else {
    V = TopExp::LastVertex(arc);
  }
  FilPoint.SetVertex(V);
  FilPoint.SetArc(Precision::PIntersection(),arc,
		  PE.Parameter(),TopAbs::Compose(arc.Orientation(),Or));
}


//=======================================================================
//function : CpInterf
//purpose  : Construct new SurfData sharing faces, surface and curves.  
//=======================================================================

static ChFiDS_FaceInterference CpInterf (TopOpeBRepDS_DataStructure&    DStr,
					 const ChFiDS_FaceInterference& FI)
{
  ChFiDS_FaceInterference newF = FI;
  const TopOpeBRepDS_Curve& toc = DStr.Curve(FI.LineIndex());
  Handle(Geom_Curve) newC;
  if (!toc.Curve().IsNull()) 
    newC = Handle(Geom_Curve)::DownCast(toc.Curve()->Copy());
  newF.SetLineIndex(DStr.AddCurve(TopOpeBRepDS_Curve(newC,toc.Tolerance())));
  
  if (!FI.PCurveOnFace().IsNull())
    newF.ChangePCurveOnFace() = 
      Handle(Geom2d_Curve)::DownCast(FI.PCurveOnFace()->Copy());
  if (!FI.PCurveOnSurf().IsNull())
    newF.ChangePCurveOnSurf() = 
      Handle(Geom2d_Curve)::DownCast(FI.PCurveOnSurf()->Copy());
  return newF;
}


//=======================================================================
//function : CpSD
//purpose  : Construct new SurfData sharing faces, surface and curves.  
//=======================================================================

static Handle(ChFiDS_SurfData) CpSD (      TopOpeBRepDS_DataStructure&         DStr,
				     const Handle(ChFiDS_SurfData)& Data)
{
  Handle(ChFiDS_SurfData) newData = new ChFiDS_SurfData();
  const TopOpeBRepDS_Surface& tos = DStr.Surface(Data->Surf());
  Handle(Geom_Surface) newS = Handle(Geom_Surface)::DownCast(tos.Surface()->Copy());
  Standard_Real tol = tos.Tolerance();
  newData->ChangeSurf(DStr.AddSurface(TopOpeBRepDS_Surface(newS,tol)));
  newData->ChangeIndexOfS1(Data->IndexOfS1());
  newData->ChangeIndexOfS2(Data->IndexOfS2());
  newData->ChangeOrientation() = Data->Orientation();
  newData->ChangeInterferenceOnS1() = CpInterf(DStr,Data->InterferenceOnS1());
  newData->ChangeInterferenceOnS2() = CpInterf(DStr,Data->InterferenceOnS2());
  return newData;
}

//=======================================================================
//function : AdjustParam
//purpose  : 
//=======================================================================

static Standard_Boolean AdjustParam(const HatchGen_Domain& Dom,
				    Standard_Real& f,
				    Standard_Real& l,
				    const Standard_Real wref,
				    const Standard_Real period,
				    const Standard_Real pitol)
{
  if(Dom.HasFirstPoint())
    f = Dom.FirstPoint().Parameter();
  else f = 0.;
  if(Dom.HasSecondPoint())
    l = Dom.SecondPoint().Parameter();  
  else l = period;
  if (period == 0.) return Standard_False;
  
  f = ElCLib::InPeriod(f,wref - pitol, wref + period - pitol);
  l = ElCLib::InPeriod(l,wref + pitol, wref + period + pitol);
  if (l < f) {
    f -= period; 
    return Standard_True;
  }
  return Standard_False;
}
//=======================================================================
//function : ComputeAbscissa
//purpose  : 
//=======================================================================

static Standard_Real ComputeAbscissa(const BRepAdaptor_Curve& C,
				     const Standard_Real U) 
{
  switch (C.GetType()) {
  case GeomAbs_Line:
    return U;
  case GeomAbs_Circle:
    return C.Circle().Radius()*U;
  default:
    return 0;
  }    
}

//=======================================================================
//function : ParamOnSpine
//purpose  : 
//=======================================================================

static Standard_Real ParamOnSpine(const TopOpeBRepDS_DataStructure& DStr,
				  const Standard_Real               ptg,
				  const Handle(ChFiDS_SurfData)&    CD,
				  const Handle(ChFiDS_Spine)&       Spine,
				  const Standard_Integer            iedge,
				  const Standard_Boolean            intf,
				  const Standard_Boolean            intl,
				  const Standard_Real               tol,
				  Standard_Boolean&                 pok) 
{
  Standard_Real Nl;
  Standard_Real f = Spine->FirstParameter(iedge);
  Standard_Real l = Spine->LastParameter(iedge);
  
  Nl = ComputeAbscissa(Spine->CurrentElementarySpine(iedge),ptg) + f;
  if ((Nl >= (f - tol) || intf) && 
      (Nl <= (l + tol) || intl) ) {
    pok = 1;
    return Nl;
  }
  else {
    //construction of the plane containing the section of CD with parameter ptg.
    gp_Pnt PP;
    gp_Vec VV;
    Handle(Geom_Curve) c3d;
    if (CD->InterferenceOnS1().LineIndex() != 0) {
      c3d = DStr.Curve(CD->InterferenceOnS1().LineIndex()).Curve();
    }
    if(c3d.IsNull()) {
      c3d = DStr.Curve(CD->InterferenceOnS2().LineIndex()).Curve();
    }
    c3d->D1(ptg,PP,VV);
    
    gp_Pln nlp(PP,gp_Dir(VV));
    Handle(Geom_Plane) pln = new Geom_Plane(nlp);
    Handle(GeomAdaptor_HSurface) 
      plan = new GeomAdaptor_HSurface(GeomAdaptor_Surface(pln));
    
    // intersection plane spine.
    Standard_Boolean found = Standard_False;
    Standard_Boolean fini  = Standard_False;
    Standard_Integer sens = 1;
    if (Nl <= f) sens = -1;
    Standard_Integer ii = iedge + sens;
    if (Spine->IsPeriodic()) {
      if (ii <= 0) ii += Spine->NbEdges();
      if (ii >  Spine->NbEdges()) ii -= Spine->NbEdges();
    } 
    else if(ii < 1 || ii >  Spine->NbEdges()) {
      pok = 1;
      return Nl;
    }
    Handle(BRepAdaptor_HCurve) HE = new BRepAdaptor_HCurve();
    BRepAdaptor_Curve& CE = HE->ChangeCurve();
    
    while (!found && !fini) {
      TopAbs_Orientation O = Spine->Edges(ii).Orientation();
      Standard_Boolean First = ((O == TopAbs_FORWARD && sens == 1) || 
				(O == TopAbs_REVERSED && sens == -1));
      CE.Initialize(Spine->Edges(ii));
      Standard_Real tolc = CE.Resolution(tol);
      found = ChFi3d_InterPlaneEdge(plan,HE,Nl,First,tolc);
      gp_Pnt point = CE.Value(Nl);
#ifdef OCCT_DEBUG
      cout<<"******* ParamOnSpine() for edge "<<iedge<<endl;
      cout<<Nl<<endl;
      cout<<"point ped "<<point.X()<<" "<<point.Y()<<" "<<point.Z()<<endl;
#endif
      if(found) Nl = Spine->Absc(Nl,ii);
      point = Spine->Value(Nl);
#ifdef OCCT_DEBUG
      if (found) cout << "found by edge " << ii << " : ";
      cout<<Nl<<endl;
      cout<<"point psp "<<point.X()<<" "<<point.Y()<<" "<<point.Z()<<endl;
      cout<<endl;
#endif
      
      ii +=sens;
      if (Spine->IsPeriodic()) {
	if (ii <= 0) ii += Spine->NbEdges();
	if (ii >  Spine->NbEdges()) ii -= Spine->NbEdges();
	fini = (ii == iedge);
      } 
      else {
	fini = (ii < 1 || ii >  Spine->NbEdges());
      }
    }
    pok = found;
    return Nl; 
  }
}

//=======================================================================
//function : YaUnVoisin
//purpose  : 
//=======================================================================

static Standard_Boolean YaUnVoisin(const Handle(ChFiDS_Spine)& Spine,
				   const Standard_Integer      iedge,
				   Standard_Integer&           ivois,
				   const Standard_Boolean      isfirst)
{
  Standard_Integer nbed = Spine->NbEdges();
  if(nbed == 1) return 0;
  Standard_Boolean periodic = Spine->IsPeriodic();
  if(isfirst) ivois = iedge - 1;
  else ivois = iedge + 1;
  if(periodic) {
    if(ivois == 0) ivois = nbed;
    if(ivois == nbed+1) ivois = 1;
  }
  return (ivois > 0 && ivois <= nbed);
}

//=======================================================================
//function : Trunc
//purpose  : 
//=======================================================================

void ChFi3d_Builder::Trunc(const Handle(ChFiDS_SurfData)&    SD,
			   const Handle(ChFiDS_Spine)&       Spine,
			   const Handle(Adaptor3d_HSurface)&   S1, 
			   const Handle(Adaptor3d_HSurface)&   S2, 
			   const Standard_Integer            iedge,
			   const Standard_Boolean            isfirst,
			   const Standard_Integer            cntlFiOnS)
{
  TopOpeBRepDS_DataStructure& DStr = myDS->ChangeDS();
  // Return points and tangents on edge and spine.
  Standard_Real wtg = SD->InterferenceOnS1().Parameter(isfirst);
  Standard_Boolean bid;
  Standard_Real wsp = ParamOnSpine(DStr,wtg,SD,Spine,iedge,0,0,tolesp,bid);
  gp_Pnt ped,psp;
  gp_Vec ded,dsp;
  TopoDS_Vertex bout1,bout2,boutemp;

  
  const BRepAdaptor_Curve& bc = Spine->CurrentElementarySpine(iedge);
//Modif against Vertex isolated on spine
  TopoDS_Edge support = bc.Edge();
  TopExp::Vertices(support,bout1,bout2);
  if (support.Orientation() == TopAbs_REVERSED) {
    boutemp = bout2;
    bout2 = bout1;
    bout1 = boutemp;
  }
  if (!isfirst) {
    bout1 = bout2;
  }
//finmodif
  Standard_Real edf = bc.FirstParameter(), edl = bc.LastParameter();
  Standard_Real edglen = edl - edf;
  if(Spine->Edges(iedge).Orientation() == TopAbs_FORWARD) {
    bc.D1(wtg+edf,ped,ded);
  }
  else{ 
    bc.D1(-wtg+edl,ped,ded);
    ded.Reverse();
  }
  Spine->D1(wsp,psp,dsp);
  gp_Pnt p1,p2;
  const Handle(Geom_Surface)& surf = DStr.Surface(SD->Surf()).Surface();
  gp_Pnt2d pp1,pp2;
  pp1 = SD->InterferenceOnS1().PCurveOnSurf()->Value(wtg);
  pp2 = SD->InterferenceOnS2().PCurveOnSurf()->Value(wtg);
  p1 = surf->Value(pp1.X(),pp1.Y());
  p2 = surf->Value(pp2.X(),pp2.Y());
  Standard_Boolean tron = Standard_False;
  Standard_Real Ang = dsp.Angle(ded);
  Standard_Real dis1 = psp.Distance(ped);
  Standard_Real dis2 = p1.Distance(p2);
  if(Ang > M_PI/18.) tron = Standard_True;
  if(dis1 >= 0.1*dis2) tron = Standard_True;
  Standard_Integer ivois;
  if(!tron && YaUnVoisin(Spine,iedge,ivois,isfirst)) {
    Handle(BRepAdaptor_HSurface) BS1 = Handle(BRepAdaptor_HSurface)::DownCast(S1);
    Handle(BRepAdaptor_HSurface) BS2 = Handle(BRepAdaptor_HSurface)::DownCast(S2);
    if(!BS1.IsNull() && !BS2.IsNull()) {
      TopoDS_Face FBID;
      TopoDS_Face F1 = BS1->ChangeSurface().Face();
      TopoDS_Face F2 = BS2->ChangeSurface().Face();
      const ChFiDS_CommonPoint& cp1 = SD->Vertex(isfirst,1);
      const ChFiDS_CommonPoint& cp2 = SD->Vertex(isfirst,2);
      if(!((cp1.IsOnArc() && SearchFace(Spine,cp1,F1,FBID)) ||
           (cp2.IsOnArc() && SearchFace(Spine,cp2,F2,FBID)))) { 
        tron = ChFi3d_KParticular(Spine,ivois,BS1->ChangeSurface(),BS2->ChangeSurface());
      }
    }
  }
  //modification of lvt against isolated vertex
  if(!tron && YaUnVoisin(Spine,iedge,ivois,isfirst)) {
    TopTools_ListIteratorOfListOfShape It;
    Standard_Integer nbed = -2;
    for (It.Initialize(myVEMap(bout1));It.More();It.Next()) {
      nbed++;
    }
    if(nbed<3) tron = Standard_True;
  }
//finmodif

  if(tron) {
    Standard_Real par = 0., x, y, dPar=0;
    if(!isfirst) par = edglen;
    if (cntlFiOnS) {
      // detect the case where FaceInterference ends before the place we are
      // going to truncate SD. Then we cut so that FaceInterference length to
      // be at least zero, not negative (eap, occ354)
      Standard_Real fiPar = SD->Interference(cntlFiOnS).Parameter(!isfirst);
      Standard_Boolean isTheCase = isfirst ? (par > fiPar) : (par < fiPar);
      if (isTheCase) {
	dPar = par - fiPar;
	par = fiPar;
      }
    }
    for (Standard_Integer i = 1; i <= 2; i++) { 
      SD->ChangeInterference(i).SetParameter(par,isfirst);
      Handle(Geom2d_Curve) pc = SD->Interference(i).PCurveOnSurf();
      pc->Value(par).Coord(x,y);
      SD->ChangeVertex(isfirst,i).Reset();
      SD->ChangeVertex(isfirst,i).SetPoint(surf->Value(x,y));
      if(isfirst) SD->FirstSpineParam(Spine->FirstParameter(iedge)-dPar);
      else        SD->LastSpineParam (Spine->LastParameter(iedge) -dPar);
    }
  }
}

//=======================================================================
//function : ResetProl
//purpose  : 
//=======================================================================

static Standard_Real ResetProl(const TopOpeBRepDS_DataStructure& DStr,
			       const Handle(ChFiDS_SurfData)&    CD,
			       const Handle(ChFiDS_Spine)&       Spine,
			       const Standard_Integer            iedge,
			       const Standard_Boolean            isfirst)
{
  const BRepAdaptor_Curve& bc = Spine->CurrentElementarySpine(iedge);
  Standard_Real edglen = bc.LastParameter() - bc.FirstParameter();
  const Handle(Geom_Surface)& surf = DStr.Surface(CD->Surf()).Surface();
  Standard_Real par = 0., x, y;
  if(!isfirst) par = edglen;
  Standard_Real sppar = 0.;
  for (Standard_Integer i = 1; i <= 2; i++) { 
    CD->ChangeInterference(i).SetParameter(par,isfirst);
    Handle(Geom2d_Curve) pc = CD->Interference(i).PCurveOnSurf();
    pc->Value(par).Coord(x,y);
    CD->ChangeVertex(isfirst,i).Reset();
    CD->ChangeVertex(isfirst,i).SetPoint(surf->Value(x,y));
    if(isfirst) {
      sppar = Spine->FirstParameter(iedge);
      CD->FirstSpineParam(sppar);
    }
    else{
      sppar = Spine->LastParameter(iedge);
      CD->LastSpineParam (sppar);
    }
  }
  return sppar;
}
//=======================================================================
//function : Tri
//purpose  : 
//=======================================================================

static Standard_Boolean Tri(const Geom2dHatch_Hatcher& H,
			    const Standard_Integer     iH,
			    TColStd_Array1OfInteger&   Ind,
			    const Standard_Real        wref,
			    const Standard_Real        period,
			    const Standard_Real        pitol,
			    Standard_Integer&    Nbdom)
{
//  for (Standard_Integer i = 1; i <= Nbdom; i++) { Ind(i) = i; }
  Standard_Integer i;
  for ( i = 1; i <= Nbdom; i++) { Ind(i) = i; }
  Standard_Real f1,f2,l;
  Standard_Integer tmp;
  Standard_Boolean Invert = Standard_True;
  
  while (Invert) {
    Invert = Standard_False;
    for ( i = 1; i < Nbdom; i++) {
      AdjustParam(H.Domain(iH,Ind(i)),f1,l,wref,period,pitol);
      AdjustParam(H.Domain(iH,Ind(i+1)),f2,l,wref,period,pitol);
      if ( f2 < f1) {
	tmp = Ind(i);
	Ind(i) = Ind(i+1);
	Ind(i+1) = tmp;
	Invert = Standard_True;
      }
    }
  }
  
  Standard_Integer iSansFirst = 0, iSansLast = 0;
  
  if (Nbdom != 1) {
    for ( i = 1; i <= Nbdom; i++) {
      if (!H.Domain(iH,Ind(i)).HasFirstPoint()) {
	iSansFirst = i;
      }
      if (!H.Domain(iH,Ind(i)).HasSecondPoint()) {
	iSansLast = i;
      }
    }
  }
  if (iSansFirst != 0) {
    if (iSansLast == 0) {
#ifdef OCCT_DEBUG
      cout<<"Parsing : Pb of Hatcher"<<endl;
#endif
      return 0;
    }
    HatchGen_Domain* Dom = ((HatchGen_Domain*) (void*) &H.Domain(iH,Ind(iSansFirst)));    
    HatchGen_PointOnHatching* PH = 
      ((HatchGen_PointOnHatching*) (void*) &H.Domain(iH,Ind(iSansLast)).FirstPoint());
    Standard_Real NewPar = H.HatchingCurve(iH).FirstParameter() - period +
      H.Domain(iH,Ind(iSansLast)).FirstPoint().Parameter();
    PH->SetParameter(NewPar);
    Dom->SetFirstPoint(*PH);
    
    for (Standard_Integer k = iSansLast; k < Nbdom; k++) {
      Ind(k) = Ind(k+1);
    }
    Nbdom--;
  }
  return 1;
}

//=======================================================================
//function : FillSD
//purpose  : 
//=======================================================================

static void FillSD (TopOpeBRepDS_DataStructure& DStr,
		    Handle(ChFiDS_SurfData)&    CD,
		    ChFiKPart_RstMap&           M,
		    const HatchGen_Domain&      Dom,
		    const Standard_Real         ponH,
		    const Standard_Boolean      isFirst,
		    const Standard_Integer      ons,
		    const Standard_Real         pitol,
		    const TopoDS_Vertex         bout)
     
{  
  Standard_Integer opp = 3 - ons;
  ChFiDS_CommonPoint& Pons = CD->ChangeVertex(isFirst,ons); 
  ChFiDS_CommonPoint& Popp = CD->ChangeVertex(isFirst,opp);   
  
  const HatchGen_PointOnHatching* pPH = 0;
  if(isFirst && Dom.HasFirstPoint()) {
    const HatchGen_PointOnHatching& PHtemp = Dom.FirstPoint();
    pPH = &PHtemp;
  }
  else if(!isFirst && Dom.HasSecondPoint()) {
    const HatchGen_PointOnHatching& PHtemp = Dom.SecondPoint();
    pPH = &PHtemp;
  }
  Standard_Real x,y;
  Handle(Geom_Surface) Surf = DStr.Surface(CD->Surf()).Surface();
  if(pPH == 0) {
    CD->ChangeInterference(ons).SetParameter(ponH,isFirst);
    Handle(Geom2d_Curve) pcons = CD->Interference(ons).PCurveOnSurf();
    pcons->Value(ponH).Coord(x,y);
    CD->ChangeVertex(isFirst,ons).SetPoint(Surf->Value(x,y));
  }
  else {
//Modification to find already existing vertexes
    Standard_Integer LeType = 1;
    Standard_Integer NbInt = pPH->NbPoints();
    if (NbInt>1) {
      Standard_Boolean trouve = Standard_True;
      Standard_Integer IE;
      TopoDS_Vertex V1 , V2;
      Standard_Boolean suite = Standard_True;
      for(;trouve;) {
	const HatchGen_PointOnElement& PEtemp = pPH->Point(LeType);
	IE = PEtemp.Index();
	Handle(BRepAdaptor_HCurve2d) HE = Handle(BRepAdaptor_HCurve2d)::DownCast(M(IE));
	if(!HE.IsNull()) {
	  const TopoDS_Edge& Etemp = HE->ChangeCurve2d().Edge();
	  TopExp::Vertices(Etemp,V1,V2);
	}
	else {
	  suite = Standard_False;
	}
	if(((V1.IsSame(bout)) || (V2.IsSame(bout))) && suite) {
	  trouve = Standard_True;
	  break;
	}
	else {
	  suite = Standard_True;
	  trouve = Standard_False;
	  LeType++;
	  if(LeType>NbInt) {
	    trouve = Standard_True;
	    LeType = 1;
	  }
	}
      }
    }
    const HatchGen_PointOnElement& PE = pPH->Point(LeType);
    Standard_Integer IE = PE.Index();
    Handle(BRepAdaptor_HCurve2d) 
      HE = Handle(BRepAdaptor_HCurve2d)::DownCast(M(IE));
    if(HE.IsNull()) return;
    const TopoDS_Edge& E = HE->ChangeCurve2d().Edge();
    
    if (PE.Position() != TopAbs_INTERNAL) {
      TopAbs_Orientation O = CD->Interference(ons).Transition();
      if(isFirst) O = TopAbs::Reverse(O);
      CompCommonPoint(Pons,E,PE,O);
    }
    else{
      Pons.SetArc(pitol,E,PE.Parameter(),
		  CompTra(CD->Interference(ons).Transition(),E.Orientation(),isFirst));
    }
    Handle(Geom2d_Curve) pcadj = CD->Interference(ons).PCurveOnSurf();
    pcadj->Value(ponH).Coord(x,y);
    CD->ChangeInterference(ons).SetParameter(ponH,isFirst);
    CD->ChangeVertex(isFirst,ons).SetPoint(Surf->Value(x,y));
  }
  if(!Popp.IsOnArc()) {
    CD->ChangeInterference(opp).SetParameter(ponH,isFirst);
    Handle(Geom2d_Curve) pcopp = CD->Interference(opp).PCurveOnSurf();
    pcopp->Value(ponH).Coord(x,y);
    CD->ChangeVertex(isFirst,opp).SetPoint(Surf->Value(x,y));
  }
}

//=======================================================================
//function : SplitKPart
//purpose  : Reconstruct SurfData depending on restrictions of faces.
//=======================================================================

Standard_Boolean ChFi3d_Builder::SplitKPart
  (const Handle(ChFiDS_SurfData)&   Data, 
   ChFiDS_SequenceOfSurfData&       SetData, 
   const Handle(ChFiDS_Spine)&      Spine,
   const Standard_Integer           Iedge,
   const Handle(Adaptor3d_HSurface)&  S1, 
   const Handle(Adaptor3d_TopolTool)& I1, 
   const Handle(Adaptor3d_HSurface)&  S2,
   const Handle(Adaptor3d_TopolTool)& I2, 
   Standard_Boolean&                intf,
   Standard_Boolean&                intl)
{
  //The the hatching of each faces is started by tangency lines.
  
  Standard_Real pitol = Precision::PIntersection();
  
  ChFiKPart_RstMap M1, M2;
  Standard_Integer iH1 = 0,iH2 = 0;
  Standard_Integer  Nb1 = 1,Nb2 = 1;
  
  // Cutting of tangency lines (hatching).
  Geom2dHatch_Intersector Inter(pitol,pitol);
  Geom2dHatch_Hatcher H1(Inter,tol2d,tolesp), H2(Inter,tol2d,tolesp);
  Standard_Integer ie;
  Handle(Geom2d_Curve) C1 = Data->InterferenceOnS1().PCurveOnFace(); 
  Geom2dAdaptor_Curve  ll1;
  if (!C1.IsNull()) {
    ll1.Load(C1);
    for(I1->Init(); I1->More(); I1->Next()) {
      Handle(BRepAdaptor_HCurve2d) 
	Bc = Handle(BRepAdaptor_HCurve2d)::DownCast(I1->Value());
      Handle(Geom2dAdaptor_HCurve) 
	Gc = Handle(Geom2dAdaptor_HCurve)::DownCast(I1->Value());
      if(Bc.IsNull()) ie = H1.AddElement(Gc->ChangeCurve2d(),TopAbs_FORWARD);
      else ie = H1.AddElement(Bc->ChangeCurve2d(),
			      Bc->ChangeCurve2d().Edge().Orientation());
      M1.Bind(ie,I1->Value());
    }
    iH1 = H1.Trim(ll1);
    H1.ComputeDomains(iH1);
    if(!H1.IsDone(iH1)) return 0;
    Nb1 = H1.NbDomains(iH1);
    if(Nb1 == 0) {
#ifdef OCCT_DEBUG
      cout<<"SplitKPart : tangency line out of the face"<<endl;
#endif
      return Standard_False;
    }
  }
  
  Handle(Geom2d_Curve) C2 = Data->InterferenceOnS2().PCurveOnFace(); 
  Geom2dAdaptor_Curve  ll2;
  if (!C2.IsNull()) {
    ll2.Load(C2);
    for(I2->Init(); I2->More(); I2->Next()) {
      Handle(BRepAdaptor_HCurve2d) 
	Bc = Handle(BRepAdaptor_HCurve2d)::DownCast(I2->Value());
      Handle(Geom2dAdaptor_HCurve) 
	Gc = Handle(Geom2dAdaptor_HCurve)::DownCast(I2->Value());
      if(Bc.IsNull()) ie = H2.AddElement(Gc->ChangeCurve2d(),TopAbs_FORWARD);
      else ie = H2.AddElement(Bc->ChangeCurve2d(),
			      Bc->ChangeCurve2d().Edge().Orientation());
      M2.Bind(ie,I2->Value());
    }
    iH2 = H2.Trim(ll2);
    H2.ComputeDomains(iH2);
    if(!H2.IsDone(iH2)) return 0;
    Nb2 = H2.NbDomains(iH2);
    if(Nb2 == 0) {
#ifdef OCCT_DEBUG
      cout<<"SplitKPart : tangency line out of the face"<<endl;
#endif
      return Standard_False;
    }
  }

  //Return start and end vertexes of the Spine
  TopoDS_Vertex bout1,bout2,boutemp;
  const BRepAdaptor_Curve& bc = Spine->CurrentElementarySpine(Iedge);
  TopoDS_Edge support = bc.Edge();
  TopExp::Vertices(support,bout1,bout2);
  if(support.Orientation() == TopAbs_REVERSED) {
    boutemp = bout2;
    bout2 = bout1;
    bout1 = boutemp;
  }
  
  // Return faces.
  TopoDS_Face F1, F2;
  Handle(BRepAdaptor_HSurface) 
    bhs = Handle(BRepAdaptor_HSurface)::DownCast(S1);
  if(!bhs.IsNull()) F1 = bhs->ChangeSurface().Face();
  bhs = Handle(BRepAdaptor_HSurface)::DownCast(S2);
  if(!bhs.IsNull()) F2 = bhs->ChangeSurface().Face();
  TopoDS_Face FBID;
  
  // Restriction of SurfDatas by cut lines.
  TopOpeBRepDS_DataStructure& DStr = myDS->ChangeDS();
  Handle(ChFiDS_SurfData) CD = Data;
  CD->ChangeIndexOfS1(DStr.AddShape(F1));
  CD->ChangeIndexOfS2(DStr.AddShape(F2));
  
  Standard_Real f1,l1,f2,l2;
  TColStd_Array1OfInteger Ind1(1,Nb1), Ind2(1,Nb2); 
  Standard_Real wref = 0.;

  Standard_Integer onS = 1; // switcher of access to surfs of SurfData, eap occ293
  Standard_Integer cntlFiOnS = 0; // FaceInterference to control length in OnSame
                                  // situation, eap occ354
  
  if (C1.IsNull() && C2.IsNull()) {
#ifdef OCCT_DEBUG
    cout<<"SplitData : 2 zero lines hatching impossible"<<endl;
#endif
    return Standard_False;
  }
  else if (C1.IsNull() || (Nb1 == 1 && !H1.Domain(iH1,1).HasFirstPoint())) {   
    // It is checked if the point 2d of the degenerated edge is in the face.
    if (C1.IsNull()) {
      gp_Pnt2d p2d1 = CD->Get2dPoints(0,1);
      TopAbs_State situ = I1->Classify(p2d1,1.e-8,0);
      if(situ == TopAbs_OUT) return Standard_False;
    }

    // Parsing of domains by increasing parameters,
    if(!Tri(H2,iH2,Ind2,wref,0.,pitol,Nb2)) return 0;
    // Filling of SurfData
    for(Standard_Integer i = 1; i <= Nb2; i++) {
      const HatchGen_Domain& Dom2 = H2.Domain(iH2,Ind2(i));
      FillSD(DStr,CD,M2,Dom2,Dom2.FirstPoint().Parameter(),1,2,pitol,bout1);
      FillSD(DStr,CD,M2,Dom2,Dom2.SecondPoint().Parameter(),0,2,pitol,bout2);
      SetData.Append(CD);
      CD = CpSD(DStr,CD);
    }
    if(intf) {
      Handle(ChFiDS_SurfData)& sd = SetData.ChangeValue(1);
      ChFiDS_CommonPoint& CP2 = sd->ChangeVertexFirstOnS2();
      if(CP2.IsOnArc() && Spine->FirstStatus() == ChFiDS_OnSame) {
	intf = !SearchFace(Spine,CP2,F2,FBID);
      }
      else intf = Standard_False;
    }
    if(intl) {
      Handle(ChFiDS_SurfData)& sd = SetData.ChangeValue(SetData.Length());
      ChFiDS_CommonPoint& CP2 = sd->ChangeVertexLastOnS2();
      if(CP2.IsOnArc() && Spine->LastStatus() == ChFiDS_OnSame) {
	intl = !SearchFace(Spine,CP2,F2,FBID);
      }
      else intl = Standard_False;
    }
  }
  else if (C2.IsNull() || (Nb2 == 1 && !H2.Domain(iH2,1).HasFirstPoint())) { 
    // It is checked if the point 2d of the degenerated is in the face.
    if (C2.IsNull()) {
      gp_Pnt2d p2d2 = CD->Get2dPoints(0,2);
      TopAbs_State situ = I2->Classify(p2d2,1.e-8,0);
      if(situ == TopAbs_OUT) return Standard_False;
    }

    // Parsing of domains by increasing parameters,
    if(!Tri(H1,iH1,Ind1,wref,0.,pitol,Nb1)) return 0;
    // Filling of SurfData
    for(Standard_Integer i = 1; i <= Nb1; i++) {
      const HatchGen_Domain& Dom1 = H1.Domain(iH1,Ind1(i));
      FillSD(DStr,CD,M1,Dom1,Dom1.FirstPoint().Parameter(),1,1,pitol,bout1);
      FillSD(DStr,CD,M1,Dom1,Dom1.SecondPoint().Parameter(),0,1,pitol,bout2);
      SetData.Append(CD);
      CD = CpSD(DStr,CD);
    }
    if(intf) {
      Handle(ChFiDS_SurfData)& sd = SetData.ChangeValue(1);
      ChFiDS_CommonPoint& CP1 = sd->ChangeVertexFirstOnS1();
      if(CP1.IsOnArc() && Spine->FirstStatus() == ChFiDS_OnSame) {
	intf = !SearchFace(Spine,CP1,F1,FBID);
      }
      else intf = Standard_False;
    }
    if(intl) {
      Handle(ChFiDS_SurfData)& sd = SetData.ChangeValue(SetData.Length());
      ChFiDS_CommonPoint& CP1 = sd->ChangeVertexLastOnS1();
      if(CP1.IsOnArc() && Spine->LastStatus() == ChFiDS_OnSame) {
	intl = !SearchFace(Spine,CP1,F1,FBID);
      }
      else intl = Standard_False;
    }
  }
  else {
    
    // Parsing of domains by increasing parameters,
    // if there is a 2d circle on a plane, one goes on 2D line of opposite face.
    Standard_Real period1 = 0., period2 = 0.;
    if(ll1.IsPeriodic()) {
      if(!Tri(H2,iH2,Ind2,wref,0.,pitol,Nb2)) return 0;
      period1 = ll1.Period();
      if(!Tri(H1,iH1,Ind1,wref,period1,pitol,Nb1)) return 0;
    }
    else{
      if(!Tri(H1,iH1,Ind1,wref,0.,pitol,Nb1)) return 0;
      if(ll2.IsPeriodic()) { period2 = ll2.Period(); }
      if(!Tri(H2,iH2,Ind2,wref,period2,pitol,Nb2)) return 0;
    }
    
    
    // Filling of SurfData
    TColStd_SequenceOfInteger ion1, ion2;
    for(Standard_Integer i = 1; i <= Nb1; i++) {
      const HatchGen_Domain& Dom1 = H1.Domain(iH1,Ind1(i));
      Standard_Integer nbcoup1 = 1;
      Standard_Boolean acheval1 = AdjustParam(Dom1,f1,l1,wref,period1,pitol);
      if(acheval1) nbcoup1 = 2;
      for (Standard_Integer icoup1 = 1; icoup1 <= nbcoup1; icoup1++) {
	for(Standard_Integer j = 1; j <= Nb2; j++) {
	  const HatchGen_Domain& Dom2 = H2.Domain(iH2,j);
	  Standard_Integer nbcoup2 = 1;
	  Standard_Boolean acheval2 = 
	    AdjustParam(Dom2,f2,l2,wref,period2,pitol);
	  if(acheval2) nbcoup2 = 2;
	  for (Standard_Integer icoup2 = 1; icoup2 <= nbcoup2; icoup2++) {
	    if(f2 <= l1 && f1 <= l2) {
	      if (f1 >= f2 - tol2d)
		FillSD(DStr,CD,M1,Dom1,f1,1,1,pitol,bout1);
	      if (f2 >= f1 - tol2d)
		FillSD(DStr,CD,M2,Dom2,f2,1,2,pitol,bout1);
	      if (l1 >= l2 - tol2d)
		FillSD(DStr,CD,M2,Dom2,l2,0,2,pitol,bout2);
	      if (l2 >= l1 - tol2d)
		FillSD(DStr,CD,M1,Dom1,l1,0,1,pitol,bout2);
	      SetData.Append(CD);
	      CD = CpSD(DStr,CD);
	      ion1.Append(i);
	      ion2.Append(j);
	    }
	    f2 += period2;
	    l2 += period2;
	  }
	}
	f1 += period1;
	l1 += period1;
      }
    }
    
    // Processing of extensions.
    // Do not truncate, otherwise, problems of intersection for PerformCorner
    // -----------------------------------------------------------------
    // After call of SplitKPart in PerformSetOfKPart, spines have been 
    // extended to the extremities by methods Extent to permit 
    // intersections. Extensions of  SurfData are preserved.
    
    if(intf) {
      // We are at the beginning of the spine
      //-------------------------
      Standard_Integer ifirst = 0;
      Standard_Real dist = RealLast(), ptg, dsp; 
      const BRepAdaptor_Curve& ed = Spine->CurrentElementarySpine(Iedge);
      for (Standard_Integer i1 = 1; i1 <= SetData.Length(); i1++) {
	Handle(ChFiDS_SurfData)& CD1 = SetData.ChangeValue(i1);
	ChFiDS_CommonPoint& CP1 = CD1->ChangeVertexFirstOnS1();
	ChFiDS_CommonPoint& CP2 = CD1->ChangeVertexFirstOnS2();
	if(CP1.IsOnArc()&&!SearchFace(Spine,CP1,F1,FBID)) {
	  ptg = CD1->InterferenceOnS1().FirstParameter();
	  dsp = ComputeAbscissa(ed,ptg);
	  if(Abs(dsp) < dist) {
            ifirst = i1;
            dist = Abs(dsp);
          }
	}
	else if(CP2.IsOnArc()&&!SearchFace(Spine,CP2,F2,FBID)) {
	  ptg = CD1->InterferenceOnS2().FirstParameter();
	  dsp = ComputeAbscissa(ed,ptg);
	  if(Abs(dsp) < dist) {
            ifirst = i1;
            dist = Abs(dsp);
          }
	}
      }
      if (ifirst>1) {
	SetData.Remove(1,ifirst-1);
	ion1.Remove(1,ifirst-1);
	ion2.Remove(1,ifirst-1);
      }
      if(SetData.IsEmpty()) return Standard_False;
      Handle(ChFiDS_SurfData)& CD2 = SetData.ChangeValue(1);
      ChFiDS_CommonPoint& CP1 = CD2->ChangeVertexFirstOnS1();
      ChFiDS_CommonPoint& CP2 = CD2->ChangeVertexFirstOnS2();
      ChFiDS_CommonPoint sov; 
      
      if(CP1.IsOnArc() && CP2.IsOnArc()) {
	intf = !SearchFace(Spine,CP1,F1,FBID) && !SearchFace(Spine,CP2,F2,FBID);
      }
      else if(CP1.IsOnArc()) {
	sov = CP2;
	if(!SearchFace(Spine,CP1,F1,FBID)) {
	  FillSD(DStr,CD2,M2,H2.Domain(iH2,Ind2(ion2.First())),
		 H2.Domain(iH2,Ind2(ion2.First())).FirstPoint().Parameter(),1,2,pitol,bout1);
	  if(!CP2.IsOnArc() || (CP2.IsOnArc() && SearchFace(Spine,CP2,F2,FBID))) {
	    CP2 = sov;
	    if(Spine->FirstStatus() != ChFiDS_OnSame) {
	      CD2->ChangeInterference(2).
		SetParameter(CD2->Interference(1).Parameter(1),1);
	      intf = Standard_False;
	    }
	  }
	}
	else intf = Standard_False;
      }
      else if(CP2.IsOnArc()) {
	sov = CP1;
	if(!SearchFace(Spine,CP2,F2,FBID)) {
	  FillSD(DStr,CD2,M1,H1.Domain(iH1,Ind1(ion1.First())),
		 H1.Domain(iH1,Ind1(ion1.First())).FirstPoint().Parameter(),1,1,pitol,bout1);
	  if(!CP1.IsOnArc() || (CP1.IsOnArc() && SearchFace(Spine,CP1,F1,FBID))) {
	    CP1 = sov;
	    if(Spine->FirstStatus() != ChFiDS_OnSame) {
	      CD2->ChangeInterference(1).
		SetParameter(CD2->Interference(2).Parameter(1),1);
	      intf = Standard_False;
	    }
	  }
	}
	else intf = Standard_False;
      }
      // select <onS> switcher so that to get on spine params from
      // Interference with a face where both edges at corner are OnSame
      // eap occ293
      if (intf && Spine->FirstStatus() == ChFiDS_OnSame) {
	TopoDS_Edge threeE[3];
	ChFi3d_cherche_element(bout1,support,F1,threeE[0],boutemp);
	ChFi3d_cherche_element(bout1,support,F2,threeE[1],boutemp);
	threeE[2] = support;
	if (ChFi3d_EdgeState(threeE,myEFMap) == ChFiDS_OnSame) 
          onS = 1;
	else
	  onS = 2;
#ifdef OCCT_DEBUG	
	if (threeE[0].IsSame(threeE[1]))
	  cout << "SplitKPart(), wrong corner vertex at switcher search" << endl;
#endif
	cntlFiOnS = 3 - onS;
      }
    }
    if(intl) {
      // we are at the end of the spine
      //-----------------------
      Standard_Integer ilast = 0;
      Standard_Real dist = RealLast(), ptg, dsp; 
      Standard_Real f = Spine->FirstParameter(Iedge);
      Standard_Real l = Spine->LastParameter(Iedge);
      const BRepAdaptor_Curve& ed = Spine->CurrentElementarySpine(Iedge);
      for (Standard_Integer i2 = 1; i2<= SetData.Length(); i2++) {
	Handle(ChFiDS_SurfData)& CD3 = SetData.ChangeValue(i2);
	ChFiDS_CommonPoint& CP1 = CD3->ChangeVertexLastOnS1();
	ChFiDS_CommonPoint& CP2 = CD3->ChangeVertexLastOnS2();
	if(CP1.IsOnArc()&&!SearchFace(Spine,CP1,F1,FBID)) {
	  ptg = CD3->InterferenceOnS1().LastParameter();
	  dsp = -ComputeAbscissa(ed,ptg) - f + l;
	  if(Abs(dsp) < dist) {
            ilast = i2;
            dist = Abs(dsp);
          }
	}
	else if(CP2.IsOnArc()&&!SearchFace(Spine,CP2,F2,FBID)) {
	  ptg = CD3->InterferenceOnS2().LastParameter();
	  dsp = -ComputeAbscissa(ed,ptg) - f + l;
	  if(Abs(dsp) < dist) {
            ilast = i2;
            dist = Abs(dsp);
          }
	}
      }
      Standard_Integer lll = SetData.Length();
      if (ilast<lll) {
	SetData.Remove(ilast+1, lll);
	ion1.Remove(ilast+1, lll);
	ion2.Remove(ilast+1, lll);
      }
      if(SetData.IsEmpty()) return Standard_False;
      Handle(ChFiDS_SurfData)& CD4 = SetData.ChangeValue(SetData.Length());
      ChFiDS_CommonPoint& CP1 = CD4->ChangeVertexLastOnS1();
      ChFiDS_CommonPoint& CP2 = CD4->ChangeVertexLastOnS2();
      ChFiDS_CommonPoint sov; 
      if(CP1.IsOnArc() && CP2.IsOnArc()) {
	intl = !SearchFace(Spine,CP1,F1,FBID) && !SearchFace(Spine,CP2,F2,FBID);
      }
      else if(CP1.IsOnArc()) {
	sov = CP2;
	if(!SearchFace(Spine,CP1,F1,FBID)) {
	  FillSD(DStr,CD4,M2,H2.Domain(iH2,Ind2(ion2.Last())),
		 H2.Domain(iH2,Ind2(ion2.Last())).SecondPoint().Parameter(),0,2,pitol,bout2);
	  if(!CP2.IsOnArc() || (CP2.IsOnArc() && SearchFace(Spine,CP2,F2,FBID))) {
	    CP2 = sov;
	    if(Spine->LastStatus() != ChFiDS_OnSame) {
	      CD4->ChangeInterference(2).
		SetParameter(CD4->Interference(1).Parameter(0),0);
	      intl = Standard_False;
	    }
	  }
	}
	else intl = Standard_False;
      }
      else if(CP2.IsOnArc()) {
	sov = CP1;
	if(!SearchFace(Spine,CP2,F2,FBID)) {
	  FillSD(DStr,CD4,M1,H1.Domain(iH1,Ind1(ion1.Last())),
		 H1.Domain(iH1,Ind1(ion1.Last())).SecondPoint().Parameter(),0,1,pitol,bout2);
	  if(!CP1.IsOnArc() || (CP1.IsOnArc() && SearchFace(Spine,CP1,F1,FBID))) {
	    CP1 = sov;
	    if(Spine->LastStatus() != ChFiDS_OnSame) {
	      CD4->ChangeInterference(1).
		SetParameter(CD4->Interference(2).Parameter(0),0);
	      intl = Standard_False;
	    }
	  }
	}
	else intl = Standard_False;
      }
      
      // select <onS> switcher so that to get on spine params from
      // Interference with a face where both edges at corner are OnSame
      // eap occ293
      if (intl && Spine->LastStatus() == ChFiDS_OnSame) {
        TopoDS_Edge threeE[3];
        ChFi3d_cherche_element(bout2,support,F1,threeE[0],boutemp);
        ChFi3d_cherche_element(bout2,support,F2,threeE[1],boutemp);
        threeE[2] = support;
        if (ChFi3d_EdgeState(threeE,myEFMap) == ChFiDS_OnSame) 
          onS = 1;
        else
          onS = 2;
#ifdef OCCT_DEBUG
        if (threeE[0].IsSame(threeE[1]))
          cout << "SplitKPart(), wrong corner vertex at switcher search" << endl;
#endif
	cntlFiOnS = 3 - onS;
      }
    }
  }

  if(!intf) { 
    // SurfData are entirely suspended before the beginning of the edge.
    Standard_Boolean okdoc = SetData.IsEmpty();
    Standard_Integer i = 1;
    while(!okdoc) {
      Handle(ChFiDS_SurfData)& CD5 = SetData.ChangeValue(i);
      Standard_Real ltg = CD5->Interference(onS).LastParameter();
      Standard_Real Nl = ComputeAbscissa(Spine->CurrentElementarySpine(Iedge),ltg);
      if (Nl < -tolesp) SetData.Remove(i);
      else i++;
      okdoc = (SetData.IsEmpty() || i > SetData.Length());
    }
  }
  if(!intl) { 
    // SurfData are entirely suspended after the end of the edge.
    Standard_Boolean okdoc = SetData.IsEmpty();
    Standard_Integer i = 1;
    while(!okdoc) {
      Handle(ChFiDS_SurfData)& CD6 = SetData.ChangeValue(i);
      Standard_Real ftg = CD6->Interference(onS).FirstParameter();
      Standard_Real f = Spine->FirstParameter(Iedge);
      Standard_Real l = Spine->LastParameter(Iedge);
      Standard_Real Nl = ComputeAbscissa(Spine->CurrentElementarySpine(Iedge),ftg);
      if (Nl > (l - f + tolesp)) SetData.Remove(i);
      else i++;
      okdoc = (SetData.IsEmpty() || i > SetData.Length());
    }
  }
  // Add parameters of the spine on SurfDatas.
//  for (Standard_Integer i = 1; i <= SetData.Length(); i++) {
  Standard_Integer i;
  for ( i = 1; i <= SetData.Length(); i++) {
    Standard_Boolean pokdeb = 0, pokfin = 0;
    Handle(ChFiDS_SurfData)& CD7 = SetData.ChangeValue(i);
    Standard_Real ftg = CD7->Interference(onS).FirstParameter();
    Standard_Real ltg = CD7->Interference(onS).LastParameter();
    Standard_Real fsp = ParamOnSpine(DStr,ftg,CD7,Spine,Iedge,intf,intl,tolesp,pokdeb);
    if(!pokdeb) fsp = ResetProl(DStr,CD7,Spine,Iedge,1);
    Standard_Real lsp = ParamOnSpine(DStr,ltg,CD7,Spine,Iedge,intf,intl,tolesp,pokfin);
    if(!pokfin) lsp = ResetProl(DStr,CD7,Spine,Iedge,0);
    if(Spine->IsPeriodic() && Iedge == Spine->NbEdges() && lsp < fsp) { 
      lsp += Spine->Period();
    }
    else if(Spine->IsPeriodic() && Iedge == 1 && lsp < fsp) { 
      fsp -= Spine->Period();
    }
    CD7->FirstSpineParam(fsp);
    CD7->LastSpineParam (lsp);
  }

  if (intf && !SetData.IsEmpty()) {
    // extension of the spine
    Spine->SetFirstParameter(SetData.First()->FirstSpineParam());
  }
  else {
    // Trnncation at the beginning.
    for (i = 1; i <= SetData.Length(); i++) {
      Handle(ChFiDS_SurfData)& CD8 = SetData.ChangeValue(i);
      Standard_Real fsp = CD8->FirstSpineParam();
      Standard_Real lsp = CD8->LastSpineParam();
      if (lsp > Spine->FirstParameter(Iedge)) {
	if (fsp > Spine->FirstParameter(Iedge)) {
	  break;
	}
	else {
	  Trunc(CD8,Spine,S1,S2,Iedge,1,cntlFiOnS);
	  break;
	}
      }
    }
    if (i > 1 ) {
      SetData.Remove(1,i-1);
    }
  }


  if (intl && !SetData.IsEmpty()) {
    // extension of the spine
    Spine->SetLastParameter(SetData.Last()->LastSpineParam()); 
  }
  else {
    // Truncation at the end.
    for (i = SetData.Length(); i >= 1; i--) {
      Handle(ChFiDS_SurfData)& CD9 = SetData.ChangeValue(i);
      Standard_Real fsp = CD9->FirstSpineParam();
      Standard_Real lsp = CD9->LastSpineParam();
      if (fsp < Spine->LastParameter(Iedge)) {
	if (lsp < Spine->LastParameter(Iedge)) {
	  break;
	}
	else {
	  Trunc(CD9,Spine,S1,S2,Iedge,0,cntlFiOnS);
	  break;
	}
      }
    }
    if (i < SetData.Length()) {
	SetData.Remove(i+1,SetData.Length());
    }
  }
#ifdef OCCT_DEBUG
  if(ChFi3d_GettraceDRAWFIL()) {
    for (i = 1; i <= SetData.Length(); i++) {
      ChFi3d_CheckSurfData(DStr,SetData.Value(i));
    }
  }
#endif
  return Standard_True;
}
