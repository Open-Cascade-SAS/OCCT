// Created on: 1994-10-12
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
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRep_define.hxx>
#include <TopOpeBRep_EdgesFiller.hxx>
#include <TopOpeBRep_EdgesIntersector.hxx>
#include <TopOpeBRep_Point2d.hxx>
#include <TopOpeBRep_PointGeomTool.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_Config.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_InterferenceTool.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS_TKI.hxx>
#include <TopOpeBRepDS_Transition.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>

#ifdef OCCT_DEBUG
#include <TopOpeBRepDS_CurvePointInterference.hxx>
extern Standard_Boolean TopOpeBRep_GettraceEEFF();
Standard_EXPORT void debposesd(void) {/*cout<<"+++ debposesd"<<endl;*/}
Standard_EXPORT void debposnesd(void) {cout<<"+++ debposnesd"<<endl;}
Standard_EXPORT void debeeff() {}
#endif

#define M_REVERSED(O) (O == TopAbs_REVERSED)
#define M_FORWARD( O) (O == TopAbs_FORWARD)
#define M_INTERNAL(O) (O == TopAbs_INTERNAL)
#define M_EXTERNAL(O) (O == TopAbs_EXTERNAL)

//=======================================================================
//function : TopOpeBRep_EdgesFiller
//purpose  : 
//=======================================================================
TopOpeBRep_EdgesFiller::TopOpeBRep_EdgesFiller() : myPDS(NULL),myPEI(NULL) {}

void rototo() {}

