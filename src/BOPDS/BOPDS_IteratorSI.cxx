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
#include <Bnd_Tools.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_IndexRange.hxx>
#include <BOPDS_IteratorSI.hxx>
#include <BOPDS_MapOfPair.hxx>
#include <BOPDS_Pair.hxx>
#include <BOPDS_ShapeInfo.hxx>
#include <BOPDS_Tools.hxx>
#include <BOPTools_BoxTree.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_Context.hxx>
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
void BOPDS_IteratorSI::Intersect (const Handle(IntTools_Context)& theCtx,
                                  const Standard_Boolean theCheckOBB,
                                  const Standard_Real theFuzzyValue)
{
  const Standard_Integer aNbS = myDS->NbSourceShapes();

  BOPTools_BoxTree aBBTree;
  aBBTree.SetSize (aNbS);

  for (Standard_Integer i = 0; i < aNbS; ++i)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo (i);
    if (!aSI.IsInterfering())
      continue;

    const Bnd_Box& aBoxEx = aSI.Box();
    aBBTree.Add (i, Bnd_Tools::Bnd2BVH (aBoxEx));
  }

  aBBTree.Build();

  // Select pairs of shapes with interfering bounding boxes
  BOPTools_BoxPairSelector aPairSelector;
  aPairSelector.SetBVHSets (&aBBTree, &aBBTree);
  aPairSelector.SetSame (Standard_True);
  aPairSelector.Select();
  aPairSelector.Sort();

  // Treat the selected pairs
  const std::vector<BOPTools_BoxPairSelector::PairIDs>& aPairs = aPairSelector.Pairs();
  const Standard_Integer aNbPairs = static_cast<Standard_Integer> (aPairs.size());

  for (Standard_Integer iPair = 0; iPair < aNbPairs; ++iPair)
  {
    const BOPTools_BoxPairSelector::PairIDs& aPair = aPairs[iPair];

    const BOPDS_ShapeInfo& aSI1 = myDS->ShapeInfo (aPair.ID1);
    const BOPDS_ShapeInfo& aSI2 = myDS->ShapeInfo (aPair.ID2);

    const TopAbs_ShapeEnum aType1 = aSI1.ShapeType();
    const TopAbs_ShapeEnum aType2 = aSI2.ShapeType();

    Standard_Integer iType1 = BOPDS_Tools::TypeToInteger (aType1);
    Standard_Integer iType2 = BOPDS_Tools::TypeToInteger (aType2);

    // avoid interfering of the shape with its sub-shapes
    if (((iType1 < iType2) && aSI1.HasSubShape (aPair.ID2)) ||
        ((iType1 > iType2) && aSI2.HasSubShape (aPair.ID1)))
      continue;

    if (theCheckOBB)
    {
      // Check intersection of Oriented bounding boxes of the shapes
      const Bnd_OBB& anOBB1 = theCtx->OBB (aSI1.Shape (), theFuzzyValue);
      const Bnd_OBB& anOBB2 = theCtx->OBB (aSI2.Shape (), theFuzzyValue);

      if (anOBB1.IsOut (anOBB2))
        continue;
    }

    Standard_Integer iX = BOPDS_Tools::TypeToInteger (aType1, aType2);
    myLists(iX).Append (BOPDS_Pair (Min (aPair.ID1, aPair.ID2),
                                    Max (aPair.ID1, aPair.ID2)));
  }
}
