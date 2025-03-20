// Created by: Peter KURNEV
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

#include <BRepAlgoAPI_BuilderAlgo.hxx>

#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPDS_DS.hxx>
#include <ShapeUpgrade_UnifySameDomain.hxx>
#include <TopoDS_Shape.hxx>

//=================================================================================================

BRepAlgoAPI_BuilderAlgo::BRepAlgoAPI_BuilderAlgo()
    : BRepAlgoAPI_Algo(),
      myNonDestructive(Standard_False),
      myGlue(BOPAlgo_GlueOff),
      myCheckInverted(Standard_True),
      myFillHistory(Standard_True),
      myIsIntersectionNeeded(Standard_True),
      myDSFiller(NULL),
      myBuilder(NULL)
{
}

//=================================================================================================

BRepAlgoAPI_BuilderAlgo::BRepAlgoAPI_BuilderAlgo(const BOPAlgo_PaveFiller& aPF)
    : BRepAlgoAPI_Algo(),
      myNonDestructive(Standard_False),
      myGlue(BOPAlgo_GlueOff),
      myCheckInverted(Standard_True),
      myFillHistory(Standard_True),
      myIsIntersectionNeeded(Standard_False),
      myBuilder(NULL)
{
  myDSFiller = (BOPAlgo_PaveFiller*)&aPF;
}

//=================================================================================================

BRepAlgoAPI_BuilderAlgo::~BRepAlgoAPI_BuilderAlgo()
{
  Clear();
}

//=================================================================================================

void BRepAlgoAPI_BuilderAlgo::Clear()
{
  BRepAlgoAPI_Algo::Clear();
  if (myDSFiller && myIsIntersectionNeeded)
  {
    delete myDSFiller;
    myDSFiller = NULL;
  }
  if (myBuilder)
  {
    delete myBuilder;
    myBuilder = NULL;
  }
  if (myHistory)
    myHistory.Nullify();

  if (mySimplifierHistory)
    mySimplifierHistory.Nullify();
}

//=================================================================================================

void BRepAlgoAPI_BuilderAlgo::Build(const Message_ProgressRange& theRange)
{
  // Setting not done status
  NotDone();
  // Destroy the tools if necessary
  Clear();
  Message_ProgressScope aPS(theRange, "Performing General Fuse operation", 100);
  // If necessary perform intersection of the argument shapes
  IntersectShapes(myArguments, aPS.Next(70));
  if (HasErrors())
    return;

  // Initialization of the Building tool
  myBuilder = new BOPAlgo_Builder(myAllocator);
  // Set arguments to builder
  myBuilder->SetArguments(myArguments);
  // Build the result basing on intersection results
  BuildResult(aPS.Next(30));
}

//=======================================================================
// function : IntersectShapes
// purpose  : Intersects the given shapes with the intersection tool
//=======================================================================
void BRepAlgoAPI_BuilderAlgo::IntersectShapes(const TopTools_ListOfShape&  theArgs,
                                              const Message_ProgressRange& theRange)
{
  if (!myIsIntersectionNeeded)
    return;

  if (myDSFiller)
    delete myDSFiller;

  // Create new Filler
  myDSFiller = new BOPAlgo_PaveFiller(myAllocator);
  // Set arguments for intersection
  myDSFiller->SetArguments(theArgs);
  // Set options for intersection
  myDSFiller->SetRunParallel(myRunParallel);

  myDSFiller->SetFuzzyValue(myFuzzyValue);
  myDSFiller->SetNonDestructive(myNonDestructive);
  myDSFiller->SetGlue(myGlue);
  myDSFiller->SetUseOBB(myUseOBB);
  // Set Face/Face intersection options to the intersection algorithm
  SetAttributes();
  // Perform intersection
  myDSFiller->Perform(theRange);
  // Check for the errors during intersection
  GetReport()->Merge(myDSFiller->GetReport());
}

//=======================================================================
// function : BuildResult
// purpose  : Builds the result shape
//=======================================================================
void BRepAlgoAPI_BuilderAlgo::BuildResult(const Message_ProgressRange& theRange)
{
  // Set options to the builder
  myBuilder->SetRunParallel(myRunParallel);

  myBuilder->SetCheckInverted(myCheckInverted);
  myBuilder->SetToFillHistory(myFillHistory);
  // Perform building of the result with pre-calculated intersections
  myBuilder->PerformWithFiller(*myDSFiller, theRange);
  // Merge the warnings of the Building part
  GetReport()->Merge(myBuilder->GetReport());
  // Check for the errors
  if (myBuilder->HasErrors())
    return;
  // Set done status
  Done();
  // Get the result shape
  myShape = myBuilder->Shape();
  // Fill history
  if (myFillHistory)
  {
    myHistory = new BRepTools_History;
    myHistory->Merge(myBuilder->History());
  }
}

//=================================================================================================

