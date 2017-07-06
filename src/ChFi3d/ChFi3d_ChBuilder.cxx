// Created on: 1995-04-26
// Created by: Flore Lantheaume
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


#include <Adaptor3d_TopolTool.hxx>
#include <Blend_Point.hxx>
#include <BlendFunc_SectionShape.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBlend_Chamfer.hxx>
#include <BRepBlend_ChamfInv.hxx>
#include <BRepBlend_ChAsym.hxx>
#include <BRepBlend_ChAsymInv.hxx>
#include <BRepBlend_Line.hxx>
#include <BRepBlend_Walking.hxx>
#include <BRepTools.hxx>
#include <ChFi3d.hxx>
#include <ChFi3d_Builder_0.hxx>
#include <ChFi3d_ChBuilder.hxx>
#include <ChFiDS_ChamfSpine.hxx>
#include <ChFiDS_CircSection.hxx>
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
#include <Extrema_GenLocateExtPS.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>
#include <TopAbs.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean ChFi3d_GettraceCHRON();
#endif

//=======================================================================
//function : SearchCommonFaces
//purpose  : search the 2 common faces <F1> and <F2> of the edge <E>
//           uses the EFMap and take the 2 first good faces 
//=======================================================================

void SearchCommonFaces(const ChFiDS_Map& EFMap, 
		       const TopoDS_Edge& E,
		       TopoDS_Face& F1,
		       TopoDS_Face& F2)
{
  TopoDS_Face Fc;
  TopTools_ListIteratorOfListOfShape It;
  
  F1.Nullify();
  F2.Nullify();
  for ( It.Initialize(EFMap(E)); It.More(); It.Next() ) {
    Fc = TopoDS::Face(It.Value());
    if ( F1.IsNull() )
      F1 = Fc;
    else if ( !Fc.IsSame(F1) ) {
      F2 = Fc;
      break;
    }
  }

  if (!F1.IsNull() && F2.IsNull() && BRepTools::IsReallyClosed( E, F1 ))
    F2 = F1;
}

//=======================================================================
//function : ExtentSpinesOnCommonFace
//purpose  : Extend spines of two chamfers by distance dis1,dis2
//           on their common face
//           Two guide lines Spine1 and Spine2 cross in V
//           isfirst(i) = False if Spine(i) is oriented to V (i = 1,2)
//=======================================================================

void ExtentSpineOnCommonFace(Handle(ChFiDS_Spine)&  Spine1,
			     Handle(ChFiDS_Spine)&  Spine2,
			     const TopoDS_Vertex&   V,
			     const Standard_Real    dis1,
			     const Standard_Real    dis2,
			     const Standard_Boolean isfirst1,
			     const Standard_Boolean isfirst2)
{		  
  Standard_Real tolesp = 1.e-7;

    // alpha, the opening angle between two 
    // tangents of two guidelines in V is found
    Standard_Real tga1,tga2;
    Standard_Real d1plus = 0., d2plus = 0.;

    gp_Pnt tmp;
    gp_Vec tg1, tg2;
    Spine1->D1(Spine1->Absc(V),tmp,tg1);
    Spine2->D1(Spine2->Absc(V),tmp,tg2);
    tg1.Normalize();
    tg2.Normalize();
    if (isfirst1)  
      tg1.Reverse();
    if (isfirst2)  
      tg2.Reverse();

    Standard_Real cosalpha,sinalpha;   
    cosalpha = tg1.Dot(tg2);
    sinalpha = sqrt(1-cosalpha*cosalpha);
    
    //a1+a2 = alpha
    Standard_Real temp;
    temp = cosalpha+dis2/dis1;
    if( Abs(temp) > tolesp ){
      tga1 = sinalpha/temp;
      d1plus = dis1/tga1;
    }
    temp = cosalpha+dis1/dis2;
    if( Abs(temp) > tolesp ){
      tga2 = sinalpha/temp;
      d2plus = dis2/tga2;
    }

    //extension by the calculated distance
    if (d1plus > 0.) {
      d1plus *= 3.;
      if (isfirst1){
	Spine1->SetFirstParameter(-d1plus);
	Spine1->SetFirstTgt(0.);
      }
      else{
	Standard_Real param = Spine1->LastParameter(Spine1->NbEdges());
	Spine1->SetLastParameter(d1plus+param);
	Spine1->SetLastTgt(param);
      }
    }
    if (d2plus > 0.) {
      d2plus *= 1.5;
      if (isfirst2){
	Spine2->SetFirstParameter(-d2plus);
	Spine2->SetFirstTgt(0.);
      }
      else{
	Standard_Real param = Spine2->LastParameter(Spine2->NbEdges());
	Spine2->SetLastParameter(d2plus+param);
	Spine2->SetLastTgt(param);
      }
    }
}

//=======================================================================
//function : ChFi3d_ChBuilder
//purpose  : 
//=======================================================================

ChFi3d_ChBuilder::ChFi3d_ChBuilder(const TopoDS_Shape& S,
				   const Standard_Real Ta) : 
				          ChFi3d_Builder(S, Ta)
{
}


//=======================================================================
//function : Add
//purpose  : create a new stripe with a spine containing the edge <E>
//           add on the spine the tangential edges to <E>
//=======================================================================

