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

#include <ShapeUpgrade_UnifySameDomain.hxx>


static
  TopAbs_ShapeEnum TypeToExplore(const Standard_Integer theDim);

static
  void MakeTypedContainers(const TopoDS_Shape& theSC,
                           const TopAbs_ShapeEnum aType,
                           TopoDS_Shape& theResult);

//=======================================================================
//function : empty constructor
//purpose  : 
//=======================================================================
BOPAlgo_CellsBuilder::BOPAlgo_CellsBuilder()
:
  BOPAlgo_Builder(),
  myType(TopAbs_SHAPE),
  myIndex(100, myAllocator),
  myMaterials(100, myAllocator),
  myShapeMaterial(100, myAllocator),
  myMapGenerated(100, myAllocator)
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
  myType(TopAbs_SHAPE),
  myIndex(100, myAllocator),
  myMaterials(100, myAllocator),
  myShapeMaterial(100, myAllocator),
  myMapGenerated(100, myAllocator)
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
  myMapGenerated.Clear();
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
// function: CheckData
// purpose: 
//=======================================================================
void BOPAlgo_CellsBuilder::CheckData()
{
  BOPAlgo_Builder::CheckData();
  if (myErrorStatus) {
    return;
  }
  //
  // additional check for the arguments to be of the same dimension.
  Standard_Integer aDim1, aDimi;
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  aIt.Initialize(myArguments);
  const TopoDS_Shape& aS1 = aIt.Value();
  aDim1 = BOPTools_AlgoTools::Dimension(aS1);
  //
  for (aIt.Next(); aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSi = aIt.Value();
    aDimi = BOPTools_AlgoTools::Dimension(aSi);
    //
    if (aDim1 != aDimi) {
      myErrorStatus = 201; // non-homogenous arguments
      break;
    }
  }
}

//=======================================================================
//function : PerformInternal1
//purpose  : 
//=======================================================================
void BOPAlgo_CellsBuilder::PerformInternal1(const BOPAlgo_PaveFiller& theFiller)
{
  BOPAlgo_Builder::PerformInternal1(theFiller);
  //
  if (myErrorStatus) {
    return;
  }
  //
  // save the splits to <myAllParts>
  TakeAllParts();
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
//function : TakeAllParts
//purpose  : 
//=======================================================================
void BOPAlgo_CellsBuilder::TakeAllParts()
{
  Standard_Integer aDim;
  TopoDS_Compound aC;
  BRep_Builder aBB;
  //
  aDim = BOPTools_AlgoTools::Dimension(myArguments.First());
  myType = TypeToExplore(aDim);
  //
  aBB.MakeCompound(aC);
  TopExp_Explorer aExp(myShape, myType);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Shape& aS = aExp.Current();
    aBB.Add(aC, aS);
  }
  myAllParts = aC;
}

