// Created on: 1995-04-25
// Created by: Modelistation
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

// Modified:    MPS :  (10-04-97) portage WNT pour GetFilletShape

#include <Adaptor3d_TopolTool.hxx>
#include <Blend_Point.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepBlend_ConstRad.hxx>
#include <BRepBlend_ConstRadInv.hxx>
#include <BRepBlend_CurvPointRadInv.hxx>
#include <BRepBlend_EvolRad.hxx>
#include <BRepBlend_EvolRadInv.hxx>
#include <BRepBlend_Line.hxx>
#include <BRepBlend_RstRstConstRad.hxx>
#include <BRepBlend_RstRstEvolRad.hxx>
#include <BRepBlend_SurfCurvConstRadInv.hxx>
#include <BRepBlend_SurfCurvEvolRadInv.hxx>
#include <BRepBlend_SurfPointConstRadInv.hxx>
#include <BRepBlend_SurfPointEvolRadInv.hxx>
#include <BRepBlend_SurfRstConstRad.hxx>
#include <BRepBlend_SurfRstEvolRad.hxx>
#include <BRepBlend_Walking.hxx>
#include <ChFi3d_Builder_0.hxx>
#include <ChFi3d_FilBuilder.hxx>
#include <ChFi3d_SearchSing.hxx>
#include <ChFiDS_ErrorStatus.hxx>
#include <ChFiDS_FilSpine.hxx>
#include <ChFiDS_HData.hxx>
#include <ChFiDS_HElSpine.hxx>
#include <ChFiDS_ListIteratorOfListOfStripe.hxx>
#include <ChFiDS_ListIteratorOfRegularities.hxx>
#include <ChFiDS_ListOfStripe.hxx>
#include <ChFiDS_Regul.hxx>
#include <ChFiDS_SecHArray1.hxx>
#include <ChFiDS_Spine.hxx>
#include <ChFiDS_Stripe.hxx>
#include <ChFiDS_SurfData.hxx>
#include <ElSLib.hxx>
#include <Geom_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <gp_XY.hxx>
#include <Law_Composite.hxx>
#include <Law_Function.hxx>
#include <math_FunctionRoot.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Failure.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Surface.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#ifdef OCCT_DEBUG
#include <OSD_Chronometer.hxx>
extern Standard_Boolean ChFi3d_GettraceCHRON();
extern Standard_Real  t_computedata ,t_completedata; 
                    
extern void ChFi3d_InitChron(OSD_Chronometer& ch);
extern void ChFi3d_ResultChron(OSD_Chronometer & ch,Standard_Real& time);
#endif

static Standard_Real MaxRad(const Handle(ChFiDS_FilSpine)& fsp,
	
			    const TopoDS_Edge& E) 
{
  Standard_Integer IE = fsp->Index(E);

  // 1: case of constant R
  if (fsp->IsConstant(IE))
    return (fsp->Radius(IE));
  else // 2,3: case of sequence ParAndRad and(or) Laws
    return (fsp->MaxRadFromSeqAndLaws());

/*
 Handle(ChFiDS_HElSpine) HGuide = fsp->ElSpine(IE);
 Standard_Real la = HGuide->LastParameter(), fi = HGuide->FirstParameter();
 Standard_Real longueur = la - fi,  temp, w;
//#ifndef OCCT_DEBUG
 Standard_Real radiussect = 0.;
//#else
// Standard_Real radiussect;
//#endif
 Handle(Law_Composite) lc = fsp->Law(HGuide);
 for(Standard_Integer i = 0; i <= 5; i++){ 
   w = fi + i*longueur*0.2;
   temp = lc->Value(w);
   if(temp>radiussect) radiussect = temp;
 }
 return  radiussect;
*/
}

static void SimulParams(const Handle(ChFiDS_HElSpine)& HGuide,
			const Handle(ChFiDS_FilSpine)& fsp,
			Standard_Real&                 MaxStep,
			Standard_Real&                 Fleche)
{
  Standard_Real la = HGuide->LastParameter(), fi = HGuide->FirstParameter();
  Standard_Real longueur = la - fi;
  MaxStep = longueur * 0.05;
  Standard_Real w;
  //gp_Pnt Pbid;
  //gp_Vec d1,d2;
  Standard_Real radiussect;
  if(fsp->IsConstant()) radiussect = fsp->Radius();
  else {
    radiussect = 0.;
    Handle(Law_Composite) lc = fsp->Law(HGuide);
    for(Standard_Integer i = 0; i <= 5; i++){ 
      w = fi + i*longueur*0.2;
      Standard_Real temp = lc->Value(w);
      if(temp>radiussect) radiussect = temp;
    }
  }
  Fleche = radiussect * 0.05;
}

//=======================================================================
//function : ChFi3d_FilBuilder
//purpose  : 
//=======================================================================

ChFi3d_FilBuilder::ChFi3d_FilBuilder(const TopoDS_Shape& S,
				     const ChFi3d_FilletShape FShape,
				     const Standard_Real Ta):
				     ChFi3d_Builder(S, Ta)
{
  SetFilletShape(FShape);
}

//=======================================================================
//function : SetFilletShape
//purpose  : 
//=======================================================================

void ChFi3d_FilBuilder::SetFilletShape(const ChFi3d_FilletShape FShape)
{
  switch (FShape) {
  case ChFi3d_Rational:
    myShape = BlendFunc_Rational;
    break;
  case ChFi3d_QuasiAngular:
    myShape = BlendFunc_QuasiAngular;
    break;
  case ChFi3d_Polynomial:
    myShape = BlendFunc_Polynomial;
    break;
  }
}

//=======================================================================
//function : GetFilletShape
//purpose  : 
//=======================================================================

ChFi3d_FilletShape ChFi3d_FilBuilder::GetFilletShape() const
{
  ChFi3d_FilletShape filshape = ChFi3d_Rational; //  need to set default value
  switch (myShape) {
  case BlendFunc_Rational:
    filshape= ChFi3d_Rational;
    break;
  case BlendFunc_QuasiAngular:
    filshape= ChFi3d_QuasiAngular;
    break;
  case BlendFunc_Polynomial:
    filshape= ChFi3d_Polynomial;
    break;
  default:
    break;
  }
  return filshape;
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  ChFi3d_FilBuilder::Add(const TopoDS_Edge& E)
{
  if(!Contains(E) && myEFMap.Contains(E)){
    Handle(ChFiDS_Stripe) Stripe = new ChFiDS_Stripe();
    Handle(ChFiDS_Spine)& Sp = Stripe->ChangeSpine();
    Sp = new ChFiDS_FilSpine(tolesp);
    Handle(ChFiDS_FilSpine) Spine = Handle(ChFiDS_FilSpine)::DownCast(Sp);

    TopoDS_Edge E_wnt = E;
    E_wnt.Orientation(TopAbs_FORWARD);
    Spine->SetEdges(E_wnt);
    if(PerformElement(Spine)){
      PerformExtremity(Spine);
      Spine->Load();
      myListStripe.Append(Stripe);
    }
  }
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  ChFi3d_FilBuilder::Add(const Standard_Real Radius, const TopoDS_Edge& E)
{
  Add(E);
  Standard_Integer IC = Contains(E);
  if (IC)
    SetRadius( Radius, IC, E );
}

//=======================================================================
//function : SetRadius
//purpose  : 
//=======================================================================

void  ChFi3d_FilBuilder::SetRadius(const Handle(Law_Function)& C, 
				   const Standard_Integer IC,
				   const Standard_Integer IinC)
{
  if(IC <= NbElements()) {
    Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Value(IC));
    fsp->SetRadius(C, IinC);
  }
}

//=======================================================================
//function : IsConstant
//purpose  : 
//=======================================================================

Standard_Boolean ChFi3d_FilBuilder::IsConstant(const Standard_Integer IC)
{
  if(IC <= NbElements()) {
    Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Value(IC));
    return fsp->IsConstant();
  }
  return 0;
}

//=======================================================================
//function : Radius
//purpose  : 
//=======================================================================

Standard_Real ChFi3d_FilBuilder::Radius(const Standard_Integer IC)
{
  if(IC <= NbElements()) {
    Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Value(IC));
    return fsp->Radius();
  }
  return -1.;
}

//=======================================================================
//function : ResetContour
//purpose  : 
//=======================================================================

void  ChFi3d_FilBuilder::ResetContour(const Standard_Integer IC)
{
  if(IC <= NbElements()) {
    Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Value(IC));
    fsp->Reset(Standard_True);
  }
}

//=======================================================================
//function : SetRadius
//purpose  : 
//=======================================================================

void  ChFi3d_FilBuilder::SetRadius(const Standard_Real    Radius, 
				   const Standard_Integer IC,
				   const TopoDS_Edge&     E)
{
  if(IC <= NbElements()) {
    Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Value(IC));
    fsp->SetRadius(Radius, E);
  }
}

//=======================================================================
//function : UnSet
//purpose  : 
//=======================================================================

void  ChFi3d_FilBuilder::UnSet(const Standard_Integer IC,
			       const TopoDS_Edge&     E)
{
  if(IC <= NbElements()) {
    Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Value(IC));
    fsp->UnSetRadius(E);
  }
}

//=======================================================================
//function : SetRadius
//purpose  : 
//=======================================================================

void  ChFi3d_FilBuilder::SetRadius(const Standard_Real    Radius, 
				   const Standard_Integer IC,
				   const TopoDS_Vertex&   V)
{
  if(IC <= NbElements()) {
    Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Value(IC));
    fsp->SetRadius(Radius, V);
  }
}

//=======================================================================
//function : UnSet
//purpose  : 
//=======================================================================

void  ChFi3d_FilBuilder::UnSet(const Standard_Integer IC,
			       const TopoDS_Vertex&   V)
{
  if(IC <= NbElements()) {
    Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Value(IC));
    fsp->UnSetRadius(V);
  }
}

//=======================================================================
//function : SetRadius
//purpose  : 
//=======================================================================

void  ChFi3d_FilBuilder::SetRadius(const gp_XY&           UandR, 
				   const Standard_Integer IC,
				   const Standard_Integer IinC)
{
  if(IC <= NbElements()) {
    Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Value(IC));
    fsp->SetRadius( UandR, IinC );
  }
}

//=======================================================================
//function : IsConstant
//purpose  : 
//=======================================================================

Standard_Boolean ChFi3d_FilBuilder::IsConstant(const Standard_Integer IC,
					       const TopoDS_Edge&     E)
{
  if(IC <= NbElements()) {
    Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Value(IC));
    return fsp->IsConstant(fsp->Index(E));
  }
  return 0;
}

//=======================================================================
//function : Radius
//purpose  : 
//=======================================================================

Standard_Real ChFi3d_FilBuilder::Radius(const Standard_Integer IC,
					const TopoDS_Edge&     E)
{
  if(IC <= NbElements()) {
    Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Value(IC));
    return fsp->Radius(E);
  }
  return -1.;
}

