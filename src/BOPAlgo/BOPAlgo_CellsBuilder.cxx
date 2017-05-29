// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2015 OPEN CASCADE SAS
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


#include <BOPAlgo_CellsBuilder.hxx>

#include <TopoDS_Compound.hxx>

#include <BRep_Builder.hxx>

#include <TopExp_Explorer.hxx>

#include <BOPTools.hxx>
#include <BOPTools_AlgoTools.hxx>

#include <BOPAlgo_BuilderSolid.hxx>
#include <BOPAlgo_Alerts.hxx>

#include <BOPCol_MapOfInteger.hxx>

#include <ShapeUpgrade_UnifySameDomain.hxx>


static
  TopAbs_ShapeEnum TypeToExplore(const Standard_Integer theDim);

static
  void MakeTypedContainers(const TopoDS_Shape& theSC,
                           TopoDS_Shape& theResult);

//=======================================================================
//function : empty constructor
//purpose  : 
//=======================================================================
BOPAlgo_CellsBuilder::BOPAlgo_CellsBuilder()
:
  BOPAlgo_Builder(),
  myIndex(100, myAllocator),
  myMaterials(100, myAllocator),
  myShapeMaterial(100, myAllocator),
  myMapModified(100, myAllocator)
{
}

//=======================================================================
//function : empty constructor
//purpose  : 
//=======================================================================
BOPAlgo_CellsBuilder::BOPAlgo_CellsBuilder
  (const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPAlgo_Builder(theAllocator),
  myIndex(100, myAllocator),
  myMaterials(100, myAllocator),
  myShapeMaterial(100, myAllocator),
  myMapModified(100, myAllocator)
{
}

//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
BOPAlgo_CellsBuilder::~BOPAlgo_CellsBuilder()
{
  Clear();
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void BOPAlgo_CellsBuilder::Clear()
{
  BOPAlgo_Builder::Clear();
  myIndex.Clear();
  myMaterials.Clear();
  myShapeMaterial.Clear();
  myMapModified.Clear();
}

//=======================================================================
//function : GetAllParts
//purpose  : 
//=======================================================================
const TopoDS_Shape& BOPAlgo_CellsBuilder::GetAllParts() const
{
  return myAllParts;
}

//=======================================================================
//function : Prepare
//purpose  : 
//=======================================================================
void BOPAlgo_CellsBuilder::Prepare()
{
  BOPAlgo_Builder::Prepare();
  //
  myFlagHistory=Standard_False;
}

//=======================================================================
//function : PerformInternal1
//purpose  : 
//=======================================================================
void BOPAlgo_CellsBuilder::PerformInternal1(const BOPAlgo_PaveFiller& theFiller)
{
  BOPAlgo_Builder::PerformInternal1(theFiller);
  //
  if (HasErrors()) {
    return;
  }
  //
  // index all the parts to its origins
  IndexParts();
  //
  // and nullify <myShape> for building the result;
  RemoveAllFromResult();
  //
  myFlagHistory = Standard_True;
}

//=======================================================================
//function : IndexParts
//purpose  : 
//=======================================================================
void BOPAlgo_CellsBuilder::IndexParts()
{
  BRep_Builder aBB;
  // all split parts of the shapes
  TopoDS_Compound anAllParts;
  aBB.MakeCompound(anAllParts);
  //
  BOPCol_MapOfShape aMFence;
  BOPCol_MapOfInteger aMDims;
  //
  BOPCol_ListIteratorOfListOfShape aIt(myArguments);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    //
    Standard_Integer iDim = BOPTools_AlgoTools::Dimension(aS);
    aMDims.Add(iDim);
    TopAbs_ShapeEnum aType = TypeToExplore(iDim);
    //
    TopExp_Explorer aExp(aS, aType);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aST = aExp.Current();
      const BOPCol_ListOfShape* pLSIm = myImages.Seek(aST);
      if (!pLSIm) {
        BOPCol_ListOfShape* pLS = myIndex.ChangeSeek(aST);
        if (!pLS) {
          pLS = &myIndex(myIndex.Add(aST, BOPCol_ListOfShape()));
        }
        pLS->Append(aS);
        //
        if (aMFence.Add(aST)) {
          aBB.Add(anAllParts, aST);
        }
        //
        continue;
      }
      //
      BOPCol_ListIteratorOfListOfShape aItIm(*pLSIm);
      for (; aItIm.More(); aItIm.Next()) {
        const TopoDS_Shape& aSTIm = aItIm.Value();
        //
        BOPCol_ListOfShape* pLS = myIndex.ChangeSeek(aSTIm);
        if (!pLS) {
          pLS = &myIndex(myIndex.Add(aSTIm, BOPCol_ListOfShape()));
        }
        pLS->Append(aS);
        //
        if (aMFence.Add(aSTIm)) {
          aBB.Add(anAllParts, aSTIm);
        }
      } // for (; aItIm.More(); aItIm.Next()) {
    } // for (; aExp.More(); aExp.Next()) {
  } // for (; aIt.More(); aIt.Next()) {
  //
  myAllParts = anAllParts;
  //
  if (aMDims.Extent() == 1) {
    return;
  }
  //
  // for the multi-dimensional case
  // add sub-shapes of the splits into the <myIndex> map
  //
  Standard_Integer i, aNbS = myIndex.Extent();
  for (i = 1; i <= aNbS; ++i) {
    const TopoDS_Shape& aSP = myIndex.FindKey(i);
    const TopTools_ListOfShape& aLSOr = myIndex(i);
    //
    Standard_Integer iType = BOPTools_AlgoTools::Dimension(aSP);
    BOPCol_MapIteratorOfMapOfInteger aItM(aMDims);
    for (; aItM.More(); aItM.Next()) {
      Standard_Integer k = aItM.Value();
      if (k >= iType) {
        continue;
      }
      //
      TopExp_Explorer aExp(aSP, TypeToExplore(k));
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aSS = aExp.Current();
        BOPCol_ListOfShape* pLSSOr = myIndex.ChangeSeek(aSS);
        if (!pLSSOr) {
          myIndex.Add(aSS, aLSOr);
          continue;
        }
        // add ancestors of the shape to the ancestors of the sub-shape
        BOPCol_ListIteratorOfListOfShape aItLS(aLSOr);
        for (; aItLS.More(); aItLS.Next()) {
          const TopoDS_Shape& aSOr = aItLS.Value();
          // provide uniqueness of the ancestors
          BOPCol_ListIteratorOfListOfShape aItLSS(*pLSSOr);
          for (; aItLSS.More(); aItLSS.Next()) {
            if (aSOr.IsSame(aItLSS.Value())) {
              break;
            }
          }
          //
          if (!aItLSS.More()) {
            pLSSOr->Append(aSOr);
          }
        }
      }
    }
  }
}