void  ChFi3d_ChBuilder::Add(const TopoDS_Edge& E)
{

  if(!Contains(E) && myEFMap.Contains(E)){
    Handle(ChFiDS_Stripe) Stripe = new ChFiDS_Stripe();
    Handle(ChFiDS_Spine)& Sp = Stripe->ChangeSpine();
    Sp = new ChFiDS_ChamfSpine(tolesp);
    Handle(ChFiDS_ChamfSpine) Spine = Handle(ChFiDS_ChamfSpine)::DownCast(Sp);

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
//purpose  : create a new stripe with a ChamfSpine containing the edge <E>
//           with the distance <Dis>   on the face <F> 
//           . Add the tangential edges continuous to E in the spine
//           
//=======================================================================

void  ChFi3d_ChBuilder::Add(const Standard_Real Dis,
			    const TopoDS_Edge& E,
			    const TopoDS_Face& F)
{
  if (!Contains(E)  && myEFMap.Contains(E)) {
     
    // Take the 2 common faces of the egde <E>
    TopoDS_Face F1,F2;
    SearchCommonFaces(myEFMap,E,F1,F2);

    if (! F1.IsSame(F) && F2.IsSame(F) ) {
      F2 = F1;
      F1 = F;
    }

    if ( F1.IsSame(F)) {
      TopoDS_Edge E_wnt = E;
      E_wnt.Orientation(TopAbs_FORWARD);
      BRepAdaptor_Surface Sb1,Sb2;
      Sb1.Initialize(F1);
      Sb2.Initialize(F2);
      TopAbs_Orientation Or1,Or2;
      ChFi3d::ConcaveSide(Sb1,Sb2,E_wnt,Or1,Or2); 
      Handle(ChFiDS_Stripe) Stripe = new ChFiDS_Stripe();
      Handle(ChFiDS_Spine)& Sp = Stripe->ChangeSpine();
      Sp = new ChFiDS_ChamfSpine(tolesp);
      Handle(ChFiDS_ChamfSpine) 
        Spine = Handle(ChFiDS_ChamfSpine)::DownCast(Sp);

      Spine->SetEdges(E_wnt);
      if(PerformElement(Spine)){
	Spine->Load();
	myListStripe.Append(Stripe);
	
	Spine->SetDist(Dis);

	PerformExtremity(Spine);
      }
    }
  }
}


//=======================================================================
//function : SetDist
//purpose  : set the distances <Dis>of the contour of 
//           index IC
//=======================================================================

void  ChFi3d_ChBuilder::SetDist(const Standard_Real    Dis,
				const Standard_Integer IC,
				const TopoDS_Face&     F)
{

  if(IC <= NbElements()) {
    Handle(ChFiDS_ChamfSpine) csp = Handle(ChFiDS_ChamfSpine)::DownCast(Value(IC));

    // Search the first edge which has a common face equal to F
    TopoDS_Face F1,F2,FirstF1,FirstF2;
    //TopAbs_Orientation Or1,Or2;
    //Standard_Integer Choix, ChoixConge;
    BRepAdaptor_Surface Sb1,Sb2;
    Standard_Integer i = 1;
    Standard_Boolean Found = Standard_False;
    while ( (i <= csp->NbEdges()) && (!Found) ) {
      SearchCommonFaces(myEFMap,csp->Edges(i),F1,F2);
      if ( i == 1) {
	FirstF1 = F1;
	FirstF2 = F2;
      }
      Found = ( F1.IsSame(F) || F2.IsSame(F) );
      i++;
    }

    if (Found) {
      if ( F2.IsSame(F) ) {
	F2 = F1;
	F1 = F;
      }
      csp->SetDist(Dis);

    }
    else
      throw Standard_DomainError("the face is not common to any of edges of the contour");

  }
}


//=======================================================================
//function : GetDist
//purpose  : 
//=======================================================================

void ChFi3d_ChBuilder::GetDist(const Standard_Integer  IC,
			       Standard_Real&          Dis) const
{
    Handle(ChFiDS_ChamfSpine) chsp = Handle(ChFiDS_ChamfSpine)::DownCast(Value(IC));
    chsp->GetDist(Dis);

}


//=======================================================================
//function : Add
//purpose  : create a new stripe with a ChAsymSpine containing the edge <E>
//           with the distance <Dis> and the angle Angle  on the face <F> 
//           . Add the tangential edges continuous to E in the spine
//           
//=======================================================================

void  ChFi3d_ChBuilder::Add(const Standard_Real Dis1,
			    const Standard_Real Dis2,
			    const TopoDS_Edge& E,
			    const TopoDS_Face& F)
{
  if (!Contains(E)  && myEFMap.Contains(E)) {
     
    // Take the 2 common faces of the egde <E>
    TopoDS_Face F1,F2;
    SearchCommonFaces(myEFMap,E,F1,F2);

    if (! F1.IsSame(F) && F2.IsSame(F) ) {
      F2 = F1;
      F1 = F;
    }

    if ( F1.IsSame(F)) {
      TopoDS_Edge E_wnt = E;
      E_wnt.Orientation(TopAbs_FORWARD);
      BRepAdaptor_Surface Sb1,Sb2;
      Sb1.Initialize(F1);
      Sb2.Initialize(F2);
      TopAbs_Orientation Or1,Or2;
      Standard_Integer Choix = ChFi3d::ConcaveSide(Sb1,Sb2,E_wnt,Or1,Or2); 

      Handle(ChFiDS_Stripe) Stripe = new ChFiDS_Stripe();
      Handle(ChFiDS_Spine)& Sp = Stripe->ChangeSpine();
      Sp = new ChFiDS_ChamfSpine(tolesp);
      Handle(ChFiDS_ChamfSpine) 
        Spine = Handle(ChFiDS_ChamfSpine)::DownCast(Sp);

      Spine->SetEdges(E_wnt);
      if(PerformElement(Spine)){
	Spine->Load();
	myListStripe.Append(Stripe);
	
	Standard_Integer ChoixConge;
	SearchCommonFaces(myEFMap,Spine->Edges(1),F1,F2);
	Sb1.Initialize(F1);
	Sb2.Initialize(F2);
	ChoixConge = ChFi3d::ConcaveSide(Sb1,Sb2,
					 Spine->Edges(1),
					 Or1,Or2);


	// compare the 2 computed choices to know how to set the 
	// distances of the Spine according to the choice done 
	// on the added edge <e>
	if ( ChoixConge%2 != Choix%2 )
	  Spine->SetDists(Dis2, Dis1);
	else Spine->SetDists(Dis1, Dis2);

	PerformExtremity(Spine);
      }
    }
  }
}


//=======================================================================
//function : SetDists
//purpose  : set the distances <Dis1> and <Dis2> of the contour of 
//           index IC
//=======================================================================

void  ChFi3d_ChBuilder::SetDists(const Standard_Real    Dis1,
				 const Standard_Real    Dis2,
				 const Standard_Integer IC,
				 const TopoDS_Face&     F)
{

  if(IC <= NbElements()) {
    Handle(ChFiDS_ChamfSpine) csp = Handle(ChFiDS_ChamfSpine)::DownCast(Value(IC));

    // Search the first edge which has a common face equal to F
    TopoDS_Face F1,F2,FirstF1,FirstF2;
    TopAbs_Orientation Or1,Or2;
    Standard_Integer Choix, ChoixConge;
    BRepAdaptor_Surface Sb1,Sb2;
    Standard_Integer i = 1;
    Standard_Boolean Found = Standard_False;
    while ( (i <= csp->NbEdges()) && (!Found) ) {
      SearchCommonFaces(myEFMap,csp->Edges(i),F1,F2);
      if ( i == 1) {
	FirstF1 = F1;
	FirstF2 = F2;
      }
      Found = ( F1.IsSame(F) || F2.IsSame(F) );
      i++;
    }

    if (Found) {
      if ( F2.IsSame(F) ) {
	F2 = F1;
	F1 = F;
      }
      Sb1.Initialize(F1);
      Sb2.Initialize(F2);
      Choix = ChFi3d::ConcaveSide(Sb1,Sb2,
				  csp->Edges(i-1),
				  Or1,Or2);
      Sb1.Initialize(FirstF1);
      Sb2.Initialize(FirstF2);
      ChoixConge = ChFi3d::ConcaveSide(Sb1,Sb2,
				       csp->Edges(1),
				       Or1,Or2);
      if ( ChoixConge%2 != Choix%2 )
	csp->SetDists(Dis2,Dis1);
      else csp->SetDists(Dis1,Dis2);
    }
    else
      throw Standard_DomainError("the face is not common to any of edges of the contour");

  }
}


//=======================================================================
//function : Dists
//purpose  : 
//=======================================================================

void ChFi3d_ChBuilder::Dists(const Standard_Integer  IC,
			     Standard_Real&          dis1,
			     Standard_Real&          dis2) const
{
    Handle(ChFiDS_ChamfSpine) chsp = Handle(ChFiDS_ChamfSpine)::DownCast(Value(IC));
    Standard_Real temp1, temp2;
    chsp->Dists(temp1,temp2);
    dis1 = temp1;
    dis2 = temp2;
}


//=======================================================================
//function : Add
//purpose  : create a new stripe with a ChAsymSpine containing the edge <E>
//           with the distance <Dis> and the angle Angle  on the face <F> 
//           . Add the tangential edges continuous to E in the spine
//           
//=======================================================================

void  ChFi3d_ChBuilder::AddDA(const Standard_Real Dis1,
			      const Standard_Real Angle,
			      const TopoDS_Edge& E,
			      const TopoDS_Face& F)
{
  if (!Contains(E)  && myEFMap.Contains(E)) {
     
    // Take the 2 common faces of the egde <E>
    TopoDS_Face F1,F2;
    SearchCommonFaces(myEFMap,E,F1,F2);
 
    if (! F1.IsSame(F) && F2.IsSame(F) ) {
      F2 = F1;
      F1 = F;
    }

    if ( F1.IsSame(F)) {
      TopoDS_Edge E_wnt = E;
      E_wnt.Orientation(TopAbs_FORWARD);
      BRepAdaptor_Surface Sb1,Sb2;
      Sb1.Initialize(F1);
      Sb2.Initialize(F2);
      TopAbs_Orientation Or1,Or2;
      Standard_Integer Choix = ChFi3d::ConcaveSide(Sb1,Sb2,E_wnt,Or1,Or2); 

      Handle(ChFiDS_Stripe) Stripe = new ChFiDS_Stripe();
      Handle(ChFiDS_Spine)& Sp = Stripe->ChangeSpine();
      Sp = new ChFiDS_ChamfSpine(tolesp);
      Handle(ChFiDS_ChamfSpine) 
        Spine = Handle(ChFiDS_ChamfSpine)::DownCast(Sp);

      Spine->SetEdges(E_wnt);
      if(PerformElement(Spine)){
	Spine->Load();
	myListStripe.Append(Stripe);
	
	Standard_Integer ChoixConge;
	SearchCommonFaces(myEFMap,Spine->Edges(1),F1,F2);
	Sb1.Initialize(F1);
	Sb2.Initialize(F2);
	ChoixConge = ChFi3d::ConcaveSide(Sb1,Sb2,
					 Spine->Edges(1),
					 Or1,Or2);

	// compare the 2 computed choices to know how to set the 
	// distances of the Spine according to the choice done 
	// on the added edge <e>
	if ( ChoixConge%2 != Choix%2 ) {
          Spine->SetDistAngle(Dis1, Angle, Standard_False);
        }
	else { 
          Spine->SetDistAngle(Dis1, Angle, Standard_True);
        }
	
	PerformExtremity(Spine);
      }
    }
  }
}


//=======================================================================
//function : SetDistAngle
//purpose  : set the distances <Dis> and <Angle> of the contour of 
//           index IC
//=======================================================================

void  ChFi3d_ChBuilder::SetDistAngle(const Standard_Real    Dis,
				     const Standard_Real    Angle,
				     const Standard_Integer IC,
				     const TopoDS_Face&     F)
{

  if(IC <= NbElements()) {
    Handle(ChFiDS_ChamfSpine) csp = Handle(ChFiDS_ChamfSpine)::DownCast(Value(IC));

    // Search the first edge which has a common face equal to F
    TopoDS_Face F1,F2,FirstF1,FirstF2;
    TopAbs_Orientation Or1,Or2;
    Standard_Integer Choix, ChoixConge;
    BRepAdaptor_Surface Sb1,Sb2;
    Standard_Integer i = 1;
    Standard_Boolean Found = Standard_False;
//    Standard_Boolean DisOnF1 = Standard_True;

    while ( (i <= csp->NbEdges()) && (!Found) ) {
      SearchCommonFaces(myEFMap,csp->Edges(i),F1,F2);
      if ( i == 1) {
	FirstF1 = F1;
	FirstF2 = F2;
      }
      Found = ( F1.IsSame(F) || F2.IsSame(F) );
      i++;
    }

    if (Found) {
      if ( F2.IsSame(F) ) {
	F2 = F1;
	F1 = F;
      }
      Sb1.Initialize(F1);
      Sb2.Initialize(F2);
      Choix = ChFi3d::ConcaveSide(Sb1,Sb2,
				  csp->Edges(i-1),
				  Or1,Or2);
      Sb1.Initialize(FirstF1);
      Sb2.Initialize(FirstF2);
      ChoixConge = ChFi3d::ConcaveSide(Sb1,Sb2,
				       csp->Edges(1),
				       Or1,Or2);
      if ( ChoixConge%2 != Choix%2 ) {
	csp->SetDistAngle(Dis, Angle, Standard_False);
      }
      else {
        csp->SetDistAngle(Dis, Angle, Standard_True);
      }
    }
    else
      throw Standard_DomainError("the face is not common to any edges of the contour");

  }
}


//=======================================================================
//function : GetDistAngle
//purpose  : 
//=======================================================================

void ChFi3d_ChBuilder::GetDistAngle(const Standard_Integer  IC,
				    Standard_Real&          Dis,
				    Standard_Real&          Angle,
				    Standard_Boolean&       DisOnFace1) const
{
    Handle(ChFiDS_ChamfSpine) chsp = Handle(ChFiDS_ChamfSpine)::DownCast(Value(IC));

    chsp->GetDistAngle(Dis, Angle, DisOnFace1);
}

//=======================================================================
//function : IsChamfer
//purpose  : 
//=======================================================================
ChFiDS_ChamfMethod ChFi3d_ChBuilder::IsChamfer(const Standard_Integer  IC) const
{
  Handle(ChFiDS_ChamfSpine) chsp = Handle(ChFiDS_ChamfSpine)::DownCast(Value(IC));
    
  ChFiDS_ChamfMethod ret = chsp->IsChamfer();

  return ret;

}

//=======================================================================
//function : ResetContour
//purpose  : 
//=======================================================================

void  ChFi3d_ChBuilder::ResetContour(const Standard_Integer IC)
{
  if(IC <= NbElements()) {
    Handle(ChFiDS_ChamfSpine) chsp = Handle(ChFiDS_ChamfSpine)::DownCast(Value(IC));
    chsp->Reset(Standard_True);
  }
}

//--------------------------------AJOUT----------------------------------
//=======================================================================
//function : Simulate
//purpose  : 
//=======================================================================

void ChFi3d_ChBuilder::Simulate (const Standard_Integer IC)
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
    cout<<"and progression time : ";
    chemine.Show();
  }
#endif
}