//=======================================================================
//function : GetBounds
//purpose  : 
//=======================================================================

Standard_Boolean ChFi3d_FilBuilder::GetBounds(const Standard_Integer IC,
					      const TopoDS_Edge&     E,
					      Standard_Real&         F,
					      Standard_Real&         L)
{
  if(IC <= NbElements()) {
    Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Value(IC));
    Handle(Law_Function)& loi = fsp->ChangeLaw(E);
    if(!loi.IsNull()){
      loi->Bounds(F,L);
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : GetLaw
//purpose  : 
//=======================================================================

Handle(Law_Function) ChFi3d_FilBuilder::GetLaw(const Standard_Integer IC,
					       const TopoDS_Edge&     E)
{
  if(IC <= NbElements()) {
    Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Value(IC));
    return fsp->ChangeLaw(E);
  }
  return Handle(Law_Function)();
}

//=======================================================================
//function : SetLaw
//purpose  : 
//=======================================================================

void ChFi3d_FilBuilder::SetLaw(const Standard_Integer      IC,
			       const TopoDS_Edge&          E,
			       const Handle(Law_Function)& L)
{
  // Check if it is necessary to check borders!
  if(IC <= NbElements()) {
    Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Value(IC));
    fsp->ChangeLaw(E) = L;
  }
}

//=======================================================================
//function : Simulate
//purpose  : 
//=======================================================================

void ChFi3d_FilBuilder::Simulate (const Standard_Integer IC)
{
#ifdef OCCT_DEBUG
  if(ChFi3d_GettraceCHRON()){
    simul.Reset();elspine.Reset();chemine.Reset();
    simul.Start();
  }
#endif
  ChFiDS_ListIteratorOfListOfStripe itel;
  Standard_Integer i = 1;
  for (itel.Initialize(myListStripe);itel.More(); itel.Next(), i++) {
    if(i == IC){
      PerformSetOfSurf(itel.Value(), Standard_True);
      break;
    }
  }
#ifdef OCCT_DEBUG
  if(ChFi3d_GettraceCHRON()){
    simul.Stop();
    cout<<"Total simulation time : ";
    simul.Show();
    cout<<"Spine construction time : ";
    elspine.Show();
    cout<<"and process time : ";
    chemine.Show();
  }
#endif
}

//=======================================================================
//function : NbSurf
//purpose  : 
//=======================================================================

Standard_Integer ChFi3d_FilBuilder::NbSurf (const Standard_Integer IC) const 
{
  ChFiDS_ListIteratorOfListOfStripe itel;
  Standard_Integer i = 1;
  for (itel.Initialize(myListStripe);itel.More(); itel.Next(), i++) {
    if(i == IC){
      return itel.Value()->SetOfSurfData()->Length();
    }
  }
  return 0;
}

//=======================================================================
//function : Sect
//purpose  : 
//=======================================================================

Handle(ChFiDS_SecHArray1) ChFi3d_FilBuilder::Sect (const Standard_Integer IC,
						   const Standard_Integer IS) const 
{
  ChFiDS_ListIteratorOfListOfStripe itel;
  Standard_Integer i = 1;
  Handle(ChFiDS_SecHArray1) res;
  for (itel.Initialize(myListStripe);itel.More(); itel.Next(), i++) {
    if(i == IC){
      Handle(Standard_Transient) bid = itel.Value()->SetOfSurfData()->Value(IS)->Simul();
      res = Handle(ChFiDS_SecHArray1)::DownCast(bid);
      return res;
    }
  }
  return Handle(ChFiDS_SecHArray1)();
}

//=======================================================================
//function : SimulKPart
//purpose  : 
//=======================================================================

void  ChFi3d_FilBuilder::SimulKPart(const Handle(ChFiDS_SurfData)& SD) const 
{
  TopOpeBRepDS_DataStructure& DStr = myDS->ChangeDS();
  Handle(Geom_Surface) S = DStr.Surface(SD->Surf()).Surface();
  gp_Pnt2d p1f = SD->InterferenceOnS1().PCurveOnSurf()->
    Value(SD->InterferenceOnS1().FirstParameter());
  gp_Pnt2d p1l = SD->InterferenceOnS1().PCurveOnSurf()->
    Value(SD->InterferenceOnS1().LastParameter());
  gp_Pnt2d p2f = SD->InterferenceOnS2().PCurveOnSurf()->
    Value(SD->InterferenceOnS2().FirstParameter());
  gp_Pnt2d p2l = SD->InterferenceOnS2().PCurveOnSurf()->
    Value(SD->InterferenceOnS2().LastParameter());
  GeomAdaptor_Surface AS(S);
  Handle(ChFiDS_SecHArray1) sec;
  Standard_Real u1,v1,u2,v2;
  GeomAbs_SurfaceType typ = AS.GetType();
  switch (typ){
  case GeomAbs_Cylinder: 
    {
      u1 = p1f.X();
      u2 = p2f.X();
      v1 = Max(p1f.Y(),p2f.Y());
      v2 = Min(p1l.Y(),p2l.Y());
      sec = new ChFiDS_SecHArray1(1,2);
      gp_Cylinder Cy = AS.Cylinder();
      ChFiDS_CircSection& sec1 = sec->ChangeValue(1);
      ChFiDS_CircSection& sec2 = sec->ChangeValue(2);
      sec1.Set(ElSLib::CylinderVIso(Cy.Position(), Cy.Radius(), v1), u1, u2);  
      sec2.Set(ElSLib::CylinderVIso(Cy.Position(), Cy.Radius(), v2), u1, u2);  
    }
    break;
  case GeomAbs_Torus:
    {
      v1 = p1f.Y();
      v2 = p2f.Y();
      u1 = Max(p1f.X(),p2f.X());
      u2 = Min(p1l.X(),p2l.X());
      Standard_Real ang = (u2-u1);
      gp_Torus To = AS.Torus();
      Standard_Real majr = To.MajorRadius(), minr = To.MinorRadius();
      Standard_Integer n = (Standard_Integer) (36.*ang/M_PI + 1);
      if(n<2) n = 2;
      sec = new ChFiDS_SecHArray1(1, n);
      for (Standard_Integer i = 1; i <= n; i++) {
	ChFiDS_CircSection& isec = sec->ChangeValue(i);
	Standard_Real u = u1 + (i - 1)*(u2 - u1)/(n-1);
	isec.Set(ElSLib::TorusUIso(To.Position(), majr, minr, u), v1, v2);  
      }
    }
    break;
  case GeomAbs_Sphere:
    {
      v1 = p1f.Y();
      v2 = p2f.Y();
      u1 = Max(p1f.X(),p2f.X());
      u2 = Min(p1l.X(),p2l.X());
      Standard_Real ang = (u2-u1);
      gp_Sphere Sp = AS.Sphere();
      Standard_Real rad = Sp.Radius();
      Standard_Integer n = (Standard_Integer) (36.*ang/M_PI + 1);
      if(n<2) n = 2;
      sec = new ChFiDS_SecHArray1(1, n);
      for (Standard_Integer i = 1; i <= n; i++) {
	ChFiDS_CircSection& isec = sec->ChangeValue(i);
	Standard_Real u = u1 + (i - 1)*(u2 - u1)/(n-1);
	isec.Set(ElSLib::SphereUIso(Sp.Position(), rad, u), v1, v2);  
      }
    }
    break;
  default:
    break;
  }
  SD->SetSimul(sec);
}


//=======================================================================
//function : SimulSurf
//purpose  : 
//=======================================================================

Standard_Boolean  
ChFi3d_FilBuilder::SimulSurf(Handle(ChFiDS_SurfData)&            Data,
			     const Handle(ChFiDS_HElSpine)&      HGuide,
			     const Handle(ChFiDS_Spine)&         Spine,
			     const Standard_Integer              Choix,
			     const Handle(BRepAdaptor_HSurface)& S1,
			     const Handle(Adaptor3d_TopolTool)&    I1,
			     const Handle(BRepAdaptor_HSurface)& S2,
			     const Handle(Adaptor3d_TopolTool)&    I2,
			     const Standard_Real                 TolGuide,
			     Standard_Real&                      First,
			     Standard_Real&                      Last,
			     const Standard_Boolean              Inside,
			     const Standard_Boolean              Appro,
			     const Standard_Boolean              Forward,
			     const Standard_Boolean              RecOnS1,
			     const Standard_Boolean              RecOnS2,
			     const math_Vector&                  Soldep,
			     Standard_Integer&                   intf,
			     Standard_Integer&                   intl)
{
  Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Spine);
  if(fsp.IsNull()) throw Standard_ConstructionError("SimulSurf : this is not the spine of the fillet");
  Handle(BRepBlend_Line) lin;
