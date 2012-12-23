// Created on: 1997-04-21
// Created by: Prestataire Mary FABIEN
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <TopOpeBRepDS_Filter.ixx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_ListOfInterference.hxx>
#include <TopOpeBRepDS_ListIteratorOfListOfInterference.hxx>
#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>
#include <TopOpeBRepDS_define.hxx>

#ifdef DEB
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
  
#ifdef DEB
  Standard_Boolean TRC=TRCE(SIX); 
  if (TRC) debfilter(SIX);
  if (TRC) cout<<"FUN_FilterEdge on "<<SIX<<" nI = "<<LI.Extent()<<endl;
#endif

  FUN_orderFFsamedomain(LI,HDS,SIX);
  FUN_orderSTATETRANS(LI,HDS,SIX);
  FUN_unkeepEinterferences(LI,BDS,SIX);
  FUN_reducedoublons(LI,BDS,SIX);
  
#ifdef DEB
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
  
#ifdef DEB
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

#ifdef DEB
  //Standard_Integer nUU =
#endif
            ::FUN_selectTRAUNKinterference(lall,lUU);
  FUN_resolveEUNKNOWN(lUU,BDS,SIX);
  lw.Append(lall);
  lw.Append(lUU);
  
#ifdef DEB
  Standard_Integer nF =
#endif
           ::FUN_selectTRASHAinterference(lw,TopAbs_FACE,lF);
#ifdef DEB
  Standard_Integer nE =
#endif
           ::FUN_selectTRASHAinterference(lw,TopAbs_EDGE,lE);
  
#ifdef DEB
  if(TRC){
    if(nE||nF){cout<<endl;cout<<"-----------------------"<<endl;}
    if(nF){cout<<"EDGE "<<SIX<<" (FACE) : "<<nF<<endl;::FDS_dumpLI(lF,"  ");}
    if(nE){cout<<"EDGE "<<SIX<<" (EDGE) : "<<nE<<endl;::FDS_dumpLI(lE,"  ");}
    cout<<"Filter::ProcessEdgeInterferences on "<<SIX;
    cout<<" nlF = "<<nF<<" nlE = "<<nE<<endl;
  }
#endif
  
#ifdef DEB
  if (TRC) cout <<"  on lF :"<<endl; 
#endif
  ::FUN_FilterEdge(lF,myHDS,SIX);
  
#ifdef DEB
  if (TRC) cout <<"  on lF :"<<endl;  
#endif
//  ::FUN_unkeepEsymetrictransitions(lF,BDS,SIX);
  
#ifdef DEB
  if (TRC) cout <<"  on lE :"<<endl;  
#endif
  ::FUN_FilterEdge(lE,myHDS,SIX);
  
  LI.Clear();
  LI.Append(lF);
  LI.Append(lE);
  
#ifdef DEB
  if (TRC) {
    cout<<"Filter::ProcessEdgeInterferences on "<<SIX;
    cout<<" returns nlF = "<<nF<<" nlE = "<<nE<<endl;
  }
#endif
  
} //ProcessEdgeInterferences