//=======================================================================
//function : Insert
//purpose  : 
//=======================================================================
void TopOpeBRep_EdgesFiller::Insert(const TopoDS_Shape& E1,const TopoDS_Shape& E2,TopOpeBRep_EdgesIntersector& EDGINT,const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  myPEI = &EDGINT;
  myPDS = &(HDS->ChangeDS());
  myE1 = TopoDS::Edge(E1);
  myE2 = TopoDS::Edge(E2);
  myLI1.Clear();
  myLI2.Clear();
  myHDS = HDS;

  Standard_Boolean esd = myPEI->SameDomain();
  if (esd) myPDS->FillShapesSameDomain(E1,E2);
  
  // exit if no point.
  myPEI->InitPoint(); if ( !myPEI->MorePoint() ) return;

  // --- Add <E1,E2> in BDS
  Standard_Integer E1index = myPDS->AddShape(E1,1);
  Standard_Integer E2index = myPDS->AddShape(E2,2);
  
  // --- get list of interferences connected to edges <E1>,<E2>
  TopOpeBRepDS_ListOfInterference& EIL1 = myPDS->ChangeShapeInterferences(E1);
  
  Handle(TopOpeBRepDS_Interference) EPI;  //edge/point interference
  Handle(TopOpeBRepDS_Interference) EVI;  //edge/vertex interference
  
//  TopOpeBRepDS_Transition TposF,TposL;

  for (; myPEI->MorePoint(); myPEI->NextPoint() ) {
    const TopOpeBRep_Point2d P2D = myPEI->Point();
    Standard_Real par1 = P2D.Parameter(1);
    Standard_Real par2 = P2D.Parameter(2);
    if ( ! myF1.IsNull() ) myPDS->AddShape(myF1,1);
    if ( ! myF2.IsNull() ) myPDS->AddShape(myF2,2);

    TopOpeBRepDS_Transition T1 = P2D.Transition(1);
    TopOpeBRepDS_Transition T2 = P2D.Transition(2);
    
    SetShapeTransition(P2D,T1,T2);
    
    Standard_Boolean isvertex1 = P2D.IsVertex(1);
    TopoDS_Vertex V1; if (isvertex1) V1 = P2D.Vertex(1);
    Standard_Boolean isvertex2 = P2D.IsVertex(2);
    TopoDS_Vertex V2; if (isvertex2) V2 = P2D.Vertex(2);
    Standard_Boolean isvertex = isvertex1 || isvertex2;

#ifdef OCCT_DEBUG
    if (isvertex1 && isvertex2) {
      gp_Pnt P3D1 = BRep_Tool::Pnt(V1);
      gp_Pnt P3D2 = BRep_Tool::Pnt(V2);
      Standard_Real tol1 = BRep_Tool::Tolerance(V1);
      Standard_Real tol2 = BRep_Tool::Tolerance(V2);
      Standard_Real dpp = P3D1.Distance(P3D2);
      if (dpp> tol1+tol2) {
	cout<<endl;
	cout<<"*** TopOpeBRep_EdgesFiller : isvertex1 && isvertex2 : P3D non confondus"<<endl;
	cout<<"point PV1 "<<P3D1.X()<<" "<<P3D1.Y()<<" "<<P3D1.Z()<<endl;
	cout<<"point PV2 "<<P3D2.X()<<" "<<P3D2.Y()<<" "<<P3D2.Z()<<endl;
	cout<<endl;
      }
    }
#endif
    
    // xpu : 080498 : CTS20072 (e12,e3,p8) 
    //       edgesintersector called for tolerances = 0.
    //       facesintersector called for greater tolerances
    //       we assume facesintersector's ouput data to be valid
    //       and we use it for correcting edgesintersector's ouput data
    TopOpeBRepDS_ListIteratorOfListOfInterference itloI1( myPDS->ShapeInterferences(E1) );
    Standard_Integer G; TopOpeBRepDS_Kind K; 
    Standard_Boolean found = GetGeometry(itloI1,P2D,G,K);
    if (!found) MakeGeometry(P2D,G,K);

    Standard_Boolean foundpoint  = (found)  && (K == TopOpeBRepDS_POINT);
    Standard_Boolean isnewpoint  = (!found) && (K == TopOpeBRepDS_POINT);
    Standard_Boolean isnewvertex = (!found) && (K == TopOpeBRepDS_VERTEX);

    Standard_Boolean faulty =  (isvertex && isnewpoint) || (!isvertex && isnewvertex);
    if (faulty) {
#ifdef OCCT_DEBUG
      Standard_Boolean foundvertex = (found)  && (K == TopOpeBRepDS_VERTEX);
      cout<<"- - - faulty EdgesFiller : G "<<G<<" K ";TopOpeBRepDS::Print(K,cout);cout.flush();
      cout<<" isvertex="<<isvertex;cout.flush();
      cout<<" isop="<<foundpoint<<" isov="<<foundvertex;cout.flush();
      cout<<" isnp="<<isnewpoint<<" isnv="<<isnewvertex<<endl;cout.flush();
#endif
    }

    if (isvertex && foundpoint) {
      Standard_Integer is = 1, ns = myPDS->NbShapes();
      for (;is<=ns;is++) {
	const TopoDS_Shape& s = myPDS->Shape(is);
	if (s.ShapeType() != TopAbs_EDGE) continue;
	const TopoDS_Edge& e = TopoDS::Edge(s);

	TopOpeBRepDS_ListOfInterference linew;
	TopOpeBRepDS_ListOfInterference& li = myPDS->ChangeShapeInterferences(e); TopOpeBRepDS_ListIteratorOfListOfInterference it(li);
	while (it.More()) {

	  Handle(TopOpeBRepDS_Interference) I=it.Value(); TopOpeBRepDS_Kind ki=I->GeometryType(); Standard_Integer gi=I->Geometry();
	  Handle(Standard_Type) DTI = I->DynamicType();
	  Standard_Boolean iscpi = (DTI == STANDARD_TYPE(TopOpeBRepDS_CurvePointInterference)) ;
	  Standard_Boolean condcpi = ((ki==TopOpeBRepDS_POINT) && (gi==G) && iscpi);
	  if (condcpi) { // remplacer G,K de I par le vertex courant

#ifdef OCCT_DEBUG
	    rototo();
#endif
	    Handle(TopOpeBRepDS_CurvePointInterference) epi = Handle(TopOpeBRepDS_CurvePointInterference)::DownCast(I);
	    const TopOpeBRepDS_Transition& tevi = epi->Transition();
	    Standard_Integer sevi = epi->Support();

	    Standard_Integer gevi=0;

	    if      (isvertex1) gevi = myPDS->AddShape(V1,1);
	    else if (isvertex2) gevi = myPDS->AddShape(V2,2);
	    Standard_Boolean bevi = Standard_False;
	    TopOpeBRepDS_Config cevi = TopOpeBRepDS_UNSHGEOMETRY;
	    Standard_Real pevi = epi->Parameter();

	    Handle(TopOpeBRepDS_Interference) evi;
	    evi = TopOpeBRepDS_InterferenceTool::MakeEdgeVertexInterference(tevi,sevi,gevi,bevi,cevi,pevi);
	    const TopOpeBRepDS_Kind& kevi = epi->SupportType();
	    evi->SupportType(kevi);

#ifdef OCCT_DEBUG
	    TopOpeBRepDS::Print(K,G,cout,"TopOpeBRep_EdgesFiller : remplacer "," ");
	    TopOpeBRepDS::Print(TopOpeBRepDS_VERTEX,gevi,cout,"par "," dans les courbes NYI\n");
#endif
	    linew.Append(evi);
	    li.Remove(it);
	  } // cond
	  else {
	    it.Next();
	  }
	} // it.More()
	if (!linew.IsEmpty()) {
	  myHDS->StoreInterferences(linew,is,"EdgesFiller modif : ");
	}
      } // (is<=ns)
    } // (isvertex && foundpoint)
    
    if (isvertex1) {
      const TopoDS_Vertex& VV1 = V1;
//      const TopoDS_Vertex& VV1 = TopoDS::Vertex(V1);
      const TopoDS_Edge& EE1 = TopoDS::Edge(E1);
      par1 = BRep_Tool::Parameter(VV1,EE1);
    }
    
    if (isvertex2) {
      const TopoDS_Vertex& VV2 = V2;
//      const TopoDS_Vertex& VV2 = TopoDS::Vertex(V2);
      const TopoDS_Edge& EE2 = TopoDS::Edge(E2);
      par2 = BRep_Tool::Parameter(VV2,EE2);
    }
    
    if ( isvertex1 && isvertex2 ) {
      myPDS->FillShapesSameDomain(V1,V2);
    }
    
    Standard_Integer DSPindex;
    Standard_Boolean EPIfound;
    
    if ( ! isvertex ) {
      
      TopOpeBRepDS_Kind KKK;
      TopOpeBRepDS_ListIteratorOfListOfInterference itEIL1(EIL1);
      EPIfound = GetGeometry(itEIL1,P2D,DSPindex,KKK);
      if ( ! EPIfound ) MakeGeometry(P2D,DSPindex,KKK);

      SetShapeTransition(P2D,T1,T2);

      if      (KKK == TopOpeBRepDS_POINT) {
	EPI = StorePI(P2D,T1,E2index,DSPindex,par1,1);
	EPI = StorePI(P2D,T2,E1index,DSPindex,par2,2);
      }
      else if ( KKK == TopOpeBRepDS_VERTEX) {
	Standard_Integer Vindex = DSPindex;
	Standard_Boolean bevi = Standard_False;
	TopOpeBRepDS_Config cevi = TopOpeBRepDS_UNSHGEOMETRY;
	EVI = StoreVI(P2D,T1,E2index,Vindex,bevi,cevi,par1,1);
	EVI = StoreVI(P2D,T2,E1index,Vindex,bevi,cevi,par2,2);
      }
 
    } // ( ! isvertex )
    
    else {
      
      SetShapeTransition(P2D,T1,T2);
      
      if (isvertex1) {
	const TopoDS_Shape V = V1;
	Standard_Integer Vindex = myPDS->AddShape(V,1);
	TopOpeBRepDS_Config SSC = P2D.EdgesConfig();
	EVI = StoreVI(P2D,T1,E2index,Vindex,Standard_True,SSC,par1,1);
	EVI = StoreVI(P2D,T2,E1index,Vindex,Standard_False,SSC,par2,2);
      }
      
      if (isvertex2) {
	const TopoDS_Shape V = V2;
	Standard_Integer Vindex = myPDS->AddShape(V,2);
	TopOpeBRepDS_Config SSC = P2D.EdgesConfig();
	EVI = StoreVI(P2D,T1,E2index,Vindex,Standard_False,SSC,par1,1);
	EVI = StoreVI(P2D,T2,E1index,Vindex,Standard_True,SSC,par2,2);
      }
      
    } // ( isvertex )
    
  } //  MorePoint()
  
  RecomputeInterferences(myE1,myLI1);
  RecomputeInterferences(myE2,myLI2);
  
} // Insert

