// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2014 OPEN CASCADE SAS
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
#include <BOPAlgo_BuilderSolid.hxx>
#include <BOPAlgo_MakerVolume.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Tools.hxx>
#include <BOPAlgo_Alerts.hxx>
#include <BOPDS_DS.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Solid.hxx>
#include <TopTools_ListOfShape.hxx>

static
  void AddFace(const TopoDS_Shape& theF,
               TopTools_ListOfShape& theLF);

//=======================================================================
//function : CheckData
//purpose  : 
//=======================================================================
void BOPAlgo_MakerVolume::CheckData()
{
  if (myArguments.IsEmpty()) {
    AddError (new BOPAlgo_AlertTooFewArguments); // no arguments to process
    return;
  }
  //
  CheckFiller();
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BOPAlgo_MakerVolume::Perform()
{
  GetReport()->Clear();
  //
  if (myEntryPoint == 1) {
    if (myPaveFiller) {
      delete myPaveFiller;
      myPaveFiller = NULL;
    }
  }
  //
  Handle(NCollection_BaseAllocator) aAllocator = 
    NCollection_BaseAllocator::CommonBaseAllocator();
  BOPAlgo_PaveFiller* pPF = new BOPAlgo_PaveFiller(aAllocator);
  //
  if (!myIntersect) {
    //if there is no need to intersect the arguments, then it is necessary
    //to create the compound of them and use it as one argument
    TopoDS_Compound anArgs;
    BRep_Builder aBB;
    TopTools_ListIteratorOfListOfShape aIt;
    TopTools_ListOfShape aLS;
    //
    aBB.MakeCompound(anArgs);
    aIt.Initialize(myArguments);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aS = aIt.Value();
      aBB.Add(anArgs, aS);
    }
    aLS.Append(anArgs);
    //
    pPF->SetArguments(aLS);
  }
  else {
    pPF->SetArguments(myArguments);
  }
  //
  pPF->SetRunParallel(myRunParallel);
  pPF->SetProgressIndicator(myProgressIndicator);
  pPF->SetFuzzyValue(myFuzzyValue);
  pPF->SetNonDestructive(myNonDestructive);
  pPF->SetGlue(myGlue);
  pPF->SetUseOBB(myUseOBB);
  pPF->Perform();
  //
  myEntryPoint = 1;
  PerformInternal(*pPF);
}

//=======================================================================
//function : PerformInternal1
//purpose  : 
//=======================================================================
void BOPAlgo_MakerVolume::PerformInternal1
  (const BOPAlgo_PaveFiller& theFiller)
{
  myPaveFiller = (BOPAlgo_PaveFiller*)&theFiller;
  myDS = myPaveFiller->PDS();
  myContext = myPaveFiller->Context();
  //
  // 1. CheckData
  CheckData();
  if (HasErrors()) {
    return;
  }
  //
  // 2. Prepare
  Prepare();
  if (HasErrors()) {
    return;
  }
  //
  // 3. Fill Images
  // 3.1. Vertice
  if (myIntersect) {
    FillImagesVertices();
    if (HasErrors()) {
      return;
    }
    // 3.2. Edges
    FillImagesEdges();
    if (HasErrors()) {
      return;
    }
    // 3.3. Wires
    FillImagesContainers(TopAbs_WIRE);
    if (HasErrors()) {
      return;
    }
    // 3.4. Faces
    FillImagesFaces();
    if (HasErrors()) {
      return;
    }
  }
  //
  // 4. Collect faces
  CollectFaces();
  if (HasErrors()) {
    return;
  }
  //
  TopTools_MapOfShape aBoxFaces;
  TopTools_ListOfShape aLSR;
  //
  // 5. Create bounding box
  MakeBox(aBoxFaces);
  //
  // 6. Make volumes
  BuildSolids(aLSR);
  if (HasErrors()) {
    return;
  }
  //
  // 7. Treat the result
  RemoveBox(aLSR, aBoxFaces);
  //
  // 8. Fill internal shapes
  FillInternalShapes(aLSR);
  //
  // 9. Build Result
  BuildShape(aLSR);
  //
  // 10. History
  PrepareHistory();
  //
  // 11. Post-treatment 
  PostTreat();  
}

