// Created on: 1993-10-15
// Created by: Remi LEQUETTE
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

#define TRC 0 
#define MODIF 1 


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgo_BooleanOperation.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepCheck.hxx>
#include <BRepCheck_Edge.hxx>
#include <BRepCheck_Shell.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepLib.hxx>
#include <BRepTools_Substitution.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRep_DSFiller.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepBuild_Tools.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepTool_GeomTool.hxx>
#include <TopOpeBRepTool_OutCurveType.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

//  sewing
#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepTool_GetcontextNOSEW();
#endif

#define Opecom(st1,st2) (((st1)==TopAbs_IN) && ((st2)==TopAbs_IN))
#define Opefus(st1,st2) (((st1)==TopAbs_OUT) && ((st2)==TopAbs_OUT))
#define Opecut(st1,st2) (((st1)==TopAbs_OUT) && ((st2)==TopAbs_IN))

// -------------------------------------------------------------------
static void Sub_Classify(TopExp_Explorer& Ex,
			 const TopAbs_State St1,
			 TopTools_ListOfShape& Solids2,
			 BRep_Builder& BB,
			 TopTools_ListIteratorOfListOfShape& LIter,
			 TopoDS_Shape& myShape); 


#ifdef OCCT_DEBUG
Standard_IMPORT Standard_Integer TopOpeBRepTool_BOOOPE_CHECK_DEB;
#endif

//modified by NIZHNY-MZV  Wed Apr 19 17:19:11 2000
//see comments at the top of file TopOpeBRepBuild_Builder1.cxx
//about using of this global variable
extern Standard_Boolean GLOBAL_USE_NEW_BUILDER;
//
//modified by NIZNHY-PKV Sun Dec 15 17:17:56 2002 f
extern void FDSCNX_Close();// see TopOpeBRepDS_connex.cxx
extern void FDSSDM_Close();// see TopOpeBRepDS_samdom.cxx

//=======================================================================
//function : ~BRepAlgo_BooleanOperation
//purpose  : 
//=======================================================================
BRepAlgo_BooleanOperation::~BRepAlgo_BooleanOperation()
{
  FDSSDM_Close();
  FDSCNX_Close();
}
//modified by NIZNHY-PKV Sun Dec 15 17:17:58 2002 t

//=======================================================================
//function : BRepAlgoAPI_BooleanOperation
//purpose  : 
//=======================================================================
  BRepAlgo_BooleanOperation::BRepAlgo_BooleanOperation(const TopoDS_Shape& S1, 
						       const TopoDS_Shape& S2)
: myS1(S1),myS2(S2),myBuilderCanWork(Standard_False)
{
  TopOpeBRepDS_BuildTool BT;
  myHBuilder = new TopOpeBRepBuild_HBuilder(BT);
}

