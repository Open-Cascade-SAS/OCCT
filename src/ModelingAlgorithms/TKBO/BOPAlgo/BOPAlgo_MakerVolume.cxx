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
#include <TopExp_Explorer.hxx>
#include <TopoDS_Solid.hxx>
#include <TopTools_ListOfShape.hxx>

static void AddFace(const TopoDS_Shape& theF, TopTools_ListOfShape& theLF);

//=================================================================================================

void BOPAlgo_MakerVolume::CheckData()
{
  if (myArguments.IsEmpty())
  {
    AddError(new BOPAlgo_AlertTooFewArguments); // no arguments to process
    return;
  }
  //
  CheckFiller();
}

//=================================================================================================

void BOPAlgo_MakerVolume::Perform(const Message_ProgressRange& theRange)
{
  Message_ProgressScope aPS(theRange, "Performing MakeVolume operation", 10);
  Standard_Real         anInterPart = myIntersect ? 9 : 0.5;
  Standard_Real         aBuildPart  = 10. - anInterPart;

  GetReport()->Clear();
  //
  if (myEntryPoint == 1)
  {
    if (myPaveFiller)
    {
      delete myPaveFiller;
      myPaveFiller = NULL;
    }
  }
  //
  Handle(NCollection_BaseAllocator) aAllocator = NCollection_BaseAllocator::CommonBaseAllocator();
  BOPAlgo_PaveFiller*               pPF        = new BOPAlgo_PaveFiller(aAllocator);
  //
  if (!myIntersect)
  {
    // if there is no need to intersect the arguments, then it is necessary
    // to create the compound of them and use it as one argument
    TopoDS_Compound                    anArgs;
    BRep_Builder                       aBB;
    TopTools_ListIteratorOfListOfShape aIt;
    TopTools_ListOfShape               aLS;
    //
    aBB.MakeCompound(anArgs);
    aIt.Initialize(myArguments);
    for (; aIt.More(); aIt.Next())
    {
      const TopoDS_Shape& aS = aIt.Value();
      aBB.Add(anArgs, aS);
    }
    aLS.Append(anArgs);
    //
    pPF->SetArguments(aLS);
  }
  else
  {
    pPF->SetArguments(myArguments);
  }
  //
  pPF->SetRunParallel(myRunParallel);
  pPF->SetFuzzyValue(myFuzzyValue);
  pPF->SetNonDestructive(myNonDestructive);
  pPF->SetGlue(myGlue);
  pPF->SetUseOBB(myUseOBB);
  pPF->Perform(aPS.Next(anInterPart));
  //
  myEntryPoint = 1;
  PerformInternal(*pPF, aPS.Next(aBuildPart));
}

//=================================================================================================

void BOPAlgo_MakerVolume::PerformInternal1(const BOPAlgo_PaveFiller&    theFiller,
                                           const Message_ProgressRange& theRange)
{
  Message_ProgressScope aPS(theRange, "Building volumes", 100);
  myPaveFiller = (BOPAlgo_PaveFiller*)&theFiller;
  myDS         = myPaveFiller->PDS();
  myContext    = myPaveFiller->Context();
  //
  // 1. CheckData
  CheckData();
  if (HasErrors())
  {
    return;
  }
  //
  // 2. Prepare
  Prepare();
  if (HasErrors())
  {
    return;
  }
  //
  BOPAlgo_PISteps aSteps(PIOperation_Last);
  analyzeProgress(100., aSteps);

  // 3. Fill Images
  if (myIntersect)
  {
    // 3.1. Vertices
    FillImagesVertices(aPS.Next(aSteps.GetStep(PIOperation_TreatVertices)));
    if (HasErrors())
    {
      return;
    }
    // 3.2. Edges
    FillImagesEdges(aPS.Next(aSteps.GetStep(PIOperation_TreatEdges)));
    if (HasErrors())
    {
      return;
    }
    // 3.3. Wires
    FillImagesContainers(TopAbs_WIRE, aPS.Next(aSteps.GetStep(PIOperation_TreatWires)));
    if (HasErrors())
    {
      return;
    }
    // 3.4. Faces
    FillImagesFaces(aPS.Next(aSteps.GetStep(PIOperation_TreatFaces)));
    if (HasErrors())
    {
      return;
    }
  }
  //
  // 4. Collect faces
  CollectFaces();
  if (HasErrors())
  {
    return;
  }
  //
  TopTools_MapOfShape  aBoxFaces;
  TopTools_ListOfShape aLSR;
  //
  // 5. Create bounding box
  MakeBox(aBoxFaces);
  //
  // 6. Make volumes
  BuildSolids(aLSR, aPS.Next(aSteps.GetStep(PIOperation_BuildSolids)));
  if (HasErrors())
  {
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
  PrepareHistory(aPS.Next(aSteps.GetStep(PIOperation_FillHistory)));
  if (HasErrors())
  {
    return;
  }
  //
  // 11. Post-treatment
  PostTreat(aPS.Next(aSteps.GetStep(PIOperation_PostTreat)));
}

//=================================================================================================

void BOPAlgo_MakerVolume::fillPISteps(BOPAlgo_PISteps& theSteps) const
{
  NbShapes aNbShapes = getNbShapes();
  if (myIntersect)
  {
    theSteps.SetStep(PIOperation_TreatVertices, aNbShapes.NbVertices());
    theSteps.SetStep(PIOperation_TreatEdges, aNbShapes.NbEdges());
    theSteps.SetStep(PIOperation_TreatWires, aNbShapes.NbWires());
    theSteps.SetStep(PIOperation_TreatFaces, 50 * aNbShapes.NbFaces());
  }
  theSteps.SetStep(PIOperation_BuildSolids, 50 * aNbShapes.NbFaces());
}

//=================================================================================================

void BOPAlgo_MakerVolume::CollectFaces()
{
  //
  Standard_Integer                   i, aNbShapes;
  TopTools_ListIteratorOfListOfShape aIt;
  TopTools_MapOfShape                aMFence;
  //
  aNbShapes = myDS->NbSourceShapes();
  for (i = 0; i < aNbShapes; ++i)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (aSI.ShapeType() != TopAbs_FACE)
    {
      continue;
    }
    //
    const Bnd_Box& aB = aSI.Box();
    myBBox.Add(aB);
    //
    const TopoDS_Shape& aF = aSI.Shape();
    if (myImages.IsBound(aF))
    {
      const TopTools_ListOfShape& aLFIm = myImages.Find(aF);
      aIt.Initialize(aLFIm);
      for (; aIt.More(); aIt.Next())
      {
        const TopoDS_Shape& aFIm = aIt.Value();
        if (aMFence.Add(aFIm))
        {
          AddFace(aFIm, myFaces);
        }
      }
    }
    else
    {
      AddFace(aF, myFaces);
    }
  }
}