//=======================================================================
//function : CollectFaces
//purpose  : 
//=======================================================================
void BOPAlgo_MakerVolume::CollectFaces()
{
  UserBreak();
  //
  Standard_Integer i, aNbShapes;
  TopTools_ListIteratorOfListOfShape aIt;
  TopTools_MapOfShape aMFence;
  //
  aNbShapes = myDS->NbSourceShapes();
  for (i = 0; i < aNbShapes; ++i) {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (aSI.ShapeType() != TopAbs_FACE) {
      continue;
    }
    //
    const Bnd_Box& aB = aSI.Box();
    myBBox.Add(aB);
    //
    const TopoDS_Shape& aF = aSI.Shape();
    if (myImages.IsBound(aF)) {
      const TopTools_ListOfShape& aLFIm = myImages.Find(aF);
      aIt.Initialize(aLFIm);
      for (; aIt.More(); aIt.Next()) {
        const TopoDS_Shape& aFIm = aIt.Value();
        if (aMFence.Add(aFIm)) {
          AddFace(aFIm, myFaces);
        }
      }
    }
    else {
      AddFace(aF, myFaces);
    }
  }
}

//=======================================================================
//function : MakeBox
//purpose  : 
//=======================================================================
void BOPAlgo_MakerVolume::MakeBox(TopTools_MapOfShape& theBoxFaces)
{
  UserBreak();
  //
  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax, anExt;
  //
  anExt = sqrt(myBBox.SquareExtent()) * 0.5;
  myBBox.Enlarge(anExt);
  myBBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  //
  gp_Pnt aPMin(aXmin, aYmin, aZmin), 
         aPMax(aXmax, aYmax, aZmax);
  //
  mySBox = BRepPrimAPI_MakeBox(aPMin, aPMax).Solid();
  //
  TopExp_Explorer aExp(mySBox, TopAbs_FACE);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Shape& aF = aExp.Current();
    myFaces.Append(aF);
    theBoxFaces.Add(aF);
  }
}

//=======================================================================
//function : BuildSolids
//purpose  : 
//=======================================================================
void BOPAlgo_MakerVolume::BuildSolids(TopTools_ListOfShape& theLSR)
{
  UserBreak();
  //
  BOPAlgo_BuilderSolid aBS;
  //
  aBS.SetShapes(myFaces);
  aBS.SetRunParallel(myRunParallel);
  aBS.SetAvoidInternalShapes(myAvoidInternalShapes);
  aBS.Perform();
  if (aBS.HasErrors())
  {
    AddError (new BOPAlgo_AlertSolidBuilderFailed); // SolidBuilder failed
    return;
  }
  //
  myReport->Merge(aBS.GetReport());
  //
  theLSR = aBS.Areas();
}

//=======================================================================
//function : TreatResult
//purpose  : 
//=======================================================================
void BOPAlgo_MakerVolume::RemoveBox(TopTools_ListOfShape&      theLSR,
                                    const TopTools_MapOfShape& theBoxFaces)
{
  UserBreak();
  //
  TopTools_ListIteratorOfListOfShape aIt;
  TopExp_Explorer aExp;
  Standard_Boolean bFound;
  //
  bFound = Standard_False;
  aIt.Initialize(theLSR);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSR = aIt.Value();
    //
    aExp.Init(aSR, TopAbs_FACE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aF = aExp.Current();
      if (theBoxFaces.Contains(aF)) {
        bFound = Standard_True;
        theLSR.Remove(aIt);
        break;
      }
    }
    if (bFound) {
      break;
    }
  }
}

