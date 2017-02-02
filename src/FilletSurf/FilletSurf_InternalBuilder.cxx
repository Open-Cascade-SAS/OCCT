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


#include <Adaptor3d_TopolTool.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_HCurve.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepBlend_ConstRad.hxx>
#include <BRepBlend_ConstRadInv.hxx>
#include <BRepBlend_Line.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <ChFi3d_Builder_0.hxx>
#include <ChFi3d_FilBuilder.hxx>
#include <ChFiDS_CircSection.hxx>
#include <ChFiDS_CommonPoint.hxx>
#include <ChFiDS_FaceInterference.hxx>
#include <ChFiDS_FilSpine.hxx>
#include <ChFiDS_HData.hxx>
#include <ChFiDS_HElSpine.hxx>
#include <ChFiDS_ListIteratorOfListOfStripe.hxx>
#include <ChFiDS_ListOfStripe.hxx>
#include <ChFiDS_SecHArray1.hxx>
#include <ChFiDS_SequenceOfSurfData.hxx>
#include <ChFiDS_Spine.hxx>
#include <ChFiDS_Stripe.hxx>
#include <ChFiDS_SurfData.hxx>
#include <ElSLib.hxx>
#include <FilletSurf_ErrorTypeStatus.hxx>
#include <FilletSurf_InternalBuilder.hxx>
#include <FilletSurf_StatusDone.hxx>
#include <FilletSurf_StatusType.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Circ.hxx>
#include <gp_Pln.hxx>
#include <IntCurveSurface_HInter.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

static Standard_Boolean isinlist(const TopoDS_Shape&         E,
				 const TopTools_ListOfShape& L){
  TopTools_ListIteratorOfListOfShape It;
  for (It.Initialize(L); It.More(); It.Next()){
    if(E.IsSame(It.Value())) return 1;
  }
  return 0;
}

static Standard_Boolean IntPlanEdge(Handle(BRepAdaptor_HCurve)& Ed,
				    const gp_Pln&               P,
				    Standard_Real&              w,
				    const Standard_Real         tol3d)
{
  Standard_Boolean done = 0;
  Standard_Real f = Ed->FirstParameter();
  Standard_Real l = Ed->LastParameter();
  gp_Pnt Or = P.Location();
  Handle(Geom_Plane) Pln = new Geom_Plane(P);
  Handle(GeomAdaptor_HSurface) 
    Plan = new GeomAdaptor_HSurface(GeomAdaptor_Surface(Pln));

  IntCurveSurface_HInter Intersection;
  Standard_Integer nbp ,iip;
  IntCurveSurface_IntersectionPoint IP;
  Standard_Real dist = RealLast();
  
  Intersection.Perform(Ed,Plan);
  
  if(Intersection.IsDone()){
    nbp = Intersection.NbPoints();
    for (iip = 1; iip <= nbp; iip++){
      IP = Intersection.Point(iip);
      gp_Pnt pint = IP.Pnt();
      Standard_Real d = pint.Distance(Or);
      if(d<dist){
	done = 1;
	w = IP.W();
	dist = d;
      }
    }
  }
  gp_Pnt pdeb = Ed->Value(f);
  gp_Pnt pfin = Ed->Value(l);
  Standard_Real u,v;
  //check if the extremities are not solution
  ElSLib::Parameters(P,pdeb,u,v);
  gp_Pnt projdeb = ElSLib::Value(u,v,P);
  Standard_Real dprojdeb = pdeb.Distance(projdeb);
  if(dprojdeb<tol3d){
    Standard_Real d = pdeb.Distance(Or);
    if(d<dist){
      done = 1;
      w = f;
      dist = d;
    }
  }
  ElSLib::Parameters(P,pfin,u,v);
  gp_Pnt projfin = ElSLib::Value(u,v,P);
  Standard_Real dprojfin = pfin.Distance(projfin);
  if(dprojfin<tol3d){
    Standard_Real d = pfin.Distance(Or);
    if(d<dist){
      done = 1;
      w = l;
      dist = d;
    }
  }
  return done;
}

