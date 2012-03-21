// Created on: 1997-04-22
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
#include <TopOpeBRepDS_ListOfShapeOn1State.hxx>
#include <TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State.hxx>

#ifdef DEB
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GetcontextNOPFI();
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettracePFI();
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettracePI();
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceSPSX(const Standard_Integer);
static Standard_Boolean TRCF(const Standard_Integer F) {
  Standard_Boolean b1 = TopOpeBRepDS_GettracePFI();
  Standard_Boolean b2 = TopOpeBRepDS_GettracePI();
  Standard_Boolean b3 = TopOpeBRepDS_GettraceSPSX(F);
  return (b1 || b2 || b3);
}
#endif

Standard_EXPORT Standard_Integer FUN_unkeepFdoubleGBoundinterferences(TopOpeBRepDS_ListOfInterference& LI,const TopOpeBRepDS_DataStructure& BDS,const Standard_Integer SIX);
Standard_EXPORT void FUN_resolveFUNKNOWN
(TopOpeBRepDS_ListOfInterference& LI,TopOpeBRepDS_DataStructure& BDS,
 const Standard_Integer SIX,
 const TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State& MEsp,
 TopOpeBRepTool_PShapeClassifier pClassif);

#ifdef DEB
void debfilfac(const Standard_Integer I) {cout<<"+ + + + debfilfac "<<I<<endl;}
void debpfi(const Standard_Integer I) {cout<<"+ + + + debpfi "<<I<<endl;}
#endif

// -------------------------------------------------------
void FUN_FilterFace
// -------------------------------------------------------
#ifdef DEB
(TopOpeBRepDS_ListOfInterference& LI,const TopOpeBRepDS_DataStructure& /*BDS*/,const Standard_Integer SIX)
#else
(TopOpeBRepDS_ListOfInterference& ,const TopOpeBRepDS_DataStructure& ,const Standard_Integer )
#endif
{
#ifdef DEB
  Standard_Boolean TRC=TRCF(SIX);
  Standard_Integer nI = 0;
  if (TRC) debfilfac(SIX);
#endif

#ifdef DEB
  nI = LI.Extent();
  if (TRC) cout <<"before FUN_FilterFace on "<<SIX<<" nI = "<<nI<<endl;
#endif

//  ::FUN_unkeepFdoubleGBoundinterferences(LI,BDS,SIX);

#ifdef DEB
  nI = LI.Extent();
  if (TRC) cout <<"after unkeepFinterferences nI = "<<nI<<endl;
#endif
}

//=======================================================================
//function : ProcessFaceInterferences
//purpose  : 
//=======================================================================
void TopOpeBRepDS_Filter::ProcessFaceInterferences
(const Standard_Integer SIX,const TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State& MEsp)
{
  TopOpeBRepDS_DataStructure& BDS = myHDS->ChangeDS();
  
#ifdef DEB
  Standard_Boolean TRC=TRCF(SIX); 
  Standard_Integer nI = 0;
  if (TRC) debpfi(SIX);
#endif
  
//                 BDS.Shape(SIX);
  TopOpeBRepDS_ListOfInterference& LI = BDS.ChangeShapeInterferences(SIX);
  ::FUN_reducedoublons(LI,BDS,SIX);

#ifdef DEB
  nI = LI.Extent();
  if (TRC) cout <<"after reducedoublons nI = "<<nI<<endl;
#endif

  TopOpeBRepDS_ListOfInterference lw, lE, lFE, lFEF, lF, lUU, lall; lall.Assign(LI);

#ifdef DEB
  Standard_Integer nUU =
#endif
            ::FUN_selectTRAUNKinterference(lall,lUU);
  FUN_resolveFUNKNOWN(lUU,BDS,SIX,MEsp,myPShapeClassif);
  lw.Append(lall);
  lw.Append(lUU);

  Standard_Integer nF = ::FUN_selectTRASHAinterference(lw,TopAbs_FACE,lF);
  Standard_Integer nFE = ::FUN_selectGKinterference(lF,TopOpeBRepDS_EDGE,lFE);
  Standard_Integer nFEF = ::FUN_selectSKinterference(lFE,TopOpeBRepDS_FACE,lFEF);
  Standard_Integer nE = ::FUN_selectTRASHAinterference(lw,TopAbs_EDGE,lE);


#ifdef DEB
  if(TRC){
    if(nF||nFE||nFEF||nE){cout<<endl;cout<<"-----------------------"<<endl;}
    if(nUU) {cout<<"FACE "<<SIX<<" UNKNOWN : "<<nUU<<endl;FDS_dumpLI(lUU,"  ");}
    if(nF) {cout<<"FACE "<<SIX<<" (FACE) : "<<nF<<endl;FDS_dumpLI(lF,"  ");}
    if(nFE){cout<<"FACE "<<SIX<<" (FACE)(GK EDGE) : "<<nFE<<endl;FDS_dumpLI(lFE,"  ");}
    if(nFEF){cout<<"FACE "<<SIX<<" (FACE)(GK EDGE)(SK FACE) : "<<nFEF<<endl;FDS_dumpLI(lFEF,"  ");}
    if(nE) {cout<<"FACE "<<SIX<<" (EDGE) : "<<nE<<endl;FDS_dumpLI(lE,"  ");}
  }
#endif

  ::FUN_FilterFace(lFEF,BDS,SIX);

  nF = lF.Extent();
  nFE = lFE.Extent();
  nFEF = lFEF.Extent();
  nE = lE.Extent();
#ifdef DEB
  if(TRC){
    if(nF||nFE||nFEF||nE)cout<<endl;
    if(nF) {cout<<"FACE "<<SIX<<" (FACE) : "<<nF<<endl;FDS_dumpLI(lF,"  ");}
    if(nFE){cout<<"FACE "<<SIX<<" (FACE)(GK EDGE) : "<<nFE<<endl;FDS_dumpLI(lFE,"  ");}
    if(nFEF){cout<<"FACE "<<SIX<<" (FACE)(GK EDGE)(SK FACE) : "<<nFEF<<endl;FDS_dumpLI(lFEF,"  ");}
    if(nE) {cout<<"FACE "<<SIX<<" (EDGE) : "<<nE<<endl;FDS_dumpLI(lE,"  ");}
    cout<<"-----------------------"<<endl;
  }
#endif
  
  LI.Clear();
  LI.Append(lF);
  LI.Append(lFE);
  LI.Append(lFEF);
  LI.Append(lE);

} // ProcessFaceInterferences
