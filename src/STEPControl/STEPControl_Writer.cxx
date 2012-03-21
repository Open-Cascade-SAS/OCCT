// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <STEPControl_Writer.ixx>
#include <STEPControl_Controller.hxx>
#include <STEPControl_ActorWrite.hxx>
#include <XSControl_TransferWriter.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Message_ProgressIndicator.hxx>
#include <TopExp_Explorer.hxx>
#include <Transfer_FinderProcess.hxx>


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
  (const TopoDS_Shape& sh, const STEPControl_StepModelType mode,
   const Standard_Boolean compgraph) 
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
  thesession->SetModeWriteShape (mws);

  // for progress indicator.
  Handle(Message_ProgressIndicator) progress =
    WS()->TransferWriter()->FinderProcess()->GetProgress();
  if ( ! progress.IsNull() ) {
    Standard_Integer nbfaces=0;
    for( TopExp_Explorer exp(sh, TopAbs_FACE); exp.More(); exp.Next())  nbfaces++;
    progress->SetScale ( "Face", 0, nbfaces, 1 );
    progress->Show();
  }

  return thesession->TransferWriteShape(sh,compgraph);
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
