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

#ifndef _TopOpeBRepBuild_SplitFace_HeaderFile
#define _TopOpeBRepBuild_SplitFace_HeaderFile

#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepBuild_FaceBuilder.hxx>

#ifdef OCCT_DEBUG
Standard_EXPORT void debspf(const Standard_Integer i) {cout<<"++  debspf"<<i<<endl;}
#endif

//=======================================================================
//function : SplitFace
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Builder::SplitFace(const TopoDS_Shape& Foriented,
					const TopAbs_State ToBuild1,
					const TopAbs_State ToBuild2)
{
#ifdef OCCT_DEBUG
  if(TopOpeBRepBuild_GetcontextSF2()){
    SplitFace2(Foriented,ToBuild1,ToBuild2);
    return;
  }
#endif
  SplitFace1(Foriented,ToBuild1,ToBuild2);
  return;
}

//=======================================================================
//function : SplitFace1
//purpose  : tout dans le meme edge set
//=======================================================================

void TopOpeBRepBuild_Builder::SplitFace1(const TopoDS_Shape& Foriented,
					 const TopAbs_State ToBuild1,
					 const TopAbs_State ToBuild2)
{
  //                              process  connect  connect
  // operation tobuild1 tobuild2  face F   to 1     to 2
  // --------- -------- --------  -------  -------  -------
  // common    IN       IN        yes      yes      yes
  // fuse      OUT      OUT       yes      yes      yes
  // cut 1-2   OUT      IN        yes      yes      no
  // cut 2-1   IN       OUT       yes      yes      no           
  //                                                       
  Standard_Boolean tosplit = ToSplit(Foriented,ToBuild1);
  if ( ! tosplit ) return;
  
  Standard_Boolean RevOri1 = Reverse(ToBuild1,ToBuild2);
  Standard_Boolean RevOri2 = Reverse(ToBuild2,ToBuild1);
  Standard_Boolean ConnectTo1 = Standard_True;
  Standard_Boolean ConnectTo2 = Standard_False;
  
  // work on a FORWARD face <Fforward>
  TopoDS_Shape Fforward = Foriented; 
  myBuildTool.Orientation(Fforward,TopAbs_FORWARD);
  
  // build the list of faces to split : LF1, LF2
  TopTools_ListOfShape LF1,LF2;
  LF1.Append(Fforward);
  FindSameDomain(LF1,LF2);
  Standard_Integer n1 = LF1.Extent();
  Standard_Integer n2 = LF2.Extent();
  
  // SplitFace on a face having other same domained faces on the
  // other shape : do not reverse orientation of faces in FillFace
  if (!n2) RevOri1 = Standard_False;
  if (!n1) RevOri2 = Standard_False;
  
  // Create an edge set <WES> connected by vertices
  // ----------------------------------------------
  TopOpeBRepBuild_WireEdgeSet WES(Fforward,this);
  
#ifdef OCCT_DEBUG
  Standard_Boolean tSPF=TopOpeBRepBuild_GettraceSPF();
  Standard_Integer iFace=myDataStructure->Shape(Foriented);
  if(tSPF){cout<<endl;GdumpSHASTA(Foriented,ToBuild1,"=== SplitFace ");}
  if(tSPF){GdumpSAMDOM(LF1, (char *) "1 : ");GdumpSAMDOM(LF2, (char *) "2 : ");}
  if(tSPF) debspf(iFace);
#endif
    
  TopTools_ListIteratorOfListOfShape itLF1,itLF2;

  for (itLF1.Initialize(LF1); itLF1.More(); itLF1.Next()) {
    const TopoDS_Shape& Fcur = itLF1.Value();
//                     myDataStructure->Shape(Fcur);//DEB
    FillFace(Fcur,ToBuild1,LF2,ToBuild2,WES,RevOri1);
  }
  
  for (itLF2.Initialize(LF2); itLF2.More(); itLF2.Next()) {
    const TopoDS_Shape& Fcur = itLF2.Value();
//                            myDataStructure->Shape(Fcur);//DEB
    FillFace(Fcur,ToBuild2,LF1,ToBuild1,WES,RevOri2);
  }
  
  // Add the intersection edges to edge set WES
  // -----------------------------------------
  AddIntersectionEdges(Fforward,ToBuild1,RevOri1,WES);
   
#ifdef OCCT_DEBUG
  Standard_Integer iF; Standard_Boolean tSPS = GtraceSPS(Fforward,iF);
  if(tSPS) WES.DumpSS();
#endif

  // Create a Face Builder FBU
  // ------------------------
  TopOpeBRepBuild_FaceBuilder FBU;
  FBU.InitFaceBuilder(WES,Fforward,Standard_False); //forceclass = False

  // Build the new faces
  // -------------------
  TopTools_ListOfShape& FaceList = ChangeMerged(Fforward,ToBuild1);
  MakeFaces(Fforward,FBU,FaceList);

  // connect new faces as faces built <ToBuild1> on LF1 faces
  // --------------------------------------------------------
  for (itLF1.Initialize(LF1); itLF1.More(); itLF1.Next()) {
    TopoDS_Shape Fcur = itLF1.Value();
    MarkSplit(Fcur,ToBuild1);
    TopTools_ListOfShape& FL = ChangeSplit(Fcur,ToBuild1);
    if ( ConnectTo1 ) FL = FaceList;
  }
  
  // connect new faces as faces built <ToBuild2> on LF2 faces
  // --------------------------------------------------------
  for (itLF2.Initialize(LF2); itLF2.More(); itLF2.Next()) {
    TopoDS_Shape Fcur = itLF2.Value();
    MarkSplit(Fcur,ToBuild2);
    TopTools_ListOfShape& FL = ChangeSplit(Fcur,ToBuild2);
    if ( ConnectTo2 ) FL = FaceList;
  }

} // SplitFace1