static Standard_Boolean ComputeEdgeParameter(const Handle(ChFiDS_Spine)&    Spine, 
					     const Standard_Integer         ind,
					     const Standard_Real            pelsp,
					     Standard_Real&                 ped,
					     const Standard_Real            tol3d)
{
  Handle(ChFiDS_HElSpine) Guide = Spine->ElSpine(ind);
  gp_Pnt P; gp_Vec V;
  Guide->D1(pelsp,P,V);
  gp_Pln pln(P,V);
  Handle(BRepAdaptor_HCurve) ed = new BRepAdaptor_HCurve();
  ed->ChangeCurve() = Spine->CurrentElementarySpine(ind);
  return IntPlanEdge(ed,pln,ped,tol3d);
}
					     
//=======================================================================
//function : FilletSurf_InternalBuilder
//purpose  : 
//=======================================================================

FilletSurf_InternalBuilder::FilletSurf_InternalBuilder
(const TopoDS_Shape&      S, 
 const ChFi3d_FilletShape FShape, 
 const Standard_Real      Ta,
 const Standard_Real      Tapp3d,
 const Standard_Real      Tapp2d):ChFi3d_FilBuilder(S,FShape,Ta)
{
  SetParams(Ta,Tapp3d,Tapp2d,
	    Tapp3d,Tapp2d,1.e-3);
  SetContinuity(GeomAbs_C2,Ta);
}

//=======================================================================
//function : Add
//purpose  : creation of spine on a set of edges
// 
//  0 : no problem  
//  1 : empty list 
//  2 : non g1 edges
//  3 : non G1 adjacent faces
//  4 : edge is not on the shape
//  5 : edge is not alive 
//=======================================================================

