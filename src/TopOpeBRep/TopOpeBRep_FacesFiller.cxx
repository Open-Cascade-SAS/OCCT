// Created on: 1994-10-10
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
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <Geom_Surface.hxx>
#include <GeomProjLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRep.hxx>
#include <TopOpeBRep_define.hxx>
#include <TopOpeBRep_FacesFiller.hxx>
#include <TopOpeBRep_FacesIntersector.hxx>
#include <TopOpeBRep_FFDumper.hxx>
#include <TopOpeBRep_FFTransitionTool.hxx>
#include <TopOpeBRep_LineInter.hxx>
#include <TopOpeBRep_PointClassifier.hxx>
#include <TopOpeBRep_VPointInter.hxx>
#include <TopOpeBRep_VPointInterClassifier.hxx>
#include <TopOpeBRep_VPointInterIterator.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS_Transition.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopOpeBRepTool_defineG.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_SC.hxx>

//#include <BRepAdaptor_Curve2d.hxx>
Standard_EXPORT Standard_Boolean FUN_projPonL(const gp_Pnt& P,const TopOpeBRep_LineInter L,const TopOpeBRep_FacesFiller& FF,
				 Standard_Real& paramL)
{
  Standard_Boolean paramLdef = Standard_False;
  Standard_Integer Esi = (L.ArcIsEdge(1)) ? 1:2;
  const TopoDS_Edge& E = TopoDS::Edge(L.Arc());
  Standard_Boolean hasC3D = FC2D_HasC3D(E);
  Standard_Real dist;
  if (hasC3D) {
    BRepAdaptor_Curve BAC(E);
    paramLdef = FUN_tool_projPonC(P,BAC,paramL,dist);
  }
  else {
    BRepAdaptor_Curve2d BAC2D;
    if      (Esi == 1)  BAC2D.Initialize(E,FF.Face(1));
    else if (Esi == 2)  BAC2D.Initialize(E,FF.Face(2));
    paramLdef = FUN_tool_projPonC2D(P,BAC2D,paramL,dist);
  }
  return paramLdef;
}

#ifdef OCCT_DEBUG
void debffsamdom(void){}
#endif

static void FUN_MakeERL(TopOpeBRep_FacesIntersector& FI,TopTools_ListOfShape& ERL)
{
  ERL.Clear();
  const TopTools_IndexedMapOfShape& mer = FI.Restrictions();
  for ( Standard_Integer ie = 1, ne = mer.Extent(); ie <= ne; ie++) {
    const TopoDS_Edge& E = TopoDS::Edge(mer.FindKey(ie));
    ERL.Append(E);
  }
}

//=======================================================================
//function : TopOpeBRep_FacesFiller
//purpose  : 
//=======================================================================
TopOpeBRep_FacesFiller::TopOpeBRep_FacesFiller() : myPShapeClassifier(NULL)
{ 
  myexF1 = myexF2 = 0;
#ifdef OCCT_DEBUG
  myHFFD = new TopOpeBRep_FFDumper(this);
#endif
}

//=======================================================================
//function : PShapeClassifier
//purpose  : 
//=======================================================================
TopOpeBRepTool_PShapeClassifier TopOpeBRep_FacesFiller::PShapeClassifier() const
{
  return myPShapeClassifier;
}

//=======================================================================
//function : SetPShapeClassifier
//purpose  : 
//=======================================================================
void TopOpeBRep_FacesFiller::SetPShapeClassifier(const TopOpeBRepTool_PShapeClassifier& PSC) 
{
  myPShapeClassifier = PSC;
}