//=======================================================================
//function : AddToResult
//purpose  : 
//=======================================================================
void BOPAlgo_CellsBuilder::AddToResult(const BOPCol_ListOfShape& theLSToTake,
                                       const BOPCol_ListOfShape& theLSToAvoid,
                                       const Standard_Integer theMaterial,
                                       const Standard_Boolean theUpdate)
{
  // find parts
  BOPCol_ListOfShape aParts;
  FindParts(theLSToTake, theLSToAvoid, aParts);
  if (aParts.IsEmpty()) {
    return;
  }
  //
  // collect result parts to avoid multiple adding of the same parts
  BOPCol_MapOfShape aResParts;
  TopoDS_Iterator aIt(myShape);
  for (; aIt.More(); aIt.Next()) {
    aResParts.Add(aIt.Value());
  }
  //
  Standard_Boolean bChanged = Standard_False;
  // add parts to result
  BOPCol_ListIteratorOfListOfShape aItLP(aParts);
  for (; aItLP.More(); aItLP.Next()) {
    const TopoDS_Shape& aPart = aItLP.Value();
    if (aResParts.Add(aPart)) {
      BRep_Builder().Add(myShape, aPart);
      bChanged = Standard_True;
    }
  }
  //
  // update the material
  if (theMaterial != 0) {
    BOPCol_ListOfShape aLSP;
    aItLP.Initialize(aParts);
    for (; aItLP.More(); aItLP.Next()) {
      const TopoDS_Shape& aPart = aItLP.Value();
      if (!myShapeMaterial.IsBound(aPart)) {
        myShapeMaterial.Bind(aPart, theMaterial);
        aLSP.Append(aPart);
      }
    } // for (; aIt.More(); aIt.Next()) {
    //
    if (aLSP.Extent()) {
      BOPCol_ListOfShape* pLS = myMaterials.ChangeSeek(theMaterial);
      if (!pLS) {
        pLS = myMaterials.Bound(theMaterial, BOPCol_ListOfShape());
      }
      pLS->Append(aLSP);
    } // if (aLSP.Extent()) {
  } // if (theMaterial != 0) {
  //
  if (!theUpdate) {
    if (bChanged) {
      PrepareHistory();
    }
  }
  else {
    RemoveInternalBoundaries();
  }
}