//=======================================================================
//function : SplitFace2
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Builder::SplitFace2(const TopoDS_Shape& Foriented,
					 const TopAbs_State ToBuild1,
					 const TopAbs_State ToBuild2)
{
  //                              process  connect  connect
  // operation tobuild1 tobuild2  face F   to 1     to 2
  // --------- -------- --------  -------  -------  -------
  // common    IN       IN        yes      yes      yes
  // fuse      OUT      OUT       yes      yes      yes
  // cut 1-2   OUT      IN        yes      yes      no
  // cut 2-1   IN       OUT       yes      yes      no           
  //                                                       
  Standard_Boolean tosplit = ToSplit(Foriented,ToBuild1);
  if ( ! tosplit ) return;
  
  Standard_Boolean RevOri1 = Reverse(ToBuild1,ToBuild2);
  Standard_Boolean RevOri2 = Reverse(ToBuild2,ToBuild1);
  Standard_Boolean ConnectTo1 = Standard_True;
  Standard_Boolean ConnectTo2 = Standard_False;

  // work on a FORWARD face <Fforward>
  TopoDS_Shape Fforward = Foriented; 
  myBuildTool.Orientation(Fforward,TopAbs_FORWARD);

  TopTools_ListOfShape LF1 ; //liste des faces de 1   samedomain
  TopTools_ListOfShape LF2 ; //liste des faces de   2 samedomain
  LF1.Append(Fforward);
  FindSameDomain(LF1,LF2);
  Standard_Integer n1 = LF1.Extent();
  Standard_Integer n2 = LF2.Extent();
  
#ifdef OCCT_DEBUG
  Standard_Boolean tSPF = TopOpeBRepBuild_GettraceSPF();
//  Standard_Integer iFace = myDataStructure->Shape(Foriented);
  if (tSPF) {
    cout<<endl;
    GdumpSHASTA(Foriented,ToBuild1,"=== SplitFace ");
    GdumpSAMDOM(LF1, (char *) "samedomain 1 : ");
    GdumpSAMDOM(LF2, (char *) "samedomain 2 : ");
  }
#endif
  
  // SplitFace on a face having other same domained faces on the
  // other shape : do not reverse orientation of faces in FillFace
  if (!n2) RevOri1 = Standard_False;
  if (!n1) RevOri2 = Standard_False;

  TopTools_ListOfShape LFSO; //liste des faces de 1,2 samedomainsameorientation
  TopTools_ListOfShape LFOO; //liste des faces de 1,2 samedomainoppositeorient

  // LFSO : faces des shapes 1 ou 2, de meme orientation que Fforward.
  // LFOO : faces des shapes 1 ou 2, d'orientation contraire que Fforward.
  LFSO.Append(Fforward);
  FindSameDomainSameOrientation(LFSO,LFOO);

  TopTools_ListOfShape LFSO1,LFOO1; // same domain, same orientation, et du shape de F
  TopTools_ListOfShape LFSO2,LFOO2; // "" "",du shape autre que celui de F

  // on construit les parties ToBuild1 de F
  Standard_Integer rankF = ShapeRank(Foriented);
  Standard_Integer rankX = (rankF) ? ((rankF == 1) ? 2 : 1) : 0;

  FindSameRank(LFSO,rankF,LFSO1);
  FindSameRank(LFOO,rankF,LFOO1);
  FindSameRank(LFSO,rankX,LFSO2);
  FindSameRank(LFOO,rankX,LFOO2);

#ifdef OCCT_DEBUG
  if ( tSPF ) {
    GdumpSAMDOM(LFSO1, (char *) "LFSO1 : ");
    GdumpSAMDOM(LFOO1, (char *) "LFOO1 : ");
    GdumpSAMDOM(LFSO2, (char *) "LFSO2 : ");
    GdumpSAMDOM(LFOO2, (char *) "LFOO2 : ");
  }
#endif

  TopAbs_State tob1 = ToBuild1;
  TopAbs_State tob2 = ToBuild2;
  TopAbs_State tob1comp = (ToBuild1 == TopAbs_IN) ? TopAbs_OUT : TopAbs_IN;
  TopAbs_State tob2comp = (ToBuild2 == TopAbs_IN) ? TopAbs_OUT : TopAbs_IN;
  TopTools_ListIteratorOfListOfShape itLF ;
  
  // --------------------------------------------------------------------
  // traitement des faces de meme orientation que Fforward dans WireEdgeSet WES1
  // --------------------------------------------------------------------
  TopOpeBRepBuild_WireEdgeSet WES1(Fforward,this);

  // traitement des faces de 1 same domain, same orientation que F : LFSO1
  for (itLF.Initialize(LFSO1); itLF.More(); itLF.Next()) {
    const TopoDS_Shape& Fcur = itLF.Value();
//                            myDataStructure->Shape(Fcur);//DEB
    // les wires de Fcur sont a comparer avec les faces de 2
    FillFace(Fcur,tob1,LF2,tob2,WES1,RevOri1);
  }

  // traitement des faces de 2 same domain, same orientation que F : LFSO2
  for (itLF.Initialize(LFSO2); itLF.More(); itLF.Next()) {
    const TopoDS_Shape& Fcur = itLF.Value();
//                            myDataStructure->Shape(Fcur);//DEB
    // les wires de Fcur sont a comparer avec les faces de 1
    FillFace(Fcur,tob2,LF1,tob1,WES1,RevOri2);
  }

  // traitement des faces de 1 same domain, oppo orientation que F : LFOO1
  for (itLF.Initialize(LFOO1); itLF.More(); itLF.Next()) {
    const TopoDS_Shape& Fcur = itLF.Value();
//                            myDataStructure->Shape(Fcur);//DEB
    // les wires de Fcur sont a comparer avec les faces de 2
    FillFace(Fcur,tob1comp,LF2,ToBuild2,WES1,!RevOri1);
  }

  // traitement des faces de 2 same domain, oppo orientation que F : LFOO2
  for (itLF.Initialize(LFOO2); itLF.More(); itLF.Next()) {
    const TopoDS_Shape& Fcur = itLF.Value();
//                      myDataStructure->Shape(Fcur);//DEB
    // les wires de Fcur sont a comparer avec les faces de 1
    FillFace(Fcur,tob2comp,LF1,ToBuild1,WES1,!RevOri2);
  }
  
  // Add the intersection edges to edge set WES1
  // ------------------------------------------
  AddIntersectionEdges(Fforward,ToBuild1,RevOri1,WES1);

  // Create a Face Builder FBU1
  // ------------------------
  TopOpeBRepBuild_FaceBuilder FBU1(WES1,Fforward);

  // Build the new faces
  // -------------------
  TopTools_ListOfShape& FaceList1 = ChangeMerged(Fforward,ToBuild1);
  MakeFaces(Fforward,FBU1,FaceList1);

  // connect new faces as faces built <ToBuild1> on LF1 faces
  // --------------------------------------------------------
  for (itLF.Initialize(LF1); itLF.More(); itLF.Next()) {
    TopoDS_Shape Fcur = itLF.Value();
    MarkSplit(Fcur,ToBuild1);
    TopTools_ListOfShape& FL = ChangeSplit(Fcur,ToBuild1);
    if ( ConnectTo1 ) FL = FaceList1;
  }

  // --------------------------------------------------------------------
  // traitement des faces de meme orientation que Fforward dans WireEdgeSet WES2
  // --------------------------------------------------------------------
  TopOpeBRepBuild_WireEdgeSet WES2(Fforward,this);

  // traitement des faces de 1 same domain, same orientation que F : LFSO1
  for (itLF.Initialize(LFSO1); itLF.More(); itLF.Next()) {
    const TopoDS_Shape& Fcur = itLF.Value();
//                            myDataStructure->Shape(Fcur);//DEB
    // les wires de Fcur sont a comparer avec les faces de 2
    FillFace(Fcur,tob1comp,LF2,tob2,WES2,!RevOri1);
  }

  // traitement des faces de 2 same domain, same orientation que F : LFSO2
  for (itLF.Initialize(LFSO2); itLF.More(); itLF.Next()) {
    const TopoDS_Shape& Fcur = itLF.Value();
//                            myDataStructure->Shape(Fcur);//DEB
    // les wires de Fcur sont a comparer avec les faces de 1
    FillFace(Fcur,tob2comp,LF1,tob1,WES2,!RevOri2);
  }

  // traitement des faces de 1 same domain, oppo orientation que F : LFOO1
  for (itLF.Initialize(LFOO1); itLF.More(); itLF.Next()) {
    const TopoDS_Shape& Fcur = itLF.Value();
//                            myDataStructure->Shape(Fcur);//DEB
    // les wires de Fcur sont a comparer avec les faces de 2
    FillFace(Fcur,tob1,LF2,ToBuild2,WES2,RevOri1);
  }

  // traitement des faces de 2 same domain, oppo orientation que F : LFOO2
  for (itLF.Initialize(LFOO2); itLF.More(); itLF.Next()) {
    const TopoDS_Shape& Fcur = itLF.Value();
//                            myDataStructure->Shape(Fcur);//DEB
    // les wires de Fcur sont a comparer avec les faces de 1
    FillFace(Fcur,tob2,LF1,ToBuild1,WES2,RevOri2);
  }
  
  // Add the intersection edges to edge set WES2
  // ------------------------------------------
  AddIntersectionEdges(Fforward,ToBuild2,RevOri2,WES2);
   
  // Create a Face Builder FBU2
  // -------------------------
  TopOpeBRepBuild_FaceBuilder FBU2(WES2,Fforward);

  // Build the new faces
  // -------------------
  TopTools_ListOfShape& FaceList2 = ChangeMerged(Fforward,ToBuild2);
  MakeFaces(Fforward,FBU2,FaceList2);

  // connect new faces as faces built <ToBuild2> on LF2 faces
  // --------------------------------------------------------
  for (itLF.Initialize(LF2); itLF.More(); itLF.Next()) {
    TopoDS_Shape Fcur = itLF.Value();
    MarkSplit(Fcur,ToBuild2);
    TopTools_ListOfShape& FL = ChangeSplit(Fcur,ToBuild2);
    if ( ConnectTo2 ) FL = FaceList2;
  }

} // SplitFace2


