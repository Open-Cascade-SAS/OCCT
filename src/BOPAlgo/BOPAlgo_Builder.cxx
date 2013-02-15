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


#include <BOPAlgo_Builder.ixx>

#include <NCollection_IncAllocator.hxx>

#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>


#include <BOPTools_AlgoTools.hxx>

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
  //myContext(NULL),
  myEntryPoint(0),
  myImages(100, myAllocator),
  myShapesSD(100, myAllocator),
  mySplits(100, myAllocator),
  myOrigins(100, myAllocator)
{
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  BOPAlgo_Builder::BOPAlgo_Builder(const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPAlgo_BuilderShape(theAllocator),
  myArguments(myAllocator),
  myMapFence(100, myAllocator),
  myPaveFiller(NULL),
  myDS(NULL),
  //myContext(NULL),
  myEntryPoint(0),
  myImages(100, myAllocator), 
  myShapesSD(100, myAllocator),
  mySplits(100, myAllocator),
  myOrigins(100, myAllocator)
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
  Handle(NCollection_BaseAllocator) aAllocator=new NCollection_IncAllocator;
  //
  BOPAlgo_PaveFiller* pPF=new BOPAlgo_PaveFiller(aAllocator);
  //
  pPF->SetArguments(myArguments);
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
  //BRepLib::SameParameter(myShape, 1.e-7, Standard_True);
  BOPTools_AlgoTools::CorrectTolerances(myShape, 0.05);
  BOPTools_AlgoTools::CorrectShapeTolerances(myShape);
}