//=======================================================================
//function : AddAllToResult
//purpose  : 
//=======================================================================
void BOPAlgo_CellsBuilder::AddAllToResult(const Standard_Integer theMaterial,
                                          const Standard_Boolean theUpdate)
{
  myShapeMaterial.Clear();
  myMaterials.Clear();
  myMapModified.Clear();
  //
  myShape = myAllParts;
  //
  if (theMaterial != 0) {
    BOPCol_ListOfShape* pLSM = myMaterials.Bound(theMaterial, BOPCol_ListOfShape());
    //
    TopoDS_Iterator aIt(myAllParts);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aPart = aIt.Value();
      myShapeMaterial.Bind(aPart, theMaterial);
      pLSM->Append(aPart);
    }
  }
  //
  if (!theUpdate) {
    PrepareHistory();
  }
  else {
    RemoveInternalBoundaries();
  }
}

//=======================================================================
//function : RemoveFromResult
//purpose  : 
//=======================================================================
void BOPAlgo_CellsBuilder::RemoveFromResult(const BOPCol_ListOfShape& theLSToTake,
                                            const BOPCol_ListOfShape& theLSToAvoid)
{
  // find parts
  BOPCol_ListOfShape aParts;
  FindParts(theLSToTake, theLSToAvoid, aParts);
  if (aParts.IsEmpty()) {
    return;
  }
  //
  // collect parts into the map and remove parts from materials
  BOPCol_MapOfShape aPartsToRemove;
  BOPCol_ListIteratorOfListOfShape aItP(aParts);
  for (; aItP.More(); aItP.Next()) {
    const TopoDS_Shape& aPart = aItP.Value();
    aPartsToRemove.Add(aPart);
    //
    const Standard_Integer* pMaterial = myShapeMaterial.Seek(aPart);
    if (pMaterial) {
      BOPCol_ListOfShape* pLSM = myMaterials.ChangeSeek(*pMaterial);
      if (pLSM) {
        BOPCol_ListIteratorOfListOfShape aItM(*pLSM);
        for (; aItM.More(); aItM.Next()) {
          if (aPart.IsSame(aItM.Value())) {
            pLSM->Remove(aItM);
            break;
          }
        }
      }
      myShapeMaterial.UnBind(aPart);
    }
  }
  //
  BRep_Builder aBB;
  TopoDS_Compound aResult;
  aBB.MakeCompound(aResult);
  Standard_Boolean bChanged = Standard_False;
  //
  TopoDS_Iterator aIt(myShape);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    TopAbs_ShapeEnum aType = aS.ShapeType();
    if (aType != TopAbs_WIRE &&
        aType != TopAbs_SHELL &&
        aType != TopAbs_COMPSOLID) {
      // basic element
      if (aPartsToRemove.Contains(aS)) {
        bChanged = Standard_True;
        continue;
      }
      aBB.Add(aResult, aS);
    }
    else {
      // container
      TopoDS_Compound aSC;
      aBB.MakeCompound(aSC);
      Standard_Boolean bSCNotEmpty = Standard_False;
      //
      TopoDS_Iterator aItSC(aS);
      for (; aItSC.More(); aItSC.Next()) {
        const TopoDS_Shape& aSS = aItSC.Value();
        if (aPartsToRemove.Contains(aSS)) {
          bChanged = Standard_True;
          continue;
        }
        //
        bSCNotEmpty = Standard_True;
        aBB.Add(aSC, aSS);
      }
      //
      if (bSCNotEmpty) {
        MakeTypedContainers(aSC, aResult);
      }
    }
  }
  //
  if (bChanged) {
    myShape = aResult;
    //
    PrepareHistory();
  }
}

//=======================================================================
//function : RemoveAllFromResult
//purpose  : 
//=======================================================================
void BOPAlgo_CellsBuilder::RemoveAllFromResult()
{
  // empty compound
  TopoDS_Compound aC;
  BRep_Builder().MakeCompound(aC);
  myShape = aC;
  //
  myMaterials.Clear();
  myShapeMaterial.Clear();
  myMapModified.Clear();
  //
  PrepareHistory();
}

