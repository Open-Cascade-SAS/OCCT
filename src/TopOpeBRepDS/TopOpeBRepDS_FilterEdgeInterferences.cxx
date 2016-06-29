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

  FUN_orderFFsamedomain(LI,HDS,SIX);
  FUN_orderSTATETRANS(LI,HDS,SIX);
  FUN_unkeepEinterferences(LI,BDS,SIX);
  FUN_reducedoublons(LI,BDS,SIX);
}

//=======================================================================
//function : ProcessEdgeInterferences
//purpose  : Filter
//=======================================================================

void TopOpeBRepDS_Filter::ProcessEdgeInterferences
(const Standard_Integer SIX)
{
  TopOpeBRepDS_DataStructure& BDS = myHDS->ChangeDS();
//                 BDS.Shape(SIX);
  TopOpeBRepDS_ListOfInterference& LI = BDS.ChangeShapeInterferences(SIX);
  TopOpeBRepDS_ListOfInterference lw, lE, lF, lUU, lall; lall.Assign(LI);

  // xpu : 270398 : 
  // deleting faulty interferences attached to section edge EIX
  // I = (T(Ftrasha),G,Ftrasha), Ftrasha sdm with FancestorofEIX
  FUN_purgeDSonSE(myHDS,SIX,lall);
  // xpu : 270398 (pro1260, edge 49,ftrasha 93)
  ::FUN_selectTRAUNKinterference(lall,lUU);
  FUN_resolveEUNKNOWN(lUU,BDS,SIX);
  lw.Append(lall);
  lw.Append(lUU);
  ::FUN_selectTRASHAinterference(lw,TopAbs_FACE,lF);
  ::FUN_selectTRASHAinterference(lw,TopAbs_EDGE,lE);
  ::FUN_FilterEdge(lF,myHDS,SIX);
//  ::FUN_unkeepEsymetrictransitions(lF,BDS,SIX);
  ::FUN_FilterEdge(lE,myHDS,SIX);
  
  LI.Clear();
  LI.Append(lF);
  LI.Append(lE);
} //ProcessEdgeInterferences
