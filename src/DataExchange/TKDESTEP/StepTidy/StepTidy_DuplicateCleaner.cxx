// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <StepTidy_DuplicateCleaner.hxx>

#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <StepTidy_Axis2Placement3dReducer.pxx>
#include <StepTidy_CartesianPointReducer.pxx>
#include <StepTidy_DirectionReducer.pxx>
#include <StepTidy_LineReducer.pxx>
#include <StepTidy_VectorReducer.pxx>
#include <StepTidy_PlaneReducer.pxx>
#include <StepTidy_CircleReducer.pxx>
#include <StepData_StepModel.hxx>
#include <utility>

//==================================================================================================

StepTidy_DuplicateCleaner::StepTidy_DuplicateCleaner(occ::handle<XSControl_WorkSession> theWS)
    : myWS(std::move(theWS))
{
}

//==================================================================================================

void StepTidy_DuplicateCleaner::Perform()
{
  occ::handle<StepData_StepModel> aModel = occ::down_cast<StepData_StepModel>(myWS->Model());
  if (aModel.IsNull())
  {
    return;
  }

  //! Initialize Reducers.
  StepTidy_CartesianPointReducer   aCartesianPointReducer(myWS);
  StepTidy_DirectionReducer        aDirectionReducer(myWS);
  StepTidy_Axis2Placement3dReducer aAxis2Placement3dReducer(myWS);
  StepTidy_VectorReducer           aVectorReducer(myWS);
  StepTidy_LineReducer             aLineReducer(myWS);
  StepTidy_PlaneReducer            aPlaneReducer(myWS);
  StepTidy_CircleReducer           aCircleReducer(myWS);

  // Process all entities.
  for (int anIndex = 1; anIndex <= aModel->NbEntities(); ++anIndex)
  {
    const occ::handle<Standard_Transient> anEntity = aModel->Value(anIndex);
    aCartesianPointReducer.ProcessEntity(anEntity);
    aDirectionReducer.ProcessEntity(anEntity);
    aAxis2Placement3dReducer.ProcessEntity(anEntity);
    aVectorReducer.ProcessEntity(anEntity);
    aLineReducer.ProcessEntity(anEntity);
    aPlaneReducer.ProcessEntity(anEntity);
    aCircleReducer.ProcessEntity(anEntity);
  }

  // Perform replacement of duplicate entities.
  NCollection_Map<occ::handle<Standard_Transient>> aReplacedEntities;
  aCartesianPointReducer.Perform(aReplacedEntities);
  aDirectionReducer.Perform(aReplacedEntities);
  aAxis2Placement3dReducer.Perform(aReplacedEntities);
  aVectorReducer.Perform(aReplacedEntities);
  aLineReducer.Perform(aReplacedEntities);
  aPlaneReducer.Perform(aReplacedEntities);
  aCircleReducer.Perform(aReplacedEntities);

  // Remove duplicate entities.
  removeEntities(aReplacedEntities);
}

//==================================================================================================

void StepTidy_DuplicateCleaner::removeEntities(
  const NCollection_Map<occ::handle<Standard_Transient>>& theToRemove)
{
  if (theToRemove.IsEmpty())
  {
    return;
  }
  // Remove entities.
  occ::handle<StepData_StepModel> anIntermediateModel = new StepData_StepModel();
  occ::handle<StepData_StepModel> aReadModel = occ::down_cast<StepData_StepModel>(myWS->Model());
  anIntermediateModel->SetProtocol(aReadModel->Protocol());
  anIntermediateModel->SetGTool(aReadModel->GTool());

  for (int i = 1; i <= aReadModel->NbEntities(); i++)
  {
    const occ::handle<Standard_Transient>& anEnt = aReadModel->Value(i);
    if (!theToRemove.Contains(anEnt))
    {
      anIntermediateModel->AddWithRefs(anEnt);
    }
  }

  myWS->SetModel(anIntermediateModel);
  myWS->ComputeGraph();

  // Clean hanged entities.
  occ::handle<StepData_StepModel> aNewModel = new StepData_StepModel();
  aNewModel->SetProtocol(anIntermediateModel->Protocol());
  aNewModel->SetGTool(anIntermediateModel->GTool());
  const auto& aGraph = myWS->Graph();

  for (int i = 1; i <= anIntermediateModel->NbEntities(); i++)
  {
    const occ::handle<Standard_Transient>& anEnt = anIntermediateModel->Value(i);
    if (aGraph.Shareds(anEnt).NbEntities() > 0 || aGraph.Sharings(anEnt).NbEntities() > 0)
    {
      aNewModel->AddWithRefs(anEnt);
    }
  }

  myWS->SetModel(aNewModel);
  myWS->ComputeGraph();
}