Standard_Integer  FilletSurf_InternalBuilder::Add(const TopTools_ListOfShape& E, 
						  const Standard_Real R)
{
  if(E.IsEmpty()) return 1;
  TopTools_ListIteratorOfListOfShape It;
  for(It.Initialize(E); It.More(); It.Next()){
    TopoDS_Edge cured = TopoDS::Edge(It.Value());
    if(cured.IsNull()) return 4;
    if(!myEFMap.Contains(cured)) return 4;
    //check if the edge is a fracture edge
    TopoDS_Face ff1,ff2;  
    for(It.Initialize(myEFMap(cured));It.More();It.Next()){  
      if (ff1.IsNull()) {
	ff1 = TopoDS::Face(It.Value());
      }
      else {
	ff2 = TopoDS::Face(It.Value());
	if(!ff2.IsSame(ff1)){
	  break;
	}
      }
    }  
    if(ff1.IsNull() || ff2.IsNull()) return 5;
    if(ff1.IsSame(ff2)) return 5;
    if(BRep_Tool::Continuity(cured,ff1,ff2) != GeomAbs_C0) return 5;
  }  
  TopoDS_Edge ed = TopoDS::Edge(E.First());
  ed.Orientation(TopAbs_FORWARD);
  ChFi3d_FilBuilder::Add(R,ed);
  Handle(ChFiDS_Stripe) st = myListStripe.First();
  Handle(ChFiDS_Spine)& sp = st->ChangeSpine();
  Standard_Boolean periodic = sp->IsPeriodic();
  
  //It is checked if edges of list E are in the contour,
  //the edges that arenot in the list are removed from the contour,
  //it is checked that the remainder is monoblock.
 
  for(It.Initialize(E); It.More(); It.Next()){
    TopoDS_Edge cured = TopoDS::Edge(It.Value());
    if(!Contains(cured)) return 2;
  }  
        
  Handle(ChFiDS_FilSpine) newsp = new ChFiDS_FilSpine();
  Standard_Boolean debut = 0;
  Standard_Integer premierquinyestpas = 0;
  Standard_Integer yatrou = 0;
  for(Standard_Integer i = 1; i <= sp->NbEdges(); i++){
    TopoDS_Edge cured = sp->Edges(i);
    if(isinlist(cured,E)){
      debut = 1;
      if(premierquinyestpas) {
	yatrou = 1;
	break;
      }
      newsp->SetEdges(cured);
    }
    else if(debut && !premierquinyestpas) premierquinyestpas = i;
  }
  if(!periodic && yatrou) return 2;
  if(periodic && yatrou){
    Standard_Boolean vraitrou = 0, aLocalDebut = 0;
    for(Standard_Integer i = sp->NbEdges(); i > yatrou; i--){
      TopoDS_Edge cured = sp->Edges(i);
      if(isinlist(cured,E)){
	if(vraitrou) return 2;
	newsp->PutInFirst(cured);
      }
      else if(aLocalDebut) vraitrou = 1;
      aLocalDebut = 1;
    }
  }

  if(newsp->NbEdges() != sp->NbEdges()) {
    newsp->Load();
    newsp->SetRadius(R);
    sp = newsp;
  }

  //ElSpine is immediately constructed
  Handle(ChFiDS_HElSpine) hels =  new ChFiDS_HElSpine();
  gp_Vec TFirst,TLast;
  gp_Pnt PFirst,PLast;
  sp->D1(sp->FirstParameter(),PFirst,TFirst);
  sp->D1(sp->LastParameter(),PLast,TLast);
  hels->ChangeCurve().FirstParameter(sp->FirstParameter());
  hels->ChangeCurve().SetFirstPointAndTgt(PFirst,TFirst);
  hels->ChangeCurve().LastParameter(sp->LastParameter());
  hels->ChangeCurve().SetLastPointAndTgt(PLast,TLast);
  ChFi3d_PerformElSpine(hels,sp,myConti,tolesp);
  sp->AppendElSpine(hels);
  sp->SplitDone(Standard_True);
  return 0;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void FilletSurf_InternalBuilder::Perform()
{
  //PerformSetOfSurfOnElSpine is enough.
  
  Handle(ChFiDS_Stripe) Stripe = myListStripe.First();
  Handle(ChFiDS_HData)&  HData  = Stripe->ChangeSetOfSurfData();
  HData =  new ChFiDS_HData();
  Handle(ChFiDS_Spine)& Spine = Stripe->ChangeSpine();
  TopAbs_Orientation RefOr1,RefOr2;
  Standard_Integer RefChoix;
  StripeOrientations(Spine,RefOr1,RefOr2,RefChoix);
  Stripe->OrientationOnFace1(RefOr1);
  Stripe->OrientationOnFace2(RefOr2);
  Stripe->Choix(RefChoix);
  PerformSetOfKGen(Stripe,0);
}

//=======================================================================
//function : PerformSurf
//purpose  : 
//=======================================================================

Standard_Boolean
 FilletSurf_InternalBuilder::PerformSurf(ChFiDS_SequenceOfSurfData& SeqData, 
					 const Handle(ChFiDS_HElSpine)& Guide, 
					 const Handle(ChFiDS_Spine)& Spine, 
					 const Standard_Integer Choix, 
					 const Handle(BRepAdaptor_HSurface)& S1, 
					 const Handle(Adaptor3d_TopolTool)& I1, 
					 const Handle(BRepAdaptor_HSurface)& S2, 
					 const Handle(Adaptor3d_TopolTool)& I2, 
					 const Standard_Real MaxStep, 
					 const Standard_Real Fleche, 
					 const Standard_Real TolGuide, 
					 Standard_Real& First, 
					 Standard_Real& Last, 
					 const Standard_Boolean Inside, 
					 const Standard_Boolean Appro, 
					 const Standard_Boolean Forward, 
					 const Standard_Boolean RecOnS1, 
					 const Standard_Boolean RecOnS2, 
					 const math_Vector& Soldep, 
					 Standard_Integer& Intf, 
					 Standard_Integer& Intl)
{
  Handle(ChFiDS_SurfData) Data = SeqData(1);
  Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(Spine);
  if(fsp.IsNull()) throw Standard_ConstructionError("PerformSurf : this is not the spine of a fillet");
  Handle(BRepBlend_Line) lin;
  TopAbs_Orientation Or = S1->ChangeSurface().Face().Orientation();
  if(!fsp->IsConstant()) throw Standard_ConstructionError("PerformSurf : no variable radiuses");
  // Standard_Boolean maybesingular; //pour scinder les Surfdata singulieres 
  
  BRepBlend_ConstRad Func(S1,S2,Guide);
  BRepBlend_ConstRadInv FInv(S1,S2,Guide);
  Func.Set(fsp->Radius(),Choix);
  FInv.Set(fsp->Radius(),Choix);
  switch (GetFilletShape()) {
  case ChFi3d_Rational:
    Func.Set(BlendFunc_Rational);
    break;
  case ChFi3d_QuasiAngular:
    Func.Set(BlendFunc_QuasiAngular);
    break;
  case ChFi3d_Polynomial:
    Func.Set(BlendFunc_Polynomial);
  }
  Standard_Real PFirst = First;
  done = SimulData(Data,Guide,lin,S1,I1,
		   S2,I2,Func,FInv,PFirst,MaxStep,Fleche,
		   TolGuide,First,Last,Inside,Appro,Forward,Soldep,
		   20,RecOnS1,RecOnS2);
  if(!done) return Standard_False;
  if(lin->StartPointOnFirst().NbPointOnRst() !=0){
    ChFi3d_FilCommonPoint(lin->StartPointOnFirst(),lin->TransitionOnS1(),
			  Standard_True, Data->ChangeVertexFirstOnS1(), tolesp);
  }
  if(lin->EndPointOnFirst().NbPointOnRst() !=0){
    ChFi3d_FilCommonPoint(lin->EndPointOnFirst(),lin->TransitionOnS1(),
			  Standard_False,Data->ChangeVertexLastOnS1(), tolesp);
  }
  if(lin->StartPointOnSecond().NbPointOnRst() !=0){
    ChFi3d_FilCommonPoint(lin->StartPointOnSecond(),lin->TransitionOnS2(),
			  Standard_True, Data->ChangeVertexFirstOnS2(), tolesp);
  }
  if(lin->EndPointOnSecond().NbPointOnRst() !=0){
    ChFi3d_FilCommonPoint(lin->EndPointOnSecond(),lin->TransitionOnS2(),
			  Standard_False, Data->ChangeVertexLastOnS2(), tolesp);
  }
  done = CompleteData(Data,Func,lin,S1,S2,Or,0,0,0,0);
  if(!done)  throw Standard_Failure("PerformSurf : Failed approximation!");
//  maybesingular = (Func.GetMinimalDistance()<=100*tolapp3d);
  Standard_Boolean ok = Standard_False;
  if(!Forward){
    Intf = 0;
    const ChFiDS_CommonPoint& cpf1 = Data->VertexFirstOnS1();
    if(cpf1.IsOnArc()){
      TopoDS_Face F1 = S1->ChangeSurface().Face();
      TopoDS_Face bid;
      Intf = !SearchFace(Spine,cpf1,F1,bid);
      ok = Intf != 0;
    }
    const ChFiDS_CommonPoint& cpf2 = Data->VertexFirstOnS2();
    if(cpf2.IsOnArc() && !ok){
      TopoDS_Face F2 = S2->ChangeSurface().Face();
      TopoDS_Face bid;
      Intf = !SearchFace(Spine,cpf2,F2,bid);
    }
  }
  Intl = 0;
  ok = Standard_False;
  const ChFiDS_CommonPoint& cpl1 = Data->VertexLastOnS1();
  if(cpl1.IsOnArc()){
    TopoDS_Face F1 = S1->ChangeSurface().Face();
    TopoDS_Face bid;
    Intl = !SearchFace(Spine,cpl1,F1,bid);
    ok = Intl != 0;
  }
  const ChFiDS_CommonPoint& cpl2 = Data->VertexLastOnS2();
  if(cpl2.IsOnArc() && !ok){
    TopoDS_Face F2 = S2->ChangeSurface().Face();
    TopoDS_Face bid;
    Intl = !SearchFace(Spine,cpl2,F2,bid);
  }
  Data->FirstSpineParam(First);
  Data->LastSpineParam (Last);

//  if (maybesingular) SplitSurf(SeqData, lin);
//  Necessite de trimer resultats : A faire
  return Standard_True;
}

void FilletSurf_InternalBuilder::PerformSurf (ChFiDS_SequenceOfSurfData& , const Handle(ChFiDS_HElSpine)& , const Handle(ChFiDS_Spine)& , const Standard_Integer , const Handle(BRepAdaptor_HSurface)& , const Handle(Adaptor3d_TopolTool)& , const Handle(BRepAdaptor_HCurve2d)& , const Handle(BRepAdaptor_HSurface)& , const Handle(BRepAdaptor_HCurve2d)& , Standard_Boolean& , const Handle(BRepAdaptor_HSurface)& , const Handle(Adaptor3d_TopolTool)& , const TopAbs_Orientation , const Standard_Real , const Standard_Real , const Standard_Real , Standard_Real& , Standard_Real& , const Standard_Boolean , const Standard_Boolean , const Standard_Boolean , const Standard_Boolean , const Standard_Boolean , const Standard_Boolean , const math_Vector& )
{
 throw Standard_DomainError("BlendFunc_CSConstRad::Section : Not implemented");
}

void FilletSurf_InternalBuilder::PerformSurf (ChFiDS_SequenceOfSurfData& , const Handle(ChFiDS_HElSpine)& , const Handle(ChFiDS_Spine)& , const Standard_Integer , const Handle(BRepAdaptor_HSurface)& , const Handle(Adaptor3d_TopolTool)& , const TopAbs_Orientation , const Handle(BRepAdaptor_HSurface)& , const Handle(Adaptor3d_TopolTool)& , const Handle(BRepAdaptor_HCurve2d)& , const Handle(BRepAdaptor_HSurface)& , const Handle(BRepAdaptor_HCurve2d)& , Standard_Boolean& , const Standard_Real , const Standard_Real , const Standard_Real , Standard_Real& , Standard_Real& , const Standard_Boolean , const Standard_Boolean , const Standard_Boolean , const Standard_Boolean , const Standard_Boolean , const Standard_Boolean , const math_Vector& )
{
 throw Standard_DomainError("BlendFunc_CSConstRad::Section : Not implemented");
}

void FilletSurf_InternalBuilder::PerformSurf (ChFiDS_SequenceOfSurfData& , const Handle(ChFiDS_HElSpine)& , const Handle(ChFiDS_Spine)& , const Standard_Integer , const Handle(BRepAdaptor_HSurface)& , const Handle(Adaptor3d_TopolTool)& , const Handle(BRepAdaptor_HCurve2d)& , const Handle(BRepAdaptor_HSurface)& , const Handle(BRepAdaptor_HCurve2d)& , Standard_Boolean& , const TopAbs_Orientation , const Handle(BRepAdaptor_HSurface)& , const Handle(Adaptor3d_TopolTool)& , const Handle(BRepAdaptor_HCurve2d)& , const Handle(BRepAdaptor_HSurface)& , const Handle(BRepAdaptor_HCurve2d)& , Standard_Boolean& , const TopAbs_Orientation , const Standard_Real , const Standard_Real , const Standard_Real , Standard_Real& , Standard_Real& , const Standard_Boolean , const Standard_Boolean , const Standard_Boolean , const Standard_Boolean , const Standard_Boolean , const Standard_Boolean , const Standard_Boolean , const math_Vector& )
{
 throw Standard_DomainError("BlendFunc_CSConstRad::Section : Not implemented");
}

Standard_Boolean  FilletSurf_InternalBuilder::Done() const
{
 return done;
}
//=======================================================================
//function : NbSurface
//purpose  :  gives the number of NUBS surfaces  of the Fillet
//=======================================================================

Standard_Integer FilletSurf_InternalBuilder::NbSurface() const 
{
  return myListStripe.First()->SetOfSurfData()->Length();
}

//=======================================================================
//function : SurfaceFillet
//purpose  : gives the NUBS surface of index Index
//=======================================================================

const Handle(Geom_Surface)& FilletSurf_InternalBuilder::SurfaceFillet(const Standard_Integer Index) const 
{
  Standard_Integer isurf = myListStripe.First()->SetOfSurfData()->Value(Index)->Surf();

  return myDS->Surface(isurf).Surface();
}

 //=======================================================================
//function : TolApp3d
//purpose  :  gives the 3d tolerance reached during approximation 
//=======================================================================

Standard_Real  FilletSurf_InternalBuilder::TolApp3d(const Standard_Integer Index) const 
{
  Standard_Integer isurf = myListStripe.First()->SetOfSurfData()->Value(Index)->Surf();

  return myDS->Surface(isurf).Tolerance();
}

//=======================================================================
//function : SupportFace1 
//purpose  : gives the first support  face relative to SurfaceFillet(Index)
//=======================================================================
const TopoDS_Face& FilletSurf_InternalBuilder::SupportFace1(const Standard_Integer Index) const
{
  Standard_Integer isurf = myListStripe.First()->SetOfSurfData()->Value(Index)->IndexOfS1();

  return TopoDS::Face(myDS->Shape(isurf));
}
//=======================================================================
//function : SupportFace2
//purpose  : gives the second support face relative to SurfaceFillet(Index)
//=======================================================================
const TopoDS_Face& FilletSurf_InternalBuilder::SupportFace2(const Standard_Integer Index) const 
{
  Standard_Integer isurf = myListStripe.First()->SetOfSurfData()->Value(Index)->IndexOfS2();

  return TopoDS::Face(myDS->Shape(isurf));
}
//===============================================================================
//function : CurveOnSup1 
//purpose  :  gives  the 3d curve  of SurfaceFillet(Index)  on SupportFace1(Index)
//===============================================================================

const Handle(Geom_Curve)& FilletSurf_InternalBuilder::CurveOnFace1(const Standard_Integer Index) const 
{
  Standard_Integer icurv = myListStripe.First()->SetOfSurfData()->Value(Index)->
    InterferenceOnS1().LineIndex();
  return myDS->Curve(icurv).Curve();
}

//=======================================================================
//function : CurveOnSup2
//purpose  : gives the 3d  curve of  SurfaceFillet(Index) on SupportFace2(Index
//=======================================================================

const Handle(Geom_Curve)& FilletSurf_InternalBuilder::CurveOnFace2(const Standard_Integer Index) const 
{
  Standard_Integer icurv=myListStripe.First()->SetOfSurfData()->Value(Index)->
    InterferenceOnS2().LineIndex();
  return myDS->Curve(icurv).Curve();
}
//=======================================================================
//function : PCurveOnFace1
//purpose  : gives the  PCurve associated to CurvOnSup1(Index)  on the support face
//=======================================================================
const Handle(Geom2d_Curve)& FilletSurf_InternalBuilder::PCurveOnFace1(const Standard_Integer Index) const 
{
  return myListStripe.First()->SetOfSurfData()->Value(Index)->
    InterferenceOnS1().PCurveOnFace();

}

//=======================================================================
//function : PCurveOnFillet1
//purpose  : gives the PCurve associated to CurveOnFace1(Index) on the Fillet
//=======================================================================

const Handle(Geom2d_Curve)& FilletSurf_InternalBuilder::PCurve1OnFillet(const Standard_Integer Index) const 
{return myListStripe.First()->SetOfSurfData()->Value(Index)->
    InterferenceOnS1().PCurveOnSurf();
 
}

//=======================================================================
//function : PCurveOnFace2
//purpose  : gives the  PCurve associated to CurvOnSup2(Index)  on the support face
//=======================================================================
const Handle(Geom2d_Curve)& FilletSurf_InternalBuilder::PCurveOnFace2(const Standard_Integer Index) const 
{
  return myListStripe.First()->SetOfSurfData()->Value(Index)->
    InterferenceOnS2().PCurveOnFace();
}

//=======================================================================
//function : PCurveOnFillet2
//purpose  : gives the PCurve associated to CurveOnFace2(Index) on the Fillet
//=======================================================================
const Handle(Geom2d_Curve)& FilletSurf_InternalBuilder::PCurve2OnFillet(const Standard_Integer Index) const 
{return myListStripe.First()->SetOfSurfData()->Value(Index)->
    InterferenceOnS2().PCurveOnSurf();
  
}

//=======================================================================
//function : FirstParameter 
//purpose  : gives the parameter of the fillet  on the first edge
//=======================================================================

Standard_Real FilletSurf_InternalBuilder::FirstParameter() const
{
  const Handle(ChFiDS_Stripe)& st = myListStripe.First();
  const Handle(ChFiDS_Spine)& sp = st->Spine();
  const Handle(ChFiDS_SurfData)& sd = st->SetOfSurfData()->Value(1);
  Standard_Real p = sd->FirstSpineParam();
  Standard_Integer ind = 1;
  if(sp->IsPeriodic()) ind = sp->Index(p);
  Standard_Real ep;
  if(ComputeEdgeParameter(sp,ind,p,ep,tolesp)) return ep;
  return 0.0;
}
//=======================================================================
//function : LastParameter
//purpose  :  gives the parameter of the fillet  on the last edge
//=======================================================================
Standard_Real FilletSurf_InternalBuilder::LastParameter() const
{
  const Handle(ChFiDS_Stripe)& st = myListStripe.First();
  const Handle(ChFiDS_Spine)& sp = st->Spine();
  const Handle(ChFiDS_SurfData)& sd = st->SetOfSurfData()->Value(NbSurface());
  Standard_Real p = sd->LastSpineParam();
  Standard_Integer ind = sp->NbEdges();
  if(sp->IsPeriodic()) ind = sp->Index(p);
  Standard_Real ep;
  if(ComputeEdgeParameter(sp,ind,p,ep,tolesp)) return ep;
  return 0.0;
}

//=======================================================================
//function : StatusStartSection
//purpose  :  returns: 
//       twoExtremityonEdge: each extremity of  start section of the Fillet is
//                        on the edge of  the corresponding support face.  
//       OneExtremityOnEdge:  only one  of  the extremities of  start  section  of the  Fillet 
//                           is on the  edge of the corresponding support face.  
//       NoExtremityOnEdge:  any extremity of  start  section  of the fillet is  on  
//                           the edge  of   the  corresponding support face.
//=======================================================================


FilletSurf_StatusType  FilletSurf_InternalBuilder::StartSectionStatus() const 
{ 
 
   Standard_Boolean isonedge1 = myListStripe.First()->SetOfSurfData()->Value(1)->
                                VertexFirstOnS1().IsOnArc();
   Standard_Boolean isonedge2=  myListStripe.First()->SetOfSurfData()->Value(1)->
                                VertexFirstOnS2().IsOnArc();;

  if (isonedge1 && isonedge2) 
  {return FilletSurf_TwoExtremityOnEdge;}
  else if ((!isonedge1)&& (!isonedge2)) 
  {return FilletSurf_NoExtremityOnEdge ;}
  else 
    {return FilletSurf_OneExtremityOnEdge;}
}
//=======================================================================
//function : StatusEndSection()
//purpose  :  returns: 
//            twoExtremityonEdge: each extremity of  end section of the Fillet is
//                                on the edge of  the corresponding support face.  
//            OneExtremityOnEdge: only one  of  the extremities of  end section  of the  Fillet 
//                                is on the  edge of the corresponding support face.  
//            NoExtremityOnEdge:  any extremity of   end  section  of the fillet is  on  
//                                the edge  of   the  corresponding support face.
//=======================================================================
FilletSurf_StatusType  FilletSurf_InternalBuilder::EndSectionStatus() const 
{ 
   Standard_Boolean isonedge1 = myListStripe.First()->SetOfSurfData()->Value(NbSurface())->
                                VertexLastOnS1().IsOnArc();
   Standard_Boolean isonedge2=  myListStripe.First()->SetOfSurfData()->Value(NbSurface())->
                                VertexLastOnS2().IsOnArc();

  if (isonedge1 && isonedge2) 
  { return FilletSurf_TwoExtremityOnEdge;}
  else if ((!isonedge1)&& (!isonedge2)) 
   { return FilletSurf_NoExtremityOnEdge;}
  else 
    { return FilletSurf_OneExtremityOnEdge;}
}


//=======================================================================
//function : Simulate
//purpose  : computes only the sections used in the computation of the fillet
//=======================================================================
void FilletSurf_InternalBuilder::Simulate()
{
 //ChFi3d_FilBuilder::Simulate(1);
 Handle(ChFiDS_Stripe) Stripe = myListStripe.First();
  Handle(ChFiDS_HData)&  HData  = Stripe->ChangeSetOfSurfData();
  HData =  new ChFiDS_HData();
  Handle(ChFiDS_Spine)& Spine = Stripe->ChangeSpine();
  TopAbs_Orientation RefOr1,RefOr2;
  Standard_Integer RefChoix;
  StripeOrientations(Spine,RefOr1,RefOr2,RefChoix);
  Stripe->OrientationOnFace1(RefOr1);
  Stripe->OrientationOnFace2(RefOr2);
  Stripe->Choix(RefChoix);
  PerformSetOfKGen(Stripe,1);
}


//=======================================================================
//function : NbSection
//purpose  :  gives the number of sections relative to SurfaceFillet(IndexSurf)
//=======================================================================
Standard_Integer FilletSurf_InternalBuilder::NbSection(const Standard_Integer IndexSurf) const 
{
return Sect(1,IndexSurf)->Length();
}

//=======================================================================
//function : Section
//purpose  : gives the   arc of circle corresponding    to section number 
//           IndexSec  of  SurfaceFillet(IndexSurf)  (The   basis curve  of the 
//           trimmed curve is a Geom_Circle) 
//=======================================================================
void FilletSurf_InternalBuilder::Section(const Standard_Integer IndexSurf, const Standard_Integer IndexSec, Handle(Geom_TrimmedCurve)& Circ) const 
{
 gp_Circ c;
 Standard_Real deb,fin; 
 Sect(1,IndexSurf)->Value(IndexSec).Get(c,deb,fin);
 Handle(Geom_Circle) Gc = new Geom_Circle(c);
 Circ= new Geom_TrimmedCurve(Gc,deb,fin);
}