//---------------------------AJOUT---------------------------------------
//=======================================================================
//function : NbSurf
//purpose  : 
//=======================================================================

Standard_Integer ChFi3d_ChBuilder::NbSurf (const Standard_Integer IC) const 
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


//--------------------------AJOUT---------------------------------------
//=======================================================================
//function : Sect
//purpose  : 
//=======================================================================

Handle(ChFiDS_SecHArray1) ChFi3d_ChBuilder::Sect (const Standard_Integer IC,
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


//-------------------MODIFS---------------------------------------------
//=======================================================================
//function : SimulKPart
//purpose  : Stores simulating sections in simul
//=======================================================================

void  ChFi3d_ChBuilder::SimulKPart(const Handle(ChFiDS_SurfData)& SD ) const 
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
  case GeomAbs_Plane:
    {
      v1 = p1f.Y();
      v2 = p2f.Y();
      u1 = Max(p1f.X(),p2f.X());
      u2 = Min(p1l.X(),p2l.X());
      sec = new ChFiDS_SecHArray1(1,2); 
      gp_Pln Pl = AS.Plane();
      ChFiDS_CircSection& sec1 = sec->ChangeValue(1);
      ChFiDS_CircSection& sec2 = sec->ChangeValue(2);
      sec1.Set(ElSLib::PlaneUIso(Pl.Position(),u1),v1,v2);
      sec2.Set(ElSLib::PlaneUIso(Pl.Position(),u2),v1,v2);
    }
    break;
  case GeomAbs_Cone:
    {
      v1 = p1f.Y();
      v2 = p2f.Y();
      u1 = Max(p1f.X(),p2f.X());
      u2 = Min(p1l.X(),p2l.X());
      Standard_Real ang = (u2-u1);
      gp_Cone Co = AS.Cone();
      Standard_Real rad = Co.RefRadius(), sang = Co.SemiAngle();
      Standard_Integer n = (Standard_Integer) (36.*ang/M_PI + 1);
      if(n<2) n = 2;
      sec = new ChFiDS_SecHArray1(1, n);
      for (Standard_Integer i = 1; i <= n; i++) {
	ChFiDS_CircSection& isec = sec->ChangeValue(i);
	Standard_Real u = u1 + (i - 1)*(u2 - u1)/(n-1);
	isec.Set(ElSLib::ConeUIso(Co.Position(), rad, sang, u), v1, v2);  
      }
    }
    break;
  default:
    break;
  }
  SD->SetSimul(sec);
}

//------------------------MODIFS---------------------------------------
//=======================================================================
//function : SimulSurf
//purpose  : 
//=======================================================================