void BRepAlgoAPI_BuilderAlgo::SimplifyResult(const Standard_Boolean theUnifyEdges,
                                             const Standard_Boolean theUnifyFaces,
                                             const Standard_Real    theAngularTol)
{
  if (HasErrors())
    return;

  if (!theUnifyEdges && !theUnifyFaces)
    return;

  // Simplification tool
  ShapeUpgrade_UnifySameDomain anUnifier(myShape, theUnifyEdges, theUnifyFaces, Standard_True);
  // Pass options
  anUnifier.SetLinearTolerance(myFuzzyValue);
  anUnifier.SetAngularTolerance(theAngularTol);
  anUnifier.SetSafeInputMode(myNonDestructive);
  anUnifier.AllowInternalEdges(Standard_False);
  // Perform simplification
  anUnifier.Build();
  // Overwrite result with simplified shape
  myShape = anUnifier.Shape();
  // Keep simplification history
  mySimplifierHistory = anUnifier.History();
  if (myFillHistory)
    // Merge simplification history into result history
    myHistory->Merge(mySimplifierHistory);
}

//=================================================================================================

const TopTools_ListOfShape& BRepAlgoAPI_BuilderAlgo::Modified(const TopoDS_Shape& theS)
{
  if (myFillHistory && myHistory)
    return myHistory->Modified(theS);
  myGenerated.Clear();
  return myGenerated;
}

//=================================================================================================

const TopTools_ListOfShape& BRepAlgoAPI_BuilderAlgo::Generated(const TopoDS_Shape& theS)
{
  if (myFillHistory && myHistory)
    return myHistory->Generated(theS);
  myGenerated.Clear();
  return myGenerated;
}

//=================================================================================================

Standard_Boolean BRepAlgoAPI_BuilderAlgo::IsDeleted(const TopoDS_Shape& theS)
{
  return (myFillHistory && myHistory ? myHistory->IsRemoved(theS) : Standard_False);
}

//=================================================================================================

Standard_Boolean BRepAlgoAPI_BuilderAlgo::HasModified() const
{
  return (myFillHistory && myHistory ? myHistory->HasModified() : Standard_False);
}

//=================================================================================================

Standard_Boolean BRepAlgoAPI_BuilderAlgo::HasGenerated() const
{
  return (myFillHistory && myHistory ? myHistory->HasGenerated() : Standard_False);
}

//=================================================================================================

Standard_Boolean BRepAlgoAPI_BuilderAlgo::HasDeleted() const
{
  return (myFillHistory && myHistory ? myHistory->HasRemoved() : Standard_False);
}

//=================================================================================================

const TopTools_ListOfShape& BRepAlgoAPI_BuilderAlgo::SectionEdges()
{
  myGenerated.Clear();
  if (myBuilder == NULL)
    return myGenerated;

  // Fence map to avoid duplicated section edges in the result list
  TopTools_MapOfShape aMFence;
  // Intersection results
  const BOPDS_PDS& pDS = myDSFiller->PDS();
  // Iterate on all Face/Face interferences and take section edges
  BOPDS_VectorOfInterfFF& aFFs  = pDS->InterfFF();
  const Standard_Integer  aNbFF = aFFs.Length();
  for (Standard_Integer i = 0; i < aNbFF; ++i)
  {
    BOPDS_InterfFF& aFFi = aFFs(i);
    // Section curves between pair of faces
    const BOPDS_VectorOfCurve& aSectionCurves = aFFi.Curves();
    const Standard_Integer     aNbC           = aSectionCurves.Length();
    for (Standard_Integer j = 0; j < aNbC; ++j)
    {
      const BOPDS_Curve& aCurve = aSectionCurves(j);
      // Section edges created from the curve
      const BOPDS_ListOfPaveBlock&        aSectionEdges = aCurve.PaveBlocks();
      BOPDS_ListIteratorOfListOfPaveBlock aItPB(aSectionEdges);
      for (; aItPB.More(); aItPB.Next())
      {
        const Handle(BOPDS_PaveBlock)& aPB = aItPB.Value();
        const TopoDS_Shape&            aSE = pDS->Shape(aPB->Edge());
        if (!aMFence.Add(aSE))
          continue;
        // Take into account simplification of the result shape
        if (mySimplifierHistory)
        {
          if (mySimplifierHistory->IsRemoved(aSE))
            continue;

          const TopTools_ListOfShape& aLSEIm = mySimplifierHistory->Modified(aSE);
          if (!aLSEIm.IsEmpty())
          {
            TopTools_ListIteratorOfListOfShape aItLEIm(aLSEIm);
            for (; aItLEIm.More(); aItLEIm.Next())
            {
              if (aMFence.Add(aItLEIm.Value()))
                myGenerated.Append(aItLEIm.Value());
            }
          }
          else
            myGenerated.Append(aSE);
        }
        else
          myGenerated.Append(aSE);
      }
    }
  }
  return myGenerated;
}
