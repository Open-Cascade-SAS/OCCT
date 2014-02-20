// Created on: 1997-04-22
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
extern Standard_Boolean TopOpeBRepDS_GetcontextNOPFI();
extern Standard_Boolean TopOpeBRepDS_GettracePFI();
extern Standard_Boolean TopOpeBRepDS_GettracePI();
extern Standard_Boolean TopOpeBRepDS_GettraceSPSX(const Standard_Integer);
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

#ifdef DEB
  Standard_Integer nF, nFE, nFEF, nE;
#endif

  ::FUN_selectTRASHAinterference(lw,TopAbs_FACE,lF);
  ::FUN_selectGKinterference(lF,TopOpeBRepDS_EDGE,lFE);
  ::FUN_selectSKinterference(lFE,TopOpeBRepDS_FACE,lFEF);
  ::FUN_selectTRASHAinterference(lw,TopAbs_EDGE,lE);

#ifdef DEB
  nF = lF.Extent();
  nFE = lFE.Extent();
  nFEF = lFEF.Extent();
  nE = lE.Extent();
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

#ifdef DEB
  nF = lF.Extent();
  nFE = lFE.Extent();
  nFEF = lFEF.Extent();
  nE = lE.Extent();
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
