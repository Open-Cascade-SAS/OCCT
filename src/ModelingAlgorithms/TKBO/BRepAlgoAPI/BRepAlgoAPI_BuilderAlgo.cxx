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
    : myNonDestructive(false),
      myGlue(BOPAlgo_GlueOff),
      myCheckInverted(true),
      myFillHistory(true),
      myIsIntersectionNeeded(true),
      myDSFiller(nullptr),
      myBuilder(nullptr)
{
}

//=================================================================================================

BRepAlgoAPI_BuilderAlgo::BRepAlgoAPI_BuilderAlgo(const BOPAlgo_PaveFiller& aPF)
    : myNonDestructive(false),
      myGlue(BOPAlgo_GlueOff),
      myCheckInverted(true),
      myFillHistory(true),
      myIsIntersectionNeeded(false),
      myBuilder(nullptr)
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
  if (myIsIntersectionNeeded)
  {
    delete myDSFiller;
    myDSFiller = nullptr;
  }
  delete myBuilder;
  myBuilder = nullptr;
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
void BRepAlgoAPI_BuilderAlgo::IntersectShapes(const NCollection_List<TopoDS_Shape>& theArgs,
                                              const Message_ProgressRange&          theRange)
{
  if (!myIsIntersectionNeeded)
    return;

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

void BRepAlgoAPI_BuilderAlgo::SimplifyResult(const bool   theUnifyEdges,
                                             const bool   theUnifyFaces,
                                             const double theAngularTol)
{
  if (HasErrors())
    return;

  if (!theUnifyEdges && !theUnifyFaces)
    return;

  // Simplification tool
  ShapeUpgrade_UnifySameDomain anUnifier(myShape, theUnifyEdges, theUnifyFaces, true);
  // Pass options
  anUnifier.SetLinearTolerance(myFuzzyValue);
  anUnifier.SetAngularTolerance(theAngularTol);
  anUnifier.SetSafeInputMode(myNonDestructive);
  anUnifier.AllowInternalEdges(false);
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

const NCollection_List<TopoDS_Shape>& BRepAlgoAPI_BuilderAlgo::Modified(const TopoDS_Shape& theS)
{
  if (myFillHistory && myHistory)
    return myHistory->Modified(theS);
  myGenerated.Clear();
  return myGenerated;
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepAlgoAPI_BuilderAlgo::Generated(const TopoDS_Shape& theS)
{
  if (myFillHistory && myHistory)
    return myHistory->Generated(theS);
  myGenerated.Clear();
  return myGenerated;
}

//=================================================================================================

bool BRepAlgoAPI_BuilderAlgo::IsDeleted(const TopoDS_Shape& theS)
{
  return (myFillHistory && myHistory ? myHistory->IsRemoved(theS) : false);
}

//=================================================================================================

bool BRepAlgoAPI_BuilderAlgo::HasModified() const
{
  return (myFillHistory && myHistory ? myHistory->HasModified() : false);
}

//=================================================================================================

bool BRepAlgoAPI_BuilderAlgo::HasGenerated() const
{
  return (myFillHistory && myHistory ? myHistory->HasGenerated() : false);
}

//=================================================================================================

bool BRepAlgoAPI_BuilderAlgo::HasDeleted() const
{
  return (myFillHistory && myHistory ? myHistory->HasRemoved() : false);
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepAlgoAPI_BuilderAlgo::SectionEdges()
{
  myGenerated.Clear();
  if (myBuilder == nullptr)
    return myGenerated;

  // Fence map to avoid duplicated section edges in the result list
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFence;
  // Intersection results
  const BOPDS_PDS& pDS = myDSFiller->PDS();
  // Iterate on all Face/Face interferences and take section edges
  NCollection_Vector<BOPDS_InterfFF>& aFFs  = pDS->InterfFF();
  const int                           aNbFF = aFFs.Length();
  for (int i = 0; i < aNbFF; ++i)
  {
    BOPDS_InterfFF& aFFi = aFFs(i);
    // Section curves between pair of faces
    const NCollection_Vector<BOPDS_Curve>& aSectionCurves = aFFi.Curves();
    const int                              aNbC           = aSectionCurves.Length();
    for (int j = 0; j < aNbC; ++j)
    {
      const BOPDS_Curve& aCurve = aSectionCurves(j);
      // Section edges created from the curve
      const NCollection_List<occ::handle<BOPDS_PaveBlock>>&    aSectionEdges = aCurve.PaveBlocks();
      NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aItPB(aSectionEdges);
      for (; aItPB.More(); aItPB.Next())
      {
        const occ::handle<BOPDS_PaveBlock>& aPB = aItPB.Value();
        const TopoDS_Shape&                 aSE = pDS->Shape(aPB->Edge());
        if (!aMFence.Add(aSE))
          continue;
        // Take into account simplification of the result shape
        if (mySimplifierHistory)
        {
          if (mySimplifierHistory->IsRemoved(aSE))
            continue;

          const NCollection_List<TopoDS_Shape>& aLSEIm = mySimplifierHistory->Modified(aSE);
          if (!aLSEIm.IsEmpty())
          {
            NCollection_List<TopoDS_Shape>::Iterator aItLEIm(aLSEIm);
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
