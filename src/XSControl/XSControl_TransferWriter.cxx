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

#include <XSControl_TransferWriter.ixx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransientMapper.hxx>
#include <XSControl_Utils.hxx>

#include <Message_Messenger.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Check.hxx>


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
