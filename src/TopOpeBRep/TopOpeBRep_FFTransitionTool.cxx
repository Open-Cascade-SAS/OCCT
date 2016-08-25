// Created on: 1994-10-27
// Created by: Jean Yves LEBEY
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


#include <BRep_Tool.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp_Vec.hxx>
#include <IntSurf_Situation.hxx>
#include <IntSurf_Transition.hxx>
#include <IntSurf_TypeTrans.hxx>
#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRep_FFTransitionTool.hxx>
#include <TopOpeBRep_LineInter.hxx>
#include <TopOpeBRep_VPointInter.hxx>
#include <TopOpeBRepDS_CurvePointInterference.hxx>
#include <TopOpeBRepDS_SolidSurfaceInterference.hxx>
#include <TopOpeBRepDS_SurfaceCurveInterference.hxx>
#include <TopOpeBRepDS_Transition.hxx>

//-----------------------------------------------------------------------
//function : TransitionToOrientation
//purpose  : static
//-----------------------------------------------------------------------

static Standard_Boolean TransitionToOrientation
(const IntSurf_Transition& T, 
 TopAbs_Orientation& O)
{
  Standard_Boolean Odefined = Standard_True;
  TopAbs_Orientation result = TopAbs_FORWARD;
  IntSurf_TypeTrans trans;
  IntSurf_Situation situa;

  trans = T.TransitionType();

  switch (trans) {
    
  case IntSurf_In  : result = TopAbs_FORWARD; break;
  case IntSurf_Out : result = TopAbs_REVERSED; break;
    
  case IntSurf_Touch :
    situa = T.Situation();
    switch (situa) {
    case IntSurf_Inside  : result = TopAbs_INTERNAL;  break;
    case IntSurf_Outside : result = TopAbs_EXTERNAL; break;
    case IntSurf_Unknown :
    Odefined = Standard_False;
    break;
    }
    break;

  case IntSurf_Undecided :
  Odefined = Standard_False;
  break;
  }

  O = result;
  return Odefined;
}


//=======================================================================
//function : ProcessLineTransition
//purpose  : compute the transition of the intersection
//         : point <P> on the intersected shape of index <Index> (1 or 2)
//         : for a line crossing an edge
//=======================================================================

TopOpeBRepDS_Transition TopOpeBRep_FFTransitionTool::ProcessLineTransition
  (const TopOpeBRep_VPointInter &P,
   const Standard_Integer Index,
   const TopAbs_Orientation EdgeOrientation)
{
  TopOpeBRepDS_Transition TT;

  if ((EdgeOrientation==TopAbs_INTERNAL)||(EdgeOrientation==TopAbs_EXTERNAL)) {
    TT.Set(EdgeOrientation);
  }
  else {
    TopAbs_Orientation O;
    
    IntSurf_Transition T; {
    switch (Index) {
    case 1 : T = P.TransitionLineArc1(); break;
    case 2 : T = P.TransitionLineArc2(); break;
    }
    Standard_Boolean Odefined = ::TransitionToOrientation(T,O);
    if (Odefined) {
      if (EdgeOrientation == TopAbs_REVERSED) O = TopAbs::Complement(O);
      TT.Set(O);
    }
    else {
      TT.Set(TopAbs_UNKNOWN,TopAbs_UNKNOWN);
    }
    }
  }
  return TT;
}

//=======================================================================
//function : ProcessLineTransition
//purpose  : compute the transition of point P on line L, P lying on
//           neither of the intersecting shapes
//=======================================================================

TopOpeBRepDS_Transition TopOpeBRep_FFTransitionTool::ProcessLineTransition
(const TopOpeBRep_VPointInter &P, const TopOpeBRep_LineInter& LI)
{
  TopOpeBRepDS_Transition TT;
  TopAbs_Orientation result;
  
  // P.IsOnDomS1() and P.IsOnDomS2() are both false

  Standard_Integer nbv = LI.NbVPoint();
  TopOpeBRep_VPointInter P1 = LI.VPoint(1);
  Standard_Real par1 = P1.ParameterOnLine();
  TopOpeBRep_VPointInter Pn = LI.VPoint(nbv);
  Standard_Real parn = Pn.ParameterOnLine();

  Standard_Real par = P.ParameterOnLine();
  if      ( par == par1 ) result = TopAbs_FORWARD;
  else if ( par == parn ) result = TopAbs_REVERSED;
  else result = TopAbs_INTERNAL;

  TT.Set(result);
  return TT;
}


