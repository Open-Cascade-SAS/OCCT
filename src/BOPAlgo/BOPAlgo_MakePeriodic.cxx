// Created on: 2018-03-16
// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2018 OPEN CASCADE SAS
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

#include <BOPAlgo_MakePeriodic.hxx>

#include <BOPAlgo_Alerts.hxx>

#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_PaveFiller.hxx>

#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Splitter.hxx>

#include <BRepBndLib.hxx>

#include <BRepBuilderAPI_Transform.hxx>

#include <BRepPrimAPI_MakeBox.hxx>

#include <gp_Pln.hxx>

#include <Precision.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>

#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

// Periodic/Trim/Repeat directions
static const gp_Dir MY_DIRECTIONS[3] = { gp::DX(),
                                         gp::DY(),
                                         gp::DZ() };

//=======================================================================
//function : Perform
//purpose  : Performs the operation
//=======================================================================
void BOPAlgo_MakePeriodic::Perform()
{
  // Check the validity of input data
  CheckData();
  if (HasErrors())
    return;

  // Trim the shape to fit to the required period in
  // required periodic directions
  Trim();
  if (HasErrors())
    return;

  // Make the shape identical on the opposite sides in
  // required periodic directions
  MakeIdentical();
  if (HasErrors())
    return;
}

//=======================================================================
//function : CheckData
//purpose  : Checks the validity of input data
//=======================================================================
void BOPAlgo_MakePeriodic::CheckData()
{
  if ( (!IsXPeriodic() || XPeriod() < Precision::Confusion())
    && (!IsYPeriodic() || YPeriod() < Precision::Confusion())
    && (!IsZPeriodic() || ZPeriod() < Precision::Confusion()))
  {
    // Add error informing the user that no periodicity is required
    // or no valid period is set.

    AddError(new BOPAlgo_AlertNoPeriodicityRequired());
    return;
  }
}

//=======================================================================
//function : AddToShape
//purpose  : Adds the shape <theWhat> to the shape <theWhere>
//=======================================================================
static void AddToShape(const TopoDS_Shape& theWhat,
                       TopoDS_Shape& theWhere)
{
  if (theWhere.IsNull())
    BRep_Builder().MakeCompound(TopoDS::Compound(theWhere));
  BRep_Builder().Add(theWhere, theWhat);
}
//=======================================================================
//function : AddToShape
//purpose  : Adds the shape in the list <theLWhat> to the shape <theWhere>
//=======================================================================
static void AddToShape(const TopTools_ListOfShape& theLWhat,
                       TopoDS_Shape& theWhere)
{
  TopTools_ListIteratorOfListOfShape it(theLWhat);
  for (; it.More(); it.Next())
    AddToShape(it.Value(), theWhere);
}

//=======================================================================
//function : Trim
//purpose  : Make the trim of the shape to fit to the periodic bounds.
//=======================================================================
void BOPAlgo_MakePeriodic::Trim()
{
  // Check if trim is required at all
  if (IsInputXTrimmed() &&
      IsInputYTrimmed() &&
      IsInputZTrimmed())
      return;

  // Compute bounding box for the shape to use it as a starting
  // volume for trimming. If required, the volume will be modified
  // to the requested trimming size in requested directions.
  Bnd_Box aBox;
  BRepBndLib::Add(myInputShape, aBox);
  // Enlarge box to avoid overlapping with the shape
  aBox.Enlarge(0.1 * sqrt(aBox.SquareExtent()));

  // Get Corner points of the bounding box
  gp_Pnt aPMin = aBox.CornerMin();
  gp_Pnt aPMax = aBox.CornerMax();

  // Update corner points according to the requested trim parameters
  for (Standard_Integer i = 0; i < 3; ++i)
  {
    if (IsInputTrimmed(i))
      continue;

    aPMin.SetCoord(i + 1, PeriodFirst(i));
    aPMax.SetCoord(i + 1, PeriodFirst(i) + Period(i));
  }

  // Build Trimming solid using corner points
  BRepPrimAPI_MakeBox aMBox(aPMin, aPMax);
  const TopoDS_Shape& aTrimBox = aMBox.Solid();

  // Perform trimming of the shape by solid
  BRepAlgoAPI_Common aCommon;
  // Set Object
  TopTools_ListOfShape anObj;
  anObj.Append(myInputShape);
  aCommon.SetArguments(anObj);
  // Set Tool
  TopTools_ListOfShape aTool;
  aTool.Append(aTrimBox);
  aCommon.SetTools(aTool);
  // Set the parallel processing mode
  aCommon.SetRunParallel(myRunParallel);
  // Build
  aCommon.Build();
  if (aCommon.HasErrors())
  {
    // Unable to trim the shape
    // Merge errors from Common operation
    myReport->Merge(aCommon.GetReport());
    // Add new error saving the shapes for analysis
    TopoDS_Compound aWS;
    AddToShape(myInputShape, aWS);
    AddToShape(aTrimBox, aWS);
    AddError(new BOPAlgo_AlertUnableToTrim(aWS));
    return;
  }
  // Get the trimmed shape
  myShape = aCommon.Shape();
  // Fill the History for the object only
  mySplitHistory = new BRepTools_History();
  mySplitHistory->Merge(anObj, aCommon);
}