#ifdef OCCT_DEBUG
//  TopAbs_Orientation Or = S1->ChangeSurface().Face().Orientation();
#endif
  // Flexible parameters!!!
  Standard_Real locfleche, MaxStep;
  SimulParams(HGuide,fsp,MaxStep,locfleche);
  Handle(ChFiDS_SecHArray1) sec;
  gp_Pnt2d pf1,pl1,pf2,pl2;

  Standard_Real PFirst = First;
  if(intf) First = fsp->FirstParameter(1);
  if(intl) Last = fsp->LastParameter(fsp->NbEdges());
  if (fsp->IsConstant()) {
    BRepBlend_ConstRad Func(S1,S2,HGuide);
    BRepBlend_ConstRadInv FInv(S1,S2,HGuide);
    Func.Set(fsp->Radius(),Choix);
    FInv.Set(fsp->Radius(),Choix);
    Func.Set(myShape);
    done = SimulData(Data,HGuide,lin,S1,I1 ,
		     S2,I2,Func,FInv,PFirst,MaxStep,locfleche,
		     TolGuide,First,Last,Inside,Appro,Forward,
		     Soldep,4,RecOnS1,RecOnS2);
    if(!done) return Standard_False;
    Standard_Integer nbp = lin->NbPoints();
    sec = new ChFiDS_SecHArray1(1,nbp);
    for(Standard_Integer i = 1; i <= nbp; i++){
      ChFiDS_CircSection& isec = sec->ChangeValue(i);
      Standard_Real u1,v1,u2,v2,w,p1,p2;
      gp_Circ ci;
      const Blend_Point& p = lin->Point(i);
      p.ParametersOnS1(u1,v1);
      p.ParametersOnS2(u2,v2);
      w = p.Parameter();
      Func.Section(w,u1,v1,u2,v2,p1,p2,ci);
      isec.Set(ci,p1,p2);
      if(i == 1) {pf1.SetCoord(u1,v1); pf2.SetCoord(u2,v2);} 
      if(i == nbp) {pl1.SetCoord(u1,v1); pl2.SetCoord(u2,v2);} 
    }
  }
  else{
    BRepBlend_EvolRad Func(S1,S2,HGuide,fsp->Law(HGuide));
    BRepBlend_EvolRadInv FInv(S1,S2,HGuide,fsp->Law(HGuide));
    Func.Set(Choix);
    FInv.Set(Choix);
    Func.Set(myShape);
    done = SimulData(Data,HGuide,lin,S1,I1 ,
		     S2,I2,Func,FInv,PFirst,MaxStep,locfleche,
		     TolGuide,First,Last,Inside,Appro,Forward,
		     Soldep,4,RecOnS1,RecOnS2);
    if(!done) return Standard_False;
    Standard_Integer nbp = lin->NbPoints();
    sec = new ChFiDS_SecHArray1(1,nbp);
    for(Standard_Integer i = 1; i <= nbp; i++){
      ChFiDS_CircSection& isec = sec->ChangeValue(i);
      Standard_Real u1,v1,u2,v2,w,p1,p2;
      gp_Circ ci;
      const Blend_Point& p = lin->Point(i);
      p.ParametersOnS1(u1,v1);
      p.ParametersOnS2(u2,v2);
      w = p.Parameter();
      Func.Section(w,u1,v1,u2,v2,p1,p2,ci);
      isec.Set(ci,p1,p2);
      if(i == 1) {pf1.SetCoord(u1,v1); pf2.SetCoord(u2,v2);} 
      if(i == nbp) {pl1.SetCoord(u1,v1); pl2.SetCoord(u2,v2);} 
    }
  }
  Data->SetSimul(sec);
  Data->Set2dPoints(pf1,pl1,pf2,pl2);
  ChFi3d_FilCommonPoint(lin->StartPointOnFirst(),lin->TransitionOnS1(),
			Standard_True, Data->ChangeVertexFirstOnS1(),tolesp);
  ChFi3d_FilCommonPoint(lin->EndPointOnFirst(),lin->TransitionOnS1(),
			Standard_False,Data->ChangeVertexLastOnS1(),tolesp);
  ChFi3d_FilCommonPoint(lin->StartPointOnSecond(),lin->TransitionOnS2(),
			Standard_True, Data->ChangeVertexFirstOnS2(),tolesp);
  ChFi3d_FilCommonPoint(lin->EndPointOnSecond(),lin->TransitionOnS2(),
			Standard_False, Data->ChangeVertexLastOnS2(),tolesp);
  Standard_Boolean reverse = (!Forward || Inside);
  if(intf && reverse){
    Standard_Boolean ok = Standard_False;
    const ChFiDS_CommonPoint& cp1 = Data->VertexFirstOnS1();
    if(cp1.IsOnArc()){
      TopoDS_Face F1 = S1->ChangeSurface().Face();
      TopoDS_Face bid;
      intf = !SearchFace(Spine,cp1,F1,bid);
      ok = intf != 0;
    }
    const ChFiDS_CommonPoint& cp2 = Data->VertexFirstOnS2();
    if(cp2.IsOnArc() && !ok){
      TopoDS_Face F2 = S2->ChangeSurface().Face();
      TopoDS_Face bid;
      intf = !SearchFace(Spine,cp2,F2,bid);
    }
  }
  if(intl){
    Standard_Boolean ok = 0;
    const ChFiDS_CommonPoint& cp1 = Data->VertexLastOnS1();
    if(cp1.IsOnArc()){
      TopoDS_Face F1 = S1->ChangeSurface().Face();
      TopoDS_Face bid;
      intl = !SearchFace(Spine,cp1,F1,bid);
      ok = intl != 0;
    }
    const ChFiDS_CommonPoint& cp2 = Data->VertexLastOnS2();
    if(cp2.IsOnArc() && !ok){
      TopoDS_Face F2 = S2->ChangeSurface().Face();
      TopoDS_Face bid;
      intl = !SearchFace(Spine,cp2,F2,bid);
    }
  }
  return Standard_True;
}

//=======================================================================
//function : SimulSurf
//purpose  : 
//=======================================================================

void  ChFi3d_FilBuilder::SimulSurf(Handle(ChFiDS_SurfData)&            Data,
				   const Handle(ChFiDS_HElSpine)&      HGuide,
				   const Handle(ChFiDS_Spine)&         Spine,
				   const Standard_Integer              Choix,
				   const Handle(BRepAdaptor_HSurface)& HS1,
				   const Handle(Adaptor3d_TopolTool)&    I1,
				   const Handle(BRepAdaptor_HCurve2d)& PC1,
				   const Handle(BRepAdaptor_HSurface)& HSref1,
				   const Handle(BRepAdaptor_HCurve2d)& PCref1,
				   Standard_Boolean&                   Decroch1,
				   const Handle(BRepAdaptor_HSurface)& HS2,
				   const Handle(Adaptor3d_TopolTool)&    I2,
				   const TopAbs_Orientation            Or2,
				   const Standard_Real                 /*Fleche*/,
				   const Standard_Real                 TolGuide,
				   Standard_Real&                      First,
				   Standard_Real&                      Last,
				   const Standard_Boolean              Inside,
				   const Standard_Boolean              Appro,
				   const Standard_Boolean              Forward,
				   const Standard_Boolean              RecP,
				   const Standard_Boolean              RecS,
				   const Standard_Boolean              RecRst,
				   const math_Vector&                  Soldep)
{
  Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Spine);
  if(fsp.IsNull()) throw Standard_ConstructionError("PerformSurf : this is not the spine of the fillet");
  Handle(BRepBlend_Line) lin;
  
  // Flexible parameters!
  Standard_Real locfleche, MaxStep;
  SimulParams(HGuide,fsp,MaxStep,locfleche);
  Handle(ChFiDS_SecHArray1) sec;
  gp_Pnt2d pf,pl,ppcf,ppcl;
  
  Standard_Real PFirst = First;
  if(fsp->IsConstant()){
    BRepBlend_SurfRstConstRad func(HS2,HS1,PC1,HGuide);
    func.Set(HSref1,PCref1);
    Handle(Adaptor3d_HCurveOnSurface) HC = new Adaptor3d_HCurveOnSurface();
    HC->ChangeCurve().Load(PC1, HS1);
    BRepBlend_SurfCurvConstRadInv finvc(HS2,HC,HGuide);
    BRepBlend_SurfPointConstRadInv finvp(HS2,HGuide);
    BRepBlend_ConstRadInv finv(HS2,HSref1,HGuide);
    finv.Set(Standard_False,PCref1);
    
    Standard_Real rad = fsp->Radius();
    Standard_Integer petitchoix = 1;
    if(Or2 == TopAbs_REVERSED) petitchoix = 3;
    if(Choix%2 == 0) petitchoix++;
    finv.Set(rad,Choix);
    finvc.Set(rad,petitchoix);
    finvp.Set(rad,petitchoix);
    func.Set(rad,petitchoix);
    func.Set(myShape);
    
    done = SimulData(Data,HGuide,lin,HS2,I2,HS1,PC1,I1,Decroch1,
		     func,finv,finvp,finvc,
		     PFirst,MaxStep,locfleche,TolGuide,First,Last,
		     Soldep,4,Inside,Appro,Forward,RecP,RecS,RecRst);
    if(!done) {
      throw Standard_Failure("SimulSurf : Failed process!");
    }
    Standard_Integer nbp = lin->NbPoints();
    sec = new ChFiDS_SecHArray1(1,nbp);
    for(Standard_Integer i = 1; i <= nbp; i++){
      ChFiDS_CircSection& isec = sec->ChangeValue(i);
      Standard_Real u,v,w,param,p1,p2;
      gp_Circ ci;
      const Blend_Point& p = lin->Point(i);
      p.ParametersOnS(u,v);
      w = p.ParameterOnC();
      param = p.Parameter();
      func.Section(param,u,v,w,p1,p2,ci);
      isec.Set(ci,p1,p2);
      if(i == 1) {pf.SetCoord(u,v);p.ParametersOnS2(u,v);ppcf.SetCoord(u,v);} 
      if(i == nbp) {pl.SetCoord(u,v);p.ParametersOnS2(u,v);ppcl.SetCoord(u,v);} 
    }
  }
  else {
    BRepBlend_SurfRstEvolRad func(HS2,HS1,PC1,HGuide,fsp->Law(HGuide));
    Handle(Adaptor3d_HCurveOnSurface) HC = new Adaptor3d_HCurveOnSurface();
    HC->ChangeCurve().Load(PC1, HS1);
    BRepBlend_SurfCurvEvolRadInv finvc(HS2,HC,HGuide,fsp->Law(HGuide));
    BRepBlend_SurfPointEvolRadInv finvp(HS2,HGuide,fsp->Law(HGuide));
    BRepBlend_EvolRadInv finv(HS2,HSref1,HGuide,fsp->Law(HGuide));
    finv.Set(Standard_False,PCref1);
    Standard_Integer petitchoix = 1;
    if(Or2 == TopAbs_REVERSED) petitchoix = 3;
    if(Choix%2 == 0) petitchoix++;
    finv.Set(Choix);
    finvc.Set(petitchoix);
    finvp.Set(petitchoix);
    func.Set(petitchoix);
    func.Set(myShape);
    done = SimulData(Data,HGuide,lin,HS2,I2,HS1,PC1,I1,Decroch1,
		     func,finv,finvp,finvc,
		     PFirst,MaxStep,locfleche,TolGuide,First,Last,
		     Soldep,4,Inside,Appro,Forward,RecP,RecS,RecRst);
    if(!done) throw Standard_Failure("SimulSurf : Fail !");
    Standard_Integer nbp = lin->NbPoints();
    sec = new ChFiDS_SecHArray1(1,nbp);
    for(Standard_Integer i = 1; i <= nbp; i++){
      ChFiDS_CircSection& isec = sec->ChangeValue(i);
      Standard_Real u,v,w,param,p1,p2;
      gp_Circ ci;
      const Blend_Point& p = lin->Point(i);
      p.ParametersOnS(u,v);
      w = p.ParameterOnC();
      param = p.Parameter();
      func.Section(param,u,v,w,p1,p2,ci);
      isec.Set(ci,p1,p2);
      if(i == 1) {pf.SetCoord(u,v);p.ParametersOnS2(u,v);ppcf.SetCoord(u,v);} 
      if(i == nbp) {pl.SetCoord(u,v);p.ParametersOnS2(u,v);ppcl.SetCoord(u,v);} 
    }
  }
  Data->SetSimul(sec);
