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


#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_SectionAttribute.hxx>
#include <BOPAlgo_Alerts.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_Iterator.hxx>
#include <IntTools_Context.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>

namespace
{
  //=======================================================================
  //function : BOPAlgo_PIOperation
  //purpose  : List of operations to be supported by the Progress Indicator
  //=======================================================================
  enum BOPAlgo_PIOperation
  {
    PIOperation_Prepare = 0,
    PIOperation_PerformVV,
    PIOperation_PerformVE,
    PIOperation_PerformEE,
    PIOperation_PerformVF,
    PIOperation_PerformEF,
    PIOperation_RepeatIntersection,
    PIOperation_ForceInterfEE,
    PIOperation_ForceInterfEF,
    PIOperation_PerformFF,
    PIOperation_MakeSplitEdges,
    PIOperation_MakeBlocks,
    PIOperation_MakePCurves,
    PIOperation_ProcessDE,
    PIOperation_Last
  };
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_PaveFiller::BOPAlgo_PaveFiller()
  :
  BOPAlgo_Algo()
{
  myDS = NULL;
  myIterator = NULL;
  myNonDestructive = Standard_False;
  myIsPrimary = Standard_True;
  myAvoidBuildPCurve = Standard_False;
  myGlue = BOPAlgo_GlueOff;
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_PaveFiller::BOPAlgo_PaveFiller
(const Handle (NCollection_BaseAllocator)& theAllocator)
  :
  BOPAlgo_Algo (theAllocator),
  myFPBDone (1, theAllocator),
  myIncreasedSS (1, theAllocator),
  myVertsToAvoidExtension (1, theAllocator),
  myDistances (1, theAllocator)
{
  myDS = NULL;
  myIterator = NULL;
  myNonDestructive = Standard_False;
  myIsPrimary = Standard_True;
  myAvoidBuildPCurve = Standard_False;
  myGlue = BOPAlgo_GlueOff;
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
BOPAlgo_PaveFiller::~BOPAlgo_PaveFiller()
{
  Clear();
}
//=======================================================================
//function : SetNonDestructive
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::SetNonDestructive (const Standard_Boolean bFlag)
{
  myNonDestructive = bFlag;
}
//=======================================================================
//function : NonDestructive
//purpose  : 
//=======================================================================
Standard_Boolean BOPAlgo_PaveFiller::NonDestructive() const
{
  return myNonDestructive;
}
//=======================================================================
//function : SetGlue
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::SetGlue (const BOPAlgo_GlueEnum theGlue)
{
  myGlue = theGlue;
}
//=======================================================================
//function : Glue
//purpose  : 
//=======================================================================
BOPAlgo_GlueEnum BOPAlgo_PaveFiller::Glue() const
{
  return myGlue;
}
//=======================================================================
//function : SetIsPrimary
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::SetIsPrimary (const Standard_Boolean bFlag)
{
  myIsPrimary = bFlag;
}
//=======================================================================
//function : IsPrimary
//purpose  : 
//=======================================================================
Standard_Boolean BOPAlgo_PaveFiller::IsPrimary() const
{
  return myIsPrimary;
}
//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::Clear()
{
  BOPAlgo_Algo::Clear();
  if (myIterator) {
    delete myIterator;
    myIterator = NULL;
  }
  if (myDS) {
    delete myDS;
    myDS = NULL;
  }
  myIncreasedSS.Clear();
}
//=======================================================================
//function : DS
//purpose  : 
//=======================================================================
const BOPDS_DS& BOPAlgo_PaveFiller::DS()
{
  return *myDS;
}
//=======================================================================
//function : PDS
//purpose  : 
//=======================================================================
BOPDS_PDS BOPAlgo_PaveFiller::PDS()
{
  return myDS;
}
//=======================================================================
//function : Context
//purpose  : 
//=======================================================================
const Handle (IntTools_Context)& BOPAlgo_PaveFiller::Context()
{
  return myContext;
}
//=======================================================================
//function : SectionAttribute
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::SetSectionAttribute
(const BOPAlgo_SectionAttribute& theSecAttr)
{
  mySectionAttribute = theSecAttr;
}
//=======================================================================
// function: Init
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::Init (const Message_ProgressRange& theRange)
{
  if (!myArguments.Extent()) {
    AddError (new BOPAlgo_AlertTooFewArguments);
    return;
  }
  //
  Message_ProgressScope aPS (theRange, "Initialization of Intersection algorithm", 1);
  TopTools_ListIteratorOfListOfShape aIt (myArguments);
  for (; aIt.More(); aIt.Next()) {
    if (aIt.Value().IsNull()) {
      AddError (new BOPAlgo_AlertNullInputShapes);
      return;
    }
  }
  //
  // 0 Clear
  Clear();
  //
  // 1.myDS 
  myDS = new BOPDS_DS (myAllocator);
  myDS->SetArguments (myArguments);
  myDS->Init (myFuzzyValue);
  //
  // 2 myContext
  myContext = new IntTools_Context;
  //
  // 3.myIterator 
  myIterator = new BOPDS_Iterator (myAllocator);
  myIterator->SetRunParallel (myRunParallel);
  myIterator->SetDS (myDS);
  myIterator->Prepare (myContext, myUseOBB, myFuzzyValue);
  //
  // 4 NonDestructive flag
  SetNonDestructive();
}

//=======================================================================
// function: Perform
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::Perform (const Message_ProgressRange& theRange)
{
  try {
    OCC_CATCH_SIGNALS
      //
      PerformInternal (theRange);
  }
  //
  catch (Standard_Failure const&) {
    AddError (new BOPAlgo_AlertIntersectionFailed);
  }
}

//=======================================================================
// function: PerformInternal
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::PerformInternal (const Message_ProgressRange& theRange)
{
  Message_ProgressScope aPS (theRange, "Performing intersection of shapes", 100);

  Init (aPS.Next (5));
  if (HasErrors()) {
    return;
  }

  // Compute steps of the PI
  BOPAlgo_PISteps aSteps (PIOperation_Last);
  analyzeProgress (95, aSteps);
  //
  Prepare (aPS.Next (aSteps.GetStep (PIOperation_Prepare)));
  if (HasErrors()) {
    return;
  }
  // 00
  PerformVV (aPS.Next (aSteps.GetStep (PIOperation_PerformVV)));
  if (HasErrors()) {
    return;
  }
  // 01
  PerformVE (aPS.Next (aSteps.GetStep (PIOperation_PerformVE)));
  if (HasErrors()) {
    return;
  }
  //
  UpdatePaveBlocksWithSDVertices();
  // 11
  PerformEE (aPS.Next (aSteps.GetStep (PIOperation_PerformEE)));
  if (HasErrors()) {
    return;
  }
  UpdatePaveBlocksWithSDVertices();
  // 02
  PerformVF (aPS.Next (aSteps.GetStep (PIOperation_PerformVF)));
  if (HasErrors()) {
    return;
  }
  UpdatePaveBlocksWithSDVertices();
  // 12
  PerformEF (aPS.Next (aSteps.GetStep (PIOperation_PerformEF)));
  if (HasErrors()) {
    return;
  }
  UpdatePaveBlocksWithSDVertices();
  UpdateInterfsWithSDVertices();

  // Repeat Intersection with increased vertices
  RepeatIntersection (aPS.Next (aSteps.GetStep (PIOperation_RepeatIntersection)));
  if (HasErrors())
    return;
  // Force intersection of edges after increase
  // of the tolerance values of their vertices
  ForceInterfEE (aPS.Next (aSteps.GetStep (PIOperation_ForceInterfEE)));
  if (HasErrors())
  {
    return;
  }
  // Force Edge/Face intersection after increase
  // of the tolerance values of their vertices
  ForceInterfEF (aPS.Next (aSteps.GetStep (PIOperation_ForceInterfEF)));
  if (HasErrors())
  {
    return;
  }
  //
  // 22
  PerformFF (aPS.Next (aSteps.GetStep (PIOperation_PerformFF)));
  if (HasErrors()) {
    return;
  }
  //
  UpdateBlocksWithSharedVertices();
  //
  myDS->RefineFaceInfoIn();
  //
  MakeSplitEdges (aPS.Next (aSteps.GetStep (PIOperation_MakeSplitEdges)));
  if (HasErrors()) {
    return;
  }
  //
  UpdatePaveBlocksWithSDVertices();
  //
  MakeBlocks (aPS.Next (aSteps.GetStep (PIOperation_MakeBlocks)));
  if (HasErrors()) {
    return;
  }
  //
  CheckSelfInterference();
  //
  UpdateInterfsWithSDVertices();
  myDS->ReleasePaveBlocks();
  myDS->RefineFaceInfoOn();
  //
  RemoveMicroEdges();
  //
  MakePCurves (aPS.Next (aSteps.GetStep (PIOperation_MakePCurves)));
  if (HasErrors()) {
    return;
  }
  //
  ProcessDE (aPS.Next (aSteps.GetStep (PIOperation_ProcessDE)));
  if (HasErrors()) {
    return;
  }
}

//=======================================================================
// function: RepeatIntersection
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::RepeatIntersection (const Message_ProgressRange& theRange)
{
  // Find all vertices with increased tolerance
  TColStd_MapOfInteger anExtraInterfMap;
  const Standard_Integer aNbS = myDS->NbSourceShapes();
  Message_ProgressScope aPS (theRange, "Repeat intersection", 3);
  for (Standard_Integer i = 0; i < aNbS; ++i)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo (i);
    if (aSI.ShapeType() != TopAbs_VERTEX)
      continue;
    // Check if the tolerance of the original vertex has been increased
    if (myIncreasedSS.Contains (i))
    {
      anExtraInterfMap.Add (i);
      continue;
    }

    // Check if the vertex created a new vertex with greater tolerance
    Standard_Integer nVSD;
    if (!myDS->HasShapeSD (i, nVSD))
      continue;

    if (myIncreasedSS.Contains (nVSD))
      anExtraInterfMap.Add (i);
  }

  if (anExtraInterfMap.IsEmpty())
    return;

  // Update iterator of pairs of shapes with interfering boxes
  myIterator->IntersectExt (anExtraInterfMap);

  // Perform intersections with vertices

  PerformVV (aPS.Next());
  if (HasErrors())
    return;
  UpdatePaveBlocksWithSDVertices();

  PerformVE (aPS.Next());
  if (HasErrors())
    return;
  UpdatePaveBlocksWithSDVertices();

  PerformVF (aPS.Next());
  if (HasErrors())
    return;

  UpdatePaveBlocksWithSDVertices();
  UpdateInterfsWithSDVertices();
}


//=======================================================================
// function: fillPISteps
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::fillPIConstants (const Standard_Real theWhole,
                                          BOPAlgo_PISteps& theSteps) const
{
  if (!myNonDestructive)
  {
    theSteps.SetStep (PIOperation_Prepare, 1 * theWhole / 100.);
  }
}

//=======================================================================
// function: fillPISteps
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::fillPISteps (BOPAlgo_PISteps& theSteps) const
{
  // Get number of all intersecting pairs
  Standard_Integer aVVSize = 0, aVESize = 0, aEESize = 0, aVFSize = 0, aEFSize = 0, aFFSize = 0;

  myIterator->Initialize (TopAbs_VERTEX, TopAbs_VERTEX);
  aVVSize = myIterator->ExpectedLength();

  myIterator->Initialize (TopAbs_VERTEX, TopAbs_EDGE);
  aVESize = myIterator->ExpectedLength();

  myIterator->Initialize (TopAbs_EDGE, TopAbs_EDGE);
  aEESize = myIterator->ExpectedLength();

  myIterator->Initialize (TopAbs_VERTEX, TopAbs_FACE);
  aVFSize = myIterator->ExpectedLength();

  if (myGlue != BOPAlgo_GlueFull)
  {
    myIterator->Initialize (TopAbs_EDGE, TopAbs_FACE);
    aEFSize = myIterator->ExpectedLength();
  }

  myIterator->Initialize (TopAbs_FACE, TopAbs_FACE);
  aFFSize = myIterator->ExpectedLength();

  theSteps.SetStep (PIOperation_PerformVV, aVVSize);
  theSteps.SetStep (PIOperation_PerformVE, 2 * aVESize);
  theSteps.SetStep (PIOperation_PerformEE, 5 * aEESize);
  theSteps.SetStep (PIOperation_PerformVF, 5 * aVFSize);
  theSteps.SetStep (PIOperation_PerformEF, 10 * aEFSize);
  theSteps.SetStep (PIOperation_RepeatIntersection, 0.2 * (aVVSize + aVESize + aVFSize));
  theSteps.SetStep (PIOperation_ForceInterfEE, 2 * aEESize);
  theSteps.SetStep (PIOperation_ForceInterfEF, 2 * aEFSize);
  theSteps.SetStep (PIOperation_PerformFF, (myGlue == BOPAlgo_GlueFull ? 1 : 30) * aFFSize);
  theSteps.SetStep (PIOperation_MakeSplitEdges, aEESize);
  theSteps.SetStep (PIOperation_MakeBlocks, (myGlue == BOPAlgo_GlueFull ? 0 : 5) * aFFSize);
  theSteps.SetStep (PIOperation_MakePCurves, myAvoidBuildPCurve ? 0 : 0.2 * (aEESize + aEFSize));
  theSteps.SetStep (PIOperation_ProcessDE, 0.1 * aEESize);
}
