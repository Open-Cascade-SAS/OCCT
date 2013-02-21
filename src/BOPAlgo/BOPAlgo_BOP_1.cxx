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
#include <TopExp_Explorer.hxx>


//=======================================================================
//function : BuildSection
//purpose  : 
//=======================================================================
  void BOPAlgo_BOP::BuildSection()
{

  Standard_Integer i, j, k, nE, nF1, nF2, aNbPB, aNbFF;
  Standard_Boolean bFlag;
  TopoDS_Shape aRC;
  BRep_Builder aBB;
  BOPCol_MapOfShape aME;
  BOPCol_IndexedMapOfShape aME1, aME2;
  //
  myErrorStatus=0;
  //
  BOPTools_AlgoTools::MakeContainer(TopAbs_COMPOUND, aRC);  
  BOPDS_VectorOfInterfFF& aFFs=myDS->InterfFF();
  aNbFF=aFFs.Extent();
  //
  for (i=0; i<aNbFF; ++i) {
    BOPDS_InterfFF& aFF=aFFs(i);
    aFF.Indices(nF1, nF2);
    const BOPDS_FaceInfo& aFI1=myDS->FaceInfo(nF1);
    const BOPDS_FaceInfo& aFI2=myDS->FaceInfo(nF2);
    //
    const BOPDS_IndexedMapOfPaveBlock& aMPBIn1=aFI1.PaveBlocksIn();
    const BOPDS_IndexedMapOfPaveBlock& aMPBOn1=aFI1.PaveBlocksOn();
    const BOPDS_IndexedMapOfPaveBlock& aMPBSc1=aFI1.PaveBlocksSc();
    //
    const BOPDS_IndexedMapOfPaveBlock& aMPBIn2=aFI2.PaveBlocksIn();
    const BOPDS_IndexedMapOfPaveBlock& aMPBOn2=aFI2.PaveBlocksOn();
    //
    //1. Section edges
    aNbPB = aMPBSc1.Extent();
    for (j=1; j<=aNbPB; ++j) {
      const Handle(BOPDS_PaveBlock)& aPB=aMPBSc1(j);
      nE=aPB->Edge();
      const TopoDS_Shape& aE=myDS->Shape(nE);
      if (aME.Add(aE)) {
        aBB.Add(aRC, aE);
      }
    }
    //2. Common edges
    BOPDS_IndexedMapOfPaveBlock aMPB[4] = {aMPBOn2, aMPBIn1, aMPBIn2, aMPBOn1};
    for (k = 0; k < 3; ++k) {
      aNbPB = aMPB[k].Extent();
      for (j=1; j<=aNbPB; ++j) {
        const Handle(BOPDS_PaveBlock)& aPB=aMPB[k](j);
        bFlag = (k==0) ? aMPB[3].Contains(aPB) :
          (aMPB[k-1].Contains(aPB) || aMPB[k+1].Contains(aPB));
        if (bFlag) {
          nE=aPB->Edge();
          const TopoDS_Shape& aE=myDS->Shape(nE);
          if (aME.Add(aE)) {
            aBB.Add(aRC, aE);
          }
        }
      }
    }
    //3. Shared edges
    aME1.Clear();
    aME2.Clear();
    //
    const TopoDS_Face& aF1=(*(TopoDS_Face *)(&myDS->Shape(nF1)));
    const TopoDS_Face& aF2=(*(TopoDS_Face *)(&myDS->Shape(nF2)));
    //
    BOPTools::MapShapes(aF1, TopAbs_EDGE, aME1);
    BOPTools::MapShapes(aF2, TopAbs_EDGE, aME2);
    //
    aNbPB = aME1.Extent();
    for (j=1; j<=aNbPB; ++j) {
      const TopoDS_Shape& aE = aME1(j);
      if (aME2.Contains(aE)) {
        if (aME.Add(aE)) {
          aBB.Add(aRC, aE);
        }
      }
    }
  }
  //
  //case when arguments are the same
  if (!aNbFF) {
    if (myArgs[0].IsSame(myArgs[1])) {
      TopExp_Explorer anExpF, anExpE;
      //
      anExpF.Init(myArgs[0], TopAbs_FACE);
      for(; anExpF.More(); anExpF.Next()) {
        const TopoDS_Shape& aF = anExpF.Current();
        //
        anExpE.Init(aF, TopAbs_EDGE);
        for (; anExpE.More(); anExpE.Next()) {
          const TopoDS_Shape& aE = anExpE.Current();
          //
          if (aME.Add(aE)) {
            aBB.Add(aRC, aE);
          }
        }
      }
    }
  }
  //
  myShape=aRC;
}