//=======================================================================
//function : IndexParts
//purpose  : 
//=======================================================================
void BOPAlgo_CellsBuilder::IndexParts()
{
  BOPCol_ListIteratorOfListOfShape aIt, aItIm;
  TopExp_Explorer aExp;
  //
  aIt.Initialize(myArguments);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    //
    aExp.Init(aS, myType);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aST = aExp.Current();
      const BOPCol_ListOfShape* pLSIm = myImages.Seek(aST);
      if (!pLSIm) {
        BOPCol_ListOfShape* pLS = myIndex.ChangeSeek(aST);
        if (!pLS) {
          pLS = &myIndex(myIndex.Add(aST, BOPCol_ListOfShape()));
        }
        pLS ->Append(aS);
        continue;
      }
      //
      aItIm.Initialize(*pLSIm);
      for (; aItIm.More(); aItIm.Next()) {
        const TopoDS_Shape& aSTIm = aItIm.Value();
        //
        BOPCol_ListOfShape* pLS = myIndex.ChangeSeek(aSTIm);
        if (!pLS) {
          pLS = &myIndex(myIndex.Add(aSTIm, BOPCol_ListOfShape()));
        }
        pLS ->Append(aS);
      } // for (; aItIm.More(); aItIm.Next()) {
    } // for (; aExp.More(); aExp.Next()) {
  } // for (; aIt.More(); aIt.Next()) {
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
  Standard_Boolean bChanged;
  BRep_Builder aBB;
  BOPCol_MapOfShape aResParts;
  TopExp_Explorer aExp;
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  bChanged = Standard_False;
  // collect result parts to avoid multiple adding of the same parts
  aExp.Init(myShape, myType);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Shape& aPart = aExp.Current();
    aResParts.Add(aPart);
  }
  // add parts to result 
  aIt.Initialize(aParts);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aPart = aIt.Value();
    if (aResParts.Add(aPart)) {
      aBB.Add(myShape, aPart);
      bChanged = Standard_True;
    }
  }
  //
  // update the material
  if (theMaterial != 0) {
    BOPCol_ListOfShape aLSP;
    aIt.Initialize(aParts);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aPart = aIt.Value();
      if (!myShapeMaterial.IsBound(aPart)) {
        myShapeMaterial.Bind(aPart, theMaterial);
        aLSP.Append(aPart);
      }
    } // for (; aIt.More(); aIt.Next()) {
    //
    if (aLSP.Extent()) {
      if (myMaterials.IsBound(theMaterial)) {
        BOPCol_ListOfShape& aLS = myMaterials.ChangeFind(theMaterial);
        aLS.Append(aLSP);
      } // if (myMaterials.IsBound(theMaterial)) {
      else {
        myMaterials.Bind(theMaterial, aLSP);
      }
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
  TopoDS_Compound aResult;
  BRep_Builder aBB;
  BOPCol_ListOfShape aLSM;
  //
  aBB.MakeCompound(aResult);
  myShapeMaterial.Clear();
  myMaterials.Clear();
  myMapGenerated.Clear();
  //
  TopoDS_Iterator aIt(myAllParts);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aPart = aIt.Value();
    aBB.Add(aResult, aPart);
    //
    if (theMaterial != 0) {
      myShapeMaterial.Bind(aPart, theMaterial);
      aLSM.Append(aPart);
    }
  }
  //
  myShape = aResult;
  //
  if (theMaterial != 0) {
    myMaterials.Bind(theMaterial, aLSM);
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
  BOPCol_MapOfShape aPartsToRemove;
  BOPCol_ListIteratorOfListOfShape aItP, aItM;
  //
  // collect parts into the map and remove parts from materials
  aItP.Initialize(aParts);
  for (; aItP.More(); aItP.Next()) {
    const TopoDS_Shape& aPart = aItP.Value();
    aPartsToRemove.Add(aPart);
    //
    if (myShapeMaterial.IsBound(aPart)) {
      Standard_Integer iMaterial = myShapeMaterial.Find(aPart);
      if (!myMaterials.IsBound(iMaterial)) {
        myShapeMaterial.UnBind(aPart);
        continue;
      }
      //
      BOPCol_ListOfShape& aLSM = myMaterials.ChangeFind(iMaterial);
      //
      aItM.Initialize(aLSM);
      for (; aItM.More(); aItM.Next()) {
        const TopoDS_Shape& aSM = aItM.Value();
        if (aSM.IsSame(aPart)) {
          aLSM.Remove(aItM);
          break;
        } // if (aSM.IsSame(aPart)) {
      } // for (; aItM.More(); aItM.Next()) {
      //
      myShapeMaterial.UnBind(aPart);
    } // if (myShapeMaterial.IsBound(aPart)) {
  } // for (; aItP.More(); aItP.Next()) {
  //
  Standard_Boolean bChanged;
  TopoDS_Compound aResult;
  BRep_Builder aBB;
  TopoDS_Iterator aIt1, aIt2;
  TopAbs_ShapeEnum aType;
  //
  aBB.MakeCompound(aResult);
  bChanged = Standard_False;
  //
  aIt1.Initialize(myShape);
  for (; aIt1.More(); aIt1.Next()) {
    const TopoDS_Shape& aS = aIt1.Value();
    aType = aS.ShapeType();
    //
    if (aType == myType) {
      // basic element
      if (aPartsToRemove.Contains(aS)) {
        bChanged = Standard_True;
        continue;
      }
      aBB.Add(aResult, aS);
    } // if (aType == myType) {
    else {
      // container
      Standard_Boolean bNotEmpty;
      TopoDS_Compound aSC;
      aBB.MakeCompound(aSC);
      //
      bNotEmpty = Standard_False;
      aIt2.Initialize(aS);
      for (; aIt2.More(); aIt2.Next()) {
        const TopoDS_Shape& aSS = aIt2.Value();
        if (aPartsToRemove.Contains(aSS)) {
          bChanged = Standard_True;
          continue;
        }
        aBB.Add(aSC, aSS);
        bNotEmpty = Standard_True;
      } // for (; aIt2.More(); aIt2.Next()) {
      //
      if (bNotEmpty) {
        MakeTypedContainers(aSC, myType, aResult);
      }
    } // else {
  } // for (; aIt1.More(); aIt1.Next()) {
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
  BRep_Builder aBB;
  //
  aBB.MakeCompound(aC);
  myShape = aC;
  //
  myMaterials.Clear();
  myShapeMaterial.Clear();
  myMapGenerated.Clear();
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
  Standard_Integer iMaterial, iErr;
  TopoDS_Compound aResult;
  BRep_Builder aBB;
  TopExp_Explorer aExp;
  BOPCol_ListIteratorOfListOfShape aItS;
  BOPCol_DataMapIteratorOfDataMapOfIntegerListOfShape aItM;
  //
  aBB.MakeCompound(aResult);
  aExp.Init(myShape, myType);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Shape& aS = aExp.Current();
    if (!myShapeMaterial.IsBound(aS)) {
      aBB.Add(aResult, aS);
    }
  }
  //
  aItM.Initialize(myMaterials);
  for (; aItM.More(); aItM.Next()) {
    iMaterial = aItM.Key();
    //
    BOPCol_ListOfShape aLSNew;
    BOPCol_ListOfShape& aLS = myMaterials(iMaterial);
    iErr = RemoveInternals(aLS, aLSNew);
    if (iErr || aLSNew.IsEmpty()) {
      myErrorStatus = 202; // unable to remove internal boundaries
      return;
    }
    //
    // update materials maps and add new shapes to result
    aLS.Assign(aLSNew);
    aItS.Initialize(aLSNew);
    for (; aItS.More(); aItS.Next()) {
      const TopoDS_Shape& aS = aItS.Value();
      aBB.Add(aResult, aS);
      if (!myShapeMaterial.IsBound(aS)) {
        myShapeMaterial.Bind(aS, iMaterial);
      }
    }
  }
  //
  myShape = aResult;
  //
  PrepareHistory();
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
  Standard_Boolean bFound;
  Standard_Integer aNbS;
  BOPCol_ListIteratorOfListOfShape aItIm, aItArgs;
  BOPCol_MapOfShape aMSToTake, aMSToAvoid, aMS;
  TopExp_Explorer aExp;
  //
  aItArgs.Initialize(theLSToAvoid);
  for (; aItArgs.More(); aItArgs.Next()) {
    const TopoDS_Shape& aS = aItArgs.Value();
    aMSToAvoid.Add(aS);
  }
  //
  aItArgs.Initialize(theLSToTake);
  for (; aItArgs.More(); aItArgs.Next()) {
    const TopoDS_Shape& aS = aItArgs.Value();
    aMSToTake.Add(aS);
  }
  //
  aNbS = aMSToTake.Extent();
  //
  const TopoDS_Shape& aSToTake = theLSToTake.First();
  aExp.Init(aSToTake, myType);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Shape& aST = aExp.Current();
    BOPCol_ListOfShape aLSTIm;
    if (!myImages.IsBound(aST)) {
      aLSTIm.Append(aST);
    } else {
      aLSTIm = myImages.Find(aST);
    }
    //
    aItIm.Initialize(aLSTIm);
    for (; aItIm.More(); aItIm.Next()) {
      const TopoDS_Shape& aPart = aItIm.Value();
      //
      if (!myIndex.Contains(aPart)) {
        continue;
      }
      //
      const BOPCol_ListOfShape& aLS = myIndex.FindFromKey(aPart);
      if (aLS.Extent() < aNbS) {
        continue;
      }
      //
      aMS.Clear();
      aItArgs.Initialize(aLS);
      for (bFound = Standard_True; aItArgs.More() && bFound; aItArgs.Next()) {
        const TopoDS_Shape& aS = aItArgs.Value();
        bFound = !aMSToAvoid.Contains(aS);
        aMS.Add(aS);
      }
      //
      if (!bFound) {
        continue;
      }
      //
      aItArgs.Initialize(theLSToTake);
      for (; aItArgs.More() && bFound; aItArgs.Next()) {
        const TopoDS_Shape& aS = aItArgs.Value();
        bFound = aMS.Contains(aS);
      }
      //
      if (bFound) {
        theParts.Append(aPart);
      } // if (bFound) {
    } // for (; aItIm.More(); aItIm.Next()) {
  } // for (; aExp.More(); aExp.Next()) {
}