//=======================================================================
//function : MakeIdentical
//purpose  : Make the shape look the same on the opposite sides in the
//           required periodic directions.
//=======================================================================
void BOPAlgo_MakePeriodic::MakeIdentical()
{
  if (myShape.IsNull())
    myShape = myInputShape;

  if (mySplitHistory.IsNull())
    mySplitHistory = new BRepTools_History;

  // Split the negative side of the shape with the geometry
  // located on the positive side
  SplitNegative();
  if (HasErrors())
    return;

  // Split the positive side of the shape with the geometry
  // located on the negative side.
  // Make sure that the opposite sides have identical geometries.
  // Make associations between identical opposite shapes.
  SplitPositive();

  myHistory = new BRepTools_History();
  myHistory->Merge(mySplitHistory);
}

//=======================================================================
//function : SplitNegative
//purpose  : Split the negative side of the shape with the geometry
//           located on the positive side.
//=======================================================================
void BOPAlgo_MakePeriodic::SplitNegative()
{
  // Copy geometry from positive side of the shape to the negative first.
  // So, translate the shape in negative periodic directions only.
  //
  // To avoid conflicts when copying geometries from positive periodic sides
  // perform split of each periodic side in a separate operation.
  for (Standard_Integer i = 0; i < 3; ++i)
  {
    if (!IsPeriodic(i))
      continue;

    // Translate the shape to the negative side
    gp_Trsf aNegTrsf;
    aNegTrsf.SetTranslationPart(Period(i) * MY_DIRECTIONS[i].Reversed());
    BRepBuilderAPI_Transform aNegT(myShape, aNegTrsf, Standard_False);

    // Split the negative side of the shape.
    TopTools_ListOfShape aTools;
    aTools.Append(aNegT.Shape());
    SplitShape(aTools, mySplitHistory);
  }
}

//=======================================================================
//function : AddTwin
//purpose  : Associates the shape <theS> with the shape <theTwin> in the map.
//=======================================================================
static void AddTwin(const TopoDS_Shape& theS,
                    const TopoDS_Shape& theTwin,
                    TopTools_DataMapOfShapeListOfShape& theMap)
{
  TopTools_ListOfShape *aTwins = theMap.ChangeSeek(theS);
  if (!aTwins)
  {
    theMap.Bound(theS, TopTools_ListOfShape())->Append(theTwin);
    return;
  }

  // Check if the twin shape is not yet present in the list
  TopTools_ListIteratorOfListOfShape itLT(*aTwins);
  for (; itLT.More(); itLT.Next())
  {
    if (theTwin.IsSame(itLT.Value()))
      break;
  }

  if (!itLT.More())
    aTwins->Append(theTwin);
}

