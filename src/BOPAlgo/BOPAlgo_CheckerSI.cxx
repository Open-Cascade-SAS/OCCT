// Created by: Peter KURNEV
// Copyright (c) 2010-2012 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
//
#include <BOPAlgo_CheckerSI.ixx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>

#include <BOPDS_DS.hxx>
#include <BOPDS_IteratorSI.hxx>
#include <BOPDS_PIteratorSI.hxx>
#include <BOPInt_Context.hxx>

#include <BOPDS_Interf.hxx>
#include <BOPDS_MapOfPassKey.hxx>
#include <BOPDS_PassKey.hxx>
#include <BOPDS_VectorOfInterfVV.hxx>
#include <BOPDS_VectorOfInterfVE.hxx>
#include <BOPDS_VectorOfInterfVE.hxx>
#include <BOPDS_VectorOfInterfVF.hxx>
#include <BOPDS_VectorOfInterfEF.hxx>
#include <BOPDS_VectorOfInterfFF.hxx>
#include <BOPDS_VectorOfPoint.hxx>
#include <BOPTools_AlgoTools.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_CheckerSI::BOPAlgo_CheckerSI()
:
  BOPAlgo_PaveFiller(),
  myLevelOfCheck(5)
{
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
  if (theLevel >= 0 && theLevel <= 5) {
    myLevelOfCheck = theLevel;
  }
}
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void BOPAlgo_CheckerSI::Init()
{
  myErrorStatus = 0;
  //
  if (!myArguments.Extent()) {
    myErrorStatus=10;
    return;
  }
  //
  Clear();
  //
  // 1. myDS
  myDS=new BOPDS_DS(myAllocator);
  myDS->SetArguments(myArguments);
  myDS->Init();
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
  myContext=new BOPInt_Context;
  //
  myErrorStatus=0;
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BOPAlgo_CheckerSI::Perform()
{
  //modified by NIZNHY-PKV Thu Sep 19 08:14:52 2013f
  try {
    OCC_CATCH_SIGNALS
    //
    BOPAlgo_PaveFiller::Perform();
    if (myErrorStatus) {
      return; 
    }
    //
    PostTreat();
  }
  catch (Standard_Failure) {
  }  
  /*  
  BOPAlgo_PaveFiller::Perform();
  if (myErrorStatus) {
   return; 
  }
  //  
  PostTreat(); 
  */
  //modified by NIZNHY-PKV Thu Sep 19 08:14:56 2013t
}
//=======================================================================
//function : PostTreat
//purpose  : 
//=======================================================================
void BOPAlgo_CheckerSI::PostTreat()
{
  Standard_Integer i, aNb, n1, n2; 
  BOPDS_PassKey aPK;
  //
  BOPDS_MapOfPassKey& aMPK=*((BOPDS_MapOfPassKey*)&myDS->Interferences());
  aMPK.Clear();
  //
  // 0
  BOPDS_VectorOfInterfVV& aVVs=myDS->InterfVV();
  aNb=aVVs.Extent();
  for (i=0; i!=aNb; ++i) {
    const BOPDS_InterfVV& aVV=aVVs(i);
    aVV.Indices(n1, n2);
    aPK.SetIds(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 1
  BOPDS_VectorOfInterfVE& aVEs=myDS->InterfVE();
  aNb=aVEs.Extent();
  for (i=0; i!=aNb; ++i) {
    const BOPDS_InterfVE& aVE=aVEs(i);
    aVE.Indices(n1, n2);
    aPK.SetIds(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 2
  BOPDS_VectorOfInterfEE& aEEs=myDS->InterfEE();
  aNb=aEEs.Extent();
  for (i=0; i!=aNb; ++i) {
    const BOPDS_InterfEE& aEE=aEEs(i);
    aEE.Indices(n1, n2);
    aPK.SetIds(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 3
  BOPDS_VectorOfInterfVF& aVFs=myDS->InterfVF();
  aNb=aVFs.Extent();
  for (i=0; i!=aNb; ++i) {
    const BOPDS_InterfVF& aVF=aVFs(i);
    aVF.Indices(n1, n2);
    aPK.SetIds(n1, n2);
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
    aPK.SetIds(n1, n2);
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
      const TopoDS_Face& aF1 = *((TopoDS_Face*)&myDS->Shape(n1));
      const TopoDS_Face& aF2 = *((TopoDS_Face*)&myDS->Shape(n2));
      bFlag=BOPTools_AlgoTools::AreFacesSameDomain(aF1, aF2, myContext);
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
    aPK.SetIds(n1, n2);
    aMPK.Add(aPK);
  }
}