//  gp_Pnt2d pbid;
  Data->Set2dPoints(ppcf,ppcl,pf,pl);
  ChFi3d_FilCommonPoint(lin->StartPointOnFirst(),lin->TransitionOnS1(),
			Standard_True, Data->ChangeVertexFirstOnS2(),tolesp);
  ChFi3d_FilCommonPoint(lin->EndPointOnFirst(),lin->TransitionOnS1(),
			Standard_False,Data->ChangeVertexLastOnS2(),tolesp);
  ChFi3d_FilCommonPoint(lin->StartPointOnSecond(),lin->TransitionOnS2(),
			Standard_True, Data->ChangeVertexFirstOnS1(),tolesp);
  ChFi3d_FilCommonPoint(lin->EndPointOnSecond(),lin->TransitionOnS2(),
			Standard_False, Data->ChangeVertexLastOnS1(),tolesp);
}

//=======================================================================
//function : SimulSurf
//purpose  : 
//=======================================================================

void  ChFi3d_FilBuilder::SimulSurf(Handle(ChFiDS_SurfData)&            Data,
				   const Handle(ChFiDS_HElSpine)&      HGuide,
				   const Handle(ChFiDS_Spine)&         Spine,
				   const Standard_Integer              Choix,
				   const Handle(BRepAdaptor_HSurface)& HS1,
				   const Handle(Adaptor3d_TopolTool)&    I1,
				   const TopAbs_Orientation            Or1,
				   const Handle(BRepAdaptor_HSurface)& HS2,
				   const Handle(Adaptor3d_TopolTool)&    I2,
				   const Handle(BRepAdaptor_HCurve2d)& PC2,
				   const Handle(BRepAdaptor_HSurface)& HSref2,
				   const Handle(BRepAdaptor_HCurve2d)& PCref2,
				   Standard_Boolean&                   Decroch2,
				   const Standard_Real                 /*Arrow*/,
				   const Standard_Real                 TolGuide,
				   Standard_Real&                      First,
				   Standard_Real&                      Last,
				   const Standard_Boolean              Inside,
				   const Standard_Boolean              Appro,
				   const Standard_Boolean              Forward,
				   const Standard_Boolean              RecP,
				   const Standard_Boolean              RecS,
				   const Standard_Boolean              RecRst,
				   const math_Vector&                  Soldep)
{
  Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Spine);
  if(fsp.IsNull()) throw Standard_ConstructionError("PerformSurf : it is not the spine of a fillet");
  Handle(BRepBlend_Line) lin;
  
  // Flexible parameters!
  Standard_Real locfleche, MaxStep;
  SimulParams(HGuide,fsp,MaxStep,locfleche);
  Handle(ChFiDS_SecHArray1) sec;
  gp_Pnt2d pf,pl,ppcf,ppcl;
  
  Standard_Real PFirst = First;
  if(fsp->IsConstant()){
    BRepBlend_SurfRstConstRad func(HS1,HS2,PC2,HGuide);
    func.Set(HSref2,PCref2);
    Handle(Adaptor3d_HCurveOnSurface) HC = new Adaptor3d_HCurveOnSurface();
    HC->ChangeCurve().Load(PC2, HS2);
    BRepBlend_SurfCurvConstRadInv finvc(HS1,HC,HGuide);
    BRepBlend_SurfPointConstRadInv finvp(HS1,HGuide);
    BRepBlend_ConstRadInv finv(HS1,HSref2,HGuide);
    finv.Set(Standard_False,PCref2);
    
    Standard_Real rad = fsp->Radius();
    Standard_Integer petitchoix = 1;
    if(Or1 == TopAbs_REVERSED) petitchoix = 3;
    if(Choix%2 == 0) petitchoix++;
    finv.Set(rad,Choix);
    finvc.Set(rad,petitchoix);
    finvp.Set(rad,petitchoix);
    func.Set(rad,petitchoix);
    func.Set(myShape);
    
    done = SimulData(Data,HGuide,lin,HS1,I1,HS2,PC2,I2,Decroch2,
		     func,finv,finvp,finvc,
		     PFirst,MaxStep,locfleche,TolGuide,First,Last,
		     Soldep,4,Inside,Appro,Forward,RecP,RecS,RecRst);
    if(!done) throw Standard_Failure("SimulSurf : Failed Processing!");
    Standard_Integer nbp = lin->NbPoints();
    sec = new ChFiDS_SecHArray1(1,nbp);
    for(Standard_Integer i = 1; i <= nbp; i++){
      ChFiDS_CircSection& isec = sec->ChangeValue(i);
      Standard_Real u,v,w,param,p1,p2;
      gp_Circ ci;
      const Blend_Point& p = lin->Point(i);
      p.ParametersOnS(u,v);
      w = p.ParameterOnC();
      param = p.Parameter();
      func.Section(param,u,v,w,p1,p2,ci);
      isec.Set(ci,p1,p2);
      if(i == 1) {pf.SetCoord(u,v);p.ParametersOnS2(u,v);ppcf.SetCoord(u,v);} 
      if(i == nbp) {pl.SetCoord(u,v);p.ParametersOnS2(u,v);ppcl.SetCoord(u,v);} 
    }
  }
  else {
    BRepBlend_SurfRstEvolRad func(HS1,HS2,PC2,HGuide,fsp->Law(HGuide));
    Handle(Adaptor3d_HCurveOnSurface) HC = new Adaptor3d_HCurveOnSurface();
    HC->ChangeCurve().Load(PC2, HS2);
    BRepBlend_SurfCurvEvolRadInv finvc(HS1,HC,HGuide,fsp->Law(HGuide));
    BRepBlend_SurfPointEvolRadInv finvp(HS1,HGuide,fsp->Law(HGuide));
    BRepBlend_EvolRadInv finv(HS1,HSref2,HGuide,fsp->Law(HGuide));
    finv.Set(Standard_False,PCref2);
    Standard_Integer petitchoix = 1;
    if(Or1 == TopAbs_REVERSED) petitchoix = 3;
    if(Choix%2 == 0) petitchoix++;
    finv.Set(Choix);
    finvc.Set(petitchoix);
    finvp.Set(petitchoix);
    func.Set(petitchoix);
    func.Set(myShape);
    done = SimulData(Data,HGuide,lin,HS1,I1,HS2,PC2,I2,Decroch2,
		     func,finv,finvp,finvc,
		     PFirst,MaxStep,locfleche,TolGuide,First,Last,
		     Soldep,4,Inside,Appro,Forward,RecP,RecS,RecRst);
    if(!done) throw Standard_Failure("SimulSurf : Fail !");
    Standard_Integer nbp = lin->NbPoints();
    sec = new ChFiDS_SecHArray1(1,nbp);
    for(Standard_Integer i = 1; i <= nbp; i++){
      ChFiDS_CircSection& isec = sec->ChangeValue(i);
      Standard_Real u,v,w,param,p1,p2;
      gp_Circ ci;
      const Blend_Point& p = lin->Point(i);
      p.ParametersOnS(u,v);
      w = p.ParameterOnC();
      param = p.Parameter();
      func.Section(param,u,v,w,p1,p2,ci);
      isec.Set(ci,p1,p2);
      if(i == 1) {pf.SetCoord(u,v);p.ParametersOnS2(u,v);ppcf.SetCoord(u,v);} 
      if(i == nbp) {pl.SetCoord(u,v);p.ParametersOnS2(u,v);ppcl.SetCoord(u,v);} 
    }
  }
  Data->SetSimul(sec);
  //gp_Pnt2d pbid;
  Data->Set2dPoints(pf,pl,ppcf,ppcl);
  ChFi3d_FilCommonPoint(lin->StartPointOnFirst(),lin->TransitionOnS1(),
			Standard_True, Data->ChangeVertexFirstOnS1(),tolesp);
  ChFi3d_FilCommonPoint(lin->EndPointOnFirst(),lin->TransitionOnS1(),
			Standard_False,Data->ChangeVertexLastOnS1(),tolesp);
  ChFi3d_FilCommonPoint(lin->StartPointOnSecond(),lin->TransitionOnS2(),
			Standard_True, Data->ChangeVertexFirstOnS2(),tolesp);
  ChFi3d_FilCommonPoint(lin->EndPointOnSecond(),lin->TransitionOnS2(),
			Standard_False, Data->ChangeVertexLastOnS2(),tolesp);
}



//=======================================================================
//function : SimulSurf
//purpose  : 
//=======================================================================

void  ChFi3d_FilBuilder::SimulSurf(Handle(ChFiDS_SurfData)&            Data,
				   const Handle(ChFiDS_HElSpine)&      HGuide,
				   const Handle(ChFiDS_Spine)&         Spine,
				   const Standard_Integer              Choix,
				   const Handle(BRepAdaptor_HSurface)& HS1,
             		           const Handle(Adaptor3d_TopolTool)&    I1,
                                   const Handle(BRepAdaptor_HCurve2d)& PC1,
				   const Handle(BRepAdaptor_HSurface)& HSref1,
				   const Handle(BRepAdaptor_HCurve2d)& PCref1,
				   Standard_Boolean&                   Decroch1,
				   const TopAbs_Orientation            Or1,
				   const Handle(BRepAdaptor_HSurface)& HS2,
				   const Handle(Adaptor3d_TopolTool)&    I2,
				   const Handle(BRepAdaptor_HCurve2d)& PC2,
				   const Handle(BRepAdaptor_HSurface)& HSref2,
				   const Handle(BRepAdaptor_HCurve2d)& PCref2,
				   Standard_Boolean&                   Decroch2,
                                   const TopAbs_Orientation            Or2,
				   const Standard_Real                 /*Fleche*/,
				   const Standard_Real                 TolGuide,
				   Standard_Real&                      First,
				   Standard_Real&                      Last,
				   const Standard_Boolean              Inside,
				   const Standard_Boolean              Appro,
				   const Standard_Boolean              Forward,
                                   const Standard_Boolean              RecP1,
				   const Standard_Boolean              RecRst1,
                                   const Standard_Boolean              RecP2,
				   const Standard_Boolean              RecRst2,
				   const math_Vector&                  Soldep)
{
  Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Spine);
  if(fsp.IsNull()) throw Standard_ConstructionError("PerformSurf : it is not the spine of a fillet");
  Handle(BRepBlend_Line) lin;
  
  // Flexible parameters!
  Standard_Real locfleche, MaxStep;
  SimulParams(HGuide,fsp,MaxStep,locfleche);
  Handle(ChFiDS_SecHArray1) sec;