// ===============
// private methods
// ===============

//=======================================================================
//function : SetShapeTransition
//purpose  : 
//=======================================================================
void TopOpeBRep_EdgesFiller::SetShapeTransition(const TopOpeBRep_Point2d& P2D,
			       TopOpeBRepDS_Transition& T1,TopOpeBRepDS_Transition& T2) const
{
  Standard_Boolean pointofsegment = P2D.IsPointOfSegment();
  Standard_Boolean esd = myPEI->SameDomain();
  Standard_Integer ie1=0,ie2=0,if1=0,if2=0;
  
  if      (pointofsegment && esd) {
    T1.ShapeBefore(TopAbs_EDGE);T1.ShapeAfter(TopAbs_EDGE);
    T2.ShapeBefore(TopAbs_EDGE);T2.ShapeAfter(TopAbs_EDGE);
    if ( ! myE1.IsNull() ) ie1 = myPDS->AddShape(myE1,1);
    if ( ! myE2.IsNull() ) ie2 = myPDS->AddShape(myE2,2);
    if ( ! myE2.IsNull() ) T1.Index(ie2);
    if ( ! myE1.IsNull() ) T2.Index(ie1);
  }
  else {
    T1.ShapeBefore(TopAbs_FACE);T1.ShapeAfter(TopAbs_FACE);
    T2.ShapeBefore(TopAbs_FACE);T2.ShapeAfter(TopAbs_FACE);
    if ( ! myF1.IsNull() ) if1 = myPDS->AddShape(myF1,1);
    if ( ! myF2.IsNull() ) if2 = myPDS->AddShape(myF2,2);
    if ( ! myF1.IsNull() ) T2.Index(if1);
    if ( ! myF2.IsNull() ) T1.Index(if2);
  }
}

