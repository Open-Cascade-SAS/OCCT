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

#include <STEPControl_Writer.hxx>

#include <DE_ShapeFixParameters.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <STEPControl_ActorWrite.hxx>
#include <STEPControl_Controller.hxx>
#include <DESTEP_Parameters.hxx>
#include <StepData_StepModel.hxx>
#include <StepData_Protocol.hxx>
#include <StepData_StepWriter.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_ShapeProcessor.hxx>
#include <XSControl_WorkSession.hxx>
#include <UnitsMethods.hxx>

//=================================================================================================

STEPControl_Writer::STEPControl_Writer()
{
  STEPControl_Controller::Init();
  SetWS(new XSControl_WorkSession);
}

//=================================================================================================

STEPControl_Writer::STEPControl_Writer(const Handle(XSControl_WorkSession)& WS,
                                       const Standard_Boolean               scratch)
{
  STEPControl_Controller::Init();
  SetWS(WS, scratch);
}

//=================================================================================================

void STEPControl_Writer::SetWS(const Handle(XSControl_WorkSession)& WS,
                               const Standard_Boolean               scratch)
{
  thesession = WS;
  thesession->SelectNorm("STEP");
  thesession->InitTransferReader(0);
  Handle(StepData_StepModel) model = Model(scratch);
}

//=================================================================================================

Handle(XSControl_WorkSession) STEPControl_Writer::WS() const
{
  return thesession;
}

//=================================================================================================

Handle(StepData_StepModel) STEPControl_Writer::Model(const Standard_Boolean newone)
{
  DeclareAndCast(StepData_StepModel, model, thesession->Model());
  if (newone || model.IsNull())
    model = GetCasted(StepData_StepModel, thesession->NewModel());
  return model;
}

//=================================================================================================

void STEPControl_Writer::SetTolerance(const Standard_Real Tol)
{
  DeclareAndCast(STEPControl_ActorWrite, act, WS()->NormAdaptor()->ActorWrite());
  if (!act.IsNull())
    act->SetTolerance(Tol);
}

//=================================================================================================

void STEPControl_Writer::UnsetTolerance()
{
  SetTolerance(-1.);
}

//=================================================================================================

IFSelect_ReturnStatus STEPControl_Writer::Transfer(const TopoDS_Shape&             sh,
                                                   const STEPControl_StepModelType mode,
                                                   const Standard_Boolean          compgraph,
                                                   const Message_ProgressRange&    theProgress)
{
  Handle(StepData_StepModel) aStepModel = Handle(StepData_StepModel)::DownCast(thesession->Model());
  if (!aStepModel.IsNull())
  {
    aStepModel->InternalParameters.InitFromStatic();
  }
  return Transfer(sh, mode, aStepModel->InternalParameters, compgraph, theProgress);
}

IFSelect_ReturnStatus STEPControl_Writer::Transfer(const TopoDS_Shape&             sh,
                                                   const STEPControl_StepModelType mode,
                                                   const DESTEP_Parameters&        theParams,
                                                   const Standard_Boolean          compgraph,
                                                   const Message_ProgressRange&    theProgress)
{
  Standard_Integer mws = -1;
  switch (mode)
  {
    case STEPControl_AsIs:
      mws = 0;
      break;
    case STEPControl_FacetedBrep:
      mws = 1;
      break;
    case STEPControl_ShellBasedSurfaceModel:
      mws = 2;
      break;
    case STEPControl_ManifoldSolidBrep:
      mws = 3;
      break;
    case STEPControl_GeometricCurveSet:
      mws = 4;
      break;
    default:
      break;
  }
  if (mws < 0)
    return IFSelect_RetError; // cas non reconnu
  thesession->TransferWriter()->SetTransferMode(mws);
  if (!Model()->IsInitializedUnit())
  {
    XSAlgo_ShapeProcessor::PrepareForTransfer(); // update unit info
    Model()->SetLocalLengthUnit(UnitsMethods::GetCasCadeLengthUnit());
  }
  if (!thesession->Model().IsNull())
  {
    Handle(StepData_StepModel)::DownCast(thesession->Model())->InternalParameters = theParams;
  }
  Handle(STEPControl_ActorWrite) ActWrite =
    Handle(STEPControl_ActorWrite)::DownCast(WS()->NormAdaptor()->ActorWrite());
  ActWrite->SetGroupMode(
    Handle(StepData_StepModel)::DownCast(thesession->Model())->InternalParameters.WriteAssembly);
  InitializeMissingParameters();
  return thesession->TransferWriteShape(sh, compgraph, theProgress);
}