//=======================================================================
//function : Insert
//purpose  : 
//=======================================================================
void TopOpeBRep_FacesFiller::Insert(const TopoDS_Shape& S1,const TopoDS_Shape& S2,TopOpeBRep_FacesIntersector& FACINT,const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  myF1 = TopoDS::Face(S1); myF1ori = S1.Orientation();
  myF2 = TopoDS::Face(S2); myF2ori = S2.Orientation(); 
  myFacesIntersector = &FACINT;
  myHDS = HDS;
  myDS = &(HDS->ChangeDS());
  if (myPShapeClassifier == NULL) myPShapeClassifier = new TopOpeBRepTool_ShapeClassifier();

#ifdef OCCT_DEBUG
  Standard_Integer exF1,exF2; GetTraceIndex(exF1,exF2);
  myFacesIntersector->InitLine();
  for (; myFacesIntersector->MoreLine(); myFacesIntersector->NextLine()) myFacesIntersector->CurrentLine().SetTraceIndex(exF1,exF2);
  myHFFD->Init(this);
#endif
  
  Standard_Boolean samdom = myFacesIntersector->SameDomain();
  if ( samdom ) {
    myDS->FillShapesSameDomain(S1,S2);
    return;
  }
  
  myFacesIntersector->InitLine();
  for (; myFacesIntersector->MoreLine(); myFacesIntersector->NextLine()) {
    TopOpeBRep_LineInter& L = myFacesIntersector->CurrentLine();
    L.SetFaces(TopoDS::Face(S1),TopoDS::Face(S2));
  }
  
  VP_Position(FACINT);

  myFacesIntersector->InitLine();
  for (; myFacesIntersector->MoreLine(); myFacesIntersector->NextLine()) {
    TopOpeBRep_LineInter& L = myFacesIntersector->CurrentLine();
    L.SetHasVPonR();
    L.SetINL();
    L.SetIsVClosed();
  }

  ProcessSectionEdges();
  myFFfirstDSP = myDS->NbPoints() + 1;

  FUN_MakeERL((*myFacesIntersector), myERL); // BUG

  myFacesIntersector->InitLine();
  for (; myFacesIntersector->MoreLine(); myFacesIntersector->NextLine()) {
    TopOpeBRep_LineInter& L = myFacesIntersector->CurrentLine();
    LoadLine(L);
    ProcessLine();
  }
  
}

//=======================================================================
//function : ChangePointClassifier
//purpose  : 
//=======================================================================
TopOpeBRep_PointClassifier& TopOpeBRep_FacesFiller::ChangePointClassifier()
{  
  return myPointClassifier;
}


//=======================================================================
//function : LoadLine
//purpose  : 
//=======================================================================
void TopOpeBRep_FacesFiller::LoadLine(TopOpeBRep_LineInter& L)
{
  myLine = &L;
  Standard_Boolean bchk = CheckLine(L);
  Standard_Boolean binl = L.INL();
  myLineINL = binl;
  {
    TopOpeBRep_TypeLineCurve t = L.TypeLineCurve();
    if ( !bchk && binl && t == TopOpeBRep_LINE ) {
      bchk = Standard_True;
    }
  }
  L.SetOK(bchk);
  myLineOK = bchk;
  if (!myLineOK) return;
  
  L.ComputeFaceFaceTransition();
} // LoadLine