//=======================================================================
//function : RemoveInternalBoundaries
//purpose  : 
//=======================================================================
void BOPAlgo_CellsBuilder::RemoveInternalBoundaries()
{
  if (myMaterials.IsEmpty()) {
    return;
  }
  //
  BRep_Builder aBB;
  TopoDS_Compound aResult;
  aBB.MakeCompound(aResult);
  //
  Standard_Boolean bChanged = Standard_False;
  // try to remove the internal boundaries between the
  // shapes of the same material
  BOPCol_DataMapIteratorOfDataMapOfIntegerListOfShape aItM(myMaterials);
  for (; aItM.More(); aItM.Next()) {
    Standard_Integer iMaterial = aItM.Key();
    BOPCol_ListOfShape& aLS = aItM.ChangeValue();
    //
    if (aLS.IsEmpty()) {
      continue;
    }
    //
    if (aLS.Extent() == 1) {
      TopAbs_ShapeEnum aType = aLS.First().ShapeType();
      if (aType != TopAbs_WIRE &&
          aType != TopAbs_SHELL &&
          aType != TopAbs_COMPSOLID) {
        aBB.Add(aResult, aLS.First());
        continue;
      }
    }
    //
    // check the shapes of the same material to be of the same type
    BOPCol_ListIteratorOfListOfShape aItLS(aLS);
    TopAbs_ShapeEnum aType = aItLS.Value().ShapeType();
    for (aItLS.Next(); aItLS.More(); aItLS.Next()) {
      if (aType != aItLS.Value().ShapeType()) {
        break;
      }
    }
    //
    BOPCol_ListOfShape aLSNew;
    if (aItLS.More()) {
      // add the warning
      {
        TopoDS_Compound aMultiDimS;
        aBB.MakeCompound(aMultiDimS);
        aBB.Add(aMultiDimS, aLS.First());
        aBB.Add(aMultiDimS, aItLS.Value());
        //
        AddWarning (new BOPAlgo_AlertRemovalOfIBForMDimShapes (aMultiDimS));
      }
      aLSNew.Assign(aLS);
    }
    else {
      if (RemoveInternals(aLS, aLSNew)) {
        bChanged = Standard_True;
      }
    }
    //
    // update materials maps and add new shapes to result
    aItLS.Initialize(aLSNew);
    for (; aItLS.More(); aItLS.Next()) {
      const TopoDS_Shape& aS = aItLS.Value();
      aBB.Add(aResult, aS);
      if (!myShapeMaterial.IsBound(aS)) {
        myShapeMaterial.Bind(aS, iMaterial);
      }
    }
  }
  //
  if (bChanged) {
    // add shapes without material into result
    TopoDS_Iterator aIt(myShape);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aS = aIt.Value();
      //
      if (myShapeMaterial.IsBound(aS)) {
        continue;
      }
      //
      // check if it is not a collection
      TopAbs_ShapeEnum aType = aS.ShapeType();
      if (aType != TopAbs_WIRE &&
          aType != TopAbs_SHELL &&
          aType != TopAbs_COMPSOLID) {
        aBB.Add(aResult, aS);
      }
      else {
        TopoDS_Compound aSC;
        aBB.MakeCompound(aSC);
        Standard_Boolean bSCEmpty(Standard_True), bSCChanged(Standard_False);
        //
        TopoDS_Iterator aItSC(aS);
        for (; aItSC.More(); aItSC.Next()) {
          const TopoDS_Shape& aSS = aItSC.Value();
          if (!myShapeMaterial.IsBound(aSS)) {
            aBB.Add(aSC, aSS);
            bSCEmpty = Standard_False;
          }
          else {
            bSCChanged = Standard_True;
          }
        }
        //
        if (bSCEmpty) {
          continue;
        }
        //
        if (bSCChanged) {
          MakeTypedContainers(aSC, aResult);
        }
        else {
          aBB.Add(aResult, aS);
        }
      }
    }
    //
    myShape = aResult;
    //
    PrepareHistory();
  }
}