//=======================================================================
//function : GetGeometry
//purpose  : private
//=======================================================================
Standard_Boolean TopOpeBRep_EdgesFiller::GetGeometry(TopOpeBRepDS_ListIteratorOfListOfInterference& IT,const TopOpeBRep_Point2d& P2D,Standard_Integer& G,TopOpeBRepDS_Kind& K) const 
		       
{
  TopOpeBRepDS_Point DSP = TopOpeBRep_PointGeomTool::MakePoint(P2D);
  Standard_Boolean b = myHDS->GetGeometry(IT,DSP,G,K);
  return b;
}

//=======================================================================
//function : MakeGeometry
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRep_EdgesFiller::MakeGeometry(const TopOpeBRep_Point2d& P2D,Standard_Integer& G,TopOpeBRepDS_Kind& K) const 
{
  Standard_Boolean isvertex1 = P2D.IsVertex(1);
  Standard_Boolean isvertex2 = P2D.IsVertex(2);
  if (isvertex1 && isvertex2) {
    Standard_Integer G1 = myPDS->AddShape(P2D.Vertex(1),1);
    myPDS->AddShape(P2D.Vertex(2),2);
    G = G1;
    K = TopOpeBRepDS_VERTEX;
  }
  else if (isvertex1) {
    G = myPDS->AddShape(P2D.Vertex(1),1);
    K = TopOpeBRepDS_VERTEX;
  }
  else if (isvertex2) {
    G = myPDS->AddShape(P2D.Vertex(2),2);
    K = TopOpeBRepDS_VERTEX;
  }
  else {
    G = myPDS->AddPoint(TopOpeBRep_PointGeomTool::MakePoint(P2D));
    K = TopOpeBRepDS_POINT;
  }
  return Standard_True;
}

//=======================================================================
//function : Face
//purpose  : 
//=======================================================================
void TopOpeBRep_EdgesFiller::Face(const Standard_Integer ISI,const TopoDS_Shape& F)
{
  if      (ISI == 1) myF1 = TopoDS::Face(F);
  else if (ISI == 2) myF2 = TopoDS::Face(F);
  else throw Standard_Failure("Face(i,f) : ISI incorrect");
}

//=======================================================================
//function : Face
//purpose  : 
//=======================================================================
const TopoDS_Shape& TopOpeBRep_EdgesFiller::Face(const Standard_Integer ISI) const
{
  if      (ISI == 1) return myF1;
  else if (ISI == 2) return myF2;
  else throw Standard_Failure("Face(i) : ISI incorrect");
}

