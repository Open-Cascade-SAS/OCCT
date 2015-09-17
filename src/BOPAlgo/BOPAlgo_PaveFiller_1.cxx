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


#include <Bnd_Box.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_SectionAttribute.hxx>
#include <BOPAlgo_Tools.hxx>
#include <BOPCol_DataMapOfIntegerListOfInteger.hxx>
#include <BOPCol_ListOfShape.hxx>
#include <BOPCol_MapOfInteger.hxx>
#include <BOPDS_Curve.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_ShapeInfo.hxx>
#include <BOPDS_VectorOfInterfVV.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BRepBndLib.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_Context.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

//=======================================================================
// function: PerformVV
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::PerformVV() 
{
  Standard_Boolean bWithSubShape;
  Standard_Integer n1, n2, iFlag, nX, n, aSize, i, j, k, aNbBlocks;
  Handle(NCollection_BaseAllocator) aAllocator;
  BOPCol_ListIteratorOfListOfInteger aItLI, aItLI2;
  TopoDS_Vertex aVn;
  BOPDS_ShapeInfo aSIn;
  //
  myErrorStatus=0;
  //
  myIterator->Initialize(TopAbs_VERTEX, TopAbs_VERTEX);
  aSize=myIterator->ExpectedLength();
  if (!aSize) {
    return; 
  }
  //
  aSIn.SetShapeType(TopAbs_VERTEX);
  
  BOPDS_VectorOfInterfVV& aVVs=myDS->InterfVV();
  aVVs.SetIncrement(aSize);
  //
  //-----------------------------------------------------scope f
  aAllocator=
    NCollection_BaseAllocator::CommonBaseAllocator();
  BOPCol_IndexedDataMapOfIntegerListOfInteger aMILI(100, aAllocator);
  BOPCol_DataMapOfIntegerListOfInteger aMBlocks(100, aAllocator);
  BOPCol_ListOfShape aLV(aAllocator);
  //
  // 1. Map V/LV
  for (; myIterator->More(); myIterator->Next()) {
    myIterator->Value(n1, n2, bWithSubShape);
    //
    const TopoDS_Vertex& aV1=(*(TopoDS_Vertex *)(&myDS->Shape(n1))); 
    const TopoDS_Vertex& aV2=(*(TopoDS_Vertex *)(&myDS->Shape(n2))); 
    //
    iFlag=BOPTools_AlgoTools::ComputeVV(aV1, aV2);
    if (!iFlag) {
      BOPAlgo_Tools::FillMap(n1, n2, aMILI, aAllocator);
    }
  } 
  //
  // 2. Make blocks
  BOPAlgo_Tools::MakeBlocksCnx(aMILI, aMBlocks, aAllocator);
  //
  // 3. Make vertices
  aNbBlocks=aMBlocks.Extent();
  for (k=0; k<aNbBlocks; ++k) {
    const BOPCol_ListOfInteger& aLI=aMBlocks.Find(k);
    //
    aLV.Clear();
    aItLI.Initialize(aLI);
    for (; aItLI.More(); aItLI.Next()) {
      nX=aItLI.Value();
      const TopoDS_Shape& aV=myDS->Shape(nX);
      aLV.Append(aV);
    }
    //
    BOPTools_AlgoTools::MakeVertex(aLV, aVn);
    //
    // Appennd new vertex to the DS
    aSIn.SetShape(aVn);
    n=myDS->Append(aSIn);
    //
    BOPDS_ShapeInfo& aSIDS=myDS->ChangeShapeInfo(n);
    Bnd_Box& aBox=aSIDS.ChangeBox();
    BRepBndLib::Add(aVn, aBox);
    //
    // Fill ShapesSD
    aItLI.Initialize(aLI);
    for (i=0; aItLI.More(); aItLI.Next(), ++i) {
      n1=aItLI.Value();
      myDS->AddShapeSD(n1, n);
      //
      aItLI2.Initialize(aLI);
      for (j=0; aItLI2.More(); aItLI2.Next(), ++j) {
        if (j>i) {
          n2=aItLI2.Value();
          //
          myDS->AddInterf(n1, n2);
          BOPDS_InterfVV& aVV=aVVs.Append1();
          //
          aVV.SetIndices(n1, n2);
          aVV.SetIndexNew(n);
        }
      }
    }
  }
  //
  //-----------------------------------------------------scope t
  aLV.Clear();
  aMBlocks.Clear();
  aMILI.Clear();
}