//=================================================================================================

IFSelect_ReturnStatus STEPControl_Writer::Write(const Standard_CString theFileName)
{
  return thesession->SendAll(theFileName);
}

//=================================================================================================

IFSelect_ReturnStatus STEPControl_Writer::WriteStream(std::ostream& theOStream)
{
  Handle(StepData_StepModel) aModel = Model();
  if (aModel.IsNull())
  {
    return IFSelect_RetFail;
  }

  Handle(StepData_Protocol) aProtocol = Handle(StepData_Protocol)::DownCast(aModel->Protocol());
  if (aProtocol.IsNull())
  {
    return IFSelect_RetFail;
  }

  StepData_StepWriter aWriter(aModel);
  aWriter.SendModel(aProtocol);
  return aWriter.Print(theOStream) ? IFSelect_RetDone : IFSelect_RetFail;
}

//=================================================================================================

void STEPControl_Writer::PrintStatsTransfer(const Standard_Integer what,
                                            const Standard_Integer mode) const
{
  thesession->TransferWriter()->PrintStats(what, mode);
}

//=============================================================================

void STEPControl_Writer::SetShapeFixParameters(const ParameterMap& theParameters)
{
  if (Handle(Transfer_ActorOfFinderProcess) anActor = GetActor())
  {
    anActor->SetShapeFixParameters(theParameters);
  }
}

//=============================================================================

void STEPControl_Writer::SetShapeFixParameters(ParameterMap&& theParameters)
{
  if (Handle(Transfer_ActorOfFinderProcess) anActor = GetActor())
  {
    anActor->SetShapeFixParameters(std::move(theParameters));
  }
}

//=============================================================================

void STEPControl_Writer::SetShapeFixParameters(const DE_ShapeFixParameters& theParameters,
                                               const ParameterMap&          theAdditionalParameters)
{
  if (Handle(Transfer_ActorOfFinderProcess) anActor = GetActor())
  {
    anActor->SetShapeFixParameters(theParameters, theAdditionalParameters);
  }
}

//=============================================================================

const STEPControl_Writer::ParameterMap& STEPControl_Writer::GetShapeFixParameters() const
{
  static const ParameterMap                   anEmptyMap;
  const Handle(Transfer_ActorOfFinderProcess) anActor = GetActor();
  return anActor.IsNull() ? anEmptyMap : anActor->GetShapeFixParameters();
}

//=============================================================================

void STEPControl_Writer::SetShapeProcessFlags(const ShapeProcess::OperationsFlags& theFlags)
{
  if (Handle(Transfer_ActorOfFinderProcess) anActor = GetActor())
  {
    anActor->SetShapeProcessFlags(theFlags);
  }
}

//=============================================================================

const STEPControl_Writer::ProcessingFlags& STEPControl_Writer::GetShapeProcessFlags() const
{
  static const ProcessingFlags                anEmptyFlags;
  const Handle(Transfer_ActorOfFinderProcess) anActor = GetActor();
  return anActor.IsNull() ? anEmptyFlags : anActor->GetShapeProcessFlags();
}

//=============================================================================

Handle(Transfer_ActorOfFinderProcess) STEPControl_Writer::GetActor() const
{
  Handle(XSControl_WorkSession) aSession = WS();
  if (aSession.IsNull())
  {
    return nullptr;
  }

  Handle(XSControl_Controller) aController = aSession->NormAdaptor();
  if (aController.IsNull())
  {
    return nullptr;
  }

  return aController->ActorWrite();
}

//=============================================================================

void STEPControl_Writer::InitializeMissingParameters()
{
  if (GetShapeFixParameters().empty())
  {
    SetShapeFixParameters(DESTEP_Parameters::GetDefaultShapeFixParameters());
  }
  if (GetShapeProcessFlags().second == false)
  {
    ShapeProcess::OperationsFlags aFlags;
    aFlags.set(ShapeProcess::Operation::SplitCommonVertex);
    aFlags.set(ShapeProcess::Operation::DirectFaces);
    SetShapeProcessFlags(aFlags);
  }
}
