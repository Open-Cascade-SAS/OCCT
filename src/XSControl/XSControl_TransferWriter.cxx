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


#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Message_Messenger.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_ActorOfFinderProcess.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransientMapper.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_TransferWriter.hxx>
#include <XSControl_Utils.hxx>

XSControl_TransferWriter::XSControl_TransferWriter ()
{  theTransferWrite = new Transfer_FinderProcess;  theTransferMode = 0;  }

    Handle(Transfer_FinderProcess)  XSControl_TransferWriter::FinderProcess () const
      {  return theTransferWrite;  }

    void  XSControl_TransferWriter::SetFinderProcess (const Handle(Transfer_FinderProcess)& FP)
      {  theTransferWrite = FP;  }

    Handle(XSControl_Controller)  XSControl_TransferWriter::Controller () const
      {  return theController;  }

    void  XSControl_TransferWriter::SetController (const Handle(XSControl_Controller)& ctl)
{
  theController = ctl;
  Clear(-1);
}

    void  XSControl_TransferWriter::Clear (const Standard_Integer mode)
{
  if (mode < 0 || theTransferWrite.IsNull())
    theTransferWrite = new Transfer_FinderProcess;
  else theTransferWrite->Clear();
}

    Standard_Integer  XSControl_TransferWriter::TransferMode () const
      {  return theTransferMode;  }

    void  XSControl_TransferWriter::SetTransferMode (const Standard_Integer mod)
      {  theTransferMode = mod;  }

    void  XSControl_TransferWriter::PrintStats
  (const Standard_Integer , const Standard_Integer ) const
{
  Handle(Message_Messenger) sout = theTransferWrite->Messenger();
//  A ameliorer ... !
  sout<<"\n*******************************************************************\n";
  sout << "******        Statistics on Transfer (Write)                 ******"<<endl;
  sout<<"\n*******************************************************************\n";
  sout << "******        Transfer Mode = "<<theTransferMode;
  Standard_CString modehelp = theController->ModeWriteHelp (theTransferMode);
  if (modehelp && modehelp[0] != 0) sout<<"  I.E.  "<<modehelp;
  sout<<"       ******"<<endl;
}


//  ##########    LES ACTIONS    ##########

    Standard_Boolean  XSControl_TransferWriter::RecognizeTransient
  (const Handle(Standard_Transient)& obj)
{
  if (theController.IsNull()) return Standard_False;
  XSControl_Utils xu;
  TopoDS_Shape sh = xu.BinderShape (obj);
  if (!sh.IsNull()) return RecognizeShape (sh);
  return theController->RecognizeWriteTransient (obj,theTransferMode);
}

    IFSelect_ReturnStatus  XSControl_TransferWriter::TransferWriteTransient
  (const Handle(Interface_InterfaceModel)& model,
   const Handle(Standard_Transient)& obj)
{
  IFSelect_ReturnStatus status = IFSelect_RetVoid;
  if (theController.IsNull()) return IFSelect_RetError;
  if (model.IsNull()) return IFSelect_RetVoid;

  if (theTransferWrite.IsNull()) theTransferWrite = new Transfer_FinderProcess;
  Handle(Transfer_ActorOfFinderProcess) nulact;
  theTransferWrite->SetActor (nulact);
  Handle(Standard_Transient) resultat;
  Handle(Message_Messenger) sout = theTransferWrite->Messenger();
  try {
    OCC_CATCH_SIGNALS
    PrintStats(theTransferMode);
    sout << "******        Transferring Transient, CDL Type = ";
    sout<<obj->DynamicType()->Name()<<"   ******"<<endl;
    status = theController->TransferWriteTransient
      (obj,theTransferWrite,model,theTransferMode);
  }
  catch(Standard_Failure) {
    sout<<"****  ****  TransferWriteShape, EXCEPTION : ";
    sout<<Standard_Failure::Caught()->GetMessageString(); 
    sout<<endl;
    status = IFSelect_RetFail;
  }
  return status;
}


    Standard_Boolean  XSControl_TransferWriter::RecognizeShape
  (const TopoDS_Shape& shape)
{
  if (theController.IsNull()) return Standard_False;
  if (shape.IsNull()) return Standard_False;
  return theController->RecognizeWriteShape (shape,theTransferMode);
}


    IFSelect_ReturnStatus  XSControl_TransferWriter::TransferWriteShape
  (const Handle(Interface_InterfaceModel)& model,
   const TopoDS_Shape& shape)
{
  IFSelect_ReturnStatus status = IFSelect_RetVoid;
  if (theController.IsNull()) return IFSelect_RetError;
  if (model.IsNull()) return IFSelect_RetVoid;

  if (theTransferWrite.IsNull()) theTransferWrite = new Transfer_FinderProcess;
//  effacer l actor : Controller s en charge
  Handle(Transfer_ActorOfFinderProcess) nulact;
  theTransferWrite->SetActor (nulact);
  Handle(Standard_Transient) resultat;
  Handle(Message_Messenger) sout = theTransferWrite->Messenger();
  try {
    OCC_CATCH_SIGNALS
    PrintStats(theTransferMode);
    sout << "******        Transferring Shape, ShapeType = " << shape.ShapeType();
    sout<<"                      ******"<<endl;
    status = theController->TransferWriteShape
      (shape,theTransferWrite,model,theTransferMode);
  }
  catch(Standard_Failure) {
    sout<<"****  ****  TransferWriteShape, EXCEPTION : "; 
    sout<<Standard_Failure::Caught()->GetMessageString(); 
    sout<<endl;
    status = IFSelect_RetFail;
  }
  return status;
}


    Interface_CheckIterator  XSControl_TransferWriter::CheckList () const
{
  Interface_CheckIterator  chl;
  if (theTransferWrite.IsNull()) return chl;
  Standard_Integer i, nb = theTransferWrite->NbMapped();
  for (i = 1; i <= nb; i ++) {
    DeclareAndCast(Transfer_SimpleBinderOfTransient,tb,theTransferWrite->MapItem
(i));
    if (tb.IsNull()) continue;
    Handle(Interface_Check) ach = tb->Check();
    if (ach->NbFails() == 0 || ach->NbWarnings() == 0) continue;
    DeclareAndCast(Transfer_TransientMapper,tm,theTransferWrite->Mapped(i));
    if (tm.IsNull()) ach->GetEntity (theTransferWrite->Mapped(i));
    else  ach->GetEntity (tm->Value());
    chl.Add(ach);
  }
  return chl;
}

    Interface_CheckIterator  XSControl_TransferWriter::ResultCheckList
  (const Handle(Interface_InterfaceModel)& model) const
{
  Interface_CheckIterator  chl;
  if (theTransferWrite.IsNull()) return chl;
  Standard_Integer i, nb = theTransferWrite->NbMapped();
  for (i = 1; i <= nb; i ++) {
    DeclareAndCast(Transfer_SimpleBinderOfTransient,tb,theTransferWrite->MapItem
(i));
    if (tb.IsNull()) continue;
    const Handle(Interface_Check) ach = tb->Check();
    if (ach->NbFails() == 0 || ach->NbWarnings() == 0) continue;
    Handle(Standard_Transient) ent = tb->Result();
    if (!ent.IsNull() && !model.IsNull()) chl.Add(ach,model->Number(ent));
    else  chl.Add(ach,0);
  }
  return chl;
}

    void  XSControl_TransferWriter::PrintStatsProcess
  (const Handle(Transfer_FinderProcess)& ,
   const Standard_Integer , const Standard_Integer )
{  }
