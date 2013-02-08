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


#include <BOPAlgo_PaveFiller.ixx>

#include <NCollection_IncAllocator.hxx>
#include <NCollection_BaseAllocator.hxx>

#include <Bnd_Box.hxx>

#include <TopoDS_Vertex.hxx>
#include <BRepBndLib.hxx>

#include <BOPCol_DataMapOfIntegerListOfInteger.hxx>
#include <BOPCol_MapOfInteger.hxx>
#include <BOPCol_ListOfShape.hxx>

#include <BOPDS_DS.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPDS_VectorOfInterfVV.hxx>
#include <BOPDS_ShapeInfo.hxx>
#include <BOPAlgo_Tools.hxx>


//=======================================================================
// function: PerformVV
// purpose: 
//=======================================================================
  void BOPAlgo_PaveFiller::PerformVV() 
{
  Standard_Boolean bWithSubShape;
  Standard_Integer n1, n2, iFlag, nX, n, aSize, i, aNbVV, j, iX, k, aNbBlocks;
  Handle(NCollection_IncAllocator) aAllocator;
  BOPCol_DataMapIteratorOfDataMapOfIntegerListOfInteger aItMILI;
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
  aVVs.SetStartSize(aSize);
  aVVs.SetIncrement(aSize);
  aVVs.Init();
  //
  //-----------------------------------------------------scope f
  aAllocator=new NCollection_IncAllocator();
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
          iX=aVVs.Append()-1;
          BOPDS_InterfVV& aVV=aVVs(iX);
          aVV.SetIndices(n1, n2);
          aVV.SetIndexNew(n);
        }
      }
    }
  }
  aNbVV=aVVs.Extent();
  //
  //-----------------------------------------------------scope t
  aLV.Clear();
  aMBlocks.Clear();
  aMILI.Clear();
  aAllocator.Nullify();
}