//=======================================================================
//function : CheckLine
//purpose  : private
//           returns False if L is WALKING line with a number of VPoints < 2
//           else returns True 
//=======================================================================
Standard_Boolean TopOpeBRep_FacesFiller::CheckLine(TopOpeBRep_LineInter& L) const
{
  Standard_Real tol1,tol2;
  myFacesIntersector->GetTolerances(tol1,tol2);
  
  Standard_Boolean check = Standard_True;
  TopOpeBRep_TypeLineCurve t = L.TypeLineCurve();
  Standard_Integer nbvp = L.NbVPoint();
  
  if ( t == TopOpeBRep_WALKING ) {
    if ( nbvp < 2 ) {
#ifdef OCCT_DEBUG
      cout<<"\n=== Nb de IntPatch_Point sur WL incorrect : "<<nbvp<<" ===\n";
#endif
      check = Standard_False;
    }
  }
  else if (t == TopOpeBRep_LINE) {
    Standard_Integer np = 0;
    TopOpeBRep_VPointInterIterator VPI;
    
    for ( VPI.Init(L); VPI.More(); VPI.Next()) {
      const TopOpeBRep_VPointInter& VP = VPI.CurrentVP();
      if ( VP.Keep() ) np++;
    }
    if ( np != 2 ) {
      return Standard_True;
    }
    
    TopOpeBRep_VPointInter A,B;
    np = 0;
    for ( VPI.Init(L); VPI.More(); VPI.Next()) {
      const TopOpeBRep_VPointInter& VP = VPI.CurrentVP();
      if ( !VP.Keep() ) continue;
      np++;
      if ( np == 1 ) A = VP;
      if ( np == 2 ) B = VP;
    }
    
    Standard_Boolean isAV1 = A.IsVertexOnS1();
    Standard_Boolean isAV2 = A.IsVertexOnS2();
    TopoDS_Shape V1;
    if (isAV1) V1 = A.VertexOnS1();
    if (isAV2) V1 = A.VertexOnS2();
    Standard_Boolean isBV1 = B.IsVertexOnS1();
    Standard_Boolean isBV2 = B.IsVertexOnS2();
    TopoDS_Shape V2;
    if (isBV1) V2 = B.VertexOnS1();
    if (isBV2) V2 = B.VertexOnS2();
    
    if ( !V1.IsNull() && ( V1.IsSame(V2) ) ) {
      return Standard_False;      
    }
  } // LINE
  else {
    Standard_Boolean notrnotw = (t != TopOpeBRep_RESTRICTION && t != TopOpeBRep_WALKING);
    if (notrnotw) {
      if (t == TopOpeBRep_CIRCLE) { 
	// cto 012 D2, faces 6 et 1, line 3 incorrecte.
	
	Standard_Integer iINON1,iINONn,nINON;
	myLine->VPBounds(iINON1,iINONn,nINON);
	if ( nINON >= 2) {
	  
	  const TopOpeBRep_VPointInter& A = myLine->VPoint(iINON1);
	  const TopOpeBRep_VPointInter& B = myLine->VPoint(iINONn);
	  Standard_Real parA = A.ParameterOnLine();    
	  Standard_Real parB = B.ParameterOnLine();    
	  Standard_Boolean conf = (fabs(parA-parB) < tol1);
	  if (conf) {
	    check = Standard_False;
	  }	  
	}
      } // CIRCLE
      else if (t == TopOpeBRep_HYPERBOLA) {
	Standard_Integer iINON1,iINONn,nINON;
	myLine->VPBounds(iINON1,iINONn,nINON);
	if ( nINON < 2 ) {
	  check = Standard_False;
	}
      }
      else if (t == TopOpeBRep_ELLIPSE) {
	Standard_Integer iINON1,iINONn,nINON;
	myLine->VPBounds(iINON1,iINONn,nINON);
	if ( nINON < 2 ) {
	  check = Standard_False;
	}
	else {
	  const TopOpeBRep_VPointInter& A = myLine->VPoint(iINON1);
	  const TopOpeBRep_VPointInter& B = myLine->VPoint(iINONn);
	  Standard_Real parA = A.ParameterOnLine();    
	  Standard_Real parB = B.ParameterOnLine();    
	  Standard_Boolean conf = (fabs(parA-parB) < tol1);
	  if (conf) {
	    check = Standard_False;
	  }
	}
      }
    }
  }
  
#ifdef OCCT_DEBUG
  if (!check) { cout<<"# DEB CheckLine : rejet de ";TopOpeBRep::Print(t,cout);cout<<" a "<<nbvp<<" points"<<endl; }
#endif

  return check;
}

