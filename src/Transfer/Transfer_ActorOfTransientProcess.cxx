// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <Transfer_ActorOfTransientProcess.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_ProcessForTransient.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSAlgo_ShapeProcessor.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Transfer_ActorOfTransientProcess, Transfer_ActorOfProcessForTransient)

Transfer_ActorOfTransientProcess::Transfer_ActorOfTransientProcess() {}

//=============================================================================

Handle(Transfer_Binder) Transfer_ActorOfTransientProcess::Transfer(
  const Handle(Standard_Transient)&        start,
  const Handle(Transfer_TransientProcess)& TP,
  const Message_ProgressRange&             theProgress)
{
  Handle(Standard_Transient) res = TransferTransient(start, TP, theProgress);
  if (res.IsNull())
    return NullResult();
  return TransientResult(res);
}

//=============================================================================

Handle(Transfer_Binder) Transfer_ActorOfTransientProcess::Transferring(
  const Handle(Standard_Transient)&           ent,
  const Handle(Transfer_ProcessForTransient)& TP,
  const Message_ProgressRange&                theProgress)
{
  return Transfer(ent, Handle(Transfer_TransientProcess)::DownCast(TP), theProgress);
}

//=============================================================================

Handle(Standard_Transient) Transfer_ActorOfTransientProcess::TransferTransient(
  const Handle(Standard_Transient)& /*ent*/,
  const Handle(Transfer_TransientProcess)& /*TP*/,
  const Message_ProgressRange&)
{
  Handle(Standard_Transient) nulres;
  return nulres;
}

//=============================================================================

void Transfer_ActorOfTransientProcess::SetShapeFixParameters(const ParameterMap& theParameters)
{
  myShapeProcParams = theParameters;
}

//=============================================================================

void Transfer_ActorOfTransientProcess::SetShapeFixParameters(ParameterMap&& theParameters)
{
  myShapeProcParams = std::move(theParameters);
}

//=============================================================================

void Transfer_ActorOfTransientProcess::SetShapeFixParameters(
  const DE_ShapeFixParameters& theParameters,
  const ParameterMap&          theAdditionalParameters)
{
  myShapeProcParams.clear();
  XSAlgo_ShapeProcessor::FillParameterMap(theParameters, true, myShapeProcParams);
  for (const auto& aParam : theAdditionalParameters)
  {
    if (myShapeProcParams.find(aParam.first) == myShapeProcParams.end())
    {
      myShapeProcParams[aParam.first] = aParam.second;
    }
  }
}

//=============================================================================

void Transfer_ActorOfTransientProcess::SetProcessingFlags(
  const ShapeProcess::OperationsFlags& theFlags)
{
  myShapeProcFlags.first  = theFlags;
  myShapeProcFlags.second = true;
}