//=======================================================================
//function : PerformDS
//purpose  : 
//=======================================================================
  void BRepAlgo_BooleanOperation::PerformDS()
{
//  const Standard_Boolean CheckShapes = Standard_True;

  // create a data structure
  Handle(TopOpeBRepDS_HDataStructure) HDS;
  if (myHBuilder->DataStructure().IsNull())
    HDS = new TopOpeBRepDS_HDataStructure();
  else {
    HDS = myHBuilder->DataStructure();
    HDS->ChangeDS().Init();
  }

  // fill the data Structure
  TopOpeBRep_DSFiller DSFiller;
 
  // define face/face intersection tolerances
  Standard_Boolean forcetoli = Standard_False;
  if (forcetoli) {
    Standard_Real tolarc=0,toltang=0;
    TopOpeBRep_ShapeIntersector& tobsi = DSFiller.ChangeShapeIntersector();
    TopOpeBRep_FacesIntersector& tobfi = tobsi.ChangeFacesIntersector();
    tobfi.ForceTolerances(tolarc,toltang);
  }
  DSFiller.Insert(myS1,myS2,HDS);

  // 020499 : JYL : reject if there is an edge of the SD
  // not coded sameparameter and not degenerated
  Standard_Boolean esp = HDS->EdgesSameParameter();
  Standard_Boolean tede = Standard_True;
  if (!esp) {
    Standard_Integer i,n = HDS->NbShapes();
    for (i = 1 ; i <= n; i++) {
      const TopoDS_Shape& s = HDS->Shape(i);
      if ( s.ShapeType() == TopAbs_EDGE ) {
	const TopoDS_Edge& e = TopoDS::Edge(s);
	Standard_Boolean sp = BRep_Tool::SameParameter(e);
	Standard_Boolean de = BRep_Tool::Degenerated(e);
	if ( !sp && !de ) {
	  tede = Standard_False;
	  break;
	}
      }
    }
  }
  myBuilderCanWork = (esp || tede) ;
#ifdef OCCT_DEBUG
  if (!esp) cout<<"BRepAlgo_BooleanOperation(DEB) some edges not SameParameter"<<endl;
#endif  
  if (!myBuilderCanWork) return;
  
  Standard_Real tol3dAPPROX = 1e-7;
  Standard_Real tol2dAPPROX = 1e-7;
  // set tolerance values used by the APPROX process
  
  TopOpeBRepDS_BuildTool& BTofBuilder = myHBuilder->ChangeBuildTool();
  TopOpeBRepTool_GeomTool& GTofBTofBuilder = BTofBuilder.ChangeGeomTool();
  GTofBTofBuilder.SetTolerances(tol3dAPPROX,tol2dAPPROX);
  
  //modified by NIZHNY-MZV  Thu Apr 20 09:35:44 2000
  //see comments at the top of file TopOpeBRepBuild_Builder1.cxx
  //about using of this global variable
  GLOBAL_USE_NEW_BUILDER = Standard_True;
  myHBuilder->Perform(HDS,myS1,myS2);
  GLOBAL_USE_NEW_BUILDER = Standard_False;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
  void  BRepAlgo_BooleanOperation::Perform(const TopAbs_State St1, 
					   const TopAbs_State St2)
{
  if ( ! BuilderCanWork() ) {
    return;
  }

  // modif JYL suite aux modifs LBR #if MODIF ...
  // on privilegie le traitement KPart (si c'en est un) 
  // a tous les autres
  Standard_Integer kp = myHBuilder->IsKPart();
  BRep_Builder BB;
  Standard_Boolean sewing = Standard_True;
  if ( kp ) {
    //modified by NIZHNY-MZV  Thu Apr 20 09:34:33 2000
    //see comments at the top of file TopOpeBRepBuild_Builder1.cxx
    //about using of this global variable
    GLOBAL_USE_NEW_BUILDER = Standard_True;
    myHBuilder->MergeKPart(St1,St2);
    GLOBAL_USE_NEW_BUILDER = Standard_False;

    BB.MakeCompound(TopoDS::Compound(myShape));
    Done();

    TopTools_ListIteratorOfListOfShape its(myHBuilder->Merged(myS1,St1));
    for(; its.More(); its.Next()) BB.Add(myShape,its.Value());

  }
  else {
#if MODIF 

    //======================================================================
    //== Exploration of input shapes 
    //== Creation of the list of solids 
    //== Creation of the list of faces OUT OF solid
    //== Creation of the list of edges OUT OF face
    Standard_Integer nbs1,nbs2,nbf1,nbf2,nbe1,nbe2,nbv1,nbv2;

    TopTools_ListOfShape Solids1,Solids2,Faces1,Faces2,Edges1,Edges2,Vertex1,Vertex2;
    TopExp_Explorer Ex;
    for(Ex.Init(myS1,TopAbs_SOLID),nbs1=0; Ex.More(); Ex.Next()) {
      Solids1.Append(Ex.Current()); nbs1++;
    }
    for(Ex.Init(myS2,TopAbs_SOLID),nbs2=0; Ex.More(); Ex.Next()) { 
      Solids2.Append(Ex.Current()); nbs2++;
    }
    //== Faces not  in a solid
    for(Ex.Init(myS1,TopAbs_FACE,TopAbs_SOLID),nbf1=0; Ex.More(); Ex.Next())  { 
      Faces1.Append(Ex.Current()); nbf1++;
    }
    for(Ex.Init(myS2,TopAbs_FACE,TopAbs_SOLID),nbf2=0; Ex.More(); Ex.Next())  {
      Faces2.Append(Ex.Current()); nbf2++;
    }
    //== Edges not in a solid
    for(Ex.Init(myS1,TopAbs_EDGE,TopAbs_FACE),nbe1=0;  Ex.More(); Ex.Next())  { 
      Edges1.Append(Ex.Current()); nbe1++;
    }
    for(Ex.Init(myS2,TopAbs_EDGE,TopAbs_FACE),nbe2=0;  Ex.More(); Ex.Next()) {
      Edges2.Append(Ex.Current()); nbe2++;
    }
    //== Vertices not in an edge
    for(Ex.Init(myS1,TopAbs_VERTEX,TopAbs_EDGE),nbv1=0;  Ex.More(); Ex.Next())  { 
      Vertex1.Append(Ex.Current()); nbv1++;
    }
    for(Ex.Init(myS2,TopAbs_VERTEX,TopAbs_EDGE),nbv2=0;  Ex.More(); Ex.Next()) {
      Vertex2.Append(Ex.Current()); nbv2++;
    }

    //-- cout<<"Solids1: "<<nbs1<<"  Faces1: "<<nbf1<<" Edges1:"<<nbe1<<" Vtx1:"<<nbv1<<endl;
    //-- cout<<"Solids2: "<<nbs2<<"  Faces2: "<<nbf2<<" Edges2:"<<nbe2<<" Vtx2:"<<nbv2<<endl;

    //== 

    //== Reject operations without direction 
  

    //-- Cut Solid by Edge 
//    Standard_Boolean Correct = Standard_True;
    if(    (nbs1 && nbs2==0 && St1==TopAbs_OUT && St2==TopAbs_IN) 
       ||  (nbs2 && nbs1==0 && St2==TopAbs_OUT && St1==TopAbs_IN)) { 
      //-- cout<<"*****  Invalid Operation : Cut of a Solid by a Non Solid "<<endl;
      Done();
      return;
    }

    if(    (nbs1 && nbs2==0 && St1==TopAbs_OUT && St2==TopAbs_OUT) 
       ||  (nbs2 && nbs1==0 && St2==TopAbs_OUT && St1==TopAbs_OUT)) { 
      //-- cout<<"*****  Invalid Operation : Fusion of a Solid and a Non Solid "<<endl;
      Done();
      return;
    }
 

    if(    (nbs1>0 && nbs2>0) 
       &&  (nbe1 || nbe2 || nbf1 || nbf2 || nbv1 || nbv2)) { 
      //-- cout<<"***** Not Yet Implemented : Compound of solid and non Solid"<<endl;
      Done();
      return;
    }
    //======================================================================
    // make a compound with the new solids
    BB.MakeCompound(TopoDS::Compound(myShape));
    
    TopTools_ListIteratorOfListOfShape LIter;
    //----------------------------------------------------------------------
    TopoDS_Shape SNULL;
    
    if (nbf1 && nbf2) {
      SNULL.Nullify();
      if ( Opecom(St1,St2) ) {
	TopTools_ListIteratorOfListOfShape itloe = myHBuilder->Section();
	for(; itloe.More(); itloe.Next()) BB.Add(myShape,itloe.Value());
      } 
      else {
	if(nbf1) { 
	  myHBuilder->MergeShapes(myS1,St1,SNULL,St2);

	  for(LIter.Initialize(Faces1);LIter.More();LIter.Next()) {
	    if (myHBuilder->IsSplit(LIter.Value(),St1)) {
	      TopTools_ListIteratorOfListOfShape its;
	      for(its.Initialize(myHBuilder->Splits(LIter.Value(),St1));
		  its.More();its.Next()) BB.Add(myShape,its.Value());
	    }
	    else {
	      const TopoDS_Shape& LV = LIter.Value();
	      if(  (LV.Orientation() == TopAbs_EXTERNAL && St1==TopAbs_OUT ) 	    
		 ||(LV.Orientation() == TopAbs_INTERNAL && St1==TopAbs_IN  )) {
		BB.Add(myShape,LV);
	      }
	      else { 
		//-- Classify : 
		Sub_Classify(Ex,St1,Solids2,BB,LIter,myShape); 
	      }
	      //-- End Classification 
	    }
	  }
	} // nbf1
	SNULL.Nullify();    
	if ( Opefus(St1,St2) ) {
	  if(nbf2) {
	    myHBuilder->MergeShapes(SNULL,St1,myS2,St2);
	    for(LIter.Initialize(Faces2);LIter.More();LIter.Next()) {
	      if (myHBuilder->IsSplit(LIter.Value(),St2)) {
		TopTools_ListIteratorOfListOfShape its;
		for(its.Initialize(myHBuilder->Splits(LIter.Value(),St2));
		    its.More();its.Next()) BB.Add(myShape,its.Value());
	      }
	      else {
		const TopoDS_Shape& LV = LIter.Value();
		if(  (LV.Orientation() == TopAbs_EXTERNAL && St2==TopAbs_OUT ) 	    
		   ||(LV.Orientation() == TopAbs_INTERNAL && St2==TopAbs_IN  )) {
		  BB.Add(myShape,LV);
		}
		else { 
		  //-- Classify : 
		  Sub_Classify(Ex,St2,Solids1,BB,LIter,myShape); 
		}
		//-- End Classification 	
	      }
	    }
	  } // nbf2
	} // Fus
      }
    } // nbf1 && nbf2
    else if (nbf1 || nbf2) {
      SNULL.Nullify();
      if(nbf1) { 
	myHBuilder->MergeShapes(myS1,St1,SNULL,St2);
	// modified by IFV for treating operation between shell and solid
	const TopTools_ListOfShape& MergedShapes = myHBuilder->Merged(myS1,St1);
	TopTools_IndexedMapOfShape aMapOfFaces;

	sewing = Standard_False;

	if(MergedShapes.Extent() != 0) {
	  TopTools_ListIteratorOfListOfShape its(MergedShapes);
	  for(; its.More(); its.Next()) {
	    BB.Add(myShape,its.Value());
	  }
	  TopExp::MapShapes(myShape, TopAbs_FACE, aMapOfFaces);
	}

	for(LIter.Initialize(Faces1);LIter.More();LIter.Next()) {

	  if (myHBuilder->IsSplit(LIter.Value(),St1)) {
	    TopTools_ListIteratorOfListOfShape its;
	    for(its.Initialize(myHBuilder->Splits(LIter.Value(),St1));
		its.More();its.Next()) {
	      if(!aMapOfFaces.Contains(its.Value())) BB.Add(myShape,its.Value());
	    }
	  }
	  else {
	    const TopoDS_Shape& LV = LIter.Value();
	    if(!aMapOfFaces.Contains(LV)) {
	      if(  (LV.Orientation() == TopAbs_EXTERNAL && St1==TopAbs_OUT ) 	    
		 ||(LV.Orientation() == TopAbs_INTERNAL && St1==TopAbs_IN  )) {
		BB.Add(myShape,LV);
	      }
	      else { 
		//-- Classify : 
		Sub_Classify(Ex,St1,Solids2,BB,LIter,myShape); 
	      }
	      //-- End Classification 
	    }
	  }
	}
      } // nbf1
      SNULL.Nullify();    
      if(nbf2) {
	myHBuilder->MergeShapes(SNULL,St1,myS2,St2);
	// modified by IFV for treating operation between shell and solid
	const TopTools_ListOfShape& MergedShapes = myHBuilder->Merged(myS2,St2);
	TopTools_IndexedMapOfShape aMapOfFaces;
	sewing = Standard_False;

	if(MergedShapes.Extent() != 0) {
	  TopTools_ListIteratorOfListOfShape its(MergedShapes);
	  for(; its.More(); its.Next()) {
	    BB.Add(myShape,its.Value());
	  }
	  TopExp::MapShapes(myShape, TopAbs_FACE, aMapOfFaces);
	}

	for(LIter.Initialize(Faces2);LIter.More();LIter.Next()) {
	  if (myHBuilder->IsSplit(LIter.Value(),St2)) {
	    TopTools_ListIteratorOfListOfShape its;
	    for(its.Initialize(myHBuilder->Splits(LIter.Value(),St2));
		its.More();its.Next()) {
	      if(!aMapOfFaces.Contains(its.Value())) BB.Add(myShape,its.Value());
	    }
	  }
	  else {
	    const TopoDS_Shape& LV = LIter.Value();
	    if(!aMapOfFaces.Contains(LV)) {
	      if(  (LV.Orientation() == TopAbs_EXTERNAL && St2==TopAbs_OUT ) 	    
		 ||(LV.Orientation() == TopAbs_INTERNAL && St2==TopAbs_IN  )) {
		BB.Add(myShape,LV);
	      }
	      else { 
		//-- Classify : 
		Sub_Classify(Ex,St2,Solids1,BB,LIter,myShape); 
	      }
	      //-- End Classification 	
	    }
	  }
	}
      } // nbf2
    } // (nbf1 || nbf2)
    
    //----------------------------------------------------------------------
    if(nbe1) { 
      myHBuilder->MergeShapes(myS1,St1,SNULL,St2);

      for(LIter.Initialize(Edges1);LIter.More();LIter.Next()) {
	if (myHBuilder->IsSplit(LIter.Value(),St1)) {
	  TopTools_ListIteratorOfListOfShape its;
	  for(its.Initialize(myHBuilder->Splits(LIter.Value(),St1));
	      its.More();its.Next()) {
	    BB.Add(myShape,its.Value());
	  }
	}
	else {
	  const TopoDS_Shape& LV = LIter.Value();
	  if(  (LV.Orientation() == TopAbs_EXTERNAL && St1==TopAbs_OUT ) 	    
	     ||(LV.Orientation() == TopAbs_INTERNAL && St1==TopAbs_IN  )) {
	    BB.Add(myShape,LV);
	  }
	  else { 
	    //-- Classify : 
	    Sub_Classify(Ex,St1,Solids2,BB,LIter,myShape); 
	  }
	  //-- End Classification 
	}
      }
    }
    if(nbe2) { 
      myHBuilder->MergeShapes(SNULL,St1,myS2,St2);
      
      for(LIter.Initialize(Edges2);LIter.More();LIter.Next()) {
	if (myHBuilder->IsSplit(LIter.Value(),St2)) {
	  TopTools_ListIteratorOfListOfShape its;
	  for(its.Initialize(myHBuilder->Splits(LIter.Value(),St2));
	      its.More();its.Next()) {
	    BB.Add(myShape,its.Value());
	  }
	}
	else {
	  const TopoDS_Shape& LV = LIter.Value();
	  if(  (LV.Orientation() == TopAbs_EXTERNAL && St2==TopAbs_OUT ) 	    
	     ||(LV.Orientation() == TopAbs_INTERNAL && St2==TopAbs_IN  ))  {	 
	    BB.Add(myShape,LV);
	  }
	  else { 
	    //-- Classify : 
	    Sub_Classify(Ex,St2,Solids1,BB,LIter,myShape); 
	  }
	  //-- End Classification 
	}
      }
    }
    //----------------------------------------------------------------------
    //-- V1:Vertex1   state1 = OUT   -> Preserve V1 if V1 is Out all S2
    //-- V1:Vertex1   state1 = IN    -> Preserve V1 if V1 is In one of S2 
    if(nbv1 && nbs2) { 
      if(St1 == TopAbs_IN) { 
	for(LIter.Initialize(Vertex1);LIter.More();LIter.Next()) {
	  Standard_Boolean keep = Standard_False;
	  Standard_Boolean ok = Standard_True;
	  const TopoDS_Vertex& V=TopoDS::Vertex(LIter.Value());
	  gp_Pnt P=BRep_Tool::Pnt(V);
	  Standard_Real Tol = BRep_Tool::Tolerance(V);
	  TopTools_ListIteratorOfListOfShape SIter;
	  for(SIter.Initialize(Solids2);
	      SIter.More() && ok==Standard_True;
	      SIter.Next()) { 	  
	    BRepClass3d_SolidClassifier SolClass(SIter.Value());
	    SolClass.Perform(P,Tol);
	    if(SolClass.State() == TopAbs_IN) {
	      ok=Standard_False;
	      keep = Standard_True;
	    }
	  }
	  if(keep) { 
	    BB.Add(myShape,LIter.Value());
	  }
	}
      }
      else { 
	if(St1 == TopAbs_OUT) { 
	  for(LIter.Initialize(Vertex1);LIter.More();LIter.Next()) {
	    Standard_Boolean keep = Standard_True;
	    Standard_Boolean ok = Standard_True;
	    const TopoDS_Vertex& V=TopoDS::Vertex(LIter.Value());
	    gp_Pnt P=BRep_Tool::Pnt(V);
	    Standard_Real Tol = BRep_Tool::Tolerance(V);
	    TopTools_ListIteratorOfListOfShape SIter;
	    for(SIter.Initialize(Solids2);
		SIter.More() && ok==Standard_True;
		SIter.Next()) { 	  
	      BRepClass3d_SolidClassifier SolClass(SIter.Value());
	      SolClass.Perform(P,Tol);
	      if(SolClass.State() != TopAbs_OUT) {
		keep = Standard_False;
		ok   = Standard_False;
	      }
	    }
	    if(keep) { 
	      BB.Add(myShape,LIter.Value());
	    }
	  }
	}
      }    
    }
  
    if(nbv2 && nbs1) { 
      if(St2 == TopAbs_IN) { 
	for(LIter.Initialize(Vertex2);LIter.More();LIter.Next()) {
	  Standard_Boolean keep = Standard_False;
	  Standard_Boolean ok = Standard_True;
	  const TopoDS_Vertex& V=TopoDS::Vertex(LIter.Value());
	  gp_Pnt P=BRep_Tool::Pnt(V);
	  Standard_Real Tol = BRep_Tool::Tolerance(V);
	  TopTools_ListIteratorOfListOfShape SIter;
	  for(SIter.Initialize(Solids1);
	      SIter.More() && ok==Standard_True;
	      SIter.Next()) { 	  
	    BRepClass3d_SolidClassifier SolClass(SIter.Value());
	    SolClass.Perform(P,Tol);
	    if(SolClass.State() == TopAbs_IN) {
	      ok=Standard_False;
	      keep = Standard_True;
	    }
	  }
	  if(keep) { 
	    BB.Add(myShape,LIter.Value());
	  }
	}
      }
      else { 
	if(St2 == TopAbs_OUT) { 
	  for(LIter.Initialize(Vertex2);LIter.More();LIter.Next()) {
	    Standard_Boolean keep = Standard_True;
	    Standard_Boolean ok = Standard_True;
	    const TopoDS_Vertex& V=TopoDS::Vertex(LIter.Value());
	    gp_Pnt P=BRep_Tool::Pnt(V);
	    Standard_Real Tol = BRep_Tool::Tolerance(V);
	    TopTools_ListIteratorOfListOfShape SIter;
	    for(SIter.Initialize(Solids1);
		SIter.More() && ok==Standard_True;
		SIter.Next()) { 	  
	      BRepClass3d_SolidClassifier SolClass(SIter.Value());
	      SolClass.Perform(P,Tol);
	      if(SolClass.State() != TopAbs_OUT) {
		keep = Standard_False;
		ok   = Standard_False;
	      }
	    }
	    if(keep) { 
	      BB.Add(myShape,LIter.Value());
	    }
	  }
	}
      }    
    }
    
    if(nbs1 && nbs2 ) { 
      myHBuilder->MergeShapes(myS1,St1,myS2,St2);
      if(myHBuilder->IsMerged(myS1,St1)) { 
        TopTools_ListIteratorOfListOfShape its;
	its = myHBuilder->Merged(myS1,St1);
	Standard_Integer nbSolids = 0;
	for(; its.More(); its.Next(), nbSolids++) { 
	  BB.Add(myShape,its.Value());
	}
      }
    }
  
#else 

    myHBuilder->MergeSolids(myS1,St1,myS2,St2);
    TopTools_ListIteratorOfListOfShape its;
  
    BB.MakeCompound(TopoDS::Compound(myShape));
    its = myHBuilder->Merged(myS1,St1);
    while (its.More()) {
      BB.Add(myShape,its.Value());
      its.Next();
    }

#endif
// #if MODIF

  }

  // Creation of the Map used in IsDeleted.
  TopExp_Explorer ex;
  ex.Init(myShape,TopAbs_FACE);
  for (; ex.More(); ex.Next()) myMap.Add(ex.Current());
  ex.Init(myShape,TopAbs_EDGE); // for FRIKO
  for (; ex.More(); ex.Next()) myMap.Add(ex.Current());
  
  // Checking same parameter of new edges of section
  Standard_Real eTol,cTol;
  for (myHBuilder->InitSection(1); 
       myHBuilder->MoreSection(); 
       myHBuilder->NextSection()) {
    const TopoDS_Shape& cur = myHBuilder->CurrentSection();
    if (cur.ShapeType()==TopAbs_EDGE) {
      BRepCheck_Edge bce(TopoDS::Edge(cur));
      cTol=bce.Tolerance();
      eTol = BRep_Tool::Tolerance(TopoDS::Edge(cur));
      if (eTol<cTol) {
	BB.UpdateEdge(TopoDS::Edge(cur), cTol);
	for (ex.Init(cur, TopAbs_VERTEX); ex.More(); ex.Next()) {
	  eTol = BRep_Tool::Tolerance(TopoDS::Vertex(ex.Current()));
	  if (eTol<cTol) {
	    // Update can only increase tolerance, so if the vertex 
	    // has a greater tolerance thanits edges it is not touched
	    BB.UpdateVertex(TopoDS::Vertex(ex.Current()), cTol);
	  }
	}
      }
    }
  }

  Standard_Real maxTol = RealLast();    // MSV: unlimit tolerance
  TopOpeBRepBuild_Tools::CorrectTolerances(myShape,maxTol);

  TopExp_Explorer ex1, ex2, ex3;
  TopTools_ListOfShape theOldShell, theNewShell;
  Standard_Boolean modif =Standard_False;

#ifdef OCCT_DEBUG
  Standard_Boolean nosew = TopOpeBRepTool_GetcontextNOSEW();
  if (nosew) sewing = Standard_False;
#endif

  if (sewing) {
    topToSew.Clear();
    for (ex1.Init(myShape, TopAbs_SHELL); ex1.More(); ex1.Next()) {
      BRepCheck_Shell bcs(TopoDS::Shell(ex1.Current()));
      if (bcs.Closed()==BRepCheck_NotClosed) {
	// it is required to add them face by face to avoid IsModified on faces
	BRepBuilderAPI_Sewing brts;
	for (ex3.Init(ex1.Current(), TopAbs_FACE); ex3.More(); ex3.Next()) {
	  brts.Add(ex3.Current());
	}
	brts.Perform();
	ex2.Init(brts.SewedShape(), TopAbs_SHELL);
	if (ex2.More()) {
	  ex2.Next();
	  if (!ex2.More()) {
	    ex2.Init(brts.SewedShape(), TopAbs_SHELL);
	    theOldShell.Append(ex1.Current());
	    theNewShell.Append(ex2.Current());
	    modif =Standard_True;
	    for (ex3.Init(ex1.Current(), TopAbs_EDGE); ex3.More(); ex3.Next()) {
	      const TopoDS_Edge& ledg = TopoDS::Edge(ex3.Current());
	      if (brts.IsSectionBound(ledg)) {
		topToSew.Bind(ledg, brts.SectionToBoundary(ledg));
		if (!BRep_Tool::SameParameter(brts.SectionToBoundary(ledg))) {
		  BRepLib::SameParameter(ledg, BRep_Tool::Tolerance(brts.SectionToBoundary(ledg)));
		}
	      }
	    }
	    for (ex3.Init(ex1.Current(), TopAbs_FACE); ex3.More(); ex3.Next()) {
	      if (brts.IsModified(ex3.Current())) {
		topToSew.Bind(ex3.Current(), brts.Modified(ex3.Current()));
	      }
	    }
	  }
	}
      }
    }
  } // sewing

  if (modif) {
    BRepTools_Substitution bsub;
    TopTools_ListIteratorOfListOfShape itl(theOldShell);
    TopTools_ListOfShape forSub;
    for (; itl.More();itl.Next()) {
      forSub.Append(theNewShell.First());
      bsub.Substitute(itl.Value(), forSub);
      theNewShell.RemoveFirst();
      forSub.Clear();
    }
    bsub.Build(myShape);
    if (bsub.IsCopied(myShape)) {
      myShape=(bsub.Copy(myShape)).First();
    }
  }
  
  Done();
}



//=======================================================================
//function : Builder
//purpose  : 
//=======================================================================
  Handle(TopOpeBRepBuild_HBuilder) BRepAlgo_BooleanOperation::Builder()const 
{
  return myHBuilder;
}


//=======================================================================
//function : TopoDS_Shape&
//purpose  : 
//=======================================================================
  const TopoDS_Shape& BRepAlgo_BooleanOperation::Shape1() const 
{
  return myS1;
}


//=======================================================================
//function : TopoDS_Shape&
//purpose  : 
//=======================================================================
  const TopoDS_Shape& BRepAlgo_BooleanOperation::Shape2() const 
{
  return myS2;
}

//=======================================================================
//function : BuilderCanWork
//purpose  : 
//=======================================================================
  void BRepAlgo_BooleanOperation::BuilderCanWork(const Standard_Boolean Val)
{
  myBuilderCanWork = Val;
}

//=======================================================================
//function : BuilderCanWork
//purpose  : 
//=======================================================================
  Standard_Boolean BRepAlgo_BooleanOperation::BuilderCanWork() const
{
  return myBuilderCanWork;
}


void Sub_Classify(TopExp_Explorer& Ex,
		  const TopAbs_State St1,
		  TopTools_ListOfShape& Solids2,
		  BRep_Builder& BB,
		  TopTools_ListIteratorOfListOfShape& LIter,
		  TopoDS_Shape& myShape) { 
  Ex.Init(LIter.Value(),TopAbs_VERTEX);
  if(Ex.More()) { 
    if(St1 == TopAbs_IN) { 
      Standard_Boolean keep = Standard_False;
      Standard_Boolean ok = Standard_True;
      const TopoDS_Vertex& V=TopoDS::Vertex(Ex.Current());
      gp_Pnt P=BRep_Tool::Pnt(V);
      Standard_Real Tol = BRep_Tool::Tolerance(V);
      TopTools_ListIteratorOfListOfShape SIter;
      for(SIter.Initialize(Solids2);
	  SIter.More() && ok==Standard_True;
	  SIter.Next()) { 	  
	BRepClass3d_SolidClassifier SolClass(SIter.Value());
	SolClass.Perform(P,Tol);
	if(SolClass.State() == TopAbs_IN) {
	  ok=Standard_False;
	  keep = Standard_True;
	}
      }
      if(keep) { 
	BB.Add(myShape,LIter.Value());
      }
    }
    else { 
      if(St1 == TopAbs_OUT) { 
	Standard_Boolean keep = Standard_True;
	Standard_Boolean ok = Standard_True;
	const TopoDS_Vertex& V=TopoDS::Vertex(Ex.Current());
	gp_Pnt P=BRep_Tool::Pnt(V);
	Standard_Real Tol = BRep_Tool::Tolerance(V);
	TopTools_ListIteratorOfListOfShape SIter;
	for(SIter.Initialize(Solids2);
	    SIter.More() && ok==Standard_True;
	    SIter.Next()) { 	  
	  BRepClass3d_SolidClassifier SolClass(SIter.Value());
	  SolClass.Perform(P,Tol);
	  if(SolClass.State() != TopAbs_OUT) {
	    keep = Standard_False;
	    ok   = Standard_False;
	  }
	}
	if(keep) { 
	  BB.Add(myShape,LIter.Value());
	}
      }
    }
  }
}


//=======================================================================
//function : InitParameters
//purpose  : Info on geometry : PCurve, Approx, ...
//=======================================================================
void BRepAlgo_BooleanOperation::InitParameters()
{
  TopOpeBRepDS_BuildTool& BTofBuilder = myHBuilder->ChangeBuildTool();
  TopOpeBRepTool_GeomTool& GTofBTofBuilder = BTofBuilder.ChangeGeomTool();

  GTofBTofBuilder.Define(TopOpeBRepTool_APPROX);
  GTofBTofBuilder.DefineCurves(Standard_True);
  GTofBTofBuilder.DefinePCurves1(Standard_True);
  GTofBTofBuilder.DefinePCurves2(Standard_True);
}

//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& BRepAlgo_BooleanOperation::Modified(const TopoDS_Shape& S) 
{
  myGenerated.Clear();
  TopTools_MapOfShape aMap; // to check if shape can be added in list more then one time
  aMap.Clear();
  if (myHBuilder->IsSplit(S, TopAbs_OUT)) {
    TopTools_ListIteratorOfListOfShape It(myHBuilder->Splits(S, TopAbs_OUT));
    for(;It.More();It.Next()) {
      if (topToSew.IsBound(It.Value())) 
	{if(aMap.Add(topToSew.Find(It.Value()))) myGenerated.Append(topToSew.Find(It.Value()));}
      else
	{if(aMap.Add(It.Value())) myGenerated.Append(It.Value());}
    }
  }
  if (myHBuilder->IsSplit(S, TopAbs_IN)) {
    TopTools_ListIteratorOfListOfShape It(myHBuilder->Splits(S, TopAbs_IN));
    for(;It.More();It.Next()) {
      if (topToSew.IsBound(It.Value())) 
	{if(aMap.Add(topToSew.Find(It.Value()))) myGenerated.Append(topToSew.Find(It.Value()));}
      else
	{if(aMap.Add(It.Value())) myGenerated.Append(It.Value());}
    }
  }
  if (myHBuilder->IsSplit(S, TopAbs_ON)) {
    TopTools_ListIteratorOfListOfShape It(myHBuilder->Splits(S, TopAbs_ON));
    for(;It.More();It.Next()) {
      if (topToSew.IsBound(It.Value())) 
	{if(aMap.Add(topToSew.Find(It.Value()))) myGenerated.Append(topToSew.Find(It.Value()));}
      else
	{if(aMap.Add(It.Value())) myGenerated.Append(It.Value());}
    }
  }

  if (myHBuilder->IsMerged(S, TopAbs_OUT)) {
    TopTools_ListIteratorOfListOfShape It(myHBuilder->Merged(S, TopAbs_OUT));
    for(;It.More();It.Next()) {
      if (topToSew.IsBound(It.Value())) 
	{if(aMap.Add(topToSew.Find(It.Value()))) myGenerated.Append(topToSew.Find(It.Value()));}
      else
	{if(aMap.Add(It.Value())) myGenerated.Append(It.Value());}
    }
  }
  if (myHBuilder->IsMerged(S, TopAbs_IN)) {
    TopTools_ListIteratorOfListOfShape It(myHBuilder->Merged(S, TopAbs_IN));
    for(;It.More();It.Next()) {
      if (topToSew.IsBound(It.Value())) 
	{if(aMap.Add(topToSew.Find(It.Value()))) myGenerated.Append(topToSew.Find(It.Value()));}
      else
	{if(aMap.Add(It.Value())) myGenerated.Append(It.Value());}
    }
  }
  if (myHBuilder->IsMerged(S, TopAbs_ON)) {
    TopTools_ListIteratorOfListOfShape It(myHBuilder->Merged(S, TopAbs_ON));
    for(;It.More();It.Next()) {
      if (topToSew.IsBound(It.Value())) 
	{if(aMap.Add(topToSew.Find(It.Value()))) myGenerated.Append(topToSew.Find(It.Value()));}
      else
	{if(aMap.Add(It.Value())) myGenerated.Append(It.Value());}
    }
  }
  return myGenerated;
}


//=======================================================================
//function : IsDeleted
//purpose  : 
//=======================================================================
Standard_Boolean BRepAlgo_BooleanOperation::IsDeleted(const TopoDS_Shape& S) 
{
  Standard_Boolean Deleted = Standard_True; 
  if (myMap.Contains(S) || 
      myHBuilder->IsMerged(S, TopAbs_OUT) ||
      myHBuilder->IsMerged(S, TopAbs_IN)  ||
      myHBuilder->IsMerged(S, TopAbs_ON)  ||
      myHBuilder->IsSplit (S, TopAbs_OUT)  ||
      myHBuilder->IsSplit (S, TopAbs_IN)   ||
      myHBuilder->IsSplit (S, TopAbs_ON))
    return Standard_False;

  return Deleted;    
}
