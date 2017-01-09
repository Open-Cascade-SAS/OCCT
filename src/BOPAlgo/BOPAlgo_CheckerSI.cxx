// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

//

#include <BOPAlgo_CheckerSI.hxx>
#include <BOPCol_MapOfShape.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_IteratorSI.hxx>
#include <BOPDS_MapOfPair.hxx>
#include <BOPDS_Pair.hxx>
#include <BOPDS_PIteratorSI.hxx>
#include <BOPDS_VectorOfInterfEF.hxx>
#include <BOPDS_VectorOfInterfFF.hxx>
#include <BOPDS_VectorOfInterfVE.hxx>
#include <BOPDS_VectorOfInterfVF.hxx>
#include <BOPDS_VectorOfInterfVV.hxx>
#include <BOPTools.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <IntTools_Context.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TopTools_ListOfShape.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_CheckerSI::BOPAlgo_CheckerSI()
:
  BOPAlgo_PaveFiller()
{
  myLevelOfCheck=BOPDS_DS::NbInterfTypes()-1;
  myNonDestructive=Standard_True;
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
BOPAlgo_CheckerSI::~BOPAlgo_CheckerSI()
{
}
//=======================================================================
//function : SetLevelOfCheck
//purpose  : 
//=======================================================================
void BOPAlgo_CheckerSI::SetLevelOfCheck(const Standard_Integer theLevel)
{
  Standard_Integer aNbLists;
  //
  aNbLists=BOPDS_DS::NbInterfTypes();
  if (theLevel >= 0 && theLevel < aNbLists) {
    myLevelOfCheck = theLevel;
  }
}
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void BOPAlgo_CheckerSI::Init()
{
  myErrorStatus=0;
  //
  Clear();
  //
  // 1. myDS
  myDS=new BOPDS_DS(myAllocator);
  myDS->SetArguments(myArguments);
  myDS->Init(myFuzzyValue);
  //
  // 2.myIterator 
  BOPDS_PIteratorSI theIterSI=new BOPDS_IteratorSI(myAllocator);
  theIterSI->SetDS(myDS);
  theIterSI->Prepare();
  theIterSI->UpdateByLevelOfCheck(myLevelOfCheck);
  //
  myIterator=theIterSI;
  //
  // 3 myContext
  myContext=new IntTools_Context;
  //
  myErrorStatus=0;
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BOPAlgo_CheckerSI::Perform()
{
  try {
    OCC_CATCH_SIGNALS
    //
    myErrorStatus = 0;
    if (myArguments.Extent() != 1) {
      myErrorStatus = 10;
      return;
    }
    //
    // Perform intersection of sub shapes
    BOPAlgo_PaveFiller::Perform();
    //
    // Perform intersection with solids
    if (!myErrorStatus)
      PerformVZ();
    //
    if (!myErrorStatus)
      PerformEZ();
    //
    if (!myErrorStatus)
      PerformFZ();
    //
    if (!myErrorStatus)
      PerformZZ();
    //
    // Treat the intersection results
    PostTreat();
  }
  //
  catch (Standard_Failure) {
    myErrorStatus = 11;
  }
}
//=======================================================================
//function : PostTreat
//purpose  : 
//=======================================================================
void BOPAlgo_CheckerSI::PostTreat()
{
  Standard_Integer i, aNb, n1, n2; 
  BOPDS_Pair aPK;
  //
  myErrorStatus=0;
  //
  BOPDS_MapOfPair& aMPK=
    *((BOPDS_MapOfPair*)&myDS->Interferences());
  aMPK.Clear();
  //
  // 0
  BOPDS_VectorOfInterfVV& aVVs=myDS->InterfVV();
  aNb=aVVs.Extent();
  for (i=0; i!=aNb; ++i) {
    const BOPDS_InterfVV& aVV=aVVs(i);
    aVV.Indices(n1, n2);
    if (myDS->IsNewShape(n1) || myDS->IsNewShape(n2)) {
      continue;
    }
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 1
  BOPDS_VectorOfInterfVE& aVEs=myDS->InterfVE();
  aNb=aVEs.Extent();
  for (i=0; i!=aNb; ++i) {
    const BOPDS_InterfVE& aVE=aVEs(i);
    aVE.Indices(n1, n2);
    if (myDS->IsNewShape(n1) || myDS->IsNewShape(n2)) {
      continue;
    }
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 2
  BOPDS_VectorOfInterfEE& aEEs=myDS->InterfEE();
  aNb=aEEs.Extent();
  for (i=0; i!=aNb; ++i) {
    const BOPDS_InterfEE& aEE=aEEs(i);
    aEE.Indices(n1, n2);
    if (myDS->IsNewShape(n1) || myDS->IsNewShape(n2)) {
      continue;
    }
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 3
  BOPDS_VectorOfInterfVF& aVFs=myDS->InterfVF();
  aNb=aVFs.Extent();
  for (i=0; i!=aNb; ++i) {
    const BOPDS_InterfVF& aVF=aVFs(i);
    aVF.Indices(n1, n2);
    if (myDS->IsNewShape(n1) || myDS->IsNewShape(n2)) {
      continue;
    }
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 4
  BOPDS_VectorOfInterfEF& aEFs=myDS->InterfEF();
  aNb=aEFs.Extent();
  for (i=0; i!=aNb; ++i) {
    const BOPDS_InterfEF& aEF=aEFs(i);
    if (aEF.CommonPart().Type()==TopAbs_SHAPE) {
      continue;
    }
    aEF.Indices(n1, n2);
    if (myDS->IsNewShape(n1) || myDS->IsNewShape(n2)) {
      continue;
    }
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 5
  BOPDS_VectorOfInterfFF& aFFs=myDS->InterfFF();
  aNb=aFFs.Extent();
  for (i=0; i!=aNb; ++i) {
    Standard_Boolean bTangentFaces, bFlag;
    Standard_Integer aNbC, aNbP, j, iFound;
    //
    const BOPDS_InterfFF& aFF=aFFs(i);
    aFF.Indices(n1, n2);
    //
    bTangentFaces=aFF.TangentFaces();
    aNbP=aFF.Points().Extent();
    const BOPDS_VectorOfCurve& aVC=aFF.Curves();
    aNbC=aVC.Extent();
    if (!aNbP && !aNbC && !bTangentFaces) {
      continue;
    }
    //
    iFound=0;
    if (bTangentFaces) {
      const TopoDS_Face& aF1=*((TopoDS_Face*)&myDS->Shape(n1));
      const TopoDS_Face& aF2=*((TopoDS_Face*)&myDS->Shape(n2));
      bFlag=BOPTools_AlgoTools::AreFacesSameDomain
        (aF1, aF2, myContext, myFuzzyValue);
      if (bFlag) {
        ++iFound;
      }
    }
    else {
      for (j=0; j!=aNbC; ++j) {
        const BOPDS_Curve& aNC=aVC(j);
        const BOPDS_ListOfPaveBlock& aLPBC=aNC.PaveBlocks();
        if (aLPBC.Extent()) {
          ++iFound;
          break;
        }
      }
    }
    //
    if (!iFound) {
      continue;
    }
    //
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  //
  // 6
  BOPDS_VectorOfInterfVZ& aVZs=myDS->InterfVZ();
  aNb=aVZs.Extent();
  for (i=0; i!=aNb; ++i) {
    //
    const BOPDS_InterfVZ& aVZ=aVZs(i);
    aVZ.Indices(n1, n2);
    if (myDS->IsNewShape(n1) || myDS->IsNewShape(n2)) {
      continue;
    }
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 7
  BOPDS_VectorOfInterfEZ& aEZs=myDS->InterfEZ();
  aNb=aEZs.Extent();
  for (i=0; i!=aNb; ++i) {
    //
    const BOPDS_InterfEZ& aEZ=aEZs(i);
    aEZ.Indices(n1, n2);
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 8
  BOPDS_VectorOfInterfFZ& aFZs=myDS->InterfFZ();
  aNb=aFZs.Extent();
  for (i=0; i!=aNb; ++i) {
    //
    const BOPDS_InterfFZ& aFZ=aFZs(i);
    aFZ.Indices(n1, n2);
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 9
  BOPDS_VectorOfInterfZZ& aZZs=myDS->InterfZZ();
  aNb=aZZs.Extent();
  for (i=0; i!=aNb; ++i) {
    //
    const BOPDS_InterfZZ& aZZ=aZZs(i);
    aZZ.Indices(n1, n2);
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
}