//=======================================================================
//function : MakeContainers
//purpose  : 
//=======================================================================
void BOPAlgo_CellsBuilder::MakeContainers()
{
  TopoDS_Compound aResult;
  BRep_Builder aBB;
  //
  aBB.MakeCompound(aResult);
  MakeTypedContainers(myShape, myType, aResult);
  myShape = aResult;
}

//=======================================================================
//function : RemoveInternals
//purpose  : 
//=======================================================================
Standard_Integer BOPAlgo_CellsBuilder::RemoveInternals(const BOPCol_ListOfShape& theLS,
                                                       BOPCol_ListOfShape& theLSNew)
{
  Standard_Integer iErr = 0;
  if (theLS.Extent() < 2) {
    theLSNew = theLS;
    return iErr;
  }
  //  
  TopExp_Explorer aExp;
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
    aExp.Init(aSNew, aType);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aSn = aExp.Current();
      theLSNew.Append(aSn);
    }
    //
    // fill map of generated shapes
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
      const TopoDS_Shape& aSGen = anUnify.Generated(aSS);
      if (!aSGen.IsNull() && !aSS.IsSame(aSGen)) {
        myMapGenerated.Bind(aSS, aSGen);
      }
    }
  }
  else if (aType == TopAbs_SOLID) {
    // build all solids from the faces
    BOPCol_ListOfShape aLSF;
    //
    for (BOPCol_ListIteratorOfListOfShape aIt(theLS); aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aS = aIt.Value();
      //
      aExp.Init(aS, TopAbs_FACE);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aF = aExp.Current();
        aLSF.Append(aF);
      }
    }
    //
    BOPAlgo_BuilderSolid aBS;
    aBS.SetShapes(aLSF);
    aBS.Perform();
    //
    iErr = aBS.ErrorStatus();
    if (iErr) {
      return iErr;
    }
    //
    theLSNew = aBS.Areas();
    if (theLSNew.Extent() == 1) {
      return iErr;
    }
    //
    // result is a list of solids. we need to select external faces.
    BOPCol_IndexedDataMapOfShapeListOfShape aDMFS;
    BOPCol_ListOfShape aLFNew;
    Standard_Integer i, aNb;
    //
    // map faces and solids
    for (BOPCol_ListIteratorOfListOfShape aIt(theLSNew); aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aS = aIt.Value();
      //
      aExp.Init(aS, TopAbs_FACE);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aF = aExp.Current();
        if (aDMFS.Contains(aF)) {
          BOPCol_ListOfShape& aLFS = aDMFS.ChangeFromKey(aF);
          aLFS.Append(aS);
        }
        else {
          BOPCol_ListOfShape aLFS;
          aLFS.Append(aS);
          aDMFS.Add(aF, aLFS);
        }
      }
    }
    //
    // select faces attached to only one solid
    aNb = aDMFS.Extent();
    for (i = 1; i <= aNb; ++i) {
      const BOPCol_ListOfShape& aLS = aDMFS(i);
      if (aLS.Extent() == 1) {
        const TopoDS_Shape& aF = aDMFS.FindKey(i);
        aLFNew.Append(aF);
      }
    }
    //
    if (aNb == aLFNew.Extent()) {
      return iErr;
    }
    //
    // build new solid
    BOPAlgo_BuilderSolid aBS1;
    aBS1.SetShapes(aLFNew);
    aBS1.Perform();
    //
    iErr = aBS1.ErrorStatus();
    if (iErr) {
      return iErr;
    }
    //
    theLSNew = aBS1.Areas();
  }
  //
  return iErr;
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
  Standard_Boolean bHasImage, bHasGenerated;
  //
  bHasImage = myImages.IsBound(theS);
  bHasGenerated = myMapGenerated.IsBound(theS);
  if (!bHasImage && !bHasGenerated) {
    bRet = !myMapShape.Contains(theS);
    return bRet;
  }
  //
  if (bHasGenerated) {
    const TopoDS_Shape& aSG = myMapGenerated.Find(theS);
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
      const TopoDS_Shape& aSpRG = myMapGenerated.IsBound(aSpR) ?
        myMapGenerated.Find(aSpR) : aSpR;
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
//function : Generated
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& BOPAlgo_CellsBuilder::Generated(const TopoDS_Shape& theS)
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
  Standard_Boolean bHasGenerated = myMapGenerated.IsBound(theS);
  if (bHasGenerated) {
    const TopoDS_Shape& aSG = myMapGenerated.Find(theS);
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
    if (myMapGenerated.IsBound(aSpR)) {
      const TopoDS_Shape& aSG = myMapGenerated.Find(aSpR);
      if (myMapShape.Contains(aSG)) {
        if (aMFence.Add(aSG)) {
          myHistShapes.Append(aSG);
        }
      }
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
                         const TopAbs_ShapeEnum aType,
                         TopoDS_Shape& theResult)
{
  TopAbs_ShapeEnum aContainerType, aConnexityType, aPartType;
  //
  aPartType = aType;
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