#if 0
//=======================================================================
//function : SplitFaceOK
//purpose  : tout dans le meme edge set
//=======================================================================

void TopOpeBRepBuild_Builder::SplitFaceOK(const TopoDS_Shape& Foriented,
					  const TopAbs_State ToBuild1,
					  const TopAbs_State ToBuild2)
{
  //                              process  connect  connect
  // operation tobuild1 tobuild2  face F   to 1     to 2
  // --------- -------- --------  -------  -------  -------
  // common    IN       IN        yes      yes      yes
  // fuse      OUT      OUT       yes      yes      yes
  // cut 1-2   OUT      IN        yes      yes      no
  // cut 2-1   IN       OUT       yes      yes      no           
  //                                                       
  Standard_Boolean tosplit = ToSplit(Foriented,ToBuild1);
  if ( ! tosplit ) return;
  
  Standard_Boolean RevOri1 = Reverse(ToBuild1,ToBuild2);
  Standard_Boolean RevOri2 = Reverse(ToBuild2,ToBuild1);
  Standard_Boolean ConnectTo1 = Standard_True;
  Standard_Boolean ConnectTo2 = Standard_False;

  // work on a FORWARD face <Fforward>
  TopoDS_Shape Fforward = Foriented; 
  myBuildTool.Orientation(Fforward,TopAbs_FORWARD);
  
  // build the list of faces to split : LF1, LF2
  TopTools_ListOfShape LF1,LF2;
  LF1.Append(Fforward);
  FindSameDomain(LF1,LF2);
  Standard_Integer n1 = LF1.Extent();
  Standard_Integer n2 = LF2.Extent();
  
  // SplitFace on a face having other same domained faces on the
  // other shape : do not reverse orientation of faces in FillFace
  if (!n2) RevOri1 = Standard_False;
  if (!n1) RevOri2 = Standard_False;
  
  // Create an edge set <WES> connected by vertices
  // ---------------------------------------------
  TopOpeBRepBuild_WireEdgeSet WES(Fforward,this);

#ifdef OCCT_DEBUG
  Standard_Boolean tSPF = TopOpeBRepBuild_GettraceSPF();
  Standard_Integer iFace = myDataStructure->Shape(Foriented);
  if(tSPF){cout<<endl;GdumpSHASTA(Foriented,ToBuild1,"=== SplitFaceOK ");}
  if(tSPF){GdumpSAMDOM(LF1,"1 : ");GdumpSAMDOM(LF2,"2 : ");}
#endif
    
  TopTools_ListIteratorOfListOfShape itLF1,itLF2;

  for (itLF1.Initialize(LF1); itLF1.More(); itLF1.Next()) {
    const TopoDS_Shape& Fcur = itLF1.Value();
    Standard_Integer icur = myDataStructure->Shape(Fcur);//DEB
    FillFace(Fcur,ToBuild1,LF2,ToBuild2,WES,RevOri1);
  }
  
  for (itLF2.Initialize(LF2); itLF2.More(); itLF2.Next()) {
    const TopoDS_Shape& Fcur = itLF2.Value();
    Standard_Integer icur = myDataStructure->Shape(Fcur);//DEB
    FillFace(Fcur,ToBuild2,LF1,ToBuild1,WES,RevOri2);
  }
  
  // Add the intersection edges to edge set WES
  // -----------------------------------------
  AddIntersectionEdges(Fforward,ToBuild1,RevOri1,WES);
   
  // Create a Face Builder FBU
  // ------------------------
  TopOpeBRepBuild_FaceBuilder FBU(WES,Fforward);

  // Build the new faces
  // -------------------
  TopTools_ListOfShape& FaceList = ChangeMerged(Fforward,ToBuild1);
  MakeFaces(Fforward,FBU,FaceList);

  // connect new faces as faces built <ToBuild1> on LF1 faces
  // --------------------------------------------------------
  for (itLF1.Initialize(LF1); itLF1.More(); itLF1.Next()) {
    TopoDS_Shape Fcur = itLF1.Value();
    MarkSplit(Fcur,ToBuild1);
    TopTools_ListOfShape& FL = ChangeSplit(Fcur,ToBuild1);
    if ( ConnectTo1 ) FL = FaceList;
  }
  
  // connect new faces as faces built <ToBuild2> on LF2 faces
  // --------------------------------------------------------
  for (itLF2.Initialize(LF2); itLF2.More(); itLF2.Next()) {
    TopoDS_Shape Fcur = itLF2.Value();
    MarkSplit(Fcur,ToBuild2);
    TopTools_ListOfShape& FL = ChangeSplit(Fcur,ToBuild2);
    if ( ConnectTo2 ) FL = FaceList;
  }

} // SplitFaceOK

// #if 0
#endif

//#ifndef _TopOpeBRepBuild_SplitFace_HeaderFile
#endif