//  gp_Pnt2d pf,pl;

  Standard_Integer ch1 = 1, ch2 = 2;      
  Standard_Real PFirst = First;

  if(fsp->IsConstant()){
    BRepBlend_RstRstConstRad func(HS1, PC1, HS2, PC2, HGuide);
    func.Set(HSref1, PCref1, HSref2, PCref2);
    Handle(Adaptor3d_HCurveOnSurface) HC1 = new Adaptor3d_HCurveOnSurface();
    HC1->ChangeCurve().Load(PC1, HS1);
    Handle(Adaptor3d_HCurveOnSurface) HC2 = new Adaptor3d_HCurveOnSurface();
    HC2->ChangeCurve().Load(PC2, HS2);
    BRepBlend_SurfCurvConstRadInv finv1(HSref1, HC2, HGuide);
    BRepBlend_CurvPointRadInv     finvp1(HGuide, HC2);
    BRepBlend_SurfCurvConstRadInv finv2(HSref2, HC1, HGuide);
    BRepBlend_CurvPointRadInv     finvp2(HGuide, HC1);

    finv1.Set(PCref1);
    finv2.Set(PCref2);


    Standard_Real rad = fsp->Radius();
    if(Or1 == TopAbs_REVERSED) ch1 = 3;
    if(Or2 == TopAbs_REVERSED) ch2 = 3;

    finv1.Set(rad, ch1);
    finvp1.Set(Choix);
    finv2.Set(rad, ch2);
    finvp2.Set(Choix);
    func.Set(rad, Choix);
    func.Set(myShape);
    
    done = SimulData(Data, HGuide, lin, HS1, PC1, I1, Decroch1, HS2, PC2, I2, Decroch2,
		     func, finv1, finvp1, finv2, finvp2,
		     PFirst, MaxStep, locfleche, TolGuide, First, Last,
		     Soldep, 4, Inside, Appro, Forward, RecP1, RecRst1, RecP2, RecRst2);
    if(!done) throw Standard_Failure("SimulSurf : Failed processing!");
    Standard_Integer nbp = lin->NbPoints();
    sec = new ChFiDS_SecHArray1(1,nbp);
    for(Standard_Integer i = 1; i <= nbp; i++){
      ChFiDS_CircSection& isec = sec->ChangeValue(i);
      Standard_Real u, v, param, p1, p2;
      gp_Circ ci;
      const Blend_Point& p = lin->Point(i);
      u = p.ParameterOnC1();
      v = p.ParameterOnC2();
      param = p.Parameter();
      func.Section(param, u, v, p1, p2, ci);
      isec.Set(ci, p1, p2);
//      if(i == 1) {pf.SetCoord(u,v);} 
//      if(i == nbp) {pl.SetCoord(u,v);} 
    }
  }
  else {
    BRepBlend_RstRstEvolRad func(HS1,PC1, HS2, PC2, HGuide, fsp->Law(HGuide));
    func.Set(HSref1, PCref1, HSref2, PCref2);
    Handle(Adaptor3d_HCurveOnSurface) HC1 = new Adaptor3d_HCurveOnSurface();
    HC1->ChangeCurve().Load(PC1, HS1);
    Handle(Adaptor3d_HCurveOnSurface) HC2 = new Adaptor3d_HCurveOnSurface();
    HC2->ChangeCurve().Load(PC2, HS2);

    BRepBlend_SurfCurvEvolRadInv  finv1(HSref1, HC2, HGuide, fsp->Law(HGuide));
    BRepBlend_CurvPointRadInv     finvp1(HGuide, HC2);
    BRepBlend_SurfCurvEvolRadInv  finv2(HSref2, HC1, HGuide, fsp->Law(HGuide));
    BRepBlend_CurvPointRadInv     finvp2(HGuide, HC1);

    finv1.Set(PCref1);
    finv2.Set(PCref2);

    Standard_Integer ch11 = 1, ch22 = 2;    

    if(Or1 == TopAbs_REVERSED) ch11 = 3;
    if(Or2 == TopAbs_REVERSED) ch22 = 3;


    finv1.Set(ch11);
    finvp1.Set(Choix);
    finv2.Set(ch22);
    finvp2.Set(Choix);
    func.Set(Choix);
    func.Set(myShape);

    done = SimulData(Data, HGuide, lin, HS1, PC1, I1, Decroch1, HS2, PC2, I2, Decroch2,
		     func, finv1, finvp1, finv2, finvp2,
		     PFirst, MaxStep, locfleche, TolGuide, First, Last,
		     Soldep, 4, Inside, Appro, Forward, RecP1, RecRst1, RecP2, RecRst2);

    if(!done) throw Standard_Failure("SimulSurf : Fail !");
    Standard_Integer nbp = lin->NbPoints();
    sec = new ChFiDS_SecHArray1(1, nbp);
    for(Standard_Integer i = 1; i <= nbp; i++){
      ChFiDS_CircSection& isec = sec->ChangeValue(i);
      Standard_Real u, v, param, p1, p2;
      gp_Circ ci;
      const Blend_Point& p = lin->Point(i);
      u = p.ParameterOnC1();
      v = p.ParameterOnC2();
      param = p.Parameter();
      func.Section(param, u, v, p1, p2, ci);
      isec.Set(ci, p1, p2);
//      if(i == 1) {pf.SetCoord(u,v);} 
//      if(i == nbp) {pl.SetCoord(u,v);} 
    }
  }
  Data->SetSimul(sec);
//  gp_Pnt2d pbid;
//  Data->Set2dPoints(pf,pl,pbid,pbid);

  ChFi3d_FilCommonPoint(lin->StartPointOnFirst(),lin->TransitionOnS1(),
			Standard_True, Data->ChangeVertexFirstOnS1(),tolesp);
  ChFi3d_FilCommonPoint(lin->EndPointOnFirst(),lin->TransitionOnS1(),
			Standard_False,Data->ChangeVertexLastOnS1(),tolesp);
  ChFi3d_FilCommonPoint(lin->StartPointOnSecond(),lin->TransitionOnS2(),
			Standard_True, Data->ChangeVertexFirstOnS2(),tolesp);
  ChFi3d_FilCommonPoint(lin->EndPointOnSecond(),lin->TransitionOnS2(),
			Standard_False, Data->ChangeVertexLastOnS2(),tolesp);
}







//=======================================================================
//function : PerformFirstSection
//purpose  : 
//=======================================================================

Standard_Boolean ChFi3d_FilBuilder::PerformFirstSection
(const Handle(ChFiDS_Spine)&         Spine,
 const Handle(ChFiDS_HElSpine)&      HGuide,
 const Standard_Integer              Choix,
 Handle(BRepAdaptor_HSurface)& S1,
 Handle(BRepAdaptor_HSurface)& S2,
 const Handle(Adaptor3d_TopolTool)&    I1,
 const Handle(Adaptor3d_TopolTool)&    I2,
 const Standard_Real                 Par,
 math_Vector&                        SolDep,
 TopAbs_State&                       Pos1,
 TopAbs_State&                       Pos2) const 
{
  Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Spine);
  if(fsp.IsNull()) throw Standard_ConstructionError("PerformSurf : this is not the spine of a fillet");
  Standard_Real TolGuide = HGuide->Resolution(tolesp);
  if(fsp->IsConstant()){
    BRepBlend_ConstRad Func(S1,S2,HGuide);
    Func.Set(fsp->Radius(),Choix);
    Func.Set(myShape);
    BRepBlend_Walking TheWalk(S1,S2,I1,I2,HGuide);
    return TheWalk.PerformFirstSection(Func,Par,SolDep,
				       tolesp,TolGuide,Pos1,Pos2);
  }
  else {
    BRepBlend_EvolRad Func(S1,S2,HGuide,fsp->Law(HGuide));
    Func.Set(Choix);
    Func.Set(myShape);
    BRepBlend_Walking TheWalk(S1,S2,I1,I2,HGuide);
    return TheWalk.PerformFirstSection(Func,Par,SolDep,
				       tolesp,TolGuide,Pos1,Pos2);
  }
}

//=======================================================================
//function : PerformSurf
//purpose  : 
//=======================================================================

Standard_Boolean 
ChFi3d_FilBuilder::PerformSurf(ChFiDS_SequenceOfSurfData&          SeqData,
			       const Handle(ChFiDS_HElSpine)&      HGuide,
			       const Handle(ChFiDS_Spine)&         Spine,
			       const Standard_Integer              Choix,
			       const Handle(BRepAdaptor_HSurface)& S1,
			       const Handle(Adaptor3d_TopolTool)&    I1,
			       const Handle(BRepAdaptor_HSurface)& S2,
			       const Handle(Adaptor3d_TopolTool)&    I2,
			       const Standard_Real                 MaxStep,
			       const Standard_Real                 Fleche,
			       const Standard_Real                 TolGuide,
			       Standard_Real&                      First,
			       Standard_Real&                      Last,
			       const Standard_Boolean              Inside,
			       const Standard_Boolean              Appro,
			       const Standard_Boolean              Forward,
			       const Standard_Boolean              RecOnS1,
			       const Standard_Boolean              RecOnS2,
			       const math_Vector&                  Soldep,
			       Standard_Integer&                   intf,
			       Standard_Integer&                   intl)
{
#ifdef OCCT_DEBUG
  OSD_Chronometer ch;
#endif
  Handle(ChFiDS_SurfData) Data = SeqData(1);
  Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Spine);
  if(fsp.IsNull()) throw Standard_ConstructionError("PerformSurf : this is not the spine of a fillet");
  Standard_Boolean gd1,gd2,gf1,gf2, maybesingular;
  Handle(BRepBlend_Line) lin;
  TopAbs_Orientation Or = S1->ChangeSurface().Face().Orientation();
  Standard_Real PFirst = First;
  if(intf) First = fsp->FirstParameter(1);
  if(intl) Last = fsp->LastParameter(fsp->NbEdges());
  if(fsp->IsConstant()){
    BRepBlend_ConstRad Func(S1,S2,HGuide);
    BRepBlend_ConstRadInv FInv(S1,S2,HGuide);
    Func.Set(fsp->Radius(),Choix);
    FInv.Set(fsp->Radius(),Choix);
    Func.Set(myShape);

#ifdef OCCT_DEBUG
        ChFi3d_InitChron(ch); //init perf ComputeData
#endif

    done = ComputeData(Data,HGuide,Spine,lin,S1,I1,S2,I2,Func,FInv,
		       PFirst,MaxStep,Fleche,TolGuide,First,Last,
		       Inside,Appro,Forward,Soldep,intf,intl,
		       gd1,gd2,gf1,gf2,RecOnS1,RecOnS2);

#ifdef OCCT_DEBUG
        ChFi3d_ResultChron(ch , t_computedata);// result perf ComputeData
#endif 

    if(!done) return Standard_False; // recovery is possible PMN 14/05/1998

#ifdef OCCT_DEBUG
          ChFi3d_InitChron(ch);// init  perf  CompleteData
#endif

    done = CompleteData(Data,Func,lin,S1,S2,Or,gd1,gd2,gf1,gf2);

#ifdef OCCT_DEBUG
         ChFi3d_ResultChron(ch , t_completedata);// result perf CompleteData
#endif 

    if(!done) throw Standard_Failure("PerformSurf : Failed approximation!");
    maybesingular = (Func.GetMinimalDistance()<=100*tolapp3d);
  }
  else {
    BRepBlend_EvolRad Func(S1, S2, HGuide, fsp->Law(HGuide));
    BRepBlend_EvolRadInv FInv(S1, S2, HGuide, fsp->Law(HGuide));
    Func.Set(Choix);
    FInv.Set(Choix);
    Func.Set(myShape);

#ifdef OCCT_DEBUG
          ChFi3d_InitChron(ch);// init perf ComputeData
#endif

    done = ComputeData(Data,HGuide,Spine,lin,S1,I1,S2,I2,Func,FInv,
		       PFirst,MaxStep,Fleche,TolGuide,First,Last,
		       Inside,Appro,Forward,Soldep,intf,intl,
		       gd1,gd2,gf1,gf2,RecOnS1,RecOnS2);
#ifdef OCCT_DEBUG
         ChFi3d_ResultChron(ch , t_computedata); //result perf ComputeData
#endif 

    if(!done) return Standard_False;

#ifdef OCCT_DEBUG
          ChFi3d_InitChron(ch);// init perf CompleteData
#endif

    done = CompleteData(Data,Func,lin,S1,S2,Or,gd1,gd2,gf1,gf2);

#ifdef OCCT_DEBUG
         ChFi3d_ResultChron(ch , t_completedata);// result perf CompleteData
#endif 

    if(!done) throw Standard_Failure("PerformSurf : Failed approximation!");
    maybesingular = (Func.GetMinimalDistance()<=100*tolapp3d); 
  }
  if (maybesingular) SplitSurf(SeqData, lin);
  return Standard_True;
}