//=================================================================================================

void BOPAlgo_MakerVolume::MakeBox(TopTools_MapOfShape& theBoxFaces)
{
  //
  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax, anExt;
  //
  anExt = sqrt(myBBox.SquareExtent()) * 0.5;
  myBBox.Enlarge(anExt);
  myBBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  //
  gp_Pnt aPMin(aXmin, aYmin, aZmin), aPMax(aXmax, aYmax, aZmax);
  //
  mySBox = BRepPrimAPI_MakeBox(aPMin, aPMax).Solid();
  //
  TopExp_Explorer aExp(mySBox, TopAbs_FACE);
  for (; aExp.More(); aExp.Next())
  {
    const TopoDS_Shape& aF = aExp.Current();
    myFaces.Append(aF);
    theBoxFaces.Add(aF);
  }
}

//=================================================================================================

void BOPAlgo_MakerVolume::BuildSolids(TopTools_ListOfShape&        theLSR,
                                      const Message_ProgressRange& theRange)
{
  BOPAlgo_BuilderSolid aBS;
  //
  aBS.SetShapes(myFaces);
  aBS.SetRunParallel(myRunParallel);
  aBS.SetAvoidInternalShapes(myAvoidInternalShapes);
  aBS.Perform(theRange);
  if (aBS.HasErrors())
  {
    AddError(new BOPAlgo_AlertSolidBuilderFailed); // SolidBuilder failed
    return;
  }
  //
  myReport->Merge(aBS.GetReport());
  //
  theLSR = aBS.Areas();
}

//=================================================================================================

void BOPAlgo_MakerVolume::RemoveBox(TopTools_ListOfShape&      theLSR,
                                    const TopTools_MapOfShape& theBoxFaces)
{
  //
  TopTools_ListIteratorOfListOfShape aIt;
  TopExp_Explorer                    aExp;
  Standard_Boolean                   bFound;
  //
  bFound = Standard_False;
  aIt.Initialize(theLSR);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aSR = aIt.Value();
    //
    aExp.Init(aSR, TopAbs_FACE);
    for (; aExp.More(); aExp.Next())
    {
      const TopoDS_Shape& aF = aExp.Current();
      if (theBoxFaces.Contains(aF))
      {
        bFound = Standard_True;
        theLSR.Remove(aIt);
        break;
      }
    }
    if (bFound)
    {
      break;
    }
  }
}

//=================================================================================================

void BOPAlgo_MakerVolume::BuildShape(const TopTools_ListOfShape& theLSR)
{
  if (theLSR.Extent() == 1)
  {
    myShape = theLSR.First();
  }
  else
  {
    BRep_Builder                       aBB;
    TopTools_ListIteratorOfListOfShape aIt;
    //
    aIt.Initialize(theLSR);
    for (; aIt.More(); aIt.Next())
    {
      const TopoDS_Shape& aSol = aIt.Value();
      aBB.Add(myShape, aSol);
    }
  }
}

//=================================================================================================

void BOPAlgo_MakerVolume::FillInternalShapes(const TopTools_ListOfShape& theLSR)
{
  if (myAvoidInternalShapes)
  {
    return;
  }

  // Get all non-compound shapes
  TopTools_ListOfShape aLSC;
  // Fence map
  TopTools_MapOfShape aMFence;

  TopTools_ListOfShape::Iterator itLA(myDS->Arguments());
  for (; itLA.More(); itLA.Next())
    BOPTools_AlgoTools::TreatCompound(itLA.Value(), aLSC, &aMFence);

  // Get only edges and vertices from arguments
  TopTools_ListOfShape aLVE;

  itLA.Initialize(aLSC);
  for (; itLA.More(); itLA.Next())
  {
    const TopoDS_Shape& aS    = itLA.Value();
    TopAbs_ShapeEnum    aType = aS.ShapeType();
    if (aType == TopAbs_WIRE)
    {
      for (TopoDS_Iterator it(aS); it.More(); it.Next())
      {
        const TopoDS_Shape& aSS = it.Value();
        if (aMFence.Add(aSS))
          aLVE.Append(aSS);
      }
    }
    else if (aType == TopAbs_VERTEX || aType == TopAbs_EDGE)
      aLVE.Append(aS);
  }

  BOPAlgo_Tools::FillInternals(theLSR, aLVE, myImages, myContext);
}

//=================================================================================================

void AddFace(const TopoDS_Shape& theF, TopTools_ListOfShape& theLF)
{
  TopoDS_Shape aFF = theF;
  aFF.Orientation(TopAbs_FORWARD);
  theLF.Append(aFF);
  aFF.Orientation(TopAbs_REVERSED);
  theLF.Append(aFF);
}