//=======================================================================
//function : FindPart
//purpose  : 
//=======================================================================
void BOPAlgo_CellsBuilder::FindParts(const BOPCol_ListOfShape& theLSToTake,
                                     const BOPCol_ListOfShape& theLSToAvoid,
                                     BOPCol_ListOfShape& theParts)
{
  if (theLSToTake.IsEmpty()) {
    return;
  }
  //
  // map shapes to avoid
  BOPCol_MapOfShape aMSToAvoid;
  BOPCol_ListIteratorOfListOfShape aItArgs(theLSToAvoid);
  for (; aItArgs.More(); aItArgs.Next()) {
    const TopoDS_Shape& aS = aItArgs.Value();
    aMSToAvoid.Add(aS);
  }
  //
  // map shapes to be taken
  BOPCol_MapOfShape aMSToTake;
  aItArgs.Initialize(theLSToTake);
  for (; aItArgs.More(); aItArgs.Next()) {
    const TopoDS_Shape& aS = aItArgs.Value();
    aMSToTake.Add(aS);
  }
  //
  Standard_Integer aNbS = aMSToTake.Extent();
  //
  // among the shapes to be taken into result, find any one
  // of minimal dimension
  Standard_Integer iDimMin = 10;
  TopoDS_Shape aSMin;
  //
  aItArgs.Initialize(theLSToTake);
  for (; aItArgs.More(); aItArgs.Next()) {
    const TopoDS_Shape& aS = aItArgs.Value();
    Standard_Integer iDim = BOPTools_AlgoTools::Dimension(aS);
    if (iDim < iDimMin) {
      iDimMin = iDim;
      aSMin = aS;
    }
  }
  //
  // among the split parts of the shape of minimal dimension
  // look for the parts to be taken into result
  TopAbs_ShapeEnum aType = TypeToExplore(iDimMin);
  TopExp_Explorer aExp(aSMin, aType);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Shape& aST = aExp.Current();
    // get split parts of the shape
    BOPCol_ListOfShape aLSTIm;
    if (!myImages.IsBound(aST)) {
      aLSTIm.Append(aST);
    } else {
      aLSTIm = myImages.Find(aST);
    }
    //
    BOPCol_ListIteratorOfListOfShape aItIm(aLSTIm);
    for (; aItIm.More(); aItIm.Next()) {
      const TopoDS_Shape& aPart = aItIm.Value();
      //
      if (!myIndex.Contains(aPart)) {
        continue;
      }
      //
      // get input shapes in which the split part is contained
      const BOPCol_ListOfShape& aLS = myIndex.FindFromKey(aPart);
      if (aLS.Extent() < aNbS) {
        continue;
      }
      //
      // check that input shapes containing the part should not be avoided
      BOPCol_MapOfShape aMS;
      aItArgs.Initialize(aLS);
      for (; aItArgs.More(); aItArgs.Next()) {
        const TopoDS_Shape& aS = aItArgs.Value();
        aMS.Add(aS);
        if (aMSToAvoid.Contains(aS)) {
          break;
        }
      }
      //
      if (aItArgs.More()) {
        continue;
      }
      //
      // check that all shapes which should be taken contain the part
      aItArgs.Initialize(theLSToTake);
      for (; aItArgs.More(); aItArgs.Next()) {
        if (!aMS.Contains(aItArgs.Value())) {
          break;
        }
      }
      //
      if (!aItArgs.More()) {
        theParts.Append(aPart);
      }
    }
  }
}

//=======================================================================
//function : MakeContainers
//purpose  : 
//=======================================================================
void BOPAlgo_CellsBuilder::MakeContainers()
{
  BRep_Builder aBB;
  TopoDS_Compound aResult;
  aBB.MakeCompound(aResult);
  //
  // basic elements of type EDGE, FACE and SOLID added into result
  BOPCol_ListOfShape aLS[3];
  //
  TopoDS_Iterator aIt(myShape);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    //
    Standard_Integer iDim = BOPTools_AlgoTools::Dimension(aS);
    if (iDim <= 0) {
      aBB.Add(aResult, aS);
      continue;
    }
    //
    aLS[iDim-1].Append(aS);
  }
  //
  for (Standard_Integer i = 0; i < 3; ++i) {
    if (aLS[i].IsEmpty()) {
      continue;
    }
    //
    TopoDS_Compound aC;
    aBB.MakeCompound(aC);
    BOPCol_ListIteratorOfListOfShape aItLS(aLS[i]);
    for (; aItLS.More(); aItLS.Next()) {
      aBB.Add(aC, aItLS.Value());
    }
    //
    MakeTypedContainers(aC, aResult);
  }
  myShape = aResult;
}