//=======================================================================
//function : PerformSurf
//purpose  : 
//=======================================================================

void  ChFi3d_FilBuilder::PerformSurf(ChFiDS_SequenceOfSurfData&          SeqData,
				     const Handle(ChFiDS_HElSpine)&      HGuide,
				     const Handle(ChFiDS_Spine)&         Spine,
				     const Standard_Integer              Choix,
				     const Handle(BRepAdaptor_HSurface)& HS1,
				     const Handle(Adaptor3d_TopolTool)&    I1,
				     const Handle(BRepAdaptor_HCurve2d)& PC1,
				     const Handle(BRepAdaptor_HSurface)& HSref1,
				     const Handle(BRepAdaptor_HCurve2d)& PCref1,
				     Standard_Boolean&                   Decroch1,
				     const Handle(BRepAdaptor_HSurface)& HS2,
				     const Handle(Adaptor3d_TopolTool)&    I2,
				     const TopAbs_Orientation            Or2,
				     const Standard_Real                 MaxStep,
				     const Standard_Real                 Fleche,
				     const Standard_Real                 TolGuide,
				     Standard_Real&                      First,
				     Standard_Real&                      Last,
				     const Standard_Boolean              Inside,
				     const Standard_Boolean              Appro,
				     const Standard_Boolean              Forward,
				     const Standard_Boolean              RecP,
				     const Standard_Boolean              RecS,
				     const Standard_Boolean              RecRst,
				     const math_Vector&                  Soldep)
{
  Handle(ChFiDS_SurfData)  Data = SeqData(1);
  Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Spine);
  if(fsp.IsNull()) throw Standard_ConstructionError("PerformSurf : this is not the spine of a fillet");
  Handle(BRepBlend_Line) lin;
  Standard_Real PFirst = First;
  Standard_Boolean maybesingular;

  if(fsp->IsConstant()){
    BRepBlend_SurfRstConstRad func(HS2,HS1,PC1,HGuide);
    func.Set(HSref1,PCref1);
    Handle(Adaptor3d_HCurveOnSurface) HC = new Adaptor3d_HCurveOnSurface();
    HC->ChangeCurve().Load(PC1, HS1);
    BRepBlend_SurfCurvConstRadInv finvc(HS2,HC,HGuide);
    BRepBlend_SurfPointConstRadInv finvp(HS2,HGuide);
    BRepBlend_ConstRadInv finv(HS2,HSref1,HGuide);
    finv.Set(Standard_False,PCref1);
     
    Standard_Real rad = fsp->Radius();
    Standard_Integer petitchoix = 1;
    if(Or2 == TopAbs_REVERSED) petitchoix = 3;
    if(Choix%2 == 0) petitchoix++;
    finv.Set(rad,Choix);
    finvc.Set(rad,petitchoix);
    finvp.Set(rad,petitchoix);
    func.Set(rad,petitchoix);
    func.Set(myShape);
    
    done = ComputeData(Data,HGuide,lin,HS2,I2,HS1,PC1,I1,Decroch1,
		       func,finv,finvp,finvc,
		       PFirst,MaxStep,Fleche,TolGuide,First,Last,
		       Soldep,Inside,Appro,Forward,RecP,RecS,RecRst);
    if(!done) {
       Spine->SetErrorStatus(ChFiDS_WalkingFailure); 
       throw Standard_Failure("PerformSurf : Failed processing!");
    }
    TopAbs_Orientation Or = HS2->ChangeSurface().Face().Orientation();
    done = CompleteData(Data,func,lin,HS1,HS2,Or,1);
    if(!done) throw Standard_Failure("PerformSurf : Failed approximation!");
    maybesingular = (func.GetMinimalDistance()<=100*tolapp3d); 
  }
  else {
    BRepBlend_SurfRstEvolRad func(HS2,HS1,PC1,HGuide,fsp->Law(HGuide));
    func.Set(HSref1,PCref1);
    Handle(Adaptor3d_HCurveOnSurface) HC = new Adaptor3d_HCurveOnSurface();
    HC->ChangeCurve().Load(PC1, HS1);
    BRepBlend_SurfCurvEvolRadInv finvc(HS2,HC,HGuide,fsp->Law(HGuide));
    BRepBlend_SurfPointEvolRadInv finvp(HS2,HGuide,fsp->Law(HGuide));
    BRepBlend_EvolRadInv finv(HS2,HSref1,HGuide,fsp->Law(HGuide));
    finv.Set(Standard_False,PCref1);
    Standard_Integer petitchoix = 1;
    if(Or2 == TopAbs_REVERSED) petitchoix = 3;
    if(Choix%2 == 0) petitchoix++;
    finv.Set(Choix);
    finvc.Set(petitchoix);
    finvp.Set(petitchoix);
    func.Set(petitchoix);
    func.Set(myShape);
    done = ComputeData(Data,HGuide,lin,HS2,I2,HS1,PC1,I1,Decroch1,
		       func,finv,finvp,finvc,
		       PFirst,MaxStep,Fleche,TolGuide,First,Last,
		       Soldep,Inside,Appro,Forward,RecP,RecS,RecRst);
    if(!done) {
      Spine->SetErrorStatus(ChFiDS_WalkingFailure);
      throw Standard_Failure("PerformSurf : Failed processing!");
    }
    TopAbs_Orientation Or = HS2->ChangeSurface().Face().Orientation();
    done = CompleteData(Data,func,lin,HS1,HS2,Or,1);
    if(!done) throw Standard_Failure("PerformSurf : Failed approximation!");
   maybesingular = (func.GetMinimalDistance()<=100*tolapp3d); 
  }
  if (maybesingular) SplitSurf(SeqData, lin);
}

//=======================================================================
//function : PerformSurf
//purpose  : 
//=======================================================================

void  ChFi3d_FilBuilder::PerformSurf(ChFiDS_SequenceOfSurfData&          SeqData,
				     const Handle(ChFiDS_HElSpine)&      HGuide,
				     const Handle(ChFiDS_Spine)&         Spine,
				     const Standard_Integer              Choix,
				     const Handle(BRepAdaptor_HSurface)& HS1,
				     const Handle(Adaptor3d_TopolTool)&    I1,
				     const TopAbs_Orientation            Or1,
				     const Handle(BRepAdaptor_HSurface)& HS2,
				     const Handle(Adaptor3d_TopolTool)&    I2,
				     const Handle(BRepAdaptor_HCurve2d)& PC2,
				     const Handle(BRepAdaptor_HSurface)& HSref2,
				     const Handle(BRepAdaptor_HCurve2d)& PCref2,
				     Standard_Boolean&                   Decroch2,
				     const Standard_Real                 MaxStep,
				     const Standard_Real                 Fleche,
				     const Standard_Real                 TolGuide,
				     Standard_Real&                      First,
				     Standard_Real&                      Last,
				     const Standard_Boolean              Inside,
				     const Standard_Boolean              Appro,
				     const Standard_Boolean              Forward,
				     const Standard_Boolean              RecP,
				     const Standard_Boolean              RecS,
				     const Standard_Boolean              RecRst,
				     const math_Vector&                  Soldep)
{
  Handle(ChFiDS_SurfData) Data = SeqData(1);;
  Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Spine);
  if(fsp.IsNull()) throw Standard_ConstructionError("PerformSurf : this is not the spine of a fillet");
  Handle(BRepBlend_Line) lin;
  Standard_Real PFirst = First;
  Standard_Boolean maybesingular;

  if(fsp->IsConstant()){
    BRepBlend_SurfRstConstRad func(HS1,HS2,PC2,HGuide);
    func.Set(HSref2,PCref2);
    Handle(Adaptor3d_HCurveOnSurface) HC = new Adaptor3d_HCurveOnSurface();
    HC->ChangeCurve().Load(PC2, HS2);
    BRepBlend_SurfCurvConstRadInv finvc(HS1,HC,HGuide);
    BRepBlend_SurfPointConstRadInv finvp(HS1,HGuide);
    BRepBlend_ConstRadInv finv(HS1,HSref2,HGuide);
    finv.Set(Standard_False,PCref2);
    
    Standard_Real rad = fsp->Radius();
    Standard_Integer petitchoix = 1;
    if(Or1 == TopAbs_REVERSED) petitchoix = 3;
    if(Choix%2 == 0) petitchoix++;
    finv.Set(rad,Choix);
    finvc.Set(rad,petitchoix);
    finvp.Set(rad,petitchoix);
    func.Set(rad,petitchoix);
    func.Set(myShape);
    
    done = ComputeData(Data,HGuide,lin,HS1,I1,HS2,PC2,I2,Decroch2,
		       func,finv,finvp,finvc,
		       PFirst,MaxStep,Fleche,TolGuide,First,Last,
		       Soldep,Inside,Appro,Forward,RecP,RecS,RecRst);
    if(!done) {
      Spine->SetErrorStatus(ChFiDS_WalkingFailure);
      throw Standard_Failure("PerformSurf : Failed processing!");
    }
    TopAbs_Orientation Or = HS1->ChangeSurface().Face().Orientation();
    done = CompleteData(Data,func,lin,HS1,HS2,Or,0);
    if(!done) throw Standard_Failure("PerformSurf : Failed approximation!");
    maybesingular = (func.GetMinimalDistance()<=100*tolapp3d); 
  }
  else {
    BRepBlend_SurfRstEvolRad func(HS1,HS2,PC2,HGuide,fsp->Law(HGuide));
    func.Set(HSref2,PCref2);
    Handle(Adaptor3d_HCurveOnSurface) HC = new Adaptor3d_HCurveOnSurface();
    HC->ChangeCurve().Load(PC2, HS2);
    BRepBlend_SurfCurvEvolRadInv finvc(HS1,HC,HGuide,fsp->Law(HGuide));
    BRepBlend_SurfPointEvolRadInv finvp(HS1,HGuide,fsp->Law(HGuide));
    BRepBlend_EvolRadInv finv(HS1,HSref2,HGuide,fsp->Law(HGuide));
    finv.Set(Standard_False,PCref2);
    Standard_Integer petitchoix = 1;
    if(Or1 == TopAbs_REVERSED) petitchoix = 3;
    if(Choix%2 == 0) petitchoix++;
    finv.Set(Choix);
    finvc.Set(petitchoix);
    finvp.Set(petitchoix);
    func.Set(petitchoix);
    func.Set(myShape);

    done = ComputeData(Data,HGuide,lin,HS1,I1,HS2,PC2,I2,Decroch2,
		       func,finv,finvp,finvc,
		       PFirst,MaxStep,Fleche,TolGuide,First,Last,
		       Soldep,Inside,Appro,Forward,RecP,RecS,RecRst);
    if(!done) {
      Spine->SetErrorStatus(ChFiDS_WalkingFailure);
      throw Standard_Failure("PerformSurf : Failed processing!");
    }
    TopAbs_Orientation Or = HS1->ChangeSurface().Face().Orientation();
    done = CompleteData(Data,func,lin,HS1,HS2,Or,0);
    if(!done) throw Standard_Failure("PerformSurf : Failed approximation!");
    maybesingular = (func.GetMinimalDistance()<=100*tolapp3d); 
  }
  if (maybesingular) SplitSurf(SeqData, lin); 
}






