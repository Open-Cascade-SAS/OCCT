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

#include <MergeSTEPEntities_Merger.hxx>

#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <MergeSTEPEntities_Axis2Placement3dProcessor.hxx>
#include <MergeSTEPEntities_CartesianPointProcessor.hxx>
#include <MergeSTEPEntities_DirectionProcessor.hxx>
#include <MergeSTEPEntities_LineProcessor.hxx>
#include <MergeSTEPEntities_VectorProcessor.hxx>
#include <MergeSTEPEntities_PlaneProcessor.hxx>
#include <MergeSTEPEntities_CircleProcessor.hxx>
#include <StepData_StepModel.hxx>

//==================================================================================================

MergeSTEPEntities_Merger::MergeSTEPEntities_Merger(Handle(XSControl_WorkSession) theWS)
    : myWS(theWS)
{
}

//==================================================================================================

void MergeSTEPEntities_Merger::Perform()
{
  Handle(StepData_StepModel) aModel = Handle(StepData_StepModel)::DownCast(myWS->Model());
  if (aModel.IsNull())
  {
    return;
  }

  //! Initialize processors.
  MergeSTEPEntities_CartesianPointProcessor   aCartesianPointProcessor(myWS);
  MergeSTEPEntities_DirectionProcessor        aDirectionProcessor(myWS);
  MergeSTEPEntities_Axis2Placement3dProcessor aAxis2Placement3dProcessor(myWS);
  MergeSTEPEntities_VectorProcessor           aVectorProcessor(myWS);
  MergeSTEPEntities_LineProcessor             aLineProcessor(myWS);
  MergeSTEPEntities_PlaneProcessor            aPlaneProcessor(myWS);
  MergeSTEPEntities_CircleProcessor           aCircleProcessor(myWS);

  // Process all entities.
  for (Standard_Integer anIndex = 1; anIndex <= aModel->NbEntities(); ++anIndex)
  {
    const Handle(Standard_Transient) anEntity = aModel->Value(anIndex);
    aCartesianPointProcessor.ProcessEntity(anEntity);
    aDirectionProcessor.ProcessEntity(anEntity);
    aAxis2Placement3dProcessor.ProcessEntity(anEntity);
    aVectorProcessor.ProcessEntity(anEntity);
    aLineProcessor.ProcessEntity(anEntity);
    aPlaneProcessor.ProcessEntity(anEntity);
    aCircleProcessor.ProcessEntity(anEntity);
  }

  // Perform replacement of duplicate entities.
  TColStd_MapOfTransient aReplacedEntities;
  aCartesianPointProcessor.Perform(aReplacedEntities);
  aDirectionProcessor.Perform(aReplacedEntities);
  aAxis2Placement3dProcessor.Perform(aReplacedEntities);
  aVectorProcessor.Perform(aReplacedEntities);
  aLineProcessor.Perform(aReplacedEntities);
  aPlaneProcessor.Perform(aReplacedEntities);
  aCircleProcessor.Perform(aReplacedEntities);

  // Remove duplicate entities.
  removeEntities(aReplacedEntities);
}

//==================================================================================================

void MergeSTEPEntities_Merger::removeEntities(const TColStd_MapOfTransient& theToRemove)
{
  if (theToRemove.IsEmpty())
  {
    return;
  }
  // Remove entities.
  Handle(StepData_StepModel) anIntermediateModel = new StepData_StepModel();
  Handle(StepData_StepModel) aReadModel = Handle(StepData_StepModel)::DownCast(myWS->Model());
  anIntermediateModel->SetProtocol(aReadModel->Protocol());
  anIntermediateModel->SetGTool(aReadModel->GTool());

  for (Standard_Integer i = 1; i <= aReadModel->NbEntities(); i++)
  {
    const Handle(Standard_Transient)& anEnt = aReadModel->Value(i);
    if (!theToRemove.Contains(anEnt))
    {
      anIntermediateModel->AddWithRefs(anEnt);
    }
  }

  myWS->SetModel(anIntermediateModel);
  myWS->ComputeGraph();

  // Clean hanged entities.
  Handle(StepData_StepModel) aNewModel = new StepData_StepModel();
  aNewModel->SetProtocol(anIntermediateModel->Protocol());
  aNewModel->SetGTool(anIntermediateModel->GTool());
  const auto& aGraph = myWS->Graph();

  for (Standard_Integer i = 1; i <= anIntermediateModel->NbEntities(); i++)
  {
    const Handle(Standard_Transient)& anEnt = anIntermediateModel->Value(i);
    if (aGraph.Shareds(anEnt).NbEntities() > 0 || aGraph.Sharings(anEnt).NbEntities() > 0)
    {
      aNewModel->AddWithRefs(anEnt);
    }
  }

  myWS->SetModel(aNewModel);
  myWS->ComputeGraph();
}