//=======================================================================
//function : SplitPositive
//purpose  : Split the positive side of the shape with the geometry of the
//           negative side. Associate the identical opposite sub-shapes.
//=======================================================================
void BOPAlgo_MakePeriodic::SplitPositive()
{
  // Prepare map of the sub-shapes of the input shape to make
  // associations of the opposite shapes
  TopTools_IndexedMapOfShape aSubShapesMap;
  TopExp::MapShapes(myShape, aSubShapesMap);
  const Standard_Integer aNbS = aSubShapesMap.Extent();

  // Translate the shape to the positive periodic directions to make the
  // shapes look identical on the opposite sides.
  TopTools_ListOfShape aTools;

  // Remember the history of shapes translation
  TopTools_IndexedDataMapOfShapeListOfShape aTranslationHistMap;

  // Make translations for all periodic directions
  for (Standard_Integer i = 0; i < 3; ++i)
  {
    if (!IsPeriodic(i))
      continue;

    // Translate the shape to the positive side
    gp_Trsf aPosTrsf;
    aPosTrsf.SetTranslationPart(Period(i) * MY_DIRECTIONS[i]);
    BRepBuilderAPI_Transform aTranslator(myShape, aPosTrsf, Standard_False);
    aTools.Append(aTranslator.Shape());

    // Fill the translation history map
    for (Standard_Integer j = 1; j <= aNbS; ++j)
    {
      const TopoDS_Shape& aS = aSubShapesMap(j);
      if (BRepTools_History::IsSupportedType(aS))
      {
        const TopTools_ListOfShape& aSM = aTranslator.Modified(aS);
        TopTools_ListOfShape* pTS = aTranslationHistMap.ChangeSeek(aS);
        if (!pTS)
          pTS = &aTranslationHistMap(aTranslationHistMap.Add(aS, TopTools_ListOfShape()));
        pTS->Append(aSM.First());
      }
    }
  }

  // Keep the split shape history and history of tools modifications
  // during the split for making association of the opposite identical shapes
  Handle(BRepTools_History) aSplitShapeHist = new BRepTools_History,
                            aSplitToolsHist = new BRepTools_History;
  // Split the positive side of the shape
  SplitShape(aTools, aSplitShapeHist, aSplitToolsHist);
  if (HasErrors())
    return;

  mySplitHistory->Merge(aSplitShapeHist);

  // Make associations between identical opposite sub-shapes
  const Standard_Integer aNbSH = aTranslationHistMap.Extent();
  for (Standard_Integer i = 1; i <= aNbSH; ++i)
  {
    const TopoDS_Shape* pS = &aTranslationHistMap.FindKey(i);
    const TopTools_ListOfShape& aSIm = aSplitShapeHist->Modified(*pS);
    if (aSIm.Extent() == 1)
      pS = &aSIm.First();
    else if (aSIm.Extent() > 1)
      continue;

    const TopTools_ListOfShape& aLTranslated = aTranslationHistMap(i);

    TopTools_ListIteratorOfListOfShape itLT(aLTranslated);
    for (; itLT.More(); itLT.Next())
    {
      const TopoDS_Shape& aT = itLT.Value();
      // Get shapes modifications during the split
      const TopTools_ListOfShape& aTSplits = aSplitToolsHist->Modified(aT);

      // Associate the shapes to each other
      TopTools_ListIteratorOfListOfShape itSp(aTSplits);
      for (; itSp.More(); itSp.Next())
      {
        const TopoDS_Shape& aSp = itSp.Value();
        AddTwin(*pS, aSp, myTwins);
        AddTwin(aSp, *pS, myTwins);
      }
    }
  }
}

