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

#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <STEPControl_ActorWrite.hxx>
#include <STEPControl_Controller.hxx>
#include <StepData_StepModel.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_FinderProcess.hxx>
#include <XSControl_TransferWriter.hxx>
#include <XSControl_WorkSession.hxx>

//=======================================================================
//function : STEPControl_Writer
//purpose  : 
//=======================================================================
STEPControl_Writer::STEPControl_Writer ()
{
  STEPControl_Controller::Init();
  SetWS (new XSControl_WorkSession);
}


//=======================================================================
//function : STEPControl_Writer

//purpose  : 
//=======================================================================

STEPControl_Writer::STEPControl_Writer
  (const Handle(XSControl_WorkSession)& WS, const Standard_Boolean scratch)
{
  STEPControl_Controller::Init();
  SetWS (WS,scratch);
}


//=======================================================================
//function : SetWS

//purpose  : 
//=======================================================================

void STEPControl_Writer::SetWS(const Handle(XSControl_WorkSession)& WS,
                               const Standard_Boolean scratch)
{
  thesession = WS;
  thesession->SelectNorm("STEP");
  thesession->InitTransferReader(0);
  Handle(StepData_StepModel) model = Model (scratch);
}


//=======================================================================
//function : WS
//purpose  : 
//=======================================================================

Handle(XSControl_WorkSession) STEPControl_Writer::WS () const
{
  return thesession;
}


//=======================================================================
//function : Model
//purpose  : 
//=======================================================================

Handle(StepData_StepModel) STEPControl_Writer::Model
       (const Standard_Boolean newone)
{
  DeclareAndCast(StepData_StepModel,model,thesession->Model());
  if (newone || model.IsNull())
    model = GetCasted(StepData_StepModel,thesession->NewModel());
  return model;
}


//=======================================================================
//function : SetTolerance
//purpose  : 
//=======================================================================

void STEPControl_Writer::SetTolerance (const Standard_Real Tol)
{
  DeclareAndCast(STEPControl_ActorWrite,act,WS()->NormAdaptor()->ActorWrite());
  if (!act.IsNull()) act->SetTolerance (Tol);
}


//=======================================================================
//function : UnsetTolerance
//purpose  : 
//=======================================================================

void STEPControl_Writer::UnsetTolerance ()
{
  SetTolerance (-1.);
}


//=======================================================================
//function : Transfer
//purpose  : 
//=======================================================================

IFSelect_ReturnStatus STEPControl_Writer::Transfer
  (const TopoDS_Shape& sh,
   const STEPControl_StepModelType mode,
   const Standard_Boolean compgraph,
   const Message_ProgressRange& theProgress)
{
  Standard_Integer mws = -1;
  switch (mode) {
    case STEPControl_AsIs :                   mws = 0;  break;
    case STEPControl_FacetedBrep :            mws = 1;  break;
    case STEPControl_ShellBasedSurfaceModel : mws = 2;  break;
    case STEPControl_ManifoldSolidBrep :      mws = 3;  break;
    case STEPControl_GeometricCurveSet :      mws = 4;  break;
    default : break;
  }
  if (mws < 0) return IFSelect_RetError;    // cas non reconnu
  thesession->TransferWriter()->SetTransferMode (mws);

  return thesession->TransferWriteShape(sh, compgraph, theProgress);
}


//=======================================================================
//function : Write
//purpose  : 
//=======================================================================

IFSelect_ReturnStatus STEPControl_Writer::Write (const Standard_CString filename)
{
  return thesession->SendAll(filename);
}


//=======================================================================
//function : PrintStatsTransfer
//purpose  : 
//=======================================================================

void STEPControl_Writer::PrintStatsTransfer
  (const Standard_Integer what, const Standard_Integer mode) const
{
  thesession->TransferWriter()->PrintStats (what,mode);
}