//=======================================================================
//function : ProcessEdgeTransition
//purpose  : compute the transition from the transition of the intersection
//         : point <P> on the intersected shape of index <Index> (1 or 2)
//         : for an edge on a line on a Face
//=======================================================================

TopOpeBRepDS_Transition TopOpeBRep_FFTransitionTool::ProcessEdgeTransition
  (const TopOpeBRep_VPointInter &P,
   const Standard_Integer Index,
   const TopAbs_Orientation FaceTransition)
{
  TopOpeBRepDS_Transition TT;

  if ((FaceTransition == TopAbs_INTERNAL) || 
      (FaceTransition == TopAbs_EXTERNAL)) {
    TT.Set(FaceTransition);
  }
  else {
    IntSurf_Transition T;
    if      ( Index == 1 ) T = P.TransitionOnS1();
    else if ( Index == 2 ) T = P.TransitionOnS2();
    
    TopAbs_Orientation O;
    Standard_Boolean defined = ::TransitionToOrientation(T,O);
    if (defined) {
      if (FaceTransition == TopAbs_REVERSED) O = TopAbs::Complement(O);
      TT.Set(O);
    }
    else {
      TT.Set(TopAbs_UNKNOWN,TopAbs_UNKNOWN);
    }
  }

  return TT;
}


//=======================================================================
//function : ProcessFaceTransition
//purpose  : compute the transition from a Line
//=======================================================================

TopOpeBRepDS_Transition TopOpeBRep_FFTransitionTool::ProcessFaceTransition
  (const TopOpeBRep_LineInter& L,
   const Standard_Integer Index,
   const TopAbs_Orientation FaceOrientation)
{
  // If Index == 1, on first shape
  // If Index == 2, on second shape
  TopOpeBRepDS_Transition TT;

  if ((FaceOrientation == TopAbs_INTERNAL) || 
      (FaceOrientation == TopAbs_EXTERNAL)) {
    TT.Set(FaceOrientation);
  }
  else {
    Standard_Boolean Odefined = Standard_True;

    TopAbs_Orientation O = TopAbs_FORWARD;

    IntSurf_TypeTrans trans;
    trans = (Index == 1) ? L.TransitionOnS1() : L.TransitionOnS2();

    switch (trans) {
    
    case IntSurf_In  : O = TopAbs_FORWARD; break;

    case IntSurf_Out : O = TopAbs_REVERSED; break;
      
    case IntSurf_Touch : {     

      IntSurf_Situation situa;
      situa = (Index == 1 ) ? L.SituationS1() : L.SituationS2();

      switch (situa) {	

      case IntSurf_Inside  : O = TopAbs_INTERNAL; break;

      case IntSurf_Outside : O = TopAbs_EXTERNAL; break;

      case IntSurf_Unknown :

	Odefined = Standard_False;
	break;
      }
      break;
      } // case Touch

    case IntSurf_Undecided :
      Odefined = Standard_False;
      break;

    } // trans
  
    if (Odefined) {
      if (FaceOrientation == TopAbs_REVERSED) O = TopAbs::Complement(O);
      TT.Set(O);
    }
    else {
      TT.Set(TopAbs_UNKNOWN,TopAbs_UNKNOWN);
    }
  }

  return TT;
}


// -------------------------------------------------
// input : P1 : point 
// input : C2 : courbe, FC2,LC2 : bornes de C2
// output : T2 = parametre de P1 sur C2
// -------------------------------------------------
static Standard_Boolean FUN_ProjectPoint(const gp_Pnt& P1,
					 const Handle(Geom_Curve)& C2,
					 const Standard_Real FC2, 
					 const Standard_Real LC2, 
					 Standard_Real& T2)
{
  if ( C2.IsNull() ) {
    return Standard_False;
  }

  GeomAPI_ProjectPointOnCurve mydist(P1,C2,FC2,LC2);
  if ( mydist.Extrema().IsDone() ) {
    if ( mydist.NbPoints() ) {
      T2 = mydist.LowerDistanceParameter();
      return Standard_True;
    }
  }

  return Standard_False;
}

