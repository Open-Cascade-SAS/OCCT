// Created on: 1995-09-12
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepBuild_SplitEdge_HeaderFile
#define _TopOpeBRepBuild_SplitEdge_HeaderFile

//=======================================================================
//function : SplitEdge
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Builder::SplitEdge(const TopoDS_Shape& E,
					const TopAbs_State ToBuild1,
					const TopAbs_State ToBuild2)
{
#ifdef OCCT_DEBUG
  if ( TopOpeBRepBuild_GetcontextSF2() ) {
    SplitEdge2(E,ToBuild1,ToBuild2);
    return;
  }
#endif
  SplitEdge1(E,ToBuild1,ToBuild2);
  return;
}

//=======================================================================
//function : SplitEdge1
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Builder::SplitEdge1(const TopoDS_Shape& Eoriented,
					 const TopAbs_State ToBuild1,
					 const TopAbs_State ToBuild2)
{
  // work on a FORWARD edge <Eforward>

  TopoDS_Shape Eforward = Eoriented; 
  Eforward.Orientation(TopAbs_FORWARD);

  Standard_Boolean tosplit = ToSplit(Eoriented,ToBuild1);

#ifdef OCCT_DEBUG
  Standard_Integer iEdge; Standard_Boolean tSPS = GtraceSPS(Eoriented,iEdge);
  if(tSPS){
    cout<<endl;
    GdumpSHASTA(Eoriented,ToBuild1,"--- SplitEdge ");
    cout<<endl;
  }
#endif
  
  if ( ! tosplit ) return;

  Reverse(ToBuild1,ToBuild2);
  Reverse(ToBuild2,ToBuild1);
  Standard_Boolean ConnectTo1 = Standard_True;
  Standard_Boolean ConnectTo2 = Standard_False;
  
  // build the list of edges to split : LE1, LE2
  TopTools_ListOfShape LE1,LE2;
  LE1.Append(Eforward);
  FindSameDomain(LE1,LE2);

#ifdef OCCT_DEBUG
  if(tSPS){GdumpSAMDOM(LE1, (char *) "1 : ");}
  if(tSPS){GdumpSAMDOM(LE2, (char *) "2 : ");}
  if(tSPS){cout<<endl;}
  if(tSPS){cout<<"V of edge ";TopAbs::Print(Eforward.Orientation(),cout);}
  if(tSPS){cout<<endl;}
  if(tSPS){GdumpEDG(Eforward);}
#endif
  
  // SplitEdge on a edge having other same domained edges on the
  // other shape : do not reverse orientation of edges in FillEdge
    
  // Make a PaveSet <PVS> on edge <Eforward>
  TopOpeBRepBuild_PaveSet PVS(Eforward);

  // Add the points/vertices found on edge <Eforward> in <PVS>
  TopOpeBRepDS_PointIterator EPIT(myDataStructure->EdgePoints(Eforward));
  FillVertexSet(EPIT,ToBuild1,PVS);
  
  TopOpeBRepBuild_PaveClassifier VCL(Eforward);
  Standard_Boolean equalpar = PVS.HasEqualParameters();
  if (equalpar) VCL.SetFirstParameter(PVS.EqualParameters());

  // ------------------------------------------
  // before return if PVS has no vertices, 
  // mark <Eforward> as split <ToBuild1>
  // ------------------------------------------
  MarkSplit(Eforward,ToBuild1);
  
  PVS.InitLoop();
  if ( !PVS.MoreLoop() ) {
#ifdef OCCT_DEBUG
    if(tSPS) {
      cout<<"NO VERTEX split "; TopAbs::Print(ToBuild1,cout);cout<<endl;
    }
#endif
    return;
  }
  
  // build the new edges
  TopOpeBRepBuild_EdgeBuilder EBU(PVS,VCL);
  
  // Build the new edges
  // -------------------
  TopTools_ListOfShape& EdgeList = ChangeMerged(Eforward,ToBuild1);
  MakeEdges(Eforward,EBU,EdgeList);

  TopTools_ListIteratorOfListOfShape itLE1,itLE2;

  // connect new edges as edges built <ToBuild1> on LE1 edge
  // --------------------------------------------------------
  for (itLE1.Initialize(LE1); itLE1.More(); itLE1.Next()) {
    TopoDS_Shape Ecur = itLE1.Value();
    MarkSplit(Ecur,ToBuild1);
    TopTools_ListOfShape& EL = ChangeSplit(Ecur,ToBuild1);
    if ( ConnectTo1 ) EL = EdgeList;
  }
  
  // connect new edges as edges built <ToBuild2> on LE2 edges
  // --------------------------------------------------------
  for (itLE2.Initialize(LE2); itLE2.More(); itLE2.Next()) {
    TopoDS_Shape Ecur = itLE2.Value();
    MarkSplit(Ecur,ToBuild2);
    TopTools_ListOfShape& EL = ChangeSplit(Ecur,ToBuild2);
    if ( ConnectTo2 ) EL = EdgeList;
  }

} // SplitEdge1

//=======================================================================
//function : SplitEdge2
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Builder::SplitEdge2(const TopoDS_Shape& Eoriented,
					 const TopAbs_State ToBuild1,
					 const TopAbs_State /*ToBuild2*/)
{
  Standard_Boolean tosplit = ToSplit(Eoriented,ToBuild1);
  if ( ! tosplit ) return;
  
  // work on a FORWARD edge <Eforward>
  TopoDS_Shape Eforward = Eoriented;
  myBuildTool.Orientation(Eforward,TopAbs_FORWARD);

#ifdef OCCT_DEBUG
  Standard_Integer iEdge; Standard_Boolean tSPS = GtraceSPS(Eoriented,iEdge);
  if(tSPS){cout<<endl;}
  if(tSPS){GdumpSHASTA(Eoriented,ToBuild1,"--- SplitEdge2 ");}
#endif
  
  // Make a PaveSet <PVS> on edge <Eforward>
  // Add the points/vertices found on edge <Eforward> in <PVS>
  TopOpeBRepBuild_PaveSet PVS(Eforward);

  TopOpeBRepDS_PointIterator EPIT(myDataStructure->EdgePoints(Eforward));
  FillVertexSet(EPIT,ToBuild1,PVS);
  
  TopOpeBRepBuild_PaveClassifier VCL(Eforward);
  Standard_Boolean equalpar = PVS.HasEqualParameters();
  if (equalpar) VCL.SetFirstParameter(PVS.EqualParameters());

  // ------------------------------------------
  // before return if PVS has no vertices, 
  // mark <Eforward> as split <ToBuild1>
  // ------------------------------------------
  MarkSplit(Eforward,ToBuild1);
  
  PVS.InitLoop();
  if ( !PVS.MoreLoop() ) {
#ifdef OCCT_DEBUG
    if(tSPS) {cout<<"NO VERTEX split ";TopAbs::Print(ToBuild1,cout);cout<<endl;}
#endif
    return;
  }
  
  // build the new edges
  TopOpeBRepBuild_EdgeBuilder EBU(PVS,VCL);
  
  // connect the new edges as split parts <ToBuild1> built on <Eforward>
  TopTools_ListOfShape& EL = ChangeSplit(Eforward,ToBuild1);
  MakeEdges(Eforward,EBU,EL);
  
} // SplitEdge2

//#ifndef _TopOpeBRepBuild_SplitEdge_HeaderFile
#endif