//=======================================================================
//function : RemoveInternals
//purpose  : 
//=======================================================================
Standard_Boolean BOPAlgo_CellsBuilder::RemoveInternals(const BOPCol_ListOfShape& theLS,
                                                       BOPCol_ListOfShape& theLSNew)
{
  Standard_Boolean bRemoved = Standard_False;
  if (theLS.Extent() < 2) {
    theLSNew = theLS;
    return bRemoved;
  }
  //
  TopAbs_ShapeEnum aType = theLS.First().ShapeType();
  //
  if (aType == TopAbs_EDGE ||
      aType == TopAbs_FACE) {
    //
    // make container
    BRep_Builder aBB;
    TopoDS_Shape aShape;
    //
    BOPTools_AlgoTools::MakeContainer
      ((aType == TopAbs_FACE) ? TopAbs_SHELL : TopAbs_WIRE, aShape);
    //
    for (BOPCol_ListIteratorOfListOfShape aIt(theLS); aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aS = aIt.Value();
      aBB.Add(aShape, aS);
    }
    //
    // Unify same domain
    Standard_Boolean bFaces, bEdges;
    //
    bFaces = (aType == TopAbs_FACE);
    bEdges = (aType == TopAbs_EDGE);
    //
    ShapeUpgrade_UnifySameDomain anUnify (aShape, bEdges, bFaces);
    anUnify.Build();
    const TopoDS_Shape& aSNew = anUnify.Shape();
    //
    TopExp_Explorer aExp(aSNew, aType);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aSn = aExp.Current();
      theLSNew.Append(aSn);
    }
    //
    if (theLSNew.IsEmpty()) {
      // add the warning
      if (bFaces)
        AddWarning (new BOPAlgo_AlertRemovalOfIBForFacesFailed (aShape));
      else
        AddWarning (new BOPAlgo_AlertRemovalOfIBForEdgesFailed (aShape));
      //
      theLSNew.Assign(theLS);
      return bRemoved;
    }
    //
    // fill map of modified shapes
    BOPCol_IndexedMapOfShape aMG;
    Standard_Integer i, aNb;
    //
    BOPTools::MapShapes(aShape, TopAbs_VERTEX, aMG);
    BOPTools::MapShapes(aShape, TopAbs_EDGE,   aMG);
    BOPTools::MapShapes(aShape, TopAbs_FACE,   aMG);
    //
    aNb = aMG.Extent();
    for (i = 1; i <= aNb; ++i) {
      const TopoDS_Shape& aSS = aMG(i);
      const TopTools_ListOfShape& aLSMod = anUnify.History()->Modified(aSS);
      TopTools_ListIteratorOfListOfShape aIt(aLSMod);
      for (; aIt.More(); aIt.Next()) {
        const TopoDS_Shape& aSU = aIt.Value();
        if (!aSU.IsNull() && !aSS.IsSame(aSU)) {
          myMapModified.Bind(aSS, aSU);
          bRemoved = Standard_True;
        }
      }
    }
  }
  else if (aType == TopAbs_SOLID) {
    BRep_Builder aBB;
    TopoDS_Compound aSolids;
    aBB.MakeCompound(aSolids);
    //
    BOPCol_ListIteratorOfListOfShape aItLS(theLS);
    for (; aItLS.More(); aItLS.Next()) {
      const TopoDS_Shape& aSol = aItLS.Value();
      aBB.Add(aSolids, aSol);
    }
    //
    // Make connexity blocks of solids to create from each isolated block one solid.
    // It will allow attaching internal entities of the solids to new solid.
    BOPCol_ListOfShape aLCB;
    BOPTools_AlgoTools::MakeConnexityBlocks(aSolids, TopAbs_FACE, TopAbs_SOLID, aLCB);
    //
    // for each block remove internal faces
    BOPCol_ListIteratorOfListOfShape aItLCB(aLCB);
    for (; aItLCB.More(); aItLCB.Next()) {
      const TopoDS_Shape& aCB = aItLCB.Value();
      //
      // Map faces and solids to find boundary faces that can be removed
      BOPCol_IndexedDataMapOfShapeListOfShape aDMFS;
      // internal entities
      BOPCol_ListOfShape aLSInt;
      //
      TopoDS_Iterator aItS(aCB);
      for (; aItS.More(); aItS.Next()) {
        const TopoDS_Shape& aSol = aItS.Value();
        //
        TopoDS_Iterator aItIS(aSol);
        for (; aItIS.More(); aItIS.Next()) {
          const TopoDS_Shape& aSI = aItIS.Value();
          if (aSI.Orientation() == TopAbs_INTERNAL) {
            aLSInt.Append(aSI);
          }
          else {
            TopoDS_Iterator aItF(aSI);
            for (; aItF.More(); aItF.Next()) {
              const TopoDS_Shape& aF = aItF.Value();
              BOPCol_ListOfShape *pLSols = aDMFS.ChangeSeek(aF);
              if (!pLSols) {
                pLSols = &aDMFS(aDMFS.Add(aF, BOPCol_ListOfShape()));
              }
              pLSols->Append(aSol);
            }
          }
        }
      }
      //
      // to build unified solid, select only faces attached to only one solid
      BOPCol_ListOfShape aLFUnique;
      Standard_Integer i, aNb = aDMFS.Extent();
      for (i = 1; i <= aNb; ++i) {
        if (aDMFS(i).Extent() == 1) {
          aLFUnique.Append(aDMFS.FindKey(i));
        }
      }
      //
      if (aNb == aLFUnique.Extent()) {
        // no faces to remove
        aItS.Initialize(aCB);
        for (; aItS.More(); aItS.Next()) {
          theLSNew.Append(aItS.Value());
        }
        continue;
      }
      //
      // build new solid
      BOPAlgo_BuilderSolid aBS;
      aBS.SetShapes(aLFUnique);
      aBS.Perform();
      //
      if (aBS.HasErrors() || aBS.Areas().Extent() != 1) {
        // add the warning
        {
          TopoDS_Compound aUniqeFaces;
          aBB.MakeCompound(aUniqeFaces);
          BOPCol_ListIteratorOfListOfShape aItLFUniqe(aLFUnique);
          for (; aItLFUniqe.More(); aItLFUniqe.Next()) {
            aBB.Add(aUniqeFaces, aItLFUniqe.Value());
          }
          //
          AddWarning (new BOPAlgo_AlertRemovalOfIBForSolidsFailed (aUniqeFaces));
        }
        //
        aItS.Initialize(aCB);
        for (; aItS.More(); aItS.Next()) {
          theLSNew.Append(aItS.Value());
        }
        continue;
      }
      //
      TopoDS_Solid& aSNew = *(TopoDS_Solid*)&aBS.Areas().First();
      //
      // put all internal parts into new solid
      aSNew.Free(Standard_True);
      BOPCol_ListIteratorOfListOfShape aItLSI(aLSInt);
      for (; aItLSI.More(); aItLSI.Next()) {
        aBB.Add(aSNew, aItLSI.Value());
      }
      aSNew.Free(Standard_False);
      //
      theLSNew.Append(aSNew);
      bRemoved = Standard_True;
    }
  }
  return bRemoved;
}