// -------------------------------------------------
// input : S1,U1,V1,C1,T1 avec D0(S1(U1,V1)) = D0(C1(T1))
// input : C2,FC2,LC2 : courbe, bornes de C2
// output : Trans : transition sur C1 en T1 en croisant C2
// -------------------------------------------------
static Standard_Boolean FUN_GeomTrans(const Handle(Geom_Surface)& S1,
				      const Standard_Real U1,
				      const Standard_Real V1,
				      const Handle(Geom_Curve)& C1,
				      const Standard_Real T1,
				      const Handle(Geom_Curve)& C2,
				      const Standard_Real FC2,
				      const Standard_Real LC2,
				      TopOpeBRepDS_Transition& Trans)
{
  if ( C1.IsNull() || C2.IsNull() ) {
    return Standard_False;
  }
  
  // P1 : D0(C1(T1), D1_C1 : D1(C1(T1))
  gp_Pnt P1; gp_Vec D1_C1; C1->D1(T1,P1,D1_C1);
  
  // D1_C2 : D1(C2(P1))
  Standard_Real T2 = 0.0; 
  Standard_Boolean projok = ::FUN_ProjectPoint(P1,C2,FC2,LC2,T2);
  if ( !projok ) {
    return Standard_False;
  }
  gp_Pnt P2; gp_Vec D1_C2; C2->D1(T2,P2,D1_C2);
  
  // N1 : D1(S1(U1,V1))
  gp_Vec N1,D1U,D1V; 
  gp_Pnt PS;
  S1->D1(U1,V1,PS,D1U,D1V);
  D1U.Normalize();
  D1V.Normalize();
  N1 = D1U.Crossed(D1V);
  N1.Normalize();
  
  gp_Vec N1D1_C1 = N1.Crossed(D1_C1);
  Standard_Real dot = N1D1_C1.Dot(D1_C2);
  if ( dot > 0 ) {
    Trans.Before(TopAbs_OUT);
    Trans.After(TopAbs_IN);
  }
  else {
    Trans.Before(TopAbs_IN);
    Trans.After(TopAbs_OUT);
  }
  
  return Standard_True;
}
 
//=======================================================================
//function : ProcessEdgeONTransition
//purpose  : 
//=======================================================================

TopOpeBRepDS_Transition TopOpeBRep_FFTransitionTool::ProcessEdgeONTransition
(const TopOpeBRep_VPointInter& VP,
 const Standard_Integer ShapeIndex,
 const TopoDS_Shape& RR,
 const TopoDS_Shape& EE,
 const TopoDS_Shape& FF)
{
 const TopoDS_Edge& R = TopoDS::Edge(RR);
 const TopoDS_Edge& E = TopoDS::Edge(EE);
 const TopoDS_Face& F = TopoDS::Face(FF);

 TopAbs_Orientation oriE = E.Orientation();
 
 const Handle(Geom_Surface)& S = BRep_Tool::Surface(F);
 Standard_Real U = 0.,V = 0.;
 if      (ShapeIndex == 1) VP.ParametersOnS1(U,V);
 else if (ShapeIndex == 2) VP.ParametersOnS2(U,V);
 
 Standard_Real fE,lE;
 const Handle(Geom_Curve)& CE = BRep_Tool::Curve(E,fE,lE);
 Standard_Real TE = VP.EdgeParameter(ShapeIndex);
 
 Standard_Real fR,lR;
 const Handle(Geom_Curve)& CR = BRep_Tool::Curve(R,fR,lR);
 
 TopOpeBRepDS_Transition Trans;
 Standard_Boolean transok = ::FUN_GeomTrans(S,U,V,CE,TE,CR,fR,lR,Trans);
 if ( transok ) {
   // Trans : transition sur R en croisant l'arete E orientee dans la face F
   if (oriE == TopAbs_REVERSED) Trans = Trans.Complement();
 }
 
  return Trans;
}
