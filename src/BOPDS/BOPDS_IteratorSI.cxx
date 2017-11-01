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

#include <Bnd_Box.hxx>
#include <Bnd_OBB.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_IndexRange.hxx>
#include <BOPDS_IteratorSI.hxx>
#include <BOPDS_MapOfPair.hxx>
#include <BOPDS_Pair.hxx>
#include <BOPDS_ShapeInfo.hxx>
#include <BOPDS_Tools.hxx>
#include <BOPTools_BoxBndTree.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_Context.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <TColStd_DataMapOfIntegerListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>

//
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPDS_IteratorSI::BOPDS_IteratorSI()
:
  BOPDS_Iterator()
{
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPDS_IteratorSI::BOPDS_IteratorSI
  (const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPDS_Iterator(theAllocator)
{
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
BOPDS_IteratorSI::~BOPDS_IteratorSI()
{
}
//=======================================================================
// function: UpdateByLevelOfCheck
// purpose: 
//=======================================================================
void BOPDS_IteratorSI::UpdateByLevelOfCheck(const Standard_Integer theLevel)
{
  Standard_Integer i, aNbInterfTypes;
  //
  aNbInterfTypes=BOPDS_DS::NbInterfTypes();
  for (i=theLevel+1; i<aNbInterfTypes; ++i) {
    myLists(i).Clear();
  }
}
//=======================================================================
// function: Intersect
// purpose: 
//=======================================================================
void BOPDS_IteratorSI::Intersect(const Handle(IntTools_Context)& theCtx,
                                 const Standard_Boolean theCheckOBB,
                                 const Standard_Real theFuzzyValue)
{
  Standard_Integer i, j, iX, aNbS;
  Standard_Integer iTi, iTj;
  TopAbs_ShapeEnum aTi, aTj;
  //
  BOPTools_BoxBndTreeSelector aSelector;
  BOPTools_BoxBndTree aBBTree;
  NCollection_UBTreeFiller <Standard_Integer, Bnd_Box> aTreeFiller(aBBTree);
  //
  aNbS = myDS->NbSourceShapes();
  for (i=0; i<aNbS; ++i) {
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
    if (!aSI.IsInterfering()) {
      continue;
    }
    //
    const Bnd_Box& aBoxEx = aSI.Box();
    aTreeFiller.Add(i, aBoxEx);
  }
  //
  aTreeFiller.Fill();
  //
  BOPDS_MapOfPair aMPFence;
  //
  for (i = 0; i < aNbS; ++i) {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (!aSI.IsInterfering()){
      continue;
    }
    //
    const Bnd_Box& aBoxEx = aSI.Box();
    //
    aSelector.Clear();
    aSelector.SetBox(aBoxEx);
    //
    Standard_Integer aNbSD = aBBTree.Select(aSelector);
    if (!aNbSD) {
      continue;
    }
    //
    aTi = aSI.ShapeType();
    iTi = BOPDS_Tools::TypeToInteger(aTi);
    //
    const TColStd_ListOfInteger& aLI = aSelector.Indices();
    TColStd_ListIteratorOfListOfInteger aIt(aLI);
    for (; aIt.More(); aIt.Next()) {
      j = aIt.Value();
      const BOPDS_ShapeInfo& aSJ = myDS->ShapeInfo(j);
      aTj = aSJ.ShapeType();
      iTj = BOPDS_Tools::TypeToInteger(aTj);
      //
      // avoid interfering of the same shapes and shape with its sub-shapes
      if ((i == j) || ((iTi < iTj) && aSI.HasSubShape(j)) ||
                      ((iTi > iTj) && aSJ.HasSubShape(i))) {
        continue;
      }
      //
      BOPDS_Pair aPair(i, j);
      if (aMPFence.Add(aPair)) {
        if (theCheckOBB)
        {
          // Check intersection of Oriented bounding boxes of the shapes
          Bnd_OBB& anOBBi = theCtx->OBB(aSI.Shape(), theFuzzyValue);
          Bnd_OBB& anOBBj = theCtx->OBB(aSJ.Shape(), theFuzzyValue);

          if (anOBBi.IsOut(anOBBj))
            continue;
        }

        iX = BOPDS_Tools::TypeToInteger(aTi, aTj);
        myLists(iX).Append(aPair);
      }// if (aMPKXB.Add(aPKXB)) {
    }// for (; aIt.More(); aIt.Next()) {
  }//for (i=1; i<=aNbS; ++i) {
  //
  aMPFence.Clear();
}
