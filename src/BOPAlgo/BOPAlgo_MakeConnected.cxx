// Created on: 2018-03-29
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

#include <BOPAlgo_MakeConnected.hxx>

#include <BOPAlgo_Alerts.hxx>
#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_Tools.hxx>

#include <BOPTools_AlgoTools.hxx>

#include <BRep_Builder.hxx>

#include <TopExp_Explorer.hxx>

#include <TopoDS_Iterator.hxx>

//=======================================================================
//function : Perform
//purpose  : Makes the shapes connected
//=======================================================================
void BOPAlgo_MakeConnected::Perform()
{
  // Check the input data
  CheckData();
  if (HasErrors())
    return;

  if (myHistory.IsNull())
    myHistory = new BRepTools_History;

  // Glue the arguments
  MakeConnected();
  if (HasErrors())
    return;

  // Perform material associations for the faces
  AssociateMaterials();
  if (HasErrors())
    return;
}

//=======================================================================
//function : CheckData
//purpose  : Check the validity of input data
//=======================================================================
void BOPAlgo_MakeConnected::CheckData()
{
  // Check the number of arguments
  if (myArguments.IsEmpty())
  {
    // Not enough arguments
    AddError(new BOPAlgo_AlertTooFewArguments());
    return;
  }

  // Check that all shapes in arguments are of the same type

  // Extract the shapes from the compound arguments
  TopTools_ListOfShape aLA;
  // Fence map
  TopTools_MapOfShape aMFence;

  TopTools_ListIteratorOfListOfShape itLA(myArguments);
  for (; itLA.More(); itLA.Next())
    BOPTools_AlgoTools::TreatCompound(itLA.Value(), aLA, &aMFence);

  if (aLA.IsEmpty())
  {
    // It seems that all argument shapes are empty compounds
    AddError(new BOPAlgo_AlertTooFewArguments());
    return;
  }

  // Check dimensions of the extracted non-compound shapes
  itLA.Initialize(aLA);
  Standard_Integer iDim = BOPTools_AlgoTools::Dimension(itLA.Value());
  for (itLA.Next(); itLA.More(); itLA.Next())
  {
    if (iDim != BOPTools_AlgoTools::Dimension(itLA.Value()))
    {
      // The arguments are of different type
      AddError(new BOPAlgo_AlertMultiDimensionalArguments());
      return;
    }
  }
}

//=======================================================================
//function : MakeConnected
//purpose  : Glues the argument shapes
//=======================================================================
void BOPAlgo_MakeConnected::MakeConnected()
{
  // Initialize the history
  if (myGlueHistory.IsNull())
    myGlueHistory = new BRepTools_History;

  if (myArguments.Extent() == 1)
  {
    // No need to glue the single shape
    myShape = myArguments.First();
  }
  else
  {
    // Glue the shapes
    BOPAlgo_Builder aGluer;
    aGluer.SetArguments(myArguments);
    aGluer.SetGlue(BOPAlgo_GlueShift);
    aGluer.SetRunParallel(myRunParallel);
    aGluer.SetNonDestructive(Standard_True);
    aGluer.Perform();
    if (aGluer.HasErrors())
    {
      // Unable to glue the shapes
      TopoDS_Compound aCW;
      BRep_Builder().MakeCompound(aCW);
      for (TopTools_ListIteratorOfListOfShape it(myArguments); it.More(); it.Next())
        BRep_Builder().Add(aCW, it.Value());
      AddError(new BOPAlgo_AlertUnableToGlue(aCW));
      return;
    }
    myShape = aGluer.Shape();
    // Save the gluing history
    myGlueHistory->Merge(aGluer.Arguments(), aGluer);
    myHistory->Merge(myGlueHistory);
  }

  // Keep the glued shape
  myGlued = myShape;

  // Fill the map of origins
  FillOrigins();
}

//=======================================================================
//function : FillOrigins
//purpose  : Fills the map of origins
//=======================================================================
void BOPAlgo_MakeConnected::FillOrigins()
{
  myOrigins.Clear();

  // Map the history shapes of the arguments
  if (myAllInputsMap.IsEmpty())
  {
    TopTools_ListIteratorOfListOfShape itLA(myArguments);
    for (; itLA.More(); itLA.Next())
      TopExp::MapShapes(itLA.Value(), myAllInputsMap);
  }

  const Standard_Integer aNbS = myAllInputsMap.Extent();
  for (Standard_Integer i = 1; i <= aNbS; ++i)
  {
    const TopoDS_Shape& aS = myAllInputsMap(i);
    if (!BRepTools_History::IsSupportedType(aS))
      continue;

    // Get Modified & Generated shapes
    for (Standard_Integer j = 0; j < 2; ++j)
    {
      const TopTools_ListOfShape& aLH = !j ? myHistory->Modified(aS) : myHistory->Generated(aS);
      TopTools_ListIteratorOfListOfShape itLH(aLH);
      for (; itLH.More(); itLH.Next())
      {
        const TopoDS_Shape& aHS = itLH.Value();
        TopTools_ListOfShape* pLOr = myOrigins.ChangeSeek(aHS);
        if (!pLOr)
          pLOr = myOrigins.Bound(aHS, TopTools_ListOfShape());
        if (!pLOr->Contains(aS))
          pLOr->Append(aS);
      }
    }
  }
}