//=======================================================================
//function : VP_Position
//purpose  : 
//=======================================================================
//void TopOpeBRep_FacesFiller::VP_Position(TopOpeBRep_FacesIntersector& FACINT)
void TopOpeBRep_FacesFiller::VP_Position(TopOpeBRep_FacesIntersector& )
{
  for (myFacesIntersector->InitLine(); 
       myFacesIntersector->MoreLine();
       myFacesIntersector->NextLine()) {
    TopOpeBRep_LineInter& L = myFacesIntersector->CurrentLine();
    const TopOpeBRep_TypeLineCurve tl = L.TypeLineCurve();
    Standard_Boolean ok = (tl == TopOpeBRep_RESTRICTION) ;
    if ( ok ) VP_Position(L);
  }
  
  for (myFacesIntersector->InitLine(); 
       myFacesIntersector->MoreLine(); 
       myFacesIntersector->NextLine()) {
    TopOpeBRep_LineInter& L = myFacesIntersector->CurrentLine();
    const TopOpeBRep_TypeLineCurve tl = L.TypeLineCurve();
    Standard_Boolean ok = (tl != TopOpeBRep_RESTRICTION) ;
    if ( ok ) VP_Position(L);
  }
}

//=======================================================================
//function : VP_Position
//purpose  : 
//=======================================================================
void TopOpeBRep_FacesFiller::VP_Position(TopOpeBRep_LineInter& L)
{
  myLine = &L;
  Standard_Boolean isrest = (L.TypeLineCurve() == TopOpeBRep_RESTRICTION) ;
  
  if (!isrest) VP_PositionOnL(L);
  else         VP_PositionOnR(L);
  
  L.SetVPBounds();
}

//=======================================================================
//function : VP_PositionOnL
//purpose  : 
//=======================================================================
void TopOpeBRep_FacesFiller::VP_PositionOnL(TopOpeBRep_LineInter& L)
{
  TopOpeBRep_VPointInterIterator VPI(L);
  Standard_Integer Lindex = L.Index();
  TopOpeBRep_VPointInterClassifier VPC;
  
  for (; VPI.More(); VPI.Next()) {
    TopOpeBRep_VPointInter& VP = VPI.ChangeCurrentVP();
    Standard_Integer VPsi = VP.ShapeIndex();
    const gp_Pnt& P3D = VP.Value();
    
    Standard_Boolean VPequalVPONRESTRICTION = Standard_False;
    TopOpeBRep_FacesIntersector& FI = *((TopOpeBRep_FacesIntersector*)((void*)myFacesIntersector));
    Standard_Integer iOL = 1,n = FI.NbLines(); 
    for (iOL=1; iOL<=n; iOL++ ) { 
      if (iOL == Lindex ) continue; 
      TopOpeBRep_LineInter& OL = FI.ChangeLine(iOL);
      VPequalVPONRESTRICTION = PequalVPonR(P3D,VPsi,VP,OL);
      if ( VPequalVPONRESTRICTION ) break;
    }
    
    if ( !VPequalVPONRESTRICTION ) {
      VP_Position(VP,VPC);
    }    
  }
}