//=======================================================================
//function : SplitShape
//purpose  : Splits the shape by the given tools
//=======================================================================
void BOPAlgo_MakePeriodic::SplitShape(const TopTools_ListOfShape& theTools,
                                      Handle(BRepTools_History) theSplitShapeHistory,
                                      Handle(BRepTools_History) theSplitToolsHistory)
{
  // Make sure that the geometry from the tools will be copied to the split
  // shape. For that, the tool shapes should be given to the Boolean Operations
  // algorithm before the shape itself. This will make all coinciding parts
  // use the geometry of the first argument.

  // Intersection tool for passing ordered arguments
  BOPAlgo_PaveFiller anIntersector;
  anIntersector.SetArguments(theTools);
  // Add the shape
  anIntersector.AddArgument(myShape);
  // Use gluing to speed-up intersections
  anIntersector.SetGlue(BOPAlgo_GlueShift);
  // Use safe input mode, to avoid reusing geometry of the shape
  anIntersector.SetNonDestructive(Standard_True);
  // Set parallel processing mode
  anIntersector.SetRunParallel(myRunParallel);
  // Perform Intersection of the arguments
  anIntersector.Perform();
  // Check for the errors
  if (anIntersector.HasErrors())
  {
    // Unable to split the shape on opposite sides
    // Copy the intersection errors
    myReport->Merge(anIntersector.GetReport());
    // Add new error saving the shapes for analysis
    TopoDS_Compound aWS;
    AddToShape(theTools, aWS);
    AddToShape(myShape, aWS);
    AddError(new BOPAlgo_AlertUnableToMakeIdentical(aWS));
    return;
  }

  // Perform the splitting of the shape with the precomputed intersection results
  BRepAlgoAPI_Splitter aSplitter(anIntersector);
  // Set Object
  TopTools_ListOfShape anObj;
  anObj.Append(myShape);
  aSplitter.SetArguments(anObj);
  // Set Tools
  aSplitter.SetTools(theTools);
  // Use Gluing
  aSplitter.SetGlue(BOPAlgo_GlueShift);
  // Set parallel processing mode
  aSplitter.SetRunParallel(myRunParallel);
  // Perform splitting
  aSplitter.Build();
  // Check for the errors
  if (aSplitter.HasErrors())
  {
    // Unable to split the shape on opposite sides
    // Copy the splitter errors
    myReport->Merge(aSplitter.GetReport());
    // Add new error saving the shape for analysis
    TopoDS_Compound aWS;
    AddToShape(theTools, aWS);
    AddToShape(myShape, aWS);
    AddError(new BOPAlgo_AlertUnableToMakeIdentical(aWS));
    return;
  }
  // Get the split shape
  myShape = aSplitter.Shape();
  // Remember the split history
  if (!theSplitShapeHistory.IsNull())
    theSplitShapeHistory->Merge(anObj, aSplitter);
  if (!theSplitToolsHistory.IsNull())
    theSplitToolsHistory->Merge(theTools, aSplitter);
}

//=======================================================================
//function : RepeatShape
//purpose  : Repeats the shape in the required periodic direction
//=======================================================================
const TopoDS_Shape& BOPAlgo_MakePeriodic::RepeatShape(const Standard_Integer theDir,
                                                      const Standard_Integer theTimes)
{
  if (myRepeatedShape.IsNull())
    myRepeatedShape = myShape;

  if (!IsPeriodic(theDir))
    return myRepeatedShape;

  if (theTimes == 0)
    return myRepeatedShape;

  // Get the shape's period in the required direction
  const Standard_Integer id = BOPAlgo_MakePeriodic::ToDirectionID(theDir);
  if (myRepeatPeriod[id] < Precision::Confusion())
    myRepeatPeriod[id] = Period(id);
  const Standard_Real aPeriod = myRepeatPeriod[id];

  // Coefficient to define in which direction the repetition will be performed:
  // theTimes is positive - in positive direction;
  // theTimes is negative - in negative direction.
  const Standard_Integer iDir = theTimes > 0 ? 1 : -1;

  // Create the translation history - all translated shapes will be
  // created as Generated from the shape.
  BRepTools_History aTranslationHistory;
  TopTools_IndexedMapOfShape aSubShapesMap;
  TopExp::MapShapes(myRepeatedShape, aSubShapesMap);
  const Standard_Integer aNbS = aSubShapesMap.Extent();

  // Add shapes for gluing
  TopTools_ListOfShape aShapes;
  // Add the shape itself
  aShapes.Append(myRepeatedShape);
  for (Standard_Integer i = 1; i <= aNbS; ++i)
  {
    const TopoDS_Shape& aS = aSubShapesMap(i);
    if (BRepTools_History::IsSupportedType(aS))
      aTranslationHistory.AddGenerated(aS, aS);
  }

  // Create translated copies of the shape
  for (Standard_Integer i = 1; i <= Abs(theTimes); ++i)
  {
    gp_Trsf aTrsf;
    aTrsf.SetTranslationPart(iDir * i * aPeriod * MY_DIRECTIONS[id]);
    BRepBuilderAPI_Transform aTranslator(myRepeatedShape, aTrsf, Standard_False);
    aShapes.Append(aTranslator.Shape());

    // Fill the translation history
    for (Standard_Integer j = 1; j <= aNbS; ++j)
    {
      const TopoDS_Shape& aS = aSubShapesMap(j);
      if (BRepTools_History::IsSupportedType(aS))
      {
        const TopTools_ListOfShape& aLT = aTranslator.Modified(aS);
        aTranslationHistory.AddGenerated(aS, aLT.First());
      }
    }
  }

  // Update the history with the translation History
  myHistory->Merge(aTranslationHistory);

  // Glue the translated shapes all together
  BOPAlgo_Builder aGluer;
  aGluer.SetArguments(aShapes);
  // Avoid intersections of the sub-shapes
  aGluer.SetGlue(BOPAlgo_GlueFull);
  // Set parallel processing mode
  aGluer.SetRunParallel(myRunParallel);
  // Perform gluing
  aGluer.Perform();
  if (aGluer.HasErrors())
  {
    // Repetition in this direction is not possible
    // Add warning saving the shapes for analysis
    TopoDS_Compound aWS;
    AddToShape(aShapes, aWS);
    AddWarning(new BOPAlgo_AlertUnableToRepeat(aWS));
    return myRepeatedShape;
  }
  // Get glued shape
  myRepeatedShape = aGluer.Shape();

  // Update repetition period for the next repetitions
  myRepeatPeriod[id] += Abs(theTimes) * myRepeatPeriod[id];

  // Update history with the Gluing history
  BRepTools_History aGluingHistory(aShapes, aGluer);
  myHistory->Merge(aGluingHistory);

  // Update the map of twins after repetition
  UpdateTwins(aTranslationHistory, aGluingHistory);

  return myRepeatedShape;
}

