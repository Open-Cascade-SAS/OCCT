// Created by: Peter KURNEV
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


#include <BOPAlgo_BOP.ixx>

#include <BOPDS_DS.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_CommonBlock.hxx>
#include <BOPDS_FaceInfo.hxx>
#include <BOPDS_ListOfPaveBlock.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_VectorOfListOfPaveBlock.hxx>
#include <BOPDS_VectorOfFaceInfo.hxx>

#include <BOPTools.hxx>
#include <BOPTools_AlgoTools.hxx>

#include <BOPCol_MapOfShape.hxx>

#include <BRep_Builder.hxx>


//=======================================================================
//function : BuildSection
//purpose  : 
//=======================================================================
  void BOPAlgo_BOP::BuildSection()
{

  Standard_Integer i, aNb, nE, aNbPB, j;
  TopoDS_Shape aRC;
  BRep_Builder aBB;
  BOPCol_MapOfShape aME;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  //
  myErrorStatus=0;
  //
  BOPTools_AlgoTools::MakeContainer(TopAbs_COMPOUND, aRC);  
  //1. Common Blocks
  const BOPDS_VectorOfListOfPaveBlock& aPBP=myDS->PaveBlocksPool();
  aNb=aPBP.Extent();
  for (i=0; i<aNb; ++i) {
    const BOPDS_ListOfPaveBlock& aLPB=aPBP(i);
    //
    aItLPB.Initialize(aLPB);
    for (; aItLPB.More(); aItLPB.Next()) {
      const Handle(BOPDS_PaveBlock)& aPB=aItLPB.Value();
      if (aPB->IsCommonBlock()) {
        const Handle(BOPDS_CommonBlock)& aCB=aPB->CommonBlock();
        const Handle(BOPDS_PaveBlock)& aPB1=aCB->PaveBlock1();
        nE=aPB1->Edge();
        const TopoDS_Shape& aE=myDS->Shape(nE);
        if (aME.Add(aE)) {
          aBB.Add(aRC, aE);
        }
      }
    }
  }
  //2. Section Edges
  const BOPDS_VectorOfFaceInfo& aFIP=myDS->FaceInfoPool();
  aNb=aFIP.Extent();
  for (i=0; i<aNb; ++i) {
    const BOPDS_FaceInfo& aFI=aFIP(i);
    const BOPDS_IndexedMapOfPaveBlock& aMPB=aFI.PaveBlocksSc();
    //
    aNbPB=aMPB.Extent();
    for (j=1; j<=aNbPB; ++j) {
      const Handle(BOPDS_PaveBlock)& aPB=aMPB(j);
      nE=aPB->Edge();
      const TopoDS_Shape& aE=myDS->Shape(nE);
      if (aME.Add(aE)) {
        aBB.Add(aRC, aE);
      }
    }
  }
  //3. Shared Edges
  BOPCol_IndexedMapOfShape aMEO, aMET;
  BOPCol_MapIteratorOfMapOfShape aItME;
  //
  BOPTools::MapShapes(myArgs[0], TopAbs_EDGE, aMEO);
  BOPTools::MapShapes(myArgs[1], TopAbs_EDGE, aMET);
  //
  aItME.Initialize(aMEO);
  for (; aItME.More(); aItME.Next()) {
    const TopoDS_Shape& aE = aItME.Value();
    if (aMET.Contains(aE)) {
      if (aME.Add(aE)) {
        aBB.Add(aRC, aE);
      }
    }
  }

  myShape=aRC;
}
