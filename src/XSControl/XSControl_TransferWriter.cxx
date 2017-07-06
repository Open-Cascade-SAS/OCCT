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

IMPLEMENT_STANDARD_RTTIEXT(XSControl_TransferWriter,Standard_Transient)


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void XSControl_TransferWriter::Clear (const Standard_Integer mode)
{
  if (mode < 0 || myTransferWriter.IsNull())
    myTransferWriter = new Transfer_FinderProcess;
  else myTransferWriter->Clear();
}

//=======================================================================
//function : PrintStats
//purpose  : 
//=======================================================================

void XSControl_TransferWriter::PrintStats (const Standard_Integer , const Standard_Integer ) const
{
  Handle(Message_Messenger) sout = myTransferWriter->Messenger();
//  A ameliorer ... !
  sout<<"\n*******************************************************************\n";
  sout << "******        Statistics on Transfer (Write)                 ******"<<endl;
  sout<<"\n*******************************************************************\n";
  sout << "******        Transfer Mode = "<<myTransferMode;
  Standard_CString modehelp = myController->ModeWriteHelp (myTransferMode);
  if (modehelp && modehelp[0] != 0) sout<<"  I.E.  "<<modehelp;
  sout<<"       ******"<<endl;
}


//  ##########    LES ACTIONS    ##########

//=======================================================================
//function : RecognizeTransient
//purpose  : 
//=======================================================================

Standard_Boolean XSControl_TransferWriter::RecognizeTransient (const Handle(Standard_Transient)& obj)
{
  if (myController.IsNull()) return Standard_False;
  XSControl_Utils xu;
  TopoDS_Shape sh = xu.BinderShape (obj);
  if (!sh.IsNull()) return RecognizeShape (sh);
  return myController->RecognizeWriteTransient (obj,myTransferMode);
}

//=======================================================================
//function : TransferWriteTransient
//purpose  : 
//=======================================================================

IFSelect_ReturnStatus XSControl_TransferWriter::TransferWriteTransient
  (const Handle(Interface_InterfaceModel)& model,
   const Handle(Standard_Transient)& obj)
{
  IFSelect_ReturnStatus status = IFSelect_RetVoid;
  if (myController.IsNull()) return IFSelect_RetError;
  if (model.IsNull()) return IFSelect_RetVoid;

  if (myTransferWriter.IsNull()) myTransferWriter = new Transfer_FinderProcess;
  Handle(Transfer_ActorOfFinderProcess) nulact;
  myTransferWriter->SetActor (nulact);
  Handle(Standard_Transient) resultat;
  Handle(Message_Messenger) sout = myTransferWriter->Messenger();
  try {
    OCC_CATCH_SIGNALS
    PrintStats(myTransferMode);
    sout << "******        Transferring Transient, CDL Type = ";
    sout<<obj->DynamicType()->Name()<<"   ******"<<endl;
    status = myController->TransferWriteTransient
      (obj,myTransferWriter,model,myTransferMode);
  }
  catch(Standard_Failure const& anException) {
    sout<<"****  ****  TransferWriteShape, EXCEPTION : ";
    sout<<anException.GetMessageString(); 
    sout<<endl;
    status = IFSelect_RetFail;
  }
  return status;
}

//=======================================================================
//function : RecognizeShape
//purpose  : 
//=======================================================================

Standard_Boolean XSControl_TransferWriter::RecognizeShape (const TopoDS_Shape& shape)
{
  if (myController.IsNull()) return Standard_False;
  if (shape.IsNull()) return Standard_False;
  return myController->RecognizeWriteShape (shape,myTransferMode);
}

//=======================================================================
//function : TransferWriteShape
//purpose  : 
//=======================================================================

IFSelect_ReturnStatus XSControl_TransferWriter::TransferWriteShape
  (const Handle(Interface_InterfaceModel)& model,
   const TopoDS_Shape& shape)
{
  IFSelect_ReturnStatus status = IFSelect_RetVoid;
  if (myController.IsNull()) return IFSelect_RetError;
  if (model.IsNull()) return IFSelect_RetVoid;

  if (myTransferWriter.IsNull()) myTransferWriter = new Transfer_FinderProcess;
//  effacer l actor : Controller s en charge
  Handle(Transfer_ActorOfFinderProcess) nulact;
  myTransferWriter->SetActor (nulact);
  Handle(Standard_Transient) resultat;
  Handle(Message_Messenger) sout = myTransferWriter->Messenger();
  try {
    OCC_CATCH_SIGNALS
    PrintStats(myTransferMode);
    sout << "******        Transferring Shape, ShapeType = " << shape.ShapeType();
    sout<<"                      ******"<<endl;
    status = myController->TransferWriteShape
      (shape,myTransferWriter,model,myTransferMode);
  }
  catch(Standard_Failure const& anException) {
    sout<<"****  ****  TransferWriteShape, EXCEPTION : "; 
    sout<<anException.GetMessageString(); 
    sout<<endl;
    status = IFSelect_RetFail;
  }
  return status;
}

//=======================================================================
//function : CheckList
//purpose  : 
//=======================================================================

Interface_CheckIterator XSControl_TransferWriter::CheckList () const
{
  Interface_CheckIterator  chl;
  if (myTransferWriter.IsNull()) return chl;
  Standard_Integer i, nb = myTransferWriter->NbMapped();
  for (i = 1; i <= nb; i ++) {
    DeclareAndCast(Transfer_SimpleBinderOfTransient,tb,myTransferWriter->MapItem(i));
    if (tb.IsNull()) continue;
    Handle(Interface_Check) ach = tb->Check();
    if (ach->NbFails() == 0 || ach->NbWarnings() == 0) continue;
    DeclareAndCast(Transfer_TransientMapper,tm,myTransferWriter->Mapped(i));
    if (tm.IsNull()) ach->GetEntity (myTransferWriter->Mapped(i));
    else  ach->GetEntity (tm->Value());
    chl.Add(ach);
  }
  return chl;
}

//=======================================================================
//function : ResultCheckList
//purpose  : 
//=======================================================================

Interface_CheckIterator XSControl_TransferWriter::ResultCheckList
  (const Handle(Interface_InterfaceModel)& model) const
{
  Interface_CheckIterator  chl;
  if (myTransferWriter.IsNull()) return chl;
  Standard_Integer i, nb = myTransferWriter->NbMapped();
  for (i = 1; i <= nb; i ++) {
    DeclareAndCast(Transfer_SimpleBinderOfTransient,tb,myTransferWriter->MapItem(i));
    if (tb.IsNull()) continue;
    const Handle(Interface_Check) ach = tb->Check();
    if (ach->NbFails() == 0 || ach->NbWarnings() == 0) continue;
    Handle(Standard_Transient) ent = tb->Result();
    if (!ent.IsNull() && !model.IsNull()) chl.Add(ach,model->Number(ent));
    else  chl.Add(ach,0);
  }
  return chl;
}