//=======================================================================
//function : PerformSurf
//purpose  : 
//=======================================================================

void  ChFi3d_FilBuilder::PerformSurf(ChFiDS_SequenceOfSurfData&          SeqData,
				     const Handle(ChFiDS_HElSpine)&      HGuide,
				     const Handle(ChFiDS_Spine)&         Spine,
				     const Standard_Integer              Choix,
				     const Handle(BRepAdaptor_HSurface)& HS1,
				     const Handle(Adaptor3d_TopolTool)&    I1,
				     const Handle(BRepAdaptor_HCurve2d)& PC1,
				     const Handle(BRepAdaptor_HSurface)& HSref1,
				     const Handle(BRepAdaptor_HCurve2d)& PCref1,
				     Standard_Boolean&                   Decroch1,
                                     const TopAbs_Orientation            Or1,
				     const Handle(BRepAdaptor_HSurface)& HS2,
				     const Handle(Adaptor3d_TopolTool)&    I2,
				     const Handle(BRepAdaptor_HCurve2d)& PC2,
				     const Handle(BRepAdaptor_HSurface)& HSref2,
				     const Handle(BRepAdaptor_HCurve2d)& PCref2,
				     Standard_Boolean&                   Decroch2,
                                     const TopAbs_Orientation            Or2,
				     const Standard_Real                 MaxStep,
				     const Standard_Real                 Fleche,
				     const Standard_Real                 TolGuide,
				     Standard_Real&                      First,
				     Standard_Real&                      Last,
				     const Standard_Boolean              Inside,
				     const Standard_Boolean              Appro,
				     const Standard_Boolean              Forward,
				     const Standard_Boolean              RecP1,
				     const Standard_Boolean              RecRst1,
                                     const Standard_Boolean              RecP2,
				     const Standard_Boolean              RecRst2,
				     const math_Vector&                  Soldep)
{
  Handle(ChFiDS_SurfData) Data = SeqData(1);;
  Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Spine);
  if(fsp.IsNull()) throw Standard_ConstructionError("PerformSurf : this is not the spine of a fillet");
  Handle(BRepBlend_Line) lin;
  Standard_Real PFirst = First;
  Standard_Boolean maybesingular;

  if(fsp->IsConstant()){
    BRepBlend_RstRstConstRad func(HS1, PC1, HS2, PC2, HGuide);
    func.Set(HSref1, PCref1, HSref2, PCref2);
    Handle(Adaptor3d_HCurveOnSurface) HC1 = new Adaptor3d_HCurveOnSurface();
    HC1->ChangeCurve().Load(PC1, HS1);
    Handle(Adaptor3d_HCurveOnSurface) HC2 = new Adaptor3d_HCurveOnSurface();
    HC2->ChangeCurve().Load(PC2, HS2);
    BRepBlend_SurfCurvConstRadInv finv1(HSref1, HC2, HGuide);
    BRepBlend_CurvPointRadInv     finvp1(HGuide, HC2);
    BRepBlend_SurfCurvConstRadInv finv2(HSref2, HC1, HGuide);
    BRepBlend_CurvPointRadInv     finvp2(HGuide, HC1);

    finv1.Set(PCref1);
    finv2.Set(PCref2);

    Standard_Integer ch1 = 1, ch2 = 2;    
    Standard_Real rad = fsp->Radius();
    if(Or1 == TopAbs_REVERSED) ch1 = 3;
    if(Or2 == TopAbs_REVERSED) ch2 = 3;

    finv1.Set(rad, ch1);
    finvp1.Set(Choix);
    finv2.Set(rad, ch2);
    finvp2.Set(Choix);
    func.Set(rad, Choix);
    func.Set(myShape);
    
    done = ComputeData(Data, HGuide, lin, HS1, PC1, I1, Decroch1, HS2, PC2, I2, Decroch2,
		       func, finv1, finvp1, finv2, finvp2,
		       PFirst, MaxStep, Fleche, TolGuide, First, Last,
		       Soldep, Inside, Appro, Forward, RecP1, RecRst1, RecP2, RecRst2);
    if(!done) {
       Spine->SetErrorStatus(ChFiDS_WalkingFailure);
       throw Standard_Failure("PerformSurf : Failed processing!");
    }
    TopAbs_Orientation Or = HS1->ChangeSurface().Face().Orientation();
    done = CompleteData(Data, func, lin, HS1, HS2, Or);
    if(!done) throw Standard_Failure("PerformSurf : Failed approximation!");
    maybesingular = (func.GetMinimalDistance()<=100*tolapp3d); 
  }
  else {
    BRepBlend_RstRstEvolRad func(HS1,PC1, HS2, PC2, HGuide, fsp->Law(HGuide));
    func.Set(HSref1, PCref1, HSref2, PCref2);
    Handle(Adaptor3d_HCurveOnSurface) HC1 = new Adaptor3d_HCurveOnSurface();
    HC1->ChangeCurve().Load(PC1, HS1);
    Handle(Adaptor3d_HCurveOnSurface) HC2 = new Adaptor3d_HCurveOnSurface();
    HC2->ChangeCurve().Load(PC2, HS2);

    BRepBlend_SurfCurvEvolRadInv finv1(HSref1, HC2, HGuide, fsp->Law(HGuide));
    BRepBlend_CurvPointRadInv     finvp1(HGuide, HC2);
    BRepBlend_SurfCurvEvolRadInv  finv2(HSref2, HC1, HGuide, fsp->Law(HGuide));
    BRepBlend_CurvPointRadInv     finvp2(HGuide, HC1);

    finv1.Set(PCref1);
    finv2.Set(PCref2);

    Standard_Integer ch1 = 1, ch2 = 2;    

    if(Or1 == TopAbs_REVERSED) ch1 = 3;
    if(Or2 == TopAbs_REVERSED) ch2 = 3;


    finv1.Set(ch1);
    finvp1.Set(Choix);
    finv2.Set(ch2);
    finvp2.Set(Choix);
    func.Set(Choix);
    func.Set(myShape);
    
    done = ComputeData(Data, HGuide, lin, HS1, PC1,  I1, Decroch1, HS2, PC2,I2, Decroch2,
		       func, finv1, finvp1, finv2, finvp2,
		       PFirst, MaxStep, Fleche, TolGuide, First, Last,
		       Soldep, Inside, Appro, Forward, RecP1, RecRst1, RecP2, RecRst2);

    if(!done) {
      Spine->SetErrorStatus(ChFiDS_WalkingFailure);
      throw Standard_Failure("PerformSurf : Failed processing!");
    }
    TopAbs_Orientation Or = HS1->ChangeSurface().Face().Orientation();
    done = CompleteData(Data, func, lin, HS1, HS2, Or);
    if(!done) throw Standard_Failure("PerformSurf : Failed approximation!");
    maybesingular = (func.GetMinimalDistance()<=100*tolapp3d); 


  }
  if (maybesingular) SplitSurf(SeqData, lin); 
}




//=======================================================================
//function : SplitSurf
//purpose  : 
//=======================================================================

