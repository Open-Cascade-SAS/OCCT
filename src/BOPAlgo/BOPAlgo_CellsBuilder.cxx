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

#include <BOPAlgo_Alerts.hxx>
#include <BOPAlgo_BuilderSolid.hxx>
#include <BOPDS_DS.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools3D.hxx>
#include <BRep_Builder.hxx>
#include <ShapeUpgrade_UnifySameDomain.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Compound.hxx>


static
  TopAbs_ShapeEnum TypeToExplore(const Standard_Integer theDim);

static
  void MakeTypedContainers(const TopoDS_Shape& theSC,
                           TopoDS_Shape& theResult);

static void CollectMaterialBoundaries(const TopTools_ListOfShape& theLS,
                                      TopTools_MapOfShape& theMapKeepBnd);

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
  TopTools_MapOfShape aMFence;
  TColStd_MapOfInteger aMDims;
  //
  TopTools_ListIteratorOfListOfShape aIt(myArguments);
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
      const TopTools_ListOfShape* pLSIm = myImages.Seek(aST);
      if (!pLSIm) {
        TopTools_ListOfShape* pLS = myIndex.ChangeSeek(aST);
        if (!pLS) {
          pLS = &myIndex(myIndex.Add(aST, TopTools_ListOfShape()));
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
      TopTools_ListIteratorOfListOfShape aItIm(*pLSIm);
      for (; aItIm.More(); aItIm.Next()) {
        const TopoDS_Shape& aSTIm = aItIm.Value();
        //
        TopTools_ListOfShape* pLS = myIndex.ChangeSeek(aSTIm);
        if (!pLS) {
          pLS = &myIndex(myIndex.Add(aSTIm, TopTools_ListOfShape()));
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
    TColStd_MapIteratorOfMapOfInteger aItM(aMDims);
    for (; aItM.More(); aItM.Next()) {
      Standard_Integer k = aItM.Value();
      if (k >= iType) {
        continue;
      }
      //
      TopExp_Explorer aExp(aSP, TypeToExplore(k));
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aSS = aExp.Current();
        TopTools_ListOfShape* pLSSOr = myIndex.ChangeSeek(aSS);
        if (!pLSSOr) {
          myIndex.Add(aSS, aLSOr);
          continue;
        }
        // add ancestors of the shape to the ancestors of the sub-shape
        TopTools_ListIteratorOfListOfShape aItLS(aLSOr);
        for (; aItLS.More(); aItLS.Next()) {
          const TopoDS_Shape& aSOr = aItLS.Value();
          // provide uniqueness of the ancestors
          TopTools_ListIteratorOfListOfShape aItLSS(*pLSSOr);
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
void BOPAlgo_CellsBuilder::AddToResult(const TopTools_ListOfShape& theLSToTake,
                                       const TopTools_ListOfShape& theLSToAvoid,
                                       const Standard_Integer theMaterial,
                                       const Standard_Boolean theUpdate)
{
  // find parts
  TopTools_ListOfShape aParts;
  FindParts(theLSToTake, theLSToAvoid, aParts);
  if (aParts.IsEmpty()) {
    return;
  }
  //
  // collect result parts to avoid multiple adding of the same parts
  TopTools_MapOfShape aResParts;
  TopoDS_Iterator aIt(myShape);
  for (; aIt.More(); aIt.Next()) {
    aResParts.Add(aIt.Value());
  }
  //
  Standard_Boolean bChanged = Standard_False;
  // add parts to result
  TopTools_ListIteratorOfListOfShape aItLP(aParts);
  for (; aItLP.More(); aItLP.Next()) {
    const TopoDS_Shape& aPart = aItLP.Value();
    // provide uniqueness of the parts 
    if (aResParts.Add(aPart) && !myShapeMaterial.IsBound(aPart)) {
      BRep_Builder().Add(myShape, aPart);
      bChanged = Standard_True;
    }
  }
  //
  // update the material
  if (theMaterial != 0) {
    TopTools_ListOfShape aLSP;
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
      TopTools_ListOfShape* pLS = myMaterials.ChangeSeek(theMaterial);
      if (!pLS) {
        pLS = myMaterials.Bound(theMaterial, TopTools_ListOfShape());
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
    TopTools_ListOfShape* pLSM = myMaterials.Bound(theMaterial, TopTools_ListOfShape());
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
void BOPAlgo_CellsBuilder::RemoveFromResult(const TopTools_ListOfShape& theLSToTake,
                                            const TopTools_ListOfShape& theLSToAvoid)
{
  // find parts
  TopTools_ListOfShape aParts;
  FindParts(theLSToTake, theLSToAvoid, aParts);
  if (aParts.IsEmpty()) {
    return;
  }
  //
  // collect parts into the map and remove parts from materials
  TopTools_MapOfShape aPartsToRemove;
  TopTools_ListIteratorOfListOfShape aItP(aParts);
  for (; aItP.More(); aItP.Next()) {
    const TopoDS_Shape& aPart = aItP.Value();
    aPartsToRemove.Add(aPart);
    //
    const Standard_Integer* pMaterial = myShapeMaterial.Seek(aPart);
    if (pMaterial) {
      TopTools_ListOfShape* pLSM = myMaterials.ChangeSeek(*pMaterial);
      if (pLSM) {
        TopTools_ListIteratorOfListOfShape aItM(*pLSM);
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
  TopTools_DataMapIteratorOfDataMapOfIntegerListOfShape aItM(myMaterials);
  TopTools_ListOfShape aLSUnify[2];
  TopTools_MapOfShape aKeepMap[2];
  for (; aItM.More(); aItM.Next()) {
    Standard_Integer iMaterial = aItM.Key();
    TopTools_ListOfShape& aLS = aItM.ChangeValue();
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
    TopTools_ListIteratorOfListOfShape aItLS(aLS);
    TopAbs_ShapeEnum aType = aItLS.Value().ShapeType();
    for (aItLS.Next(); aItLS.More(); aItLS.Next()) {
      if (aType != aItLS.Value().ShapeType()) {
        break;
      }
    }

    if (aItLS.More())
    {
      // add the warning
      TopoDS_Compound aMultiDimS;
      aBB.MakeCompound(aMultiDimS);
      aBB.Add(aMultiDimS, aLS.First());
      aBB.Add(aMultiDimS, aItLS.Value());
      AddWarning(new BOPAlgo_AlertRemovalOfIBForMDimShapes(aMultiDimS));
    }
    else
    {
      if (aType == TopAbs_EDGE || aType == TopAbs_FACE)
      {
        // for edges and faces, just collect shapes to unify them later after exiting the loop;
        // collect boundaries of shapes of current material in the keep map
        Standard_Integer iType = (aType == TopAbs_EDGE ? 0 : 1);
        CollectMaterialBoundaries(aLS, aKeepMap[iType]);
        // save shapes to unify later
        TopTools_ListOfShape aCopy(aLS);
        aLSUnify[iType].Append(aCopy);
        continue;
      }
      else
      {
        // aType is Solid;
        // remove internal faces between solids of the same material just now
        TopTools_ListOfShape aLSNew;
        if (RemoveInternals(aLS, aLSNew))
        {
          bChanged = Standard_True;
          // update materials maps
          for (aItLS.Initialize(aLSNew); aItLS.More(); aItLS.Next()) {
            const TopoDS_Shape& aS = aItLS.Value();
            myShapeMaterial.Bind(aS, iMaterial);
          }
          aLS.Assign(aLSNew);
        }
      }
    }
    // add shapes to result (multidimensional and solids)
    for (aItLS.Initialize(aLS); aItLS.More(); aItLS.Next()) {
      const TopoDS_Shape& aS = aItLS.Value();
      aBB.Add(aResult, aS);
    }
  }

  // remove internal boundaries for edges and faces
  for (Standard_Integer iType = 0; iType < 2; ++iType)
  {
    if (aLSUnify[iType].IsEmpty())
      continue;
    TopTools_ListOfShape aLSN;
    if (RemoveInternals(aLSUnify[iType], aLSN, aKeepMap[iType]))
      bChanged = Standard_True;
    // add shapes to result ([unified] edges or faces)
    for (TopTools_ListIteratorOfListOfShape aItLS(aLSN); aItLS.More(); aItLS.Next()) {
      const TopoDS_Shape& aS = aItLS.Value();
      aBB.Add(aResult, aS);
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
void BOPAlgo_CellsBuilder::FindParts(const TopTools_ListOfShape& theLSToTake,
                                     const TopTools_ListOfShape& theLSToAvoid,
                                     TopTools_ListOfShape& theParts)
{
  if (theLSToTake.IsEmpty()) {
    return;
  }
  //
  // map shapes to avoid
  TopTools_MapOfShape aMSToAvoid;
  TopTools_ListIteratorOfListOfShape aItArgs(theLSToAvoid);
  for (; aItArgs.More(); aItArgs.Next()) {
    const TopoDS_Shape& aS = aItArgs.Value();
    aMSToAvoid.Add(aS);
  }
  //
  // map shapes to be taken
  TopTools_MapOfShape aMSToTake;
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
    TopTools_ListOfShape aLSTIm;
    if (!myImages.IsBound(aST)) {
      aLSTIm.Append(aST);
    } else {
      aLSTIm = myImages.Find(aST);
    }
    //
    TopTools_ListIteratorOfListOfShape aItIm(aLSTIm);
    for (; aItIm.More(); aItIm.Next()) {
      const TopoDS_Shape& aPart = aItIm.Value();
      //
      if (!myIndex.Contains(aPart)) {
        continue;
      }
      //
      // get input shapes in which the split part is contained
      const TopTools_ListOfShape& aLS = myIndex.FindFromKey(aPart);
      if (aLS.Extent() < aNbS) {
        continue;
      }
      //
      // check that input shapes containing the part should not be avoided
      TopTools_MapOfShape aMS;
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
  TopTools_ListOfShape aLS[3];
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
    TopTools_ListIteratorOfListOfShape aItLS(aLS[i]);
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
Standard_Boolean BOPAlgo_CellsBuilder::RemoveInternals(const TopTools_ListOfShape& theLS,
                                                       TopTools_ListOfShape& theLSNew,
                                                       const TopTools_MapOfShape& theMapKeepBnd)
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
    for (TopTools_ListIteratorOfListOfShape aIt(theLS); aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aS = aIt.Value();
      aBB.Add(aShape, aS);
    }
    //
    // Unify same domain
    Standard_Boolean bFaces, bEdges;
    //
    bFaces = (aType == TopAbs_FACE);
    bEdges = (aType == TopAbs_EDGE);
    ShapeUpgrade_UnifySameDomain anUnify (aShape, bEdges, bFaces);
    anUnify.KeepShapes(theMapKeepBnd);
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
    TopTools_IndexedMapOfShape aMG;
    Standard_Integer i, aNb;
    //
    TopExp::MapShapes(aShape, TopAbs_VERTEX, aMG);
    TopExp::MapShapes(aShape, TopAbs_EDGE,   aMG);
    TopExp::MapShapes(aShape, TopAbs_FACE,   aMG);
    //
    aNb = aMG.Extent();
    for (i = 1; i <= aNb; ++i) {
      const TopoDS_Shape& aSS = aMG(i);
      const Standard_Integer* pMaterial = myShapeMaterial.Seek(aSS);
      const TopTools_ListOfShape& aLSMod = anUnify.History()->Modified(aSS);
      TopTools_ListIteratorOfListOfShape aIt(aLSMod);
      for (; aIt.More(); aIt.Next()) {
        const TopoDS_Shape& aSU = aIt.Value();
        myMapModified.Bind(aSS, aSU);
        bRemoved = Standard_True;
        if (pMaterial && !myShapeMaterial.IsBound(aSU))
          myShapeMaterial.Bind(aSU, *pMaterial);
      }
    }
  }
  else if (aType == TopAbs_SOLID) {
    BRep_Builder aBB;
    TopoDS_Compound aSolids;
    aBB.MakeCompound(aSolids);
    //
    TopTools_ListIteratorOfListOfShape aItLS(theLS);
    for (; aItLS.More(); aItLS.Next()) {
      const TopoDS_Shape& aSol = aItLS.Value();
      aBB.Add(aSolids, aSol);
    }
    //
    // Make connexity blocks of solids to create from each isolated block one solid.
    // It will allow attaching internal entities of the solids to new solid.
    TopTools_ListOfShape aLCB;
    BOPTools_AlgoTools::MakeConnexityBlocks(aSolids, TopAbs_FACE, TopAbs_SOLID, aLCB);
    //
    // for each block remove internal faces
    TopTools_ListIteratorOfListOfShape aItLCB(aLCB);
    for (; aItLCB.More(); aItLCB.Next()) {
      const TopoDS_Shape& aCB = aItLCB.Value();
      //
      // Map faces and solids to find boundary faces that can be removed
      TopTools_IndexedDataMapOfShapeListOfShape aDMFS;
      // internal entities
      TopTools_ListOfShape aLSInt;
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
              TopTools_ListOfShape *pLSols = aDMFS.ChangeSeek(aF);
              if (!pLSols) {
                pLSols = &aDMFS(aDMFS.Add(aF, TopTools_ListOfShape()));
              }
              pLSols->Append(aSol);
            }
          }
        }
      }
      //
      // to build unified solid, select only faces attached to only one solid
      TopTools_ListOfShape aLFUnique;
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
          TopTools_ListIteratorOfListOfShape aItLFUniqe(aLFUnique);
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
      myReport->Merge(aBS.GetReport());
      //
      TopoDS_Solid& aSNew = *(TopoDS_Solid*)&aBS.Areas().First();
      //
      // put all internal parts into new solid
      aSNew.Free(Standard_True);
      TopTools_ListIteratorOfListOfShape aItLSI(aLSInt);
      for (; aItLSI.More(); aItLSI.Next()) {
        aBB.Add(aSNew, aItLSI.Value());
      }
      aSNew.Free(Standard_False);
      //
      theLSNew.Append(aSNew);
      bRemoved = Standard_True;

      // Save information about the fuse of the solids into a history map
      aItS.Initialize(aCB);
      for (; aItS.More(); aItS.Next())
        myMapModified.Bind(aItS.Value(), aSNew);
    }
  }
  return bRemoved;
}

//=======================================================================
//function : LocModified
//purpose  : 
//=======================================================================
const TopTools_ListOfShape* BOPAlgo_CellsBuilder::LocModified(const TopoDS_Shape& theS)
{
  // Get shape's modification coming from GF operation
  const TopTools_ListOfShape* pLSp = BOPAlgo_Builder::LocModified(theS);
  if (myMapModified.IsEmpty())
    // No local modifications
    return pLSp;

  myHistShapes.Clear();

  // Check if the shape (or its splits) has participated in unification
  if (!pLSp)
  {
    // No splits from GF operation.
    // Check if the shape has been unified with other shapes
    const TopoDS_Shape* pSU = myMapModified.Seek(theS);
    if (!pSU)
      return NULL;

    myHistShapes.Append(*pSU);
  }
  else
  {
    TopTools_MapOfShape aMFence;
    // Process all GF splits and check them for local unification with other shapes
    TopTools_ListIteratorOfListOfShape aIt(*pLSp);
    for (; aIt.More(); aIt.Next())
    {
      const TopoDS_Shape* pSp = &aIt.Value();
      const TopoDS_Shape* pSU = myMapModified.Seek(*pSp);
      if (pSU) pSp = pSU;
      if (aMFence.Add(*pSp))
        myHistShapes.Append(*pSp);
    }
  }
  return &myHistShapes;
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
  TopTools_ListOfShape aLCB;
  BOPTools_AlgoTools::MakeConnexityBlocks(theSC, aConnexityType, aPartType, aLCB);
  if (aLCB.IsEmpty()) {
    return;
  }
  //
  BRep_Builder aBB;
  TopExp_Explorer aExp;
  TopTools_ListIteratorOfListOfShape aItCB;
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
//function : CollectMaterialBoundaries
//purpose  : Add to theMapKeepBnd the boundary shapes of the area defined by shapes from the list
//=======================================================================
static void CollectMaterialBoundaries(const TopTools_ListOfShape& theLS,
                                      TopTools_MapOfShape& theMapKeepBnd)
{
  TopAbs_ShapeEnum aType = theLS.First().ShapeType();
  TopAbs_ShapeEnum aTypeSubsh = (aType == TopAbs_FACE ? TopAbs_EDGE : TopAbs_VERTEX);
  TopTools_IndexedDataMapOfShapeListOfShape aMapSubSh;
  TopTools_ListIteratorOfListOfShape anIt(theLS);
  for (; anIt.More(); anIt.Next())
  {
    const TopoDS_Shape& aS = anIt.Value();
    TopExp::MapShapesAndAncestors(aS, aTypeSubsh, aType, aMapSubSh);
  }
  for (int i = 1; i <= aMapSubSh.Extent(); i++)
  {
    // check if the subshape belongs to boundary of the area
    if (aMapSubSh(i).Extent() == 1)
    {
      // add to theMapKeepBnd
      theMapKeepBnd.Add(aMapSubSh.FindKey(i));
    }
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