Standard_Boolean
ChFi3d_ChBuilder::SimulSurf(Handle(ChFiDS_SurfData)&            Data,
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
  Handle(ChFiDS_ChamfSpine) 
    chsp = Handle(ChFiDS_ChamfSpine)::DownCast(Spine);
  
  if (chsp.IsNull()) 
    throw Standard_ConstructionError("SimulSurf : this is not the spine of a chamfer");


  Standard_Real radius;
  // Flexible parameters!
  Standard_Real la = HGuide->LastParameter(), fi = HGuide->FirstParameter();
  Standard_Real longueur = la - fi;
  Standard_Real MaxStep = longueur * 0.05;
  Standard_Real radiusspine = 0, locfleche, w;
  gp_Pnt Pbid;
  gp_Vec d1,d2;
  // As ElSpine is parameterized by a curvilinear quasi-abscissa,
  // the min radius is estimated as 1/D2 max;
  //for(Standard_Integer i = 0; i <= 20; i++){
  Standard_Integer i;
  for( i = 0; i <= 20; i++){ 
    w = fi + i*MaxStep;
    HGuide->D2(w,Pbid,d1,d2);
    Standard_Real temp = d2.SquareMagnitude();
    if(temp>radiusspine) radiusspine = temp;
  }

  Handle(BRepBlend_Line) lin;
  Standard_Real PFirst = First;
  if(intf) First = chsp->FirstParameter(1);
  if(intl) Last = chsp->LastParameter(chsp->NbEdges());



  if (chsp->IsChamfer() == ChFiDS_Sym) {
    Standard_Real dis;
    chsp->GetDist(dis);
    radius = Max(dis, radiusspine);
    locfleche = radius*1.e-2; //graphic criterion

    BRepBlend_Chamfer Func(S1,S2,HGuide);
    BRepBlend_ChamfInv FInv(S1,S2,HGuide); 
    Func.Set(dis, dis, Choix);
    FInv.Set(dis, dis, Choix);
    
    done = SimulData(Data,HGuide,lin,S1,I1,S2,I2,
		     Func,FInv,PFirst,MaxStep,locfleche,
		     TolGuide,First,Last,Inside,Appro,Forward,
		     Soldep,4,RecOnS1,RecOnS2);

    if ( !done ) return Standard_False;
    Handle(ChFiDS_SecHArray1) sec;
    gp_Pnt2d pf1,pl1,pf2,pl2;  
    
    Standard_Integer nbp = lin->NbPoints();
    sec = new ChFiDS_SecHArray1(1,nbp);
    for( i = 1; i <= nbp; i++ ){
      ChFiDS_CircSection& isec = sec->ChangeValue(i);
      Standard_Real u1,v1,u2,v2,ww,p1,p2;
      gp_Lin line;
      const Blend_Point& p = lin->Point(i);
      p.ParametersOnS1(u1,v1);
      p.ParametersOnS2(u2,v2);
      ww = p.Parameter();
      Func.Section(ww,u1,v1,u2,v2,p1,p2,line); 
      isec.Set(line,p1,p2);
      if(i == 1) {pf1.SetCoord(u1,v1); pf2.SetCoord(u2,v2);} 
      if(i == nbp) {pl1.SetCoord(u1,v1); pl2.SetCoord(u2,v2);} 
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
      Standard_Boolean ok = Standard_False;
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
  }
  else if (chsp->IsChamfer() == ChFiDS_TwoDist) {
    Standard_Real dis1, dis2;
    chsp->Dists(dis1, dis2);
    radius = Max(dis1, dis2);
    radius = Max(radius, radiusspine);
    locfleche = radius*1.e-2; //graphic criterion

    BRepBlend_Chamfer Func(S1,S2,HGuide);
    BRepBlend_ChamfInv FInv(S1,S2,HGuide); 
    Func.Set(dis1,dis2,Choix);
    FInv.Set(dis1,dis2,Choix);
    
    done = SimulData(Data,HGuide,lin,S1,I1,S2,I2,
		     Func,FInv,PFirst,MaxStep,locfleche,
		     TolGuide,First,Last,Inside,Appro,Forward,
		     Soldep,4,RecOnS1,RecOnS2);

    if ( !done ) return Standard_False;
    Handle(ChFiDS_SecHArray1) sec;
    gp_Pnt2d pf1,pl1,pf2,pl2;  
    
    Standard_Integer nbp = lin->NbPoints();
    sec = new ChFiDS_SecHArray1(1,nbp);
    for( i = 1; i <= nbp; i++ ){
      ChFiDS_CircSection& isec = sec->ChangeValue(i);
      Standard_Real u1,v1,u2,v2,ww,p1,p2;
      gp_Lin line;
      const Blend_Point& p = lin->Point(i);
      p.ParametersOnS1(u1,v1);
      p.ParametersOnS2(u2,v2);
      ww = p.Parameter();
      Func.Section(ww,u1,v1,u2,v2,p1,p2,line); 
      isec.Set(line,p1,p2);
      if(i == 1) {pf1.SetCoord(u1,v1); pf2.SetCoord(u2,v2);} 
      if(i == nbp) {pl1.SetCoord(u1,v1); pl2.SetCoord(u2,v2);} 
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
      Standard_Boolean ok = Standard_False;
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
  }
  else  {
    Standard_Real dis, angle;
    Standard_Boolean disonF1;  
    chsp->GetDistAngle(dis, angle, disonF1);
    radius = Max(dis, dis * tan(angle));
    radius = Max(radius, radiusspine);
    locfleche = radius*1.e-2; //graphic criterion

    Standard_Integer Ch = FindChoiceDistAngle(Choix, disonF1);

    if (disonF1)  {
      BRepBlend_ChAsym    Func(S1,S2,HGuide);
      BRepBlend_ChAsymInv FInv(S1,S2,HGuide); 

      Func.Set(dis, angle, Ch);
      FInv.Set(dis, angle, Ch);

      done = SimulData(Data,HGuide,lin,S1,I1,S2,I2,
		       Func,FInv,PFirst,MaxStep,locfleche,
		       TolGuide,First,Last,Inside,Appro,Forward,
		       Soldep,4,RecOnS1,RecOnS2);
    
      if ( !done ) return Standard_False;
      Handle(ChFiDS_SecHArray1) sec;
      gp_Pnt2d pf1,pl1,pf2,pl2;  
      
      Standard_Integer nbp = lin->NbPoints();
      sec = new ChFiDS_SecHArray1(1,nbp);
      for( i = 1; i <= nbp; i++ ){
	ChFiDS_CircSection& isec = sec->ChangeValue(i);
	Standard_Real u1,v1,u2,v2,ww,p1,p2;
	gp_Lin line;
	const Blend_Point& p = lin->Point(i);
	p.ParametersOnS1(u1,v1);
	p.ParametersOnS2(u2,v2);
	ww = p.Parameter();
	Func.Section(ww,u1,v1,u2,v2,p1,p2,line); 
	isec.Set(line,p1,p2);
	if(i == 1) {pf1.SetCoord(u1,v1); pf2.SetCoord(u2,v2);} 
	if(i == nbp) {pl1.SetCoord(u1,v1); pl2.SetCoord(u2,v2);} 
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
	Standard_Boolean ok = Standard_False;
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
    }
    else {
      BRepBlend_ChAsym    Func(S2,S1,HGuide);
      BRepBlend_ChAsymInv FInv(S2,S1,HGuide); 

      Func.Set(dis, angle, Ch);
      FInv.Set(dis, angle, Ch);

      Standard_Real Rtemp;
      Rtemp     = Soldep(1);
      Soldep(1) = Soldep(3);
      Soldep(3) = Rtemp;
      Rtemp     = Soldep(2);
      Soldep(2) = Soldep(4);
      Soldep(4) = Rtemp;

      done = SimulData(Data,HGuide,lin,S2,I2,S1,I1,
		       Func,FInv,PFirst,MaxStep,locfleche,
		       TolGuide,First,Last,Inside,Appro,Forward,
		       Soldep,4,RecOnS2,RecOnS1);
    
      if ( !done ) return Standard_False;
      Handle(ChFiDS_SecHArray1) sec;
      gp_Pnt2d pf1,pl1,pf2,pl2;  
      
      Standard_Integer nbp = lin->NbPoints();
      sec = new ChFiDS_SecHArray1(1,nbp);
      for( i = 1; i <= nbp; i++ ){
	ChFiDS_CircSection& isec = sec->ChangeValue(i);
	Standard_Real u1,v1,u2,v2,ww,p1,p2;
	gp_Lin line;
	const Blend_Point& p = lin->Point(i);
	p.ParametersOnS1(u1,v1);
	p.ParametersOnS2(u2,v2);
	ww = p.Parameter();
	Func.Section(ww,u1,v1,u2,v2,p1,p2,line); 
	isec.Set(line,p1,p2);
	if(i == 1) {pf1.SetCoord(u1,v1); pf2.SetCoord(u2,v2);} 
	if(i == nbp) {pl1.SetCoord(u1,v1); pl2.SetCoord(u2,v2);} 
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
	Standard_Boolean ok = Standard_False;
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
    }
  }
  return Standard_True;
}

void  ChFi3d_ChBuilder::SimulSurf(Handle(ChFiDS_SurfData)&            ,
				const Handle(ChFiDS_HElSpine)&      , 
				const Handle(ChFiDS_Spine)&         , 
				const Standard_Integer              ,
				const Handle(BRepAdaptor_HSurface)& ,
				const Handle(Adaptor3d_TopolTool)&    ,
				const Handle(BRepAdaptor_HCurve2d)& ,
				const Handle(BRepAdaptor_HSurface)& ,
				const Handle(BRepAdaptor_HCurve2d)& ,
				Standard_Boolean&                   ,
				const Handle(BRepAdaptor_HSurface)& ,
				const Handle(Adaptor3d_TopolTool)&    ,
				const TopAbs_Orientation            ,
				const Standard_Real                 ,
				const Standard_Real                 ,
				Standard_Real&                      ,
				Standard_Real&                      ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const math_Vector&                  )
{
  throw Standard_Failure("SimulSurf Not Implemented");
}
void  ChFi3d_ChBuilder::SimulSurf(Handle(ChFiDS_SurfData)&            ,
				const Handle(ChFiDS_HElSpine)&      , 
				const Handle(ChFiDS_Spine)&         , 
				const Standard_Integer              ,
				const Handle(BRepAdaptor_HSurface)& ,
				const Handle(Adaptor3d_TopolTool)&    ,
				const TopAbs_Orientation            ,
				const Handle(BRepAdaptor_HSurface)& ,
				const Handle(Adaptor3d_TopolTool)&    ,
				const Handle(BRepAdaptor_HCurve2d)& ,
				const Handle(BRepAdaptor_HSurface)& ,
				const Handle(BRepAdaptor_HCurve2d)& ,
				Standard_Boolean&                   ,
				const Standard_Real                 ,
				const Standard_Real                 ,
				Standard_Real&                      ,
				Standard_Real&                      ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const math_Vector&                  )
{
  throw Standard_Failure("SimulSurf Not Implemented");
}
void  ChFi3d_ChBuilder::SimulSurf(Handle(ChFiDS_SurfData)&            ,
				const Handle(ChFiDS_HElSpine)&      ,
				const Handle(ChFiDS_Spine)&         ,
				const Standard_Integer              ,
				const Handle(BRepAdaptor_HSurface)& ,
				const Handle(Adaptor3d_TopolTool)&    ,
				const Handle(BRepAdaptor_HCurve2d)& ,
				const Handle(BRepAdaptor_HSurface)& ,
				const Handle(BRepAdaptor_HCurve2d)& ,
				Standard_Boolean&                   ,
				const TopAbs_Orientation            ,
				const Handle(BRepAdaptor_HSurface)& ,
				const Handle(Adaptor3d_TopolTool)&    ,
				const Handle(BRepAdaptor_HCurve2d)& ,
				const Handle(BRepAdaptor_HSurface)& ,
				const Handle(BRepAdaptor_HCurve2d)& ,
				Standard_Boolean&                   ,
				const TopAbs_Orientation            ,
				const Standard_Real                 ,
				const Standard_Real                 ,
				Standard_Real&                      ,
				Standard_Real&                      ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const math_Vector&                  )
{
  throw Standard_Failure("SimulSurf Not Implemented");
}
//------------------------MODIFS---------------------------------------
//=======================================================================
//function : PerformFirstSection
//purpose  : to implement the first section if there is no KPart
//=======================================================================

Standard_Boolean ChFi3d_ChBuilder::PerformFirstSection
(const Handle(ChFiDS_Spine)&         Spine,
 const Handle(ChFiDS_HElSpine)&      HGuide,
 const Standard_Integer              Choix,
 Handle(BRepAdaptor_HSurface)&       S1,
 Handle(BRepAdaptor_HSurface)&       S2,
 const Handle(Adaptor3d_TopolTool)&    I1,
 const Handle(Adaptor3d_TopolTool)&    I2,
 const Standard_Real                 Par,
 math_Vector&                        SolDep,
 TopAbs_State&                       Pos1,
 TopAbs_State&                       Pos2) const 
{
  Handle(ChFiDS_ChamfSpine) 
    chsp = Handle(ChFiDS_ChamfSpine)::DownCast(Spine);
  
  if (chsp.IsNull()) 
    throw Standard_ConstructionError("PerformSurf : this is not the spine of a chamfer");

  Standard_Real TolGuide = HGuide->Resolution(tolesp) ;


  if (chsp->IsChamfer() == ChFiDS_Sym) {
    Standard_Real dis;
    chsp->GetDist(dis);
    
    BRepBlend_Chamfer Func(S1,S2,HGuide);
    Func.Set(dis,dis,Choix);
    BRepBlend_Walking TheWalk(S1,S2,I1,I2,HGuide);
    
    //calculate an approximate starting solution
    gp_Vec TgF, TgL, tmp1, tmp2, d1gui;
    gp_Pnt pt1, pt2, ptgui;
    gp_XYZ temp;
    
    ( HGuide->Curve() ).D1(Par,ptgui,d1gui);
    //  ptgui = (S1->Surface()).Value(SolDep(1),SolDep(2));
    
    Func.Set(Par);
    Func.Tangent(SolDep(1),SolDep(2),SolDep(3),SolDep(4),TgF,TgL,tmp1,tmp2);
    
    Standard_Boolean rev1 = Standard_False;
    Standard_Boolean rev2 = Standard_False;
    Standard_Real    sign = (TgF.Crossed(d1gui)).Dot(TgL);
  
    if( Choix%2 == 1 )
      rev1 = Standard_True;
    else
      rev2 = Standard_True;
    
    if( sign < 0. ){
      rev1 = !rev1;
      rev2 = !rev2;
    }
    
    if( rev1 )
      TgF.Reverse();
    if( rev2 )
      TgL.Reverse();
    
    temp = (TgF.XYZ()).Multiplied(dis);
    pt1.SetXYZ( (ptgui.XYZ()).Added(temp) );
    temp = (TgL.XYZ()).Multiplied(dis);
    pt2.SetXYZ( (ptgui.XYZ()).Added(temp) );
    
    Standard_Real tol = tolesp*1.e2;
//    Standard_Real u,v;
    Extrema_GenLocateExtPS proj1(S1->Surface(), tol, tol);
    proj1.Perform(pt1, SolDep(1), SolDep(2));
    Extrema_GenLocateExtPS proj2(S2->Surface(), tol, tol);
    proj2.Perform(pt2, SolDep(3), SolDep(4));

    if( proj1.IsDone() ){
      (proj1.Point()).Parameter(SolDep(1),SolDep(2)); 
    }
    if( proj2.IsDone() ){
      (proj2.Point()).Parameter(SolDep(3),SolDep(4)); 
    }
    
    return TheWalk.PerformFirstSection(Func,Par,SolDep,
				       tolesp,TolGuide,Pos1,Pos2);
  }
  else if (chsp->IsChamfer() == ChFiDS_TwoDist)  {
    Standard_Real dis1, dis2;
    chsp->Dists(dis1, dis2);
    
    BRepBlend_Chamfer Func(S1,S2,HGuide);
    Func.Set(dis1,dis2,Choix);
    BRepBlend_Walking TheWalk(S1,S2,I1,I2,HGuide);
    
    //calculate an approximate starting solution
    gp_Vec TgF, TgL, tmp1, tmp2, d1gui;
    gp_Pnt pt1, pt2, ptgui;
    gp_XYZ temp;
    
    ( HGuide->Curve() ).D1(Par,ptgui,d1gui);
    //  ptgui = (S1->Surface()).Value(SolDep(1),SolDep(2));
    
    Func.Set(Par);
    Func.Tangent(SolDep(1),SolDep(2),SolDep(3),SolDep(4),TgF,TgL,tmp1,tmp2);
    
    Standard_Boolean rev1 = Standard_False;
    Standard_Boolean rev2 = Standard_False;
    Standard_Real    sign = (TgF.Crossed(d1gui)).Dot(TgL);
  
    if( Choix%2 == 1 )
      rev1 = Standard_True;
    else
      rev2 = Standard_True;
    
    if( sign < 0. ){
      rev1 = !rev1;
      rev2 = !rev2;
    }
    
    if( rev1 )
      TgF.Reverse();
    if( rev2 )
      TgL.Reverse();
    
    temp = (TgF.XYZ()).Multiplied(dis1);
    pt1.SetXYZ( (ptgui.XYZ()).Added(temp) );
    temp = (TgL.XYZ()).Multiplied(dis2);
    pt2.SetXYZ( (ptgui.XYZ()).Added(temp) );
    
    Standard_Real tol = tolesp*1.e2;
//    Standard_Real u,v;

    Extrema_GenLocateExtPS proj1(S1->Surface(), tol, tol);
    proj1.Perform(pt1, SolDep(1), SolDep(2));
    Extrema_GenLocateExtPS proj2(S2->Surface(), tol, tol);
    proj2.Perform(pt2, SolDep(3), SolDep(4));

    if( proj1.IsDone() ){
      (proj1.Point()).Parameter(SolDep(1),SolDep(2)); 
    }
    if( proj2.IsDone() ){
      (proj2.Point()).Parameter(SolDep(3),SolDep(4)); 
    }
    
    return TheWalk.PerformFirstSection(Func,Par,SolDep,
				       tolesp,TolGuide,Pos1,Pos2);
  }
  else {
    Standard_Real dis1, angle;
    Standard_Boolean disonF1; 
    chsp->GetDistAngle(dis1, angle, disonF1);
    
    Standard_Integer Ch = FindChoiceDistAngle(Choix, disonF1);
    
    if (disonF1)  {
      BRepBlend_ChAsym Func(S1,S2,HGuide);
      Func.Set(dis1, angle, Ch);
      BRepBlend_Walking TheWalk(S1,S2,I1,I2,HGuide);
    
    //calculate an approximate starting solution
      gp_Vec TgF, TgL, tmp1, tmp2, d1gui;
      gp_Pnt pt1, pt2, ptgui;
      gp_XYZ temp;
      
      ( HGuide->Curve() ).D1(Par,ptgui,d1gui);
      //  ptgui = (S1->Surface()).Value(SolDep(1),SolDep(2));
      
      Func.Set(Par);
      Func.Tangent(SolDep(1),SolDep(2),SolDep(3),SolDep(4),TgF,TgL,tmp1,tmp2);
      
      Standard_Boolean rev1 = Standard_False;
      Standard_Boolean rev2 = Standard_False;
      Standard_Real    sign = (TgF.Crossed(d1gui)).Dot(TgL);
      
      if( Ch%2 == 1 )
	rev1 = Standard_True;
      else
	rev2 = Standard_True;
      
      if( sign < 0. ){
	rev1 = !rev1;
	rev2 = !rev2;
      }
      
      if( rev1 )
	TgF.Reverse();
      if( rev2 )
	TgL.Reverse();
  
      temp = (TgF.XYZ()).Multiplied(dis1);
      pt1.SetXYZ( (ptgui.XYZ()).Added(temp) );
      
      Standard_Real dis2, tmpcos, tmpsin;
      tmpcos = TgF.Dot(TgL);
      tmpsin = sqrt(1. - tmpcos * tmpcos);  
      
      dis2   = dis1 / (tmpcos + tmpsin / tan(angle)); 
      
      temp = (TgL.XYZ()).Multiplied(dis2);
      pt2.SetXYZ( (ptgui.XYZ()).Added(temp) );
      
      Standard_Real tol = tolesp*1.e2;
//      Standard_Real u,v;
      Extrema_GenLocateExtPS proj1(S1->Surface(), tol, tol);
      proj1.Perform(pt1, SolDep(1), SolDep(2));
      Extrema_GenLocateExtPS proj2(S2->Surface(), tol, tol);
      proj2.Perform(pt2, SolDep(3), SolDep(4));
      if( proj1.IsDone() ){
	(proj1.Point()).Parameter(SolDep(1),SolDep(2)); 
      }
      if( proj2.IsDone() ){
	(proj2.Point()).Parameter(SolDep(3),SolDep(4)); 
      }
      
      return TheWalk.PerformFirstSection(Func,Par,SolDep,
					 tolesp,TolGuide,Pos1,Pos2);
    }
    else {
      Standard_Real Rtemp;
      BRepBlend_ChAsym Func(S2,S1,HGuide);
      Func.Set(dis1, angle, Ch);
      BRepBlend_Walking TheWalk(S2,S1,I2,I1,HGuide);
    
    //calculate an approximate starting solution
      gp_Vec TgF, TgL, tmp1, tmp2, d1gui;
      gp_Pnt pt1, pt2, ptgui;
      gp_XYZ temp;
      
      ( HGuide->Curve() ).D1(Par,ptgui,d1gui);
      //  ptgui = (S1->Surface()).Value(SolDep(1),SolDep(2));
      Rtemp     = SolDep(1);
      SolDep(1) = SolDep(3);
      SolDep(3) = Rtemp;
      Rtemp     = SolDep(2);
      SolDep(2) = SolDep(4);
      SolDep(4) = Rtemp;      
      Func.Set(Par);

      Func.Tangent(SolDep(1),SolDep(2),SolDep(3),SolDep(4),TgF,TgL,tmp1,tmp2);
      
      Standard_Boolean rev1 = Standard_False;
      Standard_Boolean rev2 = Standard_False;
      Standard_Real    sign = (TgF.Crossed(d1gui)).Dot(TgL);
      
      if( Ch%2 == 1 )
	rev1 = Standard_True;
      else
	rev2 = Standard_True;
      
      if( sign < 0. ){
	rev1 = !rev1;
	rev2 = !rev2;
      }
      
      if( rev1 )
	TgF.Reverse();
      if( rev2 )
	TgL.Reverse();
  
      temp = (TgF.XYZ()).Multiplied(dis1);
      pt1.SetXYZ( (ptgui.XYZ()).Added(temp) );
      
      Standard_Real dis2, tmpcos, tmpsin;
      tmpcos = TgF.Dot(TgL);
      tmpsin = sqrt(1. - tmpcos * tmpcos);  
      
      dis2   = dis1 / (tmpcos + tmpsin / tan(angle)); 
      
      temp = (TgL.XYZ()).Multiplied(dis2);
      pt2.SetXYZ( (ptgui.XYZ()).Added(temp) );
      
      Standard_Real tol = tolesp*1.e2;
//      Standard_Real u,v;
      Extrema_GenLocateExtPS proj1(S2->Surface(), tol, tol);
      proj1.Perform(pt1, SolDep(1), SolDep(2));
      Extrema_GenLocateExtPS proj2(S1->Surface(), tol, tol);
      proj2.Perform(pt2, SolDep(3), SolDep(4));
      if( proj1.IsDone() ) {
	(proj1.Point()).Parameter(SolDep(1),SolDep(2)); 
      }
      if( proj2.IsDone() ){
	(proj2.Point()).Parameter(SolDep(3),SolDep(4)); 
      }
      
      Standard_Boolean RetWalk =  TheWalk.PerformFirstSection(Func,Par,SolDep,
							      tolesp,TolGuide,Pos2,Pos1);
      Rtemp     = SolDep(1);
      SolDep(1) = SolDep(3);
      SolDep(3) = Rtemp;
      Rtemp     = SolDep(2);
      SolDep(2) = SolDep(4);
      SolDep(4) = Rtemp;

      return RetWalk;
    }
  }
}


//=======================================================================
//function : PerformSurf
//purpose  : 
//=======================================================================

Standard_Boolean  
ChFi3d_ChBuilder::PerformSurf(ChFiDS_SequenceOfSurfData&          SeqData,
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
  Handle(ChFiDS_SurfData) Data = SeqData(1);
  Handle(ChFiDS_ChamfSpine) 
    chsp = Handle(ChFiDS_ChamfSpine)::DownCast(Spine);
  
  if (chsp.IsNull()) 
    throw Standard_ConstructionError("PerformSurf : this is not the spine of a chamfer");
  
  Standard_Boolean gd1,gd2,gf1,gf2;
  Handle(BRepBlend_Line) lin;
  TopAbs_Orientation Or = S1->ChangeSurface().Face().Orientation();
  Standard_Real PFirst = First;
  if(intf) First = chsp->FirstParameter(1);
  if(intl) Last = chsp->LastParameter(chsp->NbEdges());

  if (chsp->IsChamfer() == ChFiDS_Sym) {
    BRepBlend_Chamfer  Func(S1,S2,HGuide);
    BRepBlend_ChamfInv FInv(S1,S2,HGuide);
    Standard_Real dis;
    chsp->GetDist(dis);
    Func.Set(dis, dis, Choix);
    FInv.Set(dis, dis, Choix);
      
    done = ComputeData(Data,HGuide,Spine,lin,S1,I1,S2,I2,Func,FInv,
		       PFirst,MaxStep,Fleche,TolGuide,First,Last,
		       Inside,Appro,Forward,Soldep,intf,intl,
		       gd1,gd2,gf1,gf2,RecOnS1,RecOnS2);
    if(!done) return Standard_False; // ratrappage possible PMN 14/05/1998
    done = CompleteData(Data,Func,lin,S1,S2,Or,gd1,gd2,gf1,gf2);
    if(!done) throw Standard_Failure("PerformSurf : Fail of approximation!");
  }
  else if (chsp->IsChamfer() == ChFiDS_TwoDist) {
    BRepBlend_Chamfer  Func(S1,S2,HGuide);
    BRepBlend_ChamfInv FInv(S1,S2,HGuide);
    Standard_Real d1, d2;
    chsp->Dists(d1,d2);
    Func.Set(d1,d2,Choix);
    FInv.Set(d1,d2,Choix);
    
    done = ComputeData(Data,HGuide,Spine,lin,S1,I1,S2,I2,Func,FInv,
		       PFirst,MaxStep,Fleche,TolGuide,First,Last,
		       Inside,Appro,Forward,Soldep,intf,intl,
		       gd1,gd2,gf1,gf2,RecOnS1,RecOnS2);
    if(!done) return Standard_False; // ratrappage possible PMN 14/05/1998
    done = CompleteData(Data,Func,lin,S1,S2,Or,gd1,gd2,gf1,gf2);
    if(!done) throw Standard_Failure("PerformSurf : Fail of approximation!");
  }
  else {
    Standard_Real d1, angle;
    Standard_Boolean disonF1;
    chsp->GetDistAngle(d1, angle, disonF1);
    
    Standard_Integer Ch = FindChoiceDistAngle(Choix, disonF1);

    if (disonF1) {
      BRepBlend_ChAsym  Func(S1,S2,HGuide);
      BRepBlend_ChAsymInv FInv(S1,S2,HGuide);
      Func.Set(d1, angle, Ch);
      FInv.Set(d1, angle, Ch);
    
      done = ComputeData(Data,HGuide,Spine,lin,S1,I1,S2,I2,Func,FInv,
			 PFirst,MaxStep,Fleche,TolGuide,First,Last,
			 Inside,Appro,Forward,Soldep,intf,intl,
			 gd1,gd2,gf1,gf2,RecOnS1,RecOnS2);

      if(!done) return Standard_False; // ratrappage possible PMN 14/05/1998
      done = CompleteData(Data,Func,lin,S1,S2,Or,gd1,gd2,gf1,gf2);
      if(!done) throw Standard_Failure("PerformSurf : Fail of approximation!");
    }
    else {
      Standard_Real Rtemp;
      BRepBlend_ChAsym  Func(S2, S1, HGuide);
      BRepBlend_ChAsymInv FInv(S2, S1,HGuide);
      Func.Set(d1, angle, Ch);
      FInv.Set(d1, angle, Ch);

      Rtemp     = Soldep(1);
      Soldep(1) = Soldep(3);
      Soldep(3) = Rtemp;
      Rtemp     = Soldep(2);
      Soldep(2) = Soldep(4);
      Soldep(4) = Rtemp;

      TopAbs_Orientation Or2 = S2->ChangeSurface().Face().Orientation();

      done = ComputeData(Data,HGuide,Spine,lin,S2,I2,S1,I1,Func,FInv,
			 PFirst,MaxStep,Fleche,TolGuide,First,Last,
			 Inside,Appro,Forward,Soldep,intf,intl,
			 gd2,gd1,gf2,gf1,RecOnS2,RecOnS1);

      ChFiDS_CommonPoint tmp = Data->VertexFirstOnS1();
      Data->ChangeVertexFirstOnS1() = Data->VertexFirstOnS2();
      Data->ChangeVertexFirstOnS2() = tmp;
      tmp = Data->VertexLastOnS1();
      Data->ChangeVertexLastOnS1() = Data->VertexLastOnS2();
      Data->ChangeVertexLastOnS2() = tmp;
      if(!done) return Standard_False; // ratrappage possible PMN 14/05/1998
      done = CompleteData(Data,Func,lin,S1,S2,Or2,gd1,gd2,gf1,gf2, Standard_True);
      if(!done) throw Standard_Failure("PerformSurf : Fail of approximation!");
    }      

  }
  return Standard_True;
}
void  ChFi3d_ChBuilder::PerformSurf(ChFiDS_SequenceOfSurfData&          ,
				  const Handle(ChFiDS_HElSpine)&      , 
				  const Handle(ChFiDS_Spine)&         , 
				  const Standard_Integer              ,
				  const Handle(BRepAdaptor_HSurface)& ,
				  const Handle(Adaptor3d_TopolTool)&    ,
				  const Handle(BRepAdaptor_HCurve2d)& ,
				  const Handle(BRepAdaptor_HSurface)& ,
				  const Handle(BRepAdaptor_HCurve2d)& ,
				  Standard_Boolean&                   ,
				  const Handle(BRepAdaptor_HSurface)& ,
				  const Handle(Adaptor3d_TopolTool)&    ,
				  const TopAbs_Orientation            ,
				  const Standard_Real                 ,
				  const Standard_Real                 ,
				  const Standard_Real                 ,
				  Standard_Real&                      ,
				  Standard_Real&                      ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const math_Vector&                  )
{
  throw Standard_Failure("PerformSurf Not Implemented");
}
void  ChFi3d_ChBuilder::PerformSurf(ChFiDS_SequenceOfSurfData&          ,
				  const Handle(ChFiDS_HElSpine)&      , 
				  const Handle(ChFiDS_Spine)&         , 
				  const Standard_Integer              ,
				  const Handle(BRepAdaptor_HSurface)& ,
				  const Handle(Adaptor3d_TopolTool)&    ,
				  const TopAbs_Orientation            ,
				  const Handle(BRepAdaptor_HSurface)& ,
				  const Handle(Adaptor3d_TopolTool)&    ,
				  const Handle(BRepAdaptor_HCurve2d)& ,
				  const Handle(BRepAdaptor_HSurface)& ,
				  const Handle(BRepAdaptor_HCurve2d)& ,
				  Standard_Boolean&                   ,
				  const Standard_Real                 ,
				  const Standard_Real                 ,
				  const Standard_Real                 ,
				  Standard_Real&                      ,
				  Standard_Real&                      ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const math_Vector&                  )
{
  throw Standard_Failure("PerformSurf Not Implemented");

}
void  ChFi3d_ChBuilder::PerformSurf(ChFiDS_SequenceOfSurfData&          ,
				  const Handle(ChFiDS_HElSpine)&      ,
				  const Handle(ChFiDS_Spine)&         ,
				  const Standard_Integer              ,
				  const Handle(BRepAdaptor_HSurface)& ,
				  const Handle(Adaptor3d_TopolTool)&    ,
				  const Handle(BRepAdaptor_HCurve2d)& ,
				  const Handle(BRepAdaptor_HSurface)& ,
				  const Handle(BRepAdaptor_HCurve2d)& ,
				  Standard_Boolean&                   ,
				  const TopAbs_Orientation            ,
				  const Handle(BRepAdaptor_HSurface)& ,
				  const Handle(Adaptor3d_TopolTool)&    ,
				  const Handle(BRepAdaptor_HCurve2d)& ,
				  const Handle(BRepAdaptor_HSurface)& ,
				  const Handle(BRepAdaptor_HCurve2d)& ,
				  Standard_Boolean&                   ,
				  const TopAbs_Orientation            ,
				  const Standard_Real                 ,
				  const Standard_Real                 ,
				  const Standard_Real                 ,
				  Standard_Real&                      ,
				  Standard_Real&                      ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const math_Vector&                  )
{
  throw Standard_Failure("PerformSurf Not Implemented");

}
//=======================================================================
//function : ExtentOneCorner
//purpose  : extends the spine of the stripe S on the side of the vertex V
// PMN : 28/11/97 : Reproduces the code of fillets, and it seems to work better...
//=======================================================================

void ChFi3d_ChBuilder::ExtentOneCorner(const TopoDS_Vertex&          V,
				       const Handle(ChFiDS_Stripe)&  S)
{
  Standard_Integer      Sens = 0;
  Standard_Real         Coeff = 0.5;
  Handle(ChFiDS_Spine)  Spine = S->Spine();
  ChFi3d_IndexOfSurfData(V,S,Sens);
  if (Spine->IsTangencyExtremity((Sens == 1))) return; //No extension on queue
  Standard_Real dU = Spine->LastParameter(Spine->NbEdges());
  if (Sens == 1) {
    Spine->SetFirstParameter(-dU*Coeff);
    Spine->SetFirstTgt(0.);
  }
  else {
    Spine->SetLastParameter(dU*(1.+Coeff));
    Spine->SetLastTgt(dU);
  }
/*
  Standard_Integer Sens;
  Standard_Boolean isfirst;
  Standard_Integer Iedge = 1;
  Standard_Real d1, d2;

  Handle(ChFiDS_Spine) Spine = S->Spine();
  Handle(ChFiDS_ChamfSpine)
      chsp = Handle(ChFiDS_ChamfSpine)::DownCast(Spine);
  chsp->Dists(d1,d2); 
  Standard_Integer IE = ChFi3d_IndexOfSurfData(V,S,Sens);
  isfirst = (Sens == 1);
  if (!isfirst)  
    Iedge = Spine->NbEdges();
 
  TopTools_ListIteratorOfListOfShape It, Jt;
  TopoDS_Edge E1, E2, Ec;
  TopoDS_Face F1, F2, Fc;
  TopoDS_Edge EdgeSp = Spine->Edges(Iedge);

  ConexFaces(Spine,Iedge,F1,F2);

  for (Jt.Initialize(myVEMap(V));Jt.More();Jt.Next()) {
    Ec = TopoDS::Edge(Jt.Value());
    if (!Ec.IsSame(EdgeSp)){
      for (It.Initialize(myEFMap(Ec));It.More();It.Next()) {
	Fc = TopoDS::Face(It.Value());
	if (Fc.IsSame(F1))
	  E1 = Ec;
	else if (Fc.IsSame(F2))
	  E2 = Ec;
      } 
    }
  }

  gp_Vec tg1, tg2, tgsp;
  gp_Pnt tmp, ptgui;
  Spine->D1(Spine->Absc(V),ptgui,tgsp);
  if (isfirst)
    tgsp.Reverse();

  // tg1
  BRepAdaptor_Curve curv;
  curv.Initialize(E1);
  curv.D1(curv.FirstParameter(),tmp,tg1); //pour eviter les projections
  tg1.Reverse();
    // pbm d'erreurs d'approx : baisser la tolerance
  if( !tmp.IsEqual(ptgui,tolesp*1.e2) )
    curv.D1(curv.LastParameter(),tmp,tg1);

  // tg2
  curv.Initialize(E2);
  curv.D1(curv.FirstParameter(),tmp,tg2);
  tg2.Reverse();
  if( !tmp.IsEqual(ptgui,tolesp*1.e2) )
    curv.D1(curv.LastParameter(),tmp,tg2);

  // calcul de dspine
  Standard_Real dspine;
  Standard_Real d1plus = 0.;
  Standard_Real d2plus = 0.;

  Standard_Real sinalpha = tg1.Dot(tgsp);
  if (sinalpha < 0.){
    Standard_Real cosalpha = Sqrt(1 - sinalpha*sinalpha);
    d1plus = -d1*sinalpha/cosalpha;
  }
  sinalpha = tg2.Dot(tgsp);
  if (sinalpha < 0.){
    Standard_Real cosalpha = Sqrt(1 - sinalpha*sinalpha);
    d2plus = -d2*sinalpha/cosalpha;
  }
  dspine = d1plus;
  if (d2plus > d1plus)
    dspine = d2plus;

  dspine *=1.5;

  // ExtentOneCorner
  if (isfirst) {
    Spine->SetFirstParameter(-dspine);
    Spine->SetFirstTgt(0.);
  }
  else{
    Standard_Real param = Spine->LastParameter(Spine->NbEdges());
    Spine->SetLastParameter(param+dspine);
    Spine->SetLastTgt(param);
  } */
}



//=======================================================================
//function : ExtentTwoCorner
//purpose  : extends the spines of the stripes contained in the list LS,
//           on the side of the vertex V
//=======================================================================


void ChFi3d_ChBuilder::ExtentTwoCorner(const TopoDS_Vertex&        V,
				       const ChFiDS_ListOfStripe&  LS)
{
  Standard_Integer Sens = 0;
  ChFiDS_ListIteratorOfListOfStripe itel(LS);
  Standard_Boolean FF = Standard_True;
  Standard_Boolean isfirst[2];
  Standard_Integer Iedge[2];
  Iedge[0] = 1;
  Iedge[1] = 1;
  Handle(ChFiDS_Stripe) Stripe[2];
  Handle(ChFiDS_Spine) Spine[2];

  Standard_Integer i = 0;
  for (; itel.More(); itel.Next(),i++) {    
  ChFi3d_IndexOfSurfData(V,itel.Value(),Sens);
    if (!FF)
      if ( Stripe[1] == itel.Value())
	Sens = -Sens; 
    
    Stripe[i] = itel.Value();
    isfirst[i] = (Sens == 1);
    Spine[i] = Stripe[i]->Spine();
    if( !isfirst[i] )
      Iedge[i] = Spine[i]->NbEdges();
    FF = Standard_False;
  }


  Handle(ChFiDS_ChamfSpine) chsp[2];
  Standard_Real d[4], dis[2] = { 0.0, 0.0 };
  Standard_Integer j;
  TopoDS_Face F[4];
  Standard_Real tmpang, tmd;
  Standard_Boolean disonF1;


  for (i=0, j=0; i<2; i++, j += 2) {
    chsp[i] = Handle(ChFiDS_ChamfSpine)::DownCast(Spine[i]);
    ConexFaces(Spine[i],Iedge[i],F[j],F[j+1]);

    if (chsp[i]->IsChamfer() == ChFiDS_Sym) {
      chsp[i]->GetDist(d[j]);
      d[j+1] = d[j]; 
    }
    else if (chsp[i]->IsChamfer() == ChFiDS_TwoDist) {
      chsp[i]->Dists(d[j],d[j+1]); 
    }
    else {
      chsp[i]->GetDistAngle(tmd, tmpang, disonF1);
      // an approximate calculation of distance 2 is done
      if (disonF1) {
	d[j]   = tmd;
	d[j+1] = tmd * tan(tmpang); 
      }
      else
      {
	d[j]   = tmd * tan(tmpang);
	d[j+1] = tmd;
      }
    }

  }

  Standard_Boolean notfound = Standard_True;
  i = 0; 
  while (notfound && (i<2)) {
    j = 0;
    while (notfound && (j<2)) {
      if (F[i].IsSame(F[j+2])) {
	dis[0] = d[i];
//	dOnArc[0] = d[(i+1)%2];

	dis[1] = d[j + 2];
//	dOnArc[1] = d[(j+1)%2 + 2];
	notfound = Standard_False;
      }
      j++;
    }
    i++;
  }
  // ExtentTwoCorner

  ChFiDS_State State[2];

  for (i=0; i<2; i++) {
    if (isfirst[i])
      State[i] = Spine[i]->FirstStatus();
    else
      State[i] = Spine[i]->LastStatus();
 }
  
  if (State[0] == ChFiDS_AllSame ){
/*      
   // The greatest intersection of the chamfer is found (on the incident edge)
    // with the face at end
    i = 0;
    j = 1;
    if(dOnArc[j] > dOnArc[i]) {
      Standard_Integer temp = i;
      i = j;
      j = temp;
    }
    ExtentOneCorner( V, Stripe[i] ); */

    // it is necessary that two chamfers touch the face at end 
    for (j=0; j<2; j++)
      ExtentOneCorner( V, Stripe[j] ); 
  }
  else if ((State[0] == ChFiDS_OnSame) && (State[1] == ChFiDS_OnSame)) {

    ExtentSpineOnCommonFace(Spine[0],Spine[1],V,dis[0],dis[1],
			    isfirst[0],isfirst[1]);
  }

}

//=======================================================================
//function : ExtentThreeCorner
//purpose  : 
//=======================================================================

void ChFi3d_ChBuilder::ExtentThreeCorner(const TopoDS_Vertex& V,
					 const ChFiDS_ListOfStripe& LS)
{
  Standard_Integer Sens = 0;
  ChFiDS_ListOfStripe check;
  Standard_Boolean isfirst[3];
  Standard_Integer Iedge[3];
  Iedge[0] = 1;
  Iedge[1] = 1;
  Iedge[2] = 1;  
  Handle(ChFiDS_Spine) Spine[3];

  Standard_Integer i = 0;
  for(ChFiDS_ListIteratorOfListOfStripe itel(LS); itel.More(); itel.Next(), i++) {    
    Handle(ChFiDS_Stripe) Stripe = itel.Value(); 
    ChFi3d_IndexOfSurfData(V,Stripe,Sens);
    for(ChFiDS_ListIteratorOfListOfStripe ich(check); ich.More(); ich.Next()){
      if(Stripe == ich.Value()){
	Sens = -Sens;
	break;
      }
    }

    isfirst[i] = (Sens == 1);
    Spine[i] = Stripe->Spine();
    if( !isfirst[i] )
      Iedge[i] = Spine[i]->NbEdges();

    check.Append(Stripe);
  }
  
  Standard_Real d[3][2], tmd, tmpangle;
  Standard_Boolean disonF1;
  Standard_Integer j;
  TopoDS_Face F[3][2];

  Handle(ChFiDS_ChamfSpine) chsp[3];

  for (i=0; i<3; i++) {
    chsp[i] = Handle(ChFiDS_ChamfSpine)::DownCast(Spine[i]);
    ConexFaces(Spine[i],Iedge[i],F[i][0],F[i][1]);

    if (chsp[i]->IsChamfer() == ChFiDS_Sym) {
      chsp[i]->GetDist(d[i][0]); 
      d[i][1] = d[i][0];
    }
    else if (chsp[i]->IsChamfer() == ChFiDS_TwoDist) {
       chsp[i]->Dists(d[i][0],d[i][1]); 
    }
    else {
      chsp[i]->GetDistAngle(tmd, tmpangle, disonF1);
      // an approximate calculation of distance 2 is done

      if (disonF1) {
	d[i][0] = tmd;
	d[i][1] = tmd * tan(tmpangle); 
      }
      else {
	d[i][0] = tmd * tan(tmpangle);
	d[i][1] = tmd;
      }     
    }
  }


  // dis[i][j] distance from chamfer i on the common face with 
  // chamfer j
  Standard_Real dis[3][3];

  for (i=0; i<3; i++) {
//    for (Standard_Integer ii=0; ii<3; ii++) {
//      j = (i+ii)%3;
      j = (i+1)%3;
      Standard_Boolean notfound = Standard_True;
      Standard_Integer k, l;
      k = 0;
      while (notfound && (k<2)) {
	l = 0;
	while (notfound && (l<2)) {
	  if (F[i][k].IsSame(F[j][l])) {
	    dis[i][j] = d[i][k];
	    dis[j][i] = d[j][l];
	    notfound = Standard_False;
	  }
	  l++;
	}
	k++;
      } 
//    }
  }
   
  //ExtentThreeCorner
  for (i=0; i<3; i++) {
    j = (i+1)%3;
    ExtentSpineOnCommonFace(Spine[i],Spine[j],V,dis[i][j],dis[j][i],
			    isfirst[i],isfirst[j]);
  }

}

//=======================================================================
//function : SetRegul
//purpose  : 
//=======================================================================

void ChFi3d_ChBuilder::SetRegul()

{
  ChFiDS_ListIteratorOfRegularities it;
  TopTools_ListIteratorOfListOfShape itc;
  TopTools_ListIteratorOfListOfShape its1;
  TopTools_ListIteratorOfListOfShape its2;
  BRepAdaptor_Surface S;
  BRepAdaptor_Curve2d PC;
  Standard_Real u,v,t;
  gp_Pnt p;
  gp_Vec n1,n2,du,dv;
  BRep_Builder B;
  Standard_Real Seuil = M_PI/360.;
  Standard_Real Seuil2 = Seuil * Seuil;
  for (it.Initialize(myRegul); it.More(); it.Next()){
    const ChFiDS_Regul& reg = it.Value();
    itc.Initialize(myCoup->NewEdges(reg.Curve()));
    if(itc.More()){
      TopoDS_Edge E = TopoDS::Edge(itc.Value());
      if(reg.IsSurface1() && reg.IsSurface2()){
	its1.Initialize(myCoup->NewFaces(reg.S1()));
	its2.Initialize(myCoup->NewFaces(reg.S2()));
	if(its1.More() && its2.More()){
	  TopoDS_Face F1 = TopoDS::Face(its1.Value());
	  TopoDS_Face F2 = TopoDS::Face(its2.Value());
	  S.Initialize(F1,Standard_False);
	  PC.Initialize(E,F1);
	  t = 0.5*(PC.FirstParameter() + PC.LastParameter());
	  PC.Value(t).Coord(u,v);
	  S.D1(u,v,p,du,dv);
	  n1 = du.Crossed(dv);

	  S.Initialize(F2,Standard_False);
	  PC.Initialize(E,F2);
	  PC.Value(t).Coord(u,v);
	  S.D1(u,v,p,du,dv);
	  n2 = du.Crossed(dv);
	  
	  if(n1.SquareMagnitude() > 1.e-14 && n2.SquareMagnitude() > 1.e-14){
	    n1.Normalize();
	    n2.Normalize();
	    Standard_Real sina2 = n1.Crossed(n2).SquareMagnitude();
	    if(sina2 < Seuil2) {
	      GeomAbs_Shape cont = ChFi3d_evalconti(E,F1,F2);
	      B.Continuity(E,F1,F2,cont);
	    }
	  }
	}
      }
    }
  }
}

//=======================================================================
//function : ConexFaces
//purpose  : F1, F2 are connected to edge so that F1 corresponds to distance
//=======================================================================

void ChFi3d_ChBuilder::ConexFaces (const Handle(ChFiDS_Spine)&  Spine,
				   const Standard_Integer       IEdge,
				   TopoDS_Face&                 F1,
				   TopoDS_Face&                 F2) const
{
  BRepAdaptor_Surface Sb1,Sb2;
  TopAbs_Orientation tmp1,tmp2;
  Standard_Integer RC,Choix;
  TopoDS_Face f1,f2,ff1,ff2;

  //calculate the reference orientation
  // ChFi3d_Builder::StripeOrientations is private
  SearchCommonFaces(myEFMap,Spine->Edges(1),ff1,ff2);
  ff1.Orientation(TopAbs_FORWARD);
  Sb1.Initialize(ff1);
  ff2.Orientation(TopAbs_FORWARD);
  Sb2.Initialize(ff2);
  RC = ChFi3d::ConcaveSide(Sb1,Sb2,Spine->Edges(1),tmp1,tmp2);
				  
  //calculate the connected faces
  SearchCommonFaces(myEFMap,Spine->Edges(IEdge),f1,f2);
  Sb1.Initialize(f1);
  Sb2.Initialize(f2);
  Choix = ChFi3d::ConcaveSide(Sb1,Sb2,Spine->Edges(IEdge),tmp1,tmp2);

  if (RC%2 != Choix%2) {
    F1 = f2;
    F2 = f1;
  }
  else {
    F1 = f1;
    F2 = f2;
  }
}


//=======================================================================
//function : FindChoiceDistAngle
//purpose  : F1, F2 connected to the edge so that F1 corresponds to distance
//=======================================================================

Standard_Integer ChFi3d_ChBuilder::FindChoiceDistAngle(const Standard_Integer Choice,
						       const Standard_Boolean DisOnF1) const
{
  Standard_Integer ch = 0;  
  if (!DisOnF1) {

    switch (Choice) {
      case 1 : ch = 2;
               break;
      case 2 : ch = 1;
               break;
      case 3 : ch = 8;
               break;
      case 4 : ch = 7;
               break;
      case 5 : ch = 6;
               break;
      case 6 : ch = 5;
               break;
      case 7 : ch = 4;
               break;
      case 8 : ch = 3;
               break;
    }

  }
  else
    ch = Choice;
 
  return ch;
}