//=======================================================================
//function : VP_PositionOnR
//purpose  : 
//=======================================================================
void TopOpeBRep_FacesFiller::VP_PositionOnR(TopOpeBRep_LineInter& L)
{
  TopOpeBRep_VPointInterClassifier VPC;
  
  TopOpeBRep_VPointInterIterator VPI(L);
  Standard_Integer Esi   = (L.ArcIsEdge(1)) ? 1:2;
  Standard_Integer OOEsi = (L.ArcIsEdge(1)) ? 2:1;
  
  Standard_Boolean isline = Standard_False;
  const TopoDS_Edge& earc = TopoDS::Edge(L.Arc());
  Standard_Boolean hasc3d = FC2D_HasC3D(earc);
  if (hasc3d) isline = FUN_tool_line(earc);
  else {
    BRepAdaptor_Curve2d BAC2D;
    if      (Esi == 1)  BAC2D.Initialize(earc,myF1);
    else if (Esi == 2)  BAC2D.Initialize(earc,myF2);
    GeomAbs_CurveType t = BAC2D.GetType();
    isline = (t == GeomAbs_Line);
  }
  
  for (; VPI.More(); VPI.Next()) {
    TopOpeBRep_VPointInter& VP = VPI.ChangeCurrentVP();
    
    Standard_Boolean isvertex = VP.IsVertex(Esi);
    if ( isvertex ) {
      if (!isline) VP_Position(VP,VPC);
      continue;
    }
    Standard_Boolean OOisvertex = VP.IsVertex(OOEsi);
    if ( OOisvertex ) {
      if (!isline) VP_Position(VP,VPC);
      continue;
    }
    
    const gp_Pnt& P = VP.Value();
    Standard_Boolean arcisE = L.ArcIsEdge(Esi);
    Standard_Boolean arcisOOE = L.ArcIsEdge(OOEsi);
    
    if (arcisE) {
      Standard_Real paramC;Standard_Boolean paramCdef = FUN_projPonL(P,L,(*this),paramC);
      if ( paramCdef ) {
	const TopoDS_Edge& E = TopoDS::Edge(L.Arc());
	VP.State(TopAbs_ON,Esi);
	VP.EdgeON(E,paramC,Esi);
      }
      else {
//	throw Standard_ProgramError("VP_Position projection failed on E");
	VP.ChangeKeep(Standard_False); // xpu051198
      }
    }
    
    if (arcisOOE) {
      Standard_Real paramC;Standard_Boolean paramCdef = FUN_projPonL(P,L,(*this),paramC);
      if ( paramCdef ) {
	const TopoDS_Edge& OOE = TopoDS::Edge(L.Arc());
	VP.State(TopAbs_ON,OOEsi);
	VP.EdgeON(OOE,paramC,OOEsi);
      }
      else {
//	throw Standard_ProgramError("VP_Position projection failed on OOE");
	VP.ChangeKeep(Standard_False); // xpu051198
      }
    }
  }
}

//=======================================================================
//function : VP_Position
//purpose  : 
//=======================================================================
void TopOpeBRep_FacesFiller::VP_Position(TopOpeBRep_VPointInter& VP,TopOpeBRep_VPointInterClassifier& VPC)
{
  Standard_Integer si = VP.ShapeIndex();
  Standard_Boolean c1=Standard_False,c2=Standard_False;

  if      (si == 0) { c1 = Standard_True; c2 = Standard_True; }
  else if (si == 1) { c1 = Standard_False; c2 = Standard_True; }
  else if (si == 2) { c1 = Standard_True; c2 = Standard_False; }
  else if (si == 3) { c1 = Standard_True; c2 = Standard_True; }
  
  Standard_Boolean AssumeINON = Standard_False;
  if (myLine) AssumeINON = (myLine->TypeLineCurve() != TopOpeBRep_RESTRICTION);

  // modified by NIZHNY-MKK  Fri Oct 27 14:50:28 2000.BEGIN
  //   Standard_Real tol = Precision::Confusion();
  Standard_Real tol1, tol2;
  tol1 = tol2 = Precision::Confusion();
  myFacesIntersector->GetTolerances(tol1, tol2);
  Standard_Real tol = (tol1 > tol2) ? tol1 : tol2;
  // modified by NIZHNY-MKK  Fri Oct 27 14:50:36 2000.END

  if (c1) VPC.VPointPosition(myF1,VP,1,myPointClassifier,AssumeINON,tol);
  if (c2) VPC.VPointPosition(myF2,VP,2,myPointClassifier,AssumeINON,tol);
}