//=======================================================================
//function : IsDeleted
//purpose  : 
//=======================================================================
Standard_Boolean BOPAlgo_CellsBuilder::IsDeleted(const TopoDS_Shape& theS)
{
  Standard_Boolean bRet = Standard_True;
  if (theS.IsNull()) {
    return bRet;
  }
  //
  TopAbs_ShapeEnum aType = theS.ShapeType();
  if (!(aType==TopAbs_EDGE || aType==TopAbs_FACE || 
      aType==TopAbs_VERTEX || aType==TopAbs_SOLID)) {
    return bRet;
  }
  //
  Standard_Boolean bHasImage, bHasModified;
  //
  bHasImage = myImages.IsBound(theS);
  bHasModified = myMapModified.IsBound(theS);
  if (!bHasImage && !bHasModified) {
    bRet = !myMapShape.Contains(theS);
    return bRet;
  }
  //
  if (bHasModified) {
    const TopoDS_Shape& aSG = myMapModified.Find(theS);
    if (myMapShape.Contains(aSG)) {
      bRet = Standard_False;
      return bRet;
    }
  }
  //
  if (bHasImage) {
    const BOPCol_ListOfShape& aLSp = myImages.Find(theS);
    BOPCol_ListIteratorOfListOfShape aIt(aLSp);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aSp = aIt.Value();
      const TopoDS_Shape& aSpR = myShapesSD.IsBound(aSp) ? 
        myShapesSD.Find(aSp) : aSp;
      //
      const TopoDS_Shape& aSpRG = myMapModified.IsBound(aSpR) ?
        myMapModified.Find(aSpR) : aSpR;
      if (myMapShape.Contains(aSpRG)) {
        bRet = Standard_False;
        break;
      }
    }
  }
  //
  return bRet;
}

