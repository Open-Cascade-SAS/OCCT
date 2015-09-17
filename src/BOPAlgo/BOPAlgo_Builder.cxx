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


#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BRep_Builder.hxx>
#include <IntTools_Context.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_Builder::BOPAlgo_Builder()
:
  BOPAlgo_BuilderShape(),
  myArguments(myAllocator),
  myMapFence(100, myAllocator),
  myPaveFiller(NULL),
  myDS(NULL),
  myEntryPoint(0),
  myImages(100, myAllocator),
  myShapesSD(100, myAllocator),
  mySplits(100, myAllocator),
  myOrigins(100, myAllocator),
  myFuzzyValue(0.)
{
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_Builder::BOPAlgo_Builder
  (const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPAlgo_BuilderShape(theAllocator),
  myArguments(myAllocator),
  myMapFence(100, myAllocator),
  myPaveFiller(NULL),
  myDS(NULL),
  myEntryPoint(0),
  myImages(100, myAllocator), 
  myShapesSD(100, myAllocator),
  mySplits(100, myAllocator),
  myOrigins(100, myAllocator),
  myFuzzyValue(0.)
{
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
BOPAlgo_Builder::~BOPAlgo_Builder()
{
  if (myEntryPoint==1) {
    if (myPaveFiller) {
      delete myPaveFiller;
      myPaveFiller=NULL;
    }
  }
}
//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::Clear()
{
  myArguments.Clear();
  myMapFence.Clear();
  myImages.Clear();
  myShapesSD.Clear();
  mySplits.Clear();
  myOrigins.Clear();
}
//=======================================================================
//function : AddArgument
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::AddArgument(const TopoDS_Shape& theShape)
{
  if (myMapFence.Add(theShape)) {
    myArguments.Append(theShape);
  }
}
//=======================================================================
//function : SetArguments
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::SetArguments(const BOPCol_ListOfShape& theShapes)
{
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  myArguments.Clear();
  //
  aIt.Initialize(theShapes);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    AddArgument(aS);
  }
}
//=======================================================================
//function : Arguments
//purpose  : 
//=======================================================================
const BOPCol_ListOfShape& BOPAlgo_Builder::Arguments()const
{
  return myArguments;
}
//=======================================================================
//function : Images
//purpose  : 
//=======================================================================
const BOPCol_DataMapOfShapeListOfShape& BOPAlgo_Builder::Images()const
{
  return myImages;
}
//=======================================================================
//function : Origins
//purpose  : 
//=======================================================================
const BOPCol_DataMapOfShapeShape& BOPAlgo_Builder::Origins()const
{
  return myOrigins;
}

//=======================================================================
//function : ShapesSd
//purpose  : 
//=======================================================================
const BOPCol_DataMapOfShapeShape& BOPAlgo_Builder::ShapesSD()const
{
  return myShapesSD;
}
//=======================================================================
//function : Splits
//purpose  : 
//=======================================================================
const BOPCol_DataMapOfShapeListOfShape& BOPAlgo_Builder::Splits()const
{
  return mySplits;
}
//=======================================================================
//function : PPaveFiller
//purpose  : 
//=======================================================================
BOPAlgo_PPaveFiller BOPAlgo_Builder::PPaveFiller()
{
  return myPaveFiller;
}
//=======================================================================
//function : PDS
//purpose  : 
//=======================================================================
BOPDS_PDS BOPAlgo_Builder::PDS()
{
  return myDS;
}
//=======================================================================
//function : SetFuzzyValue
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::SetFuzzyValue(const Standard_Real theFuzz)
{
  myFuzzyValue = (theFuzz < 0.) ? 0. : theFuzz;
}
//=======================================================================
//function : FuzzyValue
//purpose  : 
//=======================================================================
Standard_Real BOPAlgo_Builder::FuzzyValue() const
{
  return myFuzzyValue;
}
//=======================================================================
// function: CheckData
// purpose: 
//=======================================================================
void BOPAlgo_Builder::CheckData()
{
  Standard_Integer aNb;
  //
  myErrorStatus=0;
  //
  aNb=myArguments.Extent();
  if (aNb<2) {
    myErrorStatus=100; // too few arguments to process
    return;
  }
  //
  //  myPaveFiller
  if (!myPaveFiller) {
    myErrorStatus=101; 
    return;
  }
  //
  myErrorStatus=myPaveFiller->ErrorStatus();
  if (myErrorStatus) {
    myErrorStatus=102; // PaveFiller is failed
    return;
  }
}
//=======================================================================
//function : Prepare
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::Prepare()
{
  myErrorStatus=0;
  //
  BRep_Builder aBB;
  TopoDS_Compound aC;
  //
  // 1. myShape is empty compound
  aBB.MakeCompound(aC);
  myShape=aC;
  myFlagHistory=Standard_True;
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::Perform()
{
  myErrorStatus=0;
  //
  if (myEntryPoint==1) {
    if (myPaveFiller) {
      delete myPaveFiller;
      myPaveFiller=NULL;
    }
  }
  //
  Handle(NCollection_BaseAllocator) aAllocator=
    NCollection_BaseAllocator::CommonBaseAllocator();
  //
  BOPAlgo_PaveFiller* pPF=new BOPAlgo_PaveFiller(aAllocator);
  //
  pPF->SetArguments(myArguments);
  pPF->SetRunParallel(myRunParallel);
  pPF->SetProgressIndicator(myProgressIndicator);
  pPF->SetFuzzyValue(myFuzzyValue);
  //
  pPF->Perform();
  //
  myEntryPoint=1;
  PerformInternal(*pPF);
}
//=======================================================================
//function : PerformWithFiller
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::PerformWithFiller(const BOPAlgo_PaveFiller& theFiller)
{
  myEntryPoint=0;
  PerformInternal(theFiller);
}
//=======================================================================
//function : PerformInternal
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::PerformInternal(const BOPAlgo_PaveFiller& theFiller)
{
  try { 
    OCC_CATCH_SIGNALS
    PerformInternal1(theFiller);
  }
  //
  catch (Standard_Failure) {
    myErrorStatus=191;
  }  
}
//=======================================================================
//function : PerformInternal1
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::PerformInternal1(const BOPAlgo_PaveFiller& theFiller)
{
  myErrorStatus=0;
  //
  myPaveFiller=(BOPAlgo_PaveFiller*)&theFiller;
  myDS=myPaveFiller->PDS();
  myContext=myPaveFiller->Context();
  //
  // 1. CheckData
  CheckData();
  if (myErrorStatus) {
    return;
  }
  //
  // 2. Prepare
  Prepare();
  if (myErrorStatus) {
    return;
  }
  //
  // 3. Fill Images
  // 3.1 Vertice
  FillImagesVertices();
  if (myErrorStatus) {
    return;
  }
  //
  BuildResult(TopAbs_VERTEX);
  if (myErrorStatus) {
    return;
  }
  // 3.2 Edges
  FillImagesEdges();
  if (myErrorStatus) {
    return;
  }
  //
  BuildResult(TopAbs_EDGE);
  if (myErrorStatus) {
    return;
  }
  //
  // 3.3 Wires
  FillImagesContainers(TopAbs_WIRE);
  if (myErrorStatus) {
    return;
  }
  //
  BuildResult(TopAbs_WIRE);
  if (myErrorStatus) {
    return;
  }
  
  // 3.4 Faces
  FillImagesFaces();
  if (myErrorStatus) {
    return;
  }
  //
  BuildResult(TopAbs_FACE);
  if (myErrorStatus) {
    return;
  }
  // 3.5 Shells
  FillImagesContainers(TopAbs_SHELL);
  if (myErrorStatus) {
    return;
  }
  
  BuildResult(TopAbs_SHELL);
  if (myErrorStatus) {
    return;
  }
  // 3.6 Solids
  FillImagesSolids();
  if (myErrorStatus) {
    return;
  }
  
  BuildResult(TopAbs_SOLID);
  if (myErrorStatus) {
    return;
  }
  // 3.7 CompSolids
  FillImagesContainers(TopAbs_COMPSOLID);
  if (myErrorStatus) {
    return;
  }
  
  BuildResult(TopAbs_COMPSOLID);
  if (myErrorStatus) {
    return;
  }
  
  // 3.8 Compounds
  FillImagesCompounds();
  if (myErrorStatus) {
    return;
  }
  
  BuildResult(TopAbs_COMPOUND);
  if (myErrorStatus) {
    return;
  }
  //
  // 4.History
  PrepareHistory();
  //
  //
  // 5 Post-treatment 
  PostTreat();
  
}
//
// myErrorStatus
// 
// 0  - Ok
// 
//=======================================================================
//function : PostTreat
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::PostTreat()
{
  BOPTools_AlgoTools::CorrectTolerances(myShape, 0.05, myRunParallel);
  BOPTools_AlgoTools::CorrectShapeTolerances(myShape, myRunParallel);
}