//=======================================================================
//function : AssociateMaterials
//purpose  : Associates the materials for the border elements
//=======================================================================
void BOPAlgo_MakeConnected::AssociateMaterials()
{
  myMaterials.Clear();

  // Extract all non-compound shapes from the result
  TopTools_ListOfShape aLShapes;
  TopTools_MapOfShape aMFence;
  BOPTools_AlgoTools::TreatCompound(myShape, aLShapes, &aMFence);

  if (aLShapes.IsEmpty())
    return;

  // Define the element type and the material type
  TopAbs_ShapeEnum anElemType;
  const TopAbs_ShapeEnum aMaterialType = aLShapes.First().ShapeType();
  if (aMaterialType == TopAbs_SOLID || aMaterialType == TopAbs_COMPSOLID)
    anElemType = TopAbs_FACE;
  else if (aMaterialType == TopAbs_FACE || aMaterialType == TopAbs_SHELL)
    anElemType = TopAbs_EDGE;
  else if (aMaterialType == TopAbs_EDGE || aMaterialType == TopAbs_WIRE)
    anElemType = TopAbs_VERTEX;
  else
    return;

  TopTools_ListIteratorOfListOfShape itLS(aLShapes);
  for (; itLS.More(); itLS.Next())
  {
    const TopoDS_Shape& aS = itLS.Value();
    const TopTools_ListOfShape& aLOr = GetOrigins(aS);
    const TopoDS_Shape& aSOr = aLOr.IsEmpty() ? aS : aLOr.First();

    TopExp_Explorer anExp(aS, anElemType);
    for (; anExp.More(); anExp.Next())
    {
      const TopoDS_Shape& anElement = anExp.Current();
      TopTools_ListOfShape* pLM = myMaterials.ChangeSeek(anElement);
      if (!pLM)
        pLM = myMaterials.Bound(anElement, TopTools_ListOfShape());
      pLM->Append(aSOr);
    }
  }
}

//=======================================================================
//function : Update
//purpose  : Updates the history, material associations and origins map
//           after periodicity operations
//=======================================================================
void BOPAlgo_MakeConnected::Update()
{
  // Update history
  myHistory->Clear();
  if (!myGlueHistory.IsNull())
    myHistory->Merge(myGlueHistory);
  if (!myPeriodicityMaker.History().IsNull())
    myHistory->Merge(myPeriodicityMaker.History());

  // Fill the map of origins
  FillOrigins();

  // Update the material associations after making the shape periodic
  AssociateMaterials();
}

//=======================================================================
//function : MakePeriodic
//purpose  : Makes the shape periodic according to the given parameters
//=======================================================================
void BOPAlgo_MakeConnected::MakePeriodic(const BOPAlgo_MakePeriodic::PeriodicityParams& theParams)
{
  if (HasErrors())
    return;

  // Make the shape periodic
  myPeriodicityMaker.Clear();
  myPeriodicityMaker.SetShape(myGlued);
  myPeriodicityMaker.SetPeriodicityParameters(theParams);
  myPeriodicityMaker.SetRunParallel(myRunParallel);
  myPeriodicityMaker.Perform();
  if (myPeriodicityMaker.HasErrors())
  {
    // Add warning informing the user that periodicity with
    // given parameters is not possible
    AddWarning(new BOPAlgo_AlertUnableToMakePeriodic(myShape));
    return;
  }

  myShape = myPeriodicityMaker.Shape();

  // Update history, materials, origins
  Update();
}

//=======================================================================
//function : RepeatShape
//purpose  : Repeats the shape in the given direction given number of times
//=======================================================================
void BOPAlgo_MakeConnected::RepeatShape(const Standard_Integer theDirectionID,
                                        const Standard_Integer theTimes)
{
  if (HasErrors())
    return;

  if (myPeriodicityMaker.Shape().IsNull() || myPeriodicityMaker.HasErrors())
  {
    // The shape has not been made periodic yet
    AddWarning(new BOPAlgo_AlertShapeIsNotPeriodic(myShape));
    return;
  }

  // Repeat the shape
  myShape = myPeriodicityMaker.RepeatShape(theDirectionID, theTimes);

  // Update history, materials, origins
  Update();
}

//=======================================================================
//function : ClearRepetitions
//purpose  : Clears the repetitions performed on the periodic shape
//           keeping the shape periodic
//=======================================================================
void BOPAlgo_MakeConnected::ClearRepetitions()
{
  if (HasErrors())
    return;

  if (myPeriodicityMaker.Shape().IsNull() || myPeriodicityMaker.HasErrors())
  {
    // The shape has not been made periodic yet
    AddWarning(new BOPAlgo_AlertShapeIsNotPeriodic(myShape));
    return;
  }

  // Clear repetitions
  myPeriodicityMaker.ClearRepetitions();
  myShape = myPeriodicityMaker.Shape();

  // Update history, materials, origins
  Update();
}