//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& BOPAlgo_CellsBuilder::Modified(const TopoDS_Shape& theS)
{
  myHistShapes.Clear();
  if (theS.IsNull()) {
    return myHistShapes;
  }
  //
  TopAbs_ShapeEnum aType = theS.ShapeType();
  if (!(aType==TopAbs_EDGE || aType==TopAbs_FACE || aType==TopAbs_VERTEX)) {
    return myHistShapes;
  }
  //
  Standard_Boolean bHasModified = myMapModified.IsBound(theS);
  if (bHasModified) {
    const TopoDS_Shape& aSG = myMapModified.Find(theS);
    if (myMapShape.Contains(aSG)) {
      myHistShapes.Append(aSG);
    }
    return myHistShapes;
  }
  //
  Standard_Boolean bHasImage = myImages.IsBound(theS);
  if (!bHasImage) {
    return myHistShapes;
  }
  //
  BOPCol_MapOfShape aMFence;
  const BOPCol_ListOfShape& aLSp = myImages.Find(theS);
  BOPCol_ListIteratorOfListOfShape aIt(aLSp);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape aSp = aIt.Value();
    const TopoDS_Shape& aSpR = myShapesSD.IsBound(aSp) ? 
      myShapesSD.Find(aSp) : aSp;
    //
    if (myMapModified.IsBound(aSpR)) {
      const TopoDS_Shape& aSG = myMapModified.Find(aSpR);
      if (myMapShape.Contains(aSG)) {
        if (aMFence.Add(aSG)) {
          myHistShapes.Append(aSG);
        }
      }
    }
    else if (aMFence.Add(aSpR))
    {
      myHistShapes.Append(aSpR);
    }
  }
  //
  return myHistShapes;
}

//=======================================================================
//function : MakeTypedContainers
//purpose  : 
//=======================================================================
void MakeTypedContainers(const TopoDS_Shape& theSC,
                         TopoDS_Shape& theResult)
{
  TopAbs_ShapeEnum aContainerType, aConnexityType, aPartType;
  //
  aPartType = TypeToExplore(BOPTools_AlgoTools::Dimension(theSC));
  switch (aPartType) {
    case TopAbs_EDGE: {
      aContainerType = TopAbs_WIRE;
      aConnexityType = TopAbs_VERTEX;
      break;
    }
    case TopAbs_FACE: {
      aContainerType = TopAbs_SHELL;
      aConnexityType = TopAbs_EDGE;
      break;
    }
    case TopAbs_SOLID: {
      aContainerType = TopAbs_COMPSOLID;
      aConnexityType = TopAbs_FACE;
      break;
    }
    default:
      return;
  }
  //
  BOPCol_ListOfShape aLCB;
  BOPTools_AlgoTools::MakeConnexityBlocks(theSC, aConnexityType, aPartType, aLCB);
  if (aLCB.IsEmpty()) {
    return;
  }
  //
  BRep_Builder aBB;
  TopExp_Explorer aExp;
  BOPCol_ListIteratorOfListOfShape aItCB;
  //
  aItCB.Initialize(aLCB);
  for (; aItCB.More(); aItCB.Next()) {
    TopoDS_Shape aRCB;
    BOPTools_AlgoTools::MakeContainer(aContainerType, aRCB);
    //
    const TopoDS_Shape& aCB = aItCB.Value();
    aExp.Init(aCB, aPartType);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aCBS = aExp.Current();
      aBB.Add(aRCB, aCBS);
    }
    //
    if (aContainerType == TopAbs_SHELL) {
      BOPTools_AlgoTools::OrientFacesOnShell(aRCB);
    }
    //
    aBB.Add(theResult, aRCB);
  }
}

//=======================================================================
//function : TypeToExplore
//purpose  : 
//=======================================================================
TopAbs_ShapeEnum TypeToExplore(const Standard_Integer theDim)
{
  TopAbs_ShapeEnum aRet;
  //
  switch(theDim) {
  case 0:
    aRet=TopAbs_VERTEX;
    break;
  case 1:
    aRet=TopAbs_EDGE;
    break;
  case 2:
    aRet=TopAbs_FACE;
    break;
  case 3:
    aRet=TopAbs_SOLID;
    break;
  default:
    aRet=TopAbs_SHAPE;
    break;
  }
  return aRet;
}