void ChFi3d_FilBuilder::SplitSurf(ChFiDS_SequenceOfSurfData&    SeqData,
				  const Handle(BRepBlend_Line)& Line)
{
  Standard_Integer ii, Nbpnt=Line->NbPoints();
  if (Nbpnt <3) return;
  Standard_Real UFirst,ULast,VFirst,VLast;
  TopOpeBRepDS_DataStructure& DStr = myDS->ChangeDS();
  Standard_Integer ISurf;
  Handle(ChFiDS_SurfData) ref =  SeqData(1);
  Blend_Point P; 

  ISurf= ref->Surf(); 
  Handle(Geom_Surface) Surf = DStr.Surface(ISurf).Surface();
  Surf->Bounds(UFirst,ULast,VFirst,VLast);
  Handle(Geom_Curve) Courbe1 = Surf->UIso(UFirst);
  Handle(Geom_Curve) Courbe2 = Surf->UIso(ULast);
  ChFi3d_SearchSing Fonc(Courbe1, Courbe2);

  TColStd_SequenceOfReal LesVi;
  Standard_Real precedant, suivant, courant;
  Standard_Real a, b, c;
  
  // (1) Finds vi so that iso v=vi is punctual
  VFirst = Min( ref->InterferenceOnS1().FirstParameter(),
                ref->InterferenceOnS2().FirstParameter() );
  VLast  = Max( ref->InterferenceOnS1().LastParameter(),
                ref->InterferenceOnS2().LastParameter() );

  // (1.1) Finds the first point inside
  for (ii=1; ii<=Nbpnt && Line->Point(ii).Parameter()<VFirst; ii++) {}
  if (ii==1) ii++;
  P =  Line->Point(ii);
  b = P.Parameter();
  courant = P.PointOnS1().Distance(P.PointOnS2());
  P = Line->Point(ii-1);
  a = P.Parameter();    
  precedant = P.PointOnS1().Distance(P.PointOnS2());
  ii ++;

  // (1.2) Find a minimum by "points"
  for ( ; ii<=Nbpnt && Line->Point(ii).Parameter()<=VLast; ii++) {
    for (;ii<=Nbpnt && 
	 Line->Point(ii).Parameter()<VLast &&
	 Line->Point(ii).Parameter()-b<Precision::PConfusion(); ii++) {}

    const Blend_Point& pnt =  Line->Point(ii);
    c = pnt.Parameter();    
    suivant = pnt.PointOnS1().Distance(pnt.PointOnS2());
    if ( (courant <  precedant) && (courant < suivant) ) {
      // (1.3) Find the exact minimum
      math_FunctionRoot Resol(Fonc, (a+c)/2,
			      tol2d,//Surf->VResolution(toleps), 
			      a, c,
			      50);
      if (Resol.IsDone()) {
	Standard_Real Val, racine=Resol.Root();
	
	Fonc.Value(Resol.Root(), Val);
        if (Val< tolapp3d) { 
	  // the solution (avoiding the risks of confusion)
	  if (LesVi.Length()==0) {
	    if ( (racine > VFirst+tol2d) &&
		 (racine < VLast -tol2d) ) {
		   LesVi.Append(racine);
		 }
	  }
	  else {
	    if ( (racine > LesVi(LesVi.Length()) + tol2d) &&
		 (racine < VLast-tol2d) ) {
		   LesVi.Append(racine);
		 }
	  }
	}
      }
      else {
# if CHFI3D_DEB
	cout << "Failed calculation of the minimum length" << endl;
# endif
      }
    }
    // update if non duplication
    a = b;
    precedant = courant;
    b = c;
    courant = suivant;
  }

  // (2) Update of the sequence of SurfData
  if (LesVi.Length()>0) {
    TopOpeBRepDS_DataStructure& DStru = myDS->ChangeDS();
    Handle(ChFiDS_SurfData) SD;
    TopOpeBRepDS_Surface S;
    TopOpeBRepDS_Curve C1, C2;
    Standard_Real T, VertexTol;
    gp_Pnt P3d, P1, P2;
    ChFiDS_CommonPoint LePoint;
    for (ii=1 ; ii<=LesVi.Length(); ii++) {

      T = LesVi(ii);
      // (2.0) copy and insertion
      SD = new (ChFiDS_SurfData);
      SD->Copy(ref);
      SeqData.InsertBefore(ii, SD);
      S = DStru.Surface(ref->Surf());
      SD->ChangeSurf(DStru.AddSurface(S));
      C1 = DStru.Curve(SD->InterferenceOnS1().LineIndex());
      SD->ChangeInterferenceOnS1().SetLineIndex( DStru.AddCurve(C1));
      C2 = DStru.Curve(SD->InterferenceOnS2().LineIndex());
      SD->ChangeInterferenceOnS2().SetLineIndex(DStru.AddCurve(C2));

      // (2.1) Modification of common Point
      SD-> ChangeVertexLastOnS1().Reset();
      SD-> ChangeVertexLastOnS2().Reset();
      ref->ChangeVertexFirstOnS1().Reset();
      ref->ChangeVertexFirstOnS2().Reset();
      Courbe1->D0(T, P1);
      Courbe2->D0(T, P2);
      P3d.SetXYZ((P1.XYZ()+ P2.XYZ())/2);
      VertexTol = P1.Distance(P2);
      VertexTol += Max(C1.Tolerance(), C2.Tolerance());

      SD->ChangeVertexLastOnS1().SetPoint(P3d);
      SD->ChangeVertexLastOnS2().SetPoint(P3d);
      ref->ChangeVertexFirstOnS1().SetPoint(P3d);
      ref->ChangeVertexFirstOnS2().SetPoint(P3d);
      SD->ChangeVertexLastOnS1().SetTolerance(VertexTol);
      SD->ChangeVertexLastOnS2().SetTolerance(VertexTol);
      ref->ChangeVertexFirstOnS1().SetTolerance(VertexTol);
      ref->ChangeVertexFirstOnS2().SetTolerance(VertexTol);     

      // (2.2) Modification of interferences
      SD->ChangeInterferenceOnS1().SetLastParameter(T);
      SD->ChangeInterferenceOnS2().SetLastParameter(T);
      ref->ChangeInterferenceOnS1().SetFirstParameter(T);
      ref->ChangeInterferenceOnS2().SetFirstParameter(T);

      // Parameters on ElSpine
      SD->LastSpineParam(T);
      ref->FirstSpineParam(T);
    }
  }
}

//=======================================================================
//function : ExtentOneCorner
//purpose  : 
//=======================================================================

void ChFi3d_FilBuilder::ExtentOneCorner(const TopoDS_Vertex& V,
					const Handle(ChFiDS_Stripe)& S)
{
  // review by using the data at end of fillets (point, radius, normal
  // to the faces and tangents of the guideline).
  Standard_Integer      Sens = 0;
  Standard_Real         Coeff = 0.5;
  Handle(ChFiDS_Spine)  Spine = S->Spine();
  ChFi3d_IndexOfSurfData(V,S,Sens);
  Standard_Real dU = Spine->LastParameter(Spine->NbEdges());
  if (Spine->IsTangencyExtremity((Sens == 1))) 
    return; //No extension in the queue

  if (Spine->Status((Sens == 1)) == ChFiDS_FreeBoundary) {
    Coeff *= 2; // It is necessary to go to the end and to evaluate the length
  }

  if (Sens == 1) {
    Spine->SetFirstParameter(-dU*Coeff);
    Spine->SetFirstTgt(0.);
  }
  else{
    Spine->SetLastParameter(dU*(1.+Coeff));
    Spine->SetLastTgt(dU);
  }
}

//=======================================================================
//function : ExtentTwoCorner
//purpose  : 
//=======================================================================

void ChFi3d_FilBuilder::ExtentTwoCorner(const TopoDS_Vertex& V,
					const ChFiDS_ListOfStripe& LS)
{
  // Review by using the data at end of fillets (point, radius, normal
  // to faces and tangents to the guideline.
  Standard_Integer Sens;
  Standard_Real    Coeff = 0.3, Eval=0.0, dU, rad;
  ChFiDS_ListIteratorOfListOfStripe itel(LS);
  Standard_Boolean FF = Standard_True;
  Handle(ChFiDS_Stripe) Stripe;
  Handle(ChFiDS_Spine)  Spine;

  // A value of symetric extension is calculated
  for ( ; itel.More(); itel.Next()) {    
   Stripe = itel.Value();
   Spine = Stripe->Spine();
   dU = Spine->LastParameter(Spine->NbEdges())*Coeff;
   Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Spine);
   if (fsp->IsConstant()) 
     rad =  fsp->Radius();
   else
     {
       TopoDS_Edge E = ChFi3d_EdgeFromV1( V, itel.Value(), Sens);
       rad = MaxRad( fsp, E );
       /*
       IE = ChFi3d_IndexOfSurfData(V,itel.Value(),Sens);
       rad = MaxRad(fsp, IE);
       */
     }
   rad *= 1.5;
   if (rad > dU) dU = rad;
   if (dU > Eval) Eval = dU;
 }

  // One applies
  for (itel.Initialize(LS) ; itel.More(); itel.Next()) {    
    ChFi3d_IndexOfSurfData(V,itel.Value(),Sens);
    if (!FF && Stripe == itel.Value()) Sens = -Sens;
    Stripe = itel.Value();
    Spine = Stripe->Spine();
    if (! Spine->IsTangencyExtremity((Sens == 1))) { //No extension on queue
      if (Sens == 1){ 
	Spine->SetFirstParameter(-Eval);
	Spine->SetFirstTgt(0.);
      }
      else{
	dU = Spine->LastParameter(Spine->NbEdges());
	Spine->SetLastParameter(dU+Eval);
	Spine->SetLastTgt(dU);
      }
      FF = Standard_False;
    }
  }
}


//=======================================================================
//function : ExtentThreeCorner
//purpose  : 
//=======================================================================

void ChFi3d_FilBuilder::ExtentThreeCorner(const TopoDS_Vertex& V,
					  const ChFiDS_ListOfStripe& LS)
{
  // Review by using the data at end of fillets (point, radius, normal
  // to faces and tangents to the guideline.
  Standard_Integer Sens = 0;
  Standard_Real    Coeff = 0.1;
  ChFiDS_ListOfStripe check;
//  Standard_Boolean FF = Standard_True;
  for(ChFiDS_ListIteratorOfListOfStripe itel(LS); itel.More(); itel.Next()) {    
    Handle(ChFiDS_Stripe) Stripe = itel.Value(); 
    ChFi3d_IndexOfSurfData(V,Stripe,Sens);
    for(ChFiDS_ListIteratorOfListOfStripe ich(check); ich.More(); ich.Next()){
      if(Stripe == ich.Value()){
	Sens = -Sens;
	break;
      }
    }
    Handle(ChFiDS_Spine) Spine = Stripe->Spine();
    if (Spine->IsTangencyExtremity((Sens == 1))) return; //No extension on queue
    Standard_Real dU = Spine->LastParameter(Spine->NbEdges());
    if (Sens == 1){ 
      Spine->SetFirstParameter(-dU*Coeff);
      Spine->SetFirstTgt(0.);
    }
    else{
      Spine->SetLastParameter(dU*(1.+Coeff));
      Spine->SetLastTgt(dU);
    }
    check.Append(Stripe);
  }
}


//=======================================================================
//function : SetRegul
//purpose  : 
//=======================================================================

void ChFi3d_FilBuilder::SetRegul()

{
  ChFiDS_ListIteratorOfRegularities it;
  TopTools_ListIteratorOfListOfShape itc;
  TopTools_ListIteratorOfListOfShape its1;
  TopTools_ListIteratorOfListOfShape its2;
  BRep_Builder B;
  for (it.Initialize(myRegul); it.More(); it.Next()){
    const ChFiDS_Regul& reg = it.Value();
    itc.Initialize(myCoup->NewEdges(reg.Curve()));
    if(itc.More()){
      TopoDS_Edge E = TopoDS::Edge(itc.Value());
      if(reg.IsSurface1()) its1.Initialize(myCoup->NewFaces(reg.S1()));
      else its1.Initialize(myCoup->Merged(myDS->Shape(reg.S1()),TopAbs_IN));
      if(reg.IsSurface2()) its2.Initialize(myCoup->NewFaces(reg.S2()));
      else its2.Initialize(myCoup->Merged(myDS->Shape(reg.S2()),TopAbs_IN));
      if(its1.More() && its2.More()){
	TopoDS_Face F1 = TopoDS::Face(its1.Value());
	TopoDS_Face F2 = TopoDS::Face(its2.Value());
	GeomAbs_Shape cont = ChFi3d_evalconti(E,F1,F2);
 	B.Continuity(E,F1,F2,cont);
      }
    }
  }
}
