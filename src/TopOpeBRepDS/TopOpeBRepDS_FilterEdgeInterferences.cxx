// Created on: 1997-04-21
// Created by: Prestataire Mary FABIEN
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


#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepDS_Filter.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_ListIteratorOfListOfInterference.hxx>
#include <TopOpeBRepDS_ListOfInterference.hxx>
#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepDS_GettracePEI();
extern Standard_Boolean TopOpeBRepDS_GettracePI();
extern Standard_Boolean TopOpeBRepDS_GettraceSPSX(const Standard_Integer);
extern Standard_Boolean TopOpeBRepDS_GettraceEDPR();
static Standard_Boolean TRCE(const Standard_Integer SIX) {
  Standard_Boolean b1 = TopOpeBRepDS_GettracePEI();
  Standard_Boolean b2 = TopOpeBRepDS_GettracePI();
  Standard_Boolean b3 = TopOpeBRepDS_GettraceSPSX(SIX);
  return (b1 || b2 || b3);
}
Standard_EXPORT void debfilterPEI(const Standard_Integer i){cout<<endl<<"++ debfilterPEI e"<<i<<endl;}
Standard_EXPORT void debfilter(const Standard_Integer i)   {cout<<endl<<"++ debfilter e"<<i<<endl;}
#endif

Standard_EXPORT Standard_Integer FUN_unkeepEinterferences(TopOpeBRepDS_ListOfInterference& LI,const TopOpeBRepDS_DataStructure& BDS,const Standard_Integer SIX);
Standard_EXPORT void FUN_unkeepEsymetrictransitions(TopOpeBRepDS_ListOfInterference& LI,const TopOpeBRepDS_DataStructure& BDS,const Standard_Integer SIX);
Standard_EXPORT void FUN_orderFFsamedomain(TopOpeBRepDS_ListOfInterference& LI,const Handle(TopOpeBRepDS_HDataStructure)& BDS,const Standard_Integer SIX);
Standard_EXPORT void FUN_orderSTATETRANS(TopOpeBRepDS_ListOfInterference& LI,const Handle(TopOpeBRepDS_HDataStructure)& BDS,const Standard_Integer SIX);
Standard_EXPORT void FUN_resolveEUNKNOWN(TopOpeBRepDS_ListOfInterference& LI,TopOpeBRepDS_DataStructure& BDS,const Standard_Integer SIX);
Standard_EXPORT void FUN_purgeDSonSE(const Handle(TopOpeBRepDS_HDataStructure)& HDS,const Standard_Integer EIX,TopOpeBRepDS_ListOfInterference& LI);

//------------------------------------------------------
static void FUN_FilterEdge
//------------------------------------------------------
(TopOpeBRepDS_ListOfInterference& LI,const Handle(TopOpeBRepDS_HDataStructure)& HDS,const Standard_Integer SIX)
{
  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  
#ifdef OCCT_DEBUG
  Standard_Boolean TRC=TRCE(SIX); 
  if (TRC) debfilter(SIX);
  if (TRC) cout<<"FUN_FilterEdge on "<<SIX<<" nI = "<<LI.Extent()<<endl;
#endif

  FUN_orderFFsamedomain(LI,HDS,SIX);
  FUN_orderSTATETRANS(LI,HDS,SIX);
  FUN_unkeepEinterferences(LI,BDS,SIX);
  FUN_reducedoublons(LI,BDS,SIX);
  
#ifdef OCCT_DEBUG
  if (TRC) cout<<"FUN_FilterEdge on "<<SIX<<" returns nI = "<<LI.Extent()<<endl;
#endif
  
}

//=======================================================================
//function : ProcessEdgeInterferences
//purpose  : Filter
//=======================================================================

void TopOpeBRepDS_Filter::ProcessEdgeInterferences
(const Standard_Integer SIX)
{
  TopOpeBRepDS_DataStructure& BDS = myHDS->ChangeDS();
  
#ifdef OCCT_DEBUG
  Standard_Boolean TRC=TRCE(SIX);if (TRC) debfilterPEI(SIX);
#endif
  
//                 BDS.Shape(SIX);
  TopOpeBRepDS_ListOfInterference& LI = BDS.ChangeShapeInterferences(SIX);
  TopOpeBRepDS_ListOfInterference lw, lE, lF, lUU, lall; lall.Assign(LI);

  // xpu : 270398 : 
  // deleting faulty interferences attached to section edge EIX
  // I = (T(Ftrasha),G,Ftrasha), Ftrasha sdm with FancestorofEIX
  FUN_purgeDSonSE(myHDS,SIX,lall);
  // xpu : 270398 (pro1260, edge 49,ftrasha 93)

#ifdef OCCT_DEBUG
  //Standard_Integer nUU =
#endif
            ::FUN_selectTRAUNKinterference(lall,lUU);
  FUN_resolveEUNKNOWN(lUU,BDS,SIX);
  lw.Append(lall);
  lw.Append(lUU);
  
#ifdef OCCT_DEBUG
  Standard_Integer nF =
#endif
           ::FUN_selectTRASHAinterference(lw,TopAbs_FACE,lF);
#ifdef OCCT_DEBUG
  Standard_Integer nE =
#endif
           ::FUN_selectTRASHAinterference(lw,TopAbs_EDGE,lE);
  
#ifdef OCCT_DEBUG
  if(TRC){
    if(nE||nF){cout<<endl;cout<<"-----------------------"<<endl;}
    if(nF){cout<<"EDGE "<<SIX<<" (FACE) : "<<nF<<endl;::FDS_dumpLI(lF,"  ");}
    if(nE){cout<<"EDGE "<<SIX<<" (EDGE) : "<<nE<<endl;::FDS_dumpLI(lE,"  ");}
    cout<<"Filter::ProcessEdgeInterferences on "<<SIX;
    cout<<" nlF = "<<nF<<" nlE = "<<nE<<endl;
  }
#endif
  
#ifdef OCCT_DEBUG
  if (TRC) cout <<"  on lF :"<<endl; 
#endif
  ::FUN_FilterEdge(lF,myHDS,SIX);
  
#ifdef OCCT_DEBUG
  if (TRC) cout <<"  on lF :"<<endl;  
#endif
//  ::FUN_unkeepEsymetrictransitions(lF,BDS,SIX);
  
#ifdef OCCT_DEBUG
  if (TRC) cout <<"  on lE :"<<endl;  
#endif
  ::FUN_FilterEdge(lE,myHDS,SIX);
  
  LI.Clear();
  LI.Append(lF);
  LI.Append(lE);
  
#ifdef OCCT_DEBUG
  if (TRC) {
    cout<<"Filter::ProcessEdgeInterferences on "<<SIX;
    cout<<" returns nlF = "<<nF<<" nlE = "<<nE<<endl;
  }
#endif
  
} //ProcessEdgeInterferences
