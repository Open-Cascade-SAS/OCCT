// Created on: 1998-08-19
// Created by: Yves FRICAUD
// Copyright (c) 1998-1999 Matra Datavision
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


#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Geom2d_Curve.hxx>
#include <gp.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TColStd_MapOfTransient.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_Association.hxx>
#include <TopOpeBRepDS_connex.hxx>
#include <TopOpeBRepDS_GapFiller.hxx>
#include <TopOpeBRepDS_GapTool.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_ListIteratorOfListOfInterference.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopOpeBRepDS_CurvePointInterference.hxx>

//=======================================================================
//function : TopOpeBRepDS_GapFiller
//purpose  : 
//=======================================================================

TopOpeBRepDS_GapFiller::TopOpeBRepDS_GapFiller(const Handle(TopOpeBRepDS_HDataStructure)& HDS) 
:myHDS(HDS)
{
  myGapTool   = new TopOpeBRepDS_GapTool(HDS);
  myAsso = new TopOpeBRepDS_Association();
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void TopOpeBRepDS_GapFiller::Perform() 
{
  myGapTool->Init(myHDS);
  TColStd_MapOfInteger View;
  //------------------------------------------------------------
  // APPAIRAGE ...
  //------------------------------------------------------------
  Standard_Integer NbCurves = myHDS->NbCurves();
  for (Standard_Integer i = 1; i <= NbCurves; i++) {
    TopOpeBRepDS_ListOfInterference& LI = myHDS->ChangeDS().ChangeCurveInterferences(i);
    for (TopOpeBRepDS_ListIteratorOfListOfInterference it(LI); it.More(); it.Next()) {
      TopOpeBRepDS_ListOfInterference ALI;
      const Handle(TopOpeBRepDS_Interference)& I = it.Value();
      if (I->GeometryType() == TopOpeBRepDS_POINT) {
	if (View.Add(I->Geometry())) {
	  FindAssociatedPoints (I,ALI);
	  myAsso->Associate(I,ALI);
	}
      }
    }
  }
  //--------------------------------------------------------
  // Modification des Geometries
  //--------------------------------------------------------
  BuildNewGeometries();
}

//=======================================================================
//function : Contains
//purpose  : 
//=======================================================================

Standard_Boolean Contains (const TopoDS_Shape& F,const TopoDS_Shape& E)
{
  TopExp_Explorer exp;
  for (exp.Init(F,E.ShapeType()); exp.More(); exp.Next()){
//  for (TopExp_Explorer exp(F,E.ShapeType()); exp.More(); exp.Next()){
    if (exp.Current().IsSame(E)) return 1;
  }
  return 0;
}


//=======================================================================
//function : FindAssociatedPoints
//purpose  : 
//=======================================================================

void TopOpeBRepDS_GapFiller::FindAssociatedPoints(const Handle(TopOpeBRepDS_Interference)& I,
						  TopOpeBRepDS_ListOfInterference&         LI) 
{
  TopOpeBRepDS_ListIteratorOfListOfInterference itSI(myGapTool->SameInterferences(I));
  for (; itSI.More();itSI.Next()){ 
    if (myAsso->HasAssociation(itSI.Value())) return;
  }

  //------------------------------------------------------------------------
  // PREMIERE PASSE : Recherche de l association parmi les points qui sont:
  //                  - sur la meme arete
  //                  - sur l autre face connexe a l arete.
  //                  - Dans la meme face coupante.
  //------------------------------------------------------------------------
  //----------------------------------------------
  // LI = { ensemble des interference sur l'arete}
  //----------------------------------------------
  TopoDS_Shape E ;
  if (!myGapTool->EdgeSupport(I,E)) {
    return;
  }
  AddPointsOnShape(E,LI);
  
  TopoDS_Face F1,F2,F;
  if (!myGapTool->FacesSupport(I,F1,F2)) {
    LI.Clear(); return;
  }

  if (!Contains(F1,E)) {F = F2; F2 = F1; F1 = F;}

  const TopTools_ListOfShape& LF = FDSCNX_EdgeConnexitySameShape(E,myHDS);
  //------------------------------------------------------------------------
  // Si le point de I est sur deux faces connexes a E => connexite des sections
  // => pas d association a faire.
  //------------------------------------------------------------------------
//   for (TopTools_ListIteratorOfListOfShape itLF(LF); itLF.More(); itLF.Next()) {
   TopTools_ListIteratorOfListOfShape itLF(LF) ;
   for ( ; itLF.More(); itLF.Next()) {
    if (!itLF.Value().IsSame(F1)) {
      if (IsOnFace(I,TopoDS::Face(itLF.Value()))) {
	LI.Clear(); return;
      }
    }
  }

  //------------------------------------------------------------------------
  // LI = { ensemble des interference sur l'arete && sur une autre section ds 
  // la Face connexe par E} 
  //------------------------------------------------------------------------

  for (itLF.Initialize(LF); itLF.More(); itLF.Next()) {
    if (!itLF.Value().IsSame(F1)) {
      FilterByFace(TopoDS::Face(itLF.Value()),LI);
    }
  }
  //--------------------------------------------
  // LI = LI && point dans la meme face Support2
  //--------------------------------------------
  if (!LI.IsEmpty()) 
    FilterByFace (F2, LI);

  //-------------------------------
  // Controle et Selection metrique 
  //-------------------------------
  if (!LI.IsEmpty()) 
    FilterByIncidentDistance (F2,I,LI);

  if (!LI.IsEmpty()) {
    LI.Append(I);
  }
}


//=======================================================================
//function : CheckConnexity
//purpose  : 
//=======================================================================

//Standard_Boolean TopOpeBRepDS_GapFiller::CheckConnexity(TopOpeBRepDS_ListOfInterference& LI) 
Standard_Boolean TopOpeBRepDS_GapFiller::CheckConnexity(TopOpeBRepDS_ListOfInterference& ) 
{
  return 1;
}

//=======================================================================
//function : AddPointsOnFace
//purpose  : 
//=======================================================================

void TopOpeBRepDS_GapFiller::AddPointsOnShape (const TopoDS_Shape&               S,
					       TopOpeBRepDS_ListOfInterference& LI) 
{
  const TopOpeBRepDS_ListOfInterference& LIOnE = myHDS->DS().ShapeInterferences(S);
  for (TopOpeBRepDS_ListIteratorOfListOfInterference it(LIOnE); it.More(); it.Next()){
    LI.Append(it.Value());
  }
}

//=======================================================================
//function : AddPointsOnConnexShape
//purpose  : 
//=======================================================================

//void TopOpeBRepDS_GapFiller::AddPointsOnConnexShape(const TopoDS_Shape&         //           F,
//						    const TopOpeBRepDS_ListOfInterference& LI) 
void TopOpeBRepDS_GapFiller::AddPointsOnConnexShape(const TopoDS_Shape&                    ,
						    const TopOpeBRepDS_ListOfInterference& ) 
{
}


//=======================================================================
//function : FilterbyFace
//purpose  : 
//=======================================================================

void TopOpeBRepDS_GapFiller::FilterByFace(const TopoDS_Face&               F,
					  TopOpeBRepDS_ListOfInterference& LI) 
{
  //------------------------------------------------------------------------
  // il ne restera dans LI que les interference dont une des representation 
  // a ete calculee sur F .
  //------------------------------------------------------------------------
  TopOpeBRepDS_ListIteratorOfListOfInterference it(LI); 
  while (it.More()) {
    if (!IsOnFace(it.Value(),F)) {
      LI.Remove(it);
    }
    else {	
      it.Next();
    }
  }
}

//=======================================================================
//function : IsOnFace
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_GapFiller::IsOnFace
(const Handle(TopOpeBRepDS_Interference)& I,
 const TopoDS_Face&                       F) const
{
  TopOpeBRepDS_Curve C;
  if (myGapTool->Curve(I,C)) {
    TopoDS_Shape S1,S2;
    C.GetShapes(S1,S2);
    if (S1.IsSame(F)) return 1;
    if (S2.IsSame(F)) return 1;
  }
  return 0;
}


//=======================================================================
//function : FilterbyEdge
//purpose  : 
//=======================================================================

void TopOpeBRepDS_GapFiller::FilterByEdge(const TopoDS_Edge&               E,
					  TopOpeBRepDS_ListOfInterference& LI) 
{  
  // il ne restera dans LI que les interference dont une des representation 
  // a pour support E
  TopOpeBRepDS_ListIteratorOfListOfInterference it(LI);
  while (it.More()) {
    if (!IsOnEdge(it.Value(),E)) {
      LI.Remove(it);
    }
    else {	
      it.Next();
    }
  }
}

//=======================================================================
//function : IsOnEdge
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_GapFiller::IsOnEdge
(const Handle(TopOpeBRepDS_Interference)& I,
 const TopoDS_Edge&                       E) const
{
  const TopOpeBRepDS_ListOfInterference& LI = myGapTool->SameInterferences(I);
  for (TopOpeBRepDS_ListIteratorOfListOfInterference it(LI); it.More(); it.Next()) {
    const Handle(TopOpeBRepDS_Interference)& IC = it.Value();
    if (IC->SupportType() == TopOpeBRepDS_EDGE) {
      const TopoDS_Shape& S1 = myHDS->Shape(IC->Support());
      if (S1.IsSame(E)) return 1;
    }
  }
  return 0;
}



//=======================================================================
//function : Normale
//purpose  : 
//=======================================================================

static Standard_Boolean Normal(const Handle(TopOpeBRepDS_GapTool)&      A,
			       const Handle(TopOpeBRepDS_HDataStructure)& HDS,
			       const Handle(TopOpeBRepDS_Interference)&   I,
			       const TopoDS_Face&                         F,
			       gp_Dir&                                    D)

{ 
  TopOpeBRepDS_Kind GK,SK;
  Standard_Integer  IG,IS;
  gp_Pnt   PS;
  gp_Vec   TU,TV,N;
  gp_Pnt2d P2d;
  
  BRepAdaptor_Surface S(F);
  if (S.GetType() == GeomAbs_Plane) {
    D = S.Plane().Axis().Direction();
    return 1;
  }
  
  const TopOpeBRepDS_ListOfInterference& LI = A->SameInterferences(I);
  for (TopOpeBRepDS_ListIteratorOfListOfInterference it(LI); it.More(); it.Next()) {
    const Handle(TopOpeBRepDS_Interference)&   IC = it.Value();
    IC->GKGSKS(GK,IG,SK,IS);
    if (SK == TopOpeBRepDS_CURVE) {
      const TopOpeBRepDS_Curve& C = HDS->Curve(IS);
      Standard_Real P   = Handle(TopOpeBRepDS_CurvePointInterference)::DownCast (IC)->Parameter();
      
      TopoDS_Shape S1,S2;
      C.GetShapes(S1,S2);
      if (F.IsSame(S1)) {
	if (C.Curve1().IsNull()) return 0;
	P2d = C.Curve1()->Value(P);
      }
      else {
	if (C.Curve2().IsNull()) return 0;
	P2d = C.Curve2()->Value(P);
      }
      
      if (S.UContinuity() >= GeomAbs_C1 && S.VContinuity() >= GeomAbs_C1) {
	S.D1(P2d.X(),P2d.Y(),PS,TU,TV);
	N = TU^TV;
//	if (N.SquareMagnitude() >= gp::Resolution()); { // !!
	if (N.SquareMagnitude() >= gp::Resolution()) {
	  D = gp_Dir(N);
	  return 1;
	}
      }
      return 0;
    }
  } 
  return 0;
}

//=======================================================================
//function : FilterByIncidentDistance
//purpose  : 
//=======================================================================

void TopOpeBRepDS_GapFiller::FilterByIncidentDistance(const TopoDS_Face& F,
						      const Handle(TopOpeBRepDS_Interference)& I,
						      TopOpeBRepDS_ListOfInterference& LI) 
{
  Standard_Real DistMin = Precision::Infinite();  
  Handle(TopOpeBRepDS_Interference) ISol;
  
  const TopOpeBRepDS_Point& PI1 = myHDS->Point(I->Geometry());
  const gp_Pnt              GPI = PI1.Point();
 
  BRepAdaptor_Surface S(F,0);

  Standard_Real TolDef  = 0.94;// cos(20degre);
  Standard_Real TolDist = 20*PI1.Tolerance();
  gp_Dir N1,N2;
  Standard_Boolean Ok1 = Normal(myGapTool,myHDS,I,F,N1);
  
  for (TopOpeBRepDS_ListIteratorOfListOfInterference it(LI);it.More(); it.Next()) {
    
    const Handle(TopOpeBRepDS_Interference)& CI = it.Value();
  
    if (CI->HasSameGeometry(I)) continue;

    Standard_Boolean          Ok2  = Normal(myGapTool,myHDS,CI,F,N2);
    const TopOpeBRepDS_Point& P    = myHDS->Point((CI->Geometry()));
    const gp_Pnt              GP   = P.Point();
    Standard_Real             Dist = GP.Distance(GPI);
    
    //---------------------------------------------------------------------------
    // Test resjection 
    // Si les points sont plutot ecartes, on regarde si les Normales sur la face
    // sont proches   
    //---------------------------------------------------------------------------
    if (Dist > TolDist) {
      if (Ok1 && Ok2) {
	if (N1.Dot(N2) < TolDef) {
	  continue;  // Reject
	}
      }
      continue; // Reject
    }
    
    if (Dist < DistMin) {
      DistMin = Dist;
      ISol    = it.Value();
    }
  }

  LI.Clear();
  if (!ISol.IsNull()) {
    LI.Append(ISol);
  }
}

//=======================================================================
//function : ReBuildGeom
//purpose  : 
//=======================================================================

void TopOpeBRepDS_GapFiller::ReBuildGeom(const Handle(TopOpeBRepDS_Interference)& I,
					 TColStd_MapOfInteger&                    View) 
{
  if (!myAsso->HasAssociation(I)) return;

  TopOpeBRepDS_ListOfInterference& LI = myAsso->Associated(I);
  TopOpeBRepDS_ListIteratorOfListOfInterference it(LI);

  Standard_Real TolMax = 0,UMin = Precision::Infinite();
  Standard_Real UMax = -UMin, U;
  TopoDS_Edge   E,CE ;
  myGapTool->EdgeSupport(I,E);

  // Construction du nouveau point
  for  (it.Initialize(LI); it.More(); it.Next()) {
    TopOpeBRepDS_Point PP = myHDS->Point(it.Value()->Geometry());
    TolMax = Max(TolMax,PP.Tolerance());
    if (myGapTool->ParameterOnEdge (it.Value(),E,U)) {
      UMin = Min(UMin,U);
      UMax = Max(UMax,U);
    }
    myGapTool->EdgeSupport(it.Value(),CE);
    if (!CE.IsSame(E)) {
      return;
    }
  }
  U = (UMax+UMin)*0.5;
  BRepAdaptor_Curve C(E); gp_Pnt GP = C.Value(U);
  TopOpeBRepDS_Point P(GP,TolMax);

  // Mise a jour.
  Standard_Integer IP = myHDS->ChangeDS().AddPoint(P);
  for (it.Initialize(LI); it.More(); it.Next()) {
    View.Add(it.Value()->Geometry());
    myGapTool->SetParameterOnEdge(it.Value(),E,U); 
    myGapTool->SetPoint(it.Value(),IP);
  }
  myGapTool->SetParameterOnEdge(I,E,U);
  myGapTool->SetPoint(I,IP);
  View.Add(IP);
}

//=======================================================================
//function : BuildNewGeometries
//purpose  : 
//=======================================================================

void TopOpeBRepDS_GapFiller::BuildNewGeometries() 
{
  TColStd_MapOfInteger View;
  Standard_Integer NbCurves = myHDS->NbCurves();
  Standard_Integer NbPoints = myHDS->NbPoints();
  for (Standard_Integer i = 1; i <= NbCurves; i++) {
    TopOpeBRepDS_ListOfInterference& LI = myHDS->ChangeDS().ChangeCurveInterferences(i);
    for (TopOpeBRepDS_ListIteratorOfListOfInterference it(LI); it.More(); it.Next()) {
      Handle(TopOpeBRepDS_Interference) I = it.Value();
      Standard_Integer IP = I->Geometry();
      if (View.Add(IP) && IP <= NbPoints) ReBuildGeom(I,View);
    }
  } 
}