//=======================================================================
//function : UpdateTwins
//purpose  : Updates the map of twins after repetition
//=======================================================================
void BOPAlgo_MakePeriodic::UpdateTwins(const BRepTools_History& theTranslationHistory,
                                       const BRepTools_History& theGluingHistory)
{
  if (myTwins.IsEmpty())
    return;

  if (myRepeatedTwins.IsEmpty())
    myRepeatedTwins = myTwins;

  // New twins
  TopTools_DataMapOfShapeListOfShape aNewTwinsMap;

  // Fence map to avoid repeated fill for the twins
  TopTools_MapOfShape aMTwinsDone;

  // Update the map of twins with the new repeated shapes
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itDMap(myRepeatedTwins);
  for (; itDMap.More(); itDMap.Next())
  {
    const TopoDS_Shape& aS = itDMap.Key();
    aMTwinsDone.Add(aS);

    const TopTools_ListOfShape& aLTwins = itDMap.Value();

    // Check if the twins have not been already processed
    TopTools_ListIteratorOfListOfShape itLT(aLTwins);
    for (; itLT.More(); itLT.Next())
    {
      if (aMTwinsDone.Contains(itLT.Value()))
        break;
    }
    if (itLT.More())
      // Group of twins has already been processed
      continue;

    // All shapes generated from the shape itself and generated
    // from its twins will be the new twins for the shape
    TopTools_IndexedMapOfShape aNewGroup;
    itLT.Initialize(aLTwins);

    for (Standard_Boolean bShape = Standard_True; itLT.More();)
    {
      const TopoDS_Shape& aTwin = bShape ? aS : itLT.Value();
      const TopTools_ListOfShape& aLG = theTranslationHistory.Generated(aTwin);
      TopTools_ListIteratorOfListOfShape itLG(aLG);
      for (; itLG.More(); itLG.Next())
      {
        const TopoDS_Shape& aG = itLG.Value();
        const TopTools_ListOfShape& aLM = theGluingHistory.Modified(aG);
        if (aLM.IsEmpty())
          aNewGroup.Add(aG);
        else
        {
          TopTools_ListIteratorOfListOfShape itLM(aLM);
          for (; itLM.More(); itLM.Next())
            aNewGroup.Add(itLM.Value());
        }
      }

      if (bShape)
        bShape = Standard_False;
      else
        itLT.Next();
    }

    // Associate the twins to each other
    const Standard_Integer aNbTwins = aNewGroup.Extent();
    for (Standard_Integer i = 1; i <= aNbTwins; ++i)
    {
      TopTools_ListOfShape* pTwins = aNewTwinsMap.Bound(aNewGroup(i), TopTools_ListOfShape());
      for (Standard_Integer j = 1; j <= aNbTwins; ++j)
        if (i != j) pTwins->Append(aNewGroup(j));
    }
  }

  myRepeatedTwins = aNewTwinsMap;
}