//=======================================================================
//function : StorePI
//purpose  : 
//=======================================================================
Handle(TopOpeBRepDS_Interference) TopOpeBRep_EdgesFiller::StorePI(const TopOpeBRep_Point2d& P2D,
		     const TopOpeBRepDS_Transition& T,const Standard_Integer SI,const Standard_Integer GI,
		     const Standard_Real param,const Standard_Integer IEmother)
{
  Handle(TopOpeBRepDS_Interference) I = TopOpeBRepDS_InterferenceTool::MakeEdgeInterference(T,TopOpeBRepDS_EDGE,SI,TopOpeBRepDS_POINT,GI,param);
  TopoDS_Shape Emother;
  if      (IEmother == 1) Emother = myE1;
  else if (IEmother == 2) Emother = myE2;
  myHDS->StoreInterference(I,Emother);
  Standard_Boolean b = ToRecompute(P2D,I,IEmother);
  if (b) StoreRecompute(I,IEmother);
  return I;
}

//=======================================================================
//function : StoreVI
//purpose  : 
//=======================================================================
Handle(TopOpeBRepDS_Interference) TopOpeBRep_EdgesFiller::StoreVI(const TopOpeBRep_Point2d& P2D,
		     const TopOpeBRepDS_Transition& T,const Standard_Integer EI,const Standard_Integer VI,
		     const Standard_Boolean VisB,const TopOpeBRepDS_Config C,
		     const Standard_Real param,const Standard_Integer IEmother)
{
  Handle(TopOpeBRepDS_Interference) I = TopOpeBRepDS_InterferenceTool::MakeEdgeVertexInterference(T,EI,VI,VisB,C,param);
  TopoDS_Shape Emother;
  if      (IEmother == 1) Emother = myE1;
  else if (IEmother == 2) Emother = myE2;
  myHDS->StoreInterference(I,Emother);
  Standard_Boolean b = ToRecompute(P2D,I,IEmother);
  if (b) StoreRecompute(I,IEmother);
  return I;
}

//=======================================================================
//function : ToRecompute
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRep_EdgesFiller::ToRecompute(const TopOpeBRep_Point2d& P2D,const Handle(TopOpeBRepDS_Interference)& /*I*/,const Standard_Integer /*IEmother*/)
{
  Standard_Boolean b = Standard_True;
  Standard_Boolean pointofsegment = P2D.IsPointOfSegment();
  Standard_Boolean esd = myPEI->SameDomain();
  b = b && (pointofsegment && !esd);
  return b;
}

//=======================================================================
//function : StoreRecompute
//purpose  : 
//=======================================================================
void TopOpeBRep_EdgesFiller::StoreRecompute(const Handle(TopOpeBRepDS_Interference)& I,const Standard_Integer IEmother)
{
  if      (IEmother == 1) myLI1.Append(I);
  else if (IEmother == 2) myLI2.Append(I);
}

//=======================================================================
//function : RecomputeInterferences
//purpose  : 
//=======================================================================
void TopOpeBRep_EdgesFiller::RecomputeInterferences(const TopoDS_Edge& E,TopOpeBRepDS_ListOfInterference& LI)
{
  if (LI.IsEmpty()) return;

  TopOpeBRepDS_TKI tki; tki.FillOnGeometry(LI);

  for (tki.Init(); tki.More(); tki.Next()) {
    TopOpeBRepDS_Kind K; Standard_Integer G; tki.Value(K,G);
    TopOpeBRepDS_ListOfInterference& loi = tki.ChangeValue(K,G); TopOpeBRepDS_ListOfInterference Rloi;
    Standard_Integer nloi = loi.Extent();
    if (nloi == 0) continue;

    Handle(TopOpeBRepDS_Interference)& iloi = loi.First(); 
    TopOpeBRepDS_Transition& TU = iloi->ChangeTransition();
    Standard_Integer ifb = TU.IndexBefore();
    const TopoDS_Face& fb = TopoDS::Face(myPDS->Shape(ifb));

    Standard_Real pE = FDS_Parameter(iloi); TopOpeBRepDS_Transition TN;
    TN.ShapeBefore(TU.ShapeBefore());TN.IndexBefore(TU.IndexBefore());
    TN.ShapeAfter(TU.ShapeAfter());TN.IndexAfter(TU.IndexAfter());

    FDS_stateEwithF2d(*myPDS,E,pE,K,G,fb,TN);

  } // tki.More
} // RecomputeInterferences