//=======================================================================
//function : BuildShape
//purpose  : 
//=======================================================================
void BOPAlgo_MakerVolume::BuildShape(const TopTools_ListOfShape& theLSR)
{ 
  if (theLSR.Extent() == 1) {
    myShape = theLSR.First();
  }
  else {
    BRep_Builder aBB;
    TopTools_ListIteratorOfListOfShape aIt;
    //
    aIt.Initialize(theLSR);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aSol = aIt.Value();
      aBB.Add(myShape, aSol);
    }
  }
}

//=======================================================================
//function : FillInternalShapes 
//purpose  : 
//=======================================================================
void BOPAlgo_MakerVolume::FillInternalShapes(const TopTools_ListOfShape& theLSR)
{
  if (myAvoidInternalShapes) {
    return;
  }
  //
  UserBreak();
  //
  Standard_Integer aNbSI;
  TopAbs_ShapeEnum aType;
  TopAbs_State aState; 
  TopoDS_Iterator aItS;
  BRep_Builder aBB;
  TopTools_MapOfShape aMFence;
  TopTools_IndexedMapOfShape aMSS;
  TopTools_ListOfShape aLVE, aLSC, aLSIn;
  TopTools_ListIteratorOfListOfShape aIt, aIt1;
  //
  // 1. Collect shapes to process: vertices, edges, wires
  const TopTools_ListOfShape& anArguments = myDS->Arguments();
  aIt.Initialize(anArguments);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    BOPAlgo_Tools::TreatCompound(aS, aMFence, aLSC);
  }
  //
  aIt.Initialize(aLSC);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    aType = aS.ShapeType();
    if (aType == TopAbs_WIRE) {
      aItS.Initialize(aS);
      for(; aItS.More(); aItS.Next()) {
        const TopoDS_Shape& aE = aItS.Value();
        if (aMFence.Add(aE)) {
          aLVE.Append(aE);
        }
      }
    }
    else if (aType == TopAbs_VERTEX || aType == TopAbs_EDGE) {
      aLVE.Append(aS);
    } 
  }
  //
  aIt.Initialize(theLSR);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    TopExp::MapShapes(aS, TopAbs_EDGE, aMSS);
    TopExp::MapShapes(aS, TopAbs_VERTEX, aMSS);
  }
  //
  aIt.Initialize(aLVE);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    if (myImages.IsBound(aS)) {
      const TopTools_ListOfShape &aLSp = myImages.Find(aS);
      aIt1.Initialize(aLSp);
      for (; aIt1.More(); aIt1.Next()) {
        const TopoDS_Shape& aSp = aIt1.Value();
        if (aMSS.Add(aSp)) {
          aLSIn.Append(aSp);
        }
      }
    }
    else {
      if (aMSS.Add(aS)) {
        aLSIn.Append(aS);
      }
    }
  }
  //
  aNbSI = aLSIn.Extent();
  if (!aNbSI) {
    return;
  }
  //
  // 2. Settle internal vertices and edges into solids
  aIt.Initialize(theLSR);
  for (; aIt.More(); aIt.Next()) {
    TopoDS_Solid aSd = *(TopoDS_Solid*)&aIt.Value();
    //
    aIt1.Initialize(aLSIn);
    for (; aIt1.More(); ) {
      TopoDS_Shape aSI = aIt1.Value();
      aSI.Orientation(TopAbs_INTERNAL);
      //
      aState = BOPTools_AlgoTools::ComputeStateByOnePoint(aSI, aSd, 1.e-11, myContext);
      if (aState == TopAbs_IN) {
        aBB.Add(aSd, aSI);
        aLSIn.Remove(aIt1);
      }
      else {
        aIt1.Next();
      }
    }
  }
}

//=======================================================================
//function : AddFace
//purpose  : 
//=======================================================================
void AddFace(const TopoDS_Shape& theF,
             TopTools_ListOfShape& theLF)
{
  TopoDS_Shape aFF = theF;
  aFF.Orientation(TopAbs_FORWARD);
  theLF.Append(aFF);
  aFF.Orientation(TopAbs_REVERSED);
  theLF.Append(aFF);
}
