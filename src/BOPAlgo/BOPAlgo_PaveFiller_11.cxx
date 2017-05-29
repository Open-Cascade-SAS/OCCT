// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2017 OPEN CASCADE SAS
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


#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Alerts.hxx>

#include <BOPDS_DS.hxx>
#include <BOPDS_MapOfCommonBlock.hxx>

#include <BRep_Builder.hxx>

#include <TopoDS_Compound.hxx>

//=======================================================================
//function : CheckSelfInterference
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::CheckSelfInterference()
{
  if (myArguments.Extent() == 1) {
    // Self-interference mode
    return;
  }
  //
  BRep_Builder aBB;
  //
  Standard_Integer i, aNbR = myDS->NbRanges();
  for (i = 0; i < aNbR; ++i) {
    const BOPDS_IndexRange& aR = myDS->Range(i);
    //
    // Map of connections of interfering shapes
    NCollection_IndexedDataMap<TopoDS_Shape,
                               BOPCol_IndexedMapOfShape,
                               TopTools_ShapeMapHasher> aMCSI;
    BOPDS_MapOfCommonBlock aMCBFence;
    //
    Standard_Integer j = aR.First(), aRLast = aR.Last();
    for (; j <= aRLast; ++j) {
      const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(j);
      if (!aSI.HasReference()) {
        // No pave blocks and no face info
        continue;
      }
      //
      const TopoDS_Shape& aS = aSI.Shape();
      //
      if (aSI.ShapeType() == TopAbs_EDGE) {
        if (aSI.HasFlag()) {
          continue;
        }
        //
        // Analyze the shared vertices and common blocks
        //
        BOPCol_MapOfInteger aMSubS;
        BOPCol_ListIteratorOfListOfInteger aItLI(aSI.SubShapes());
        for (; aItLI.More(); aItLI.Next()) {
          Standard_Integer nV = aItLI.Value();
          myDS->HasShapeSD(nV, nV);
          aMSubS.Add(nV);
        }
        //
        const BOPDS_ListOfPaveBlock& aLPB = myDS->PaveBlocks(j);
        Standard_Boolean bAnalyzeV = aLPB.Extent() > 1;
        //
        BOPDS_ListIteratorOfListOfPaveBlock aIt(aLPB);
        for (; aIt.More(); aIt.Next()) {
          const Handle(BOPDS_PaveBlock)& aPB = aIt.Value();
          //
          // Check the vertices
          if (bAnalyzeV) {
            Standard_Integer nV[2];
            aPB->Indices(nV[0], nV[1]);
            for (Standard_Integer k = 0; k < 2; ++k) {
              if (!aR.Contains(nV[k]) && !aMSubS.Contains(nV[k])) {
                // Add connection
                const TopoDS_Shape& aV = myDS->Shape(nV[k]);
                BOPCol_IndexedMapOfShape* pMSOr = aMCSI.ChangeSeek(aV);
                if (!pMSOr) {
                  pMSOr = &aMCSI(aMCSI.Add(aV, BOPCol_IndexedMapOfShape()));
                }
                pMSOr->Add(aS);
              }
            }
          }
          //
          // Check common blocks
          if (myDS->IsCommonBlock(aPB)) {
            const Handle(BOPDS_CommonBlock)& aCB = myDS->CommonBlock(aPB);
            if (aMCBFence.Add(aCB)) {
              const BOPDS_ListOfPaveBlock& aLPBCB = aCB->PaveBlocks();
              //
              BOPCol_ListOfInteger aLE;
              BOPDS_ListIteratorOfListOfPaveBlock aItCB(aLPBCB);
              for (; aItCB.More(); aItCB.Next()) {
                const Handle(BOPDS_PaveBlock)& aPBCB = aItCB.Value();
                Standard_Integer nEOr = aPBCB->OriginalEdge();
                if (aR.Contains(nEOr)) {
                  aLE.Append(nEOr);
                }
              }
              //
              if (aLE.Extent() > 1) {
                // Add warning
                TopoDS_Compound aWC;
                aBB.MakeCompound(aWC);
                //
                BOPCol_ListIteratorOfListOfInteger aItLE(aLE);
                for (; aItLE.More(); aItLE.Next()) {
                  const TopoDS_Shape& aE1 = myDS->Shape(aItLE.Value());
                  aBB.Add(aWC, aE1);
                }
                //
                AddWarning (new BOPAlgo_AlertSelfInterferingShape (aWC));
              }
            }
          }
        }
      }
      else if(aSI.ShapeType() == TopAbs_FACE) {
        // Analyze IN and Section vertices and edges of the faces
        const BOPDS_FaceInfo& aFI = myDS->FaceInfo(j);
        //
        for (Standard_Integer k = 0; k < 2; ++k) {
          const BOPCol_MapOfInteger& aMVF = !k ? aFI.VerticesIn() : aFI.VerticesSc();
          BOPCol_MapIteratorOfMapOfInteger aItM(aMVF);
          for (; aItM.More(); aItM.Next()) {
            const TopoDS_Shape& aV = myDS->Shape(aItM.Value());
            // add connection
            BOPCol_IndexedMapOfShape* pMSOr = aMCSI.ChangeSeek(aV);
            if (!pMSOr) {
              pMSOr = &aMCSI(aMCSI.Add(aV, BOPCol_IndexedMapOfShape()));
            }
            pMSOr->Add(aS);
          }
        }
        //
        for (Standard_Integer k = 0; k < 2; ++k) {
          const BOPDS_IndexedMapOfPaveBlock& aMPBF = !k ? aFI.PaveBlocksIn() : aFI.PaveBlocksSc();
          Standard_Integer iPB, aNbPB = aMPBF.Extent();
          for (iPB = 1; iPB <= aNbPB; ++iPB) {
            const Handle(BOPDS_PaveBlock)& aPB = aMPBF(iPB);
            const TopoDS_Shape& aE = myDS->Shape(aPB->Edge());
            // add connection
            BOPCol_IndexedMapOfShape* pMSOr = aMCSI.ChangeSeek(aE);
            if (!pMSOr) {
              pMSOr = &aMCSI(aMCSI.Add(aE, BOPCol_IndexedMapOfShape()));
            }
            pMSOr->Add(aS);
          }
        }
      }
    }
    //
    // Analyze connections
    Standard_Integer aNbC = aMCSI.Extent();
    for (j = 1; j <= aNbC; ++j) {
      const BOPCol_IndexedMapOfShape& aMCS = aMCSI(j);
      if (aMCS.Extent() > 1) {
        // Add self-interference warning
        TopoDS_Compound aWC;
        aBB.MakeCompound(aWC);
        //
        Standard_Integer iS, aNbS = aMCS.Extent();
        for (iS = 1; iS <= aNbS; ++iS) {
          const TopoDS_Shape& aSx = aMCS(iS);
          aBB.Add(aWC, aSx);
        }
        AddWarning (new BOPAlgo_AlertSelfInterferingShape (aWC));
      }
    }
  }
}
