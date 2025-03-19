// Created by: Peter KURNEV
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

#include <BOPAlgo_PaveFiller.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_ListOfPaveBlock.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_ShapeInfo.hxx>
#include <BOPTools_Parallel.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_ShrunkRange.hxx>
#include <NCollection_Vector.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

//
//=================================================================================================

class BOPAlgo_ShrunkRange : public IntTools_ShrunkRange
{
public:
  BOPAlgo_ShrunkRange()
      : IntTools_ShrunkRange()
  {
  }

  //
  virtual ~BOPAlgo_ShrunkRange() {}

  //
  void SetPaveBlock(const Handle(BOPDS_PaveBlock)& aPB) { myPB = aPB; }

  //
  Handle(BOPDS_PaveBlock)& PaveBlock() { return myPB; }

  //
  virtual void Perform() { IntTools_ShrunkRange::Perform(); }

  //
protected:
  Handle(BOPDS_PaveBlock) myPB;
};

//
//=======================================================================
typedef NCollection_Vector<BOPAlgo_ShrunkRange> BOPAlgo_VectorOfShrunkRange;

//=================================================================================================

void BOPAlgo_PaveFiller::FillShrunkData(const TopAbs_ShapeEnum aType1,
                                        const TopAbs_ShapeEnum aType2)
{
  myIterator->Initialize(aType1, aType2);
  Standard_Integer iSize = myIterator->ExpectedLength();
  if (!iSize)
  {
    return;
  }
  //
  Standard_Integer                    i, nS[2], nE, nV1, nV2, aNbVSD, k;
  Standard_Real                       aT1, aT2;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  TColStd_MapOfInteger                aMI;
  BOPAlgo_VectorOfShrunkRange         aVSD;
  TopAbs_ShapeEnum                    aType[2] = {aType1, aType2};
  //
  for (; myIterator->More(); myIterator->Next())
  {
    myIterator->Value(nS[0], nS[1]);
    //
    for (i = 0; i < 2; ++i)
    {
      nE = nS[i];
      if (aType[i] != TopAbs_EDGE || !aMI.Add(nE))
      {
        continue;
      }
      //
      const BOPDS_ShapeInfo& aSIE = myDS->ShapeInfo(nE);
      if (aSIE.HasFlag())
      {
        continue;
      }
      //
      BOPDS_ListOfPaveBlock& aLPB = myDS->ChangePaveBlocks(nE);
      aItLPB.Initialize(aLPB);
      for (; aItLPB.More(); aItLPB.Next())
      {
        const Handle(BOPDS_PaveBlock)& aPB = aItLPB.ChangeValue();
        if (aPB->HasShrunkData() && myDS->IsValidShrunkData(aPB))
        {
          continue;
        }
        //
        // FillShrunkData(aPB);
        aPB->Indices(nV1, nV2);
        aPB->Range(aT1, aT2);
        //
        const TopoDS_Edge& aE = (*(TopoDS_Edge*)(&aSIE.Shape()));
        //
        const TopoDS_Vertex& aV1 = (*(TopoDS_Vertex*)(&myDS->Shape(nV1)));
        //
        const TopoDS_Vertex& aV2 = (*(TopoDS_Vertex*)(&myDS->Shape(nV2)));
        //
        BOPAlgo_ShrunkRange& aSD = aVSD.Appended();
        //
        aSD.SetPaveBlock(aPB);
        aSD.SetData(aE, aT1, aT2, aV1, aV2);
      }
    }
  }
  //
  aNbVSD = aVSD.Length();
  //=============================================================
  BOPTools_Parallel::Perform(myRunParallel, aVSD, myContext);
  //=============================================================
  //
  for (k = 0; k < aNbVSD; ++k)
  {
    BOPAlgo_ShrunkRange& aSD = aVSD(k);
    AnalyzeShrunkData(aSD.PaveBlock(), aSD);
  }
}