//=======================================================================
//function : PequalVPonR
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRep_FacesFiller::PequalVPonR(const gp_Pnt& P3D,const Standard_Integer VPsi,TopOpeBRep_VPointInter& VP,TopOpeBRep_LineInter& Lrest) const 
{
  const TopOpeBRep_TypeLineCurve tOL = Lrest.TypeLineCurve();
  Standard_Boolean OLok = (tOL == TopOpeBRep_RESTRICTION) ;
  if ( !OLok ) return Standard_False;
  
  Standard_Boolean VPequalVPONRESTRICTION = Standard_False;
  const TopoDS_Edge& EOL = TopoDS::Edge(Lrest.Arc());
  Standard_Integer EOLsi = (Lrest.ArcIsEdge(1)) ? 1:2;
  
  TopOpeBRep_VPointInterIterator VPIOL(Lrest);
  for (; VPIOL.More(); VPIOL.Next()) {
    TopOpeBRep_VPointInter& VPOL = VPIOL.ChangeCurrentVP();
    Standard_Integer VPOLsi = VPOL.ShapeIndex();
    
    Standard_Boolean VPOLisvertex = Standard_False;
    VPOLisvertex = VPOL.IsVertex(1);
    if (VPOLisvertex) continue;
    
    Standard_Boolean diffsi = (VPOLsi != VPsi);
    if ( diffsi ) continue;
    
    TopAbs_State stateEsi = VPOL.State(EOLsi);
    if (stateEsi != TopAbs_ON) continue; 
    
    const gp_Pnt& P3DOL = VPOL.Value();
    Standard_Real tolE = BRep_Tool::Tolerance(EOL);
    VPequalVPONRESTRICTION = P3DOL.IsEqual(P3D,tolE);
    
    if ( VPequalVPONRESTRICTION ) {
      Standard_Real paramCOL = VPOL.EdgeONParameter(EOLsi);
      VP.State(TopAbs_ON,EOLsi);
      VP.EdgeON(EOL,paramCOL,EOLsi);
      break;
    }
  }
  return VPequalVPONRESTRICTION;
}

//=======================================================================
//function : FacesIntersector
//purpose  : 
//=======================================================================
TopOpeBRep_FacesIntersector& TopOpeBRep_FacesFiller::ChangeFacesIntersector()
{
  return (*myFacesIntersector);
}

//=======================================================================
//function : HDataStructure
//purpose  : 
//=======================================================================
Handle(TopOpeBRepDS_HDataStructure) TopOpeBRep_FacesFiller::HDataStructure()
{
  return myHDS;
}

//=======================================================================
//function : DataStructure
//purpose  : 
//=======================================================================
TopOpeBRepDS_DataStructure& TopOpeBRep_FacesFiller::ChangeDataStructure()
{
  return (*myDS);
}

//=======================================================================
//function : Face
//purpose  : 
//=======================================================================
const TopoDS_Face& TopOpeBRep_FacesFiller::Face(const Standard_Integer I) const
{
  if      (I == 1) return myF1;
  else if (I == 2) return myF2;
  throw Standard_ProgramError("FacesFiller::Face");
}

//=======================================================================
//function : FaceFaceTransition
//purpose  : 
//=======================================================================
const TopOpeBRepDS_Transition& TopOpeBRep_FacesFiller::FaceFaceTransition(const TopOpeBRep_LineInter& L,const Standard_Integer I) const
{
  const TopOpeBRepDS_Transition& T = L.FaceFaceTransition(I);
  return T;
}

//=======================================================================
//function : FaceFaceTransition
//purpose  : 
//=======================================================================
const TopOpeBRepDS_Transition& TopOpeBRep_FacesFiller::FaceFaceTransition(const Standard_Integer I) const
{
  const TopOpeBRepDS_Transition& T = myLine->FaceFaceTransition(I);
  return T;
}

TopOpeBRep_PFacesIntersector TopOpeBRep_FacesFiller::PFacesIntersectorDummy() const 
{return myFacesIntersector;}
TopOpeBRepDS_PDataStructure TopOpeBRep_FacesFiller::PDataStructureDummy() const 
{return myDS;}
TopOpeBRep_PLineInter TopOpeBRep_FacesFiller::PLineInterDummy() const 
{return myLine;}

//=======================================================================
//function : SetTraceIndex
//purpose  : 
//=======================================================================
void TopOpeBRep_FacesFiller::SetTraceIndex(const Standard_Integer exF1,const Standard_Integer exF2)
{ 
  myexF1 = exF1;
  myexF2 = exF2;
}

//=======================================================================
//function : GetTraceIndex
//purpose  : 
//=======================================================================
void TopOpeBRep_FacesFiller::GetTraceIndex(Standard_Integer& exF1,Standard_Integer& exF2)const 
{ 
  exF1 = myexF1;
  exF2 = myexF2;
}
