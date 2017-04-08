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


#include <DBRep.hxx>
#include <Draw_Appli.hxx>
#include <IFSelect_Functions.hxx>
#include <IFSelect_SessionPilot.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Protocol.hxx>
#include <Message.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <XSControl.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_FuncShape.hxx>
#include <XSControl_Functions.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_TransferWriter.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSDRAW.hxx>
#include <XSDRAW_Vars.hxx>

#include <stdio.h>
//#include <XSDRAW_Shape.hxx>
static int deja = 0, dejald = 0;
//unused variable 
//static int okxset = 0;

static NCollection_DataMap<TCollection_AsciiString, Standard_Integer> theolds;
static Handle(TColStd_HSequenceOfAsciiString) thenews;

static Handle(IFSelect_SessionPilot)    thepilot;  // detient Session, Model

static Standard_Integer XSTEPDRAWRUN (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  TCollection_AsciiString mess;
  for (Standard_Integer i = 0; i < argc; i ++) {
    mess.AssignCat(argv[i]);  mess.AssignCat(" ");
  }

  IFSelect_ReturnStatus stat = thepilot->Execute (mess.ToCString());
  if (stat == IFSelect_RetError || stat == IFSelect_RetFail) return 1;
  else return 0;
}

    void  XSDRAW::ChangeCommand
  (const Standard_CString oldname, const Standard_CString newname)
{
  Standard_Integer num = 0;
  if (newname[0] != '\0') {
    if (thenews.IsNull()) thenews = new TColStd_HSequenceOfAsciiString();
    TCollection_AsciiString newstr(newname);
    thenews->Append(newstr);
    num = thenews->Length();
  }
  theolds.Bind(oldname,num);
}

    void  XSDRAW::RemoveCommand
  (const Standard_CString oldname)
{
  ChangeCommand (oldname,"");
}

    Standard_Boolean  XSDRAW::LoadSession ()
{
  if (deja) return Standard_False;
  deja = 1;
  thepilot   = new IFSelect_SessionPilot("XSTEP-DRAW>");
  Handle(XSControl_WorkSession) WS = new XSControl_WorkSession;
  WS->SetVars (new XSDRAW_Vars);
  thepilot->SetSession (WS);

  IFSelect_Functions::Init();
  XSControl_Functions::Init();
  XSControl_FuncShape::Init();
//  XSDRAW_Shape::Init();   passe a present par theCommands
  return Standard_True;
}

void XSDRAW::LoadDraw (Draw_Interpretor& theCommands)
{
  if (dejald)
  {
    return;
  }
  dejald = 1;
//  Pour tout faire d un coup : BRepTest & cie:
  LoadSession();

  //skl: we make remove commands "x" and "exit" in order to this commands are
  //     performed not in IFSelect_SessionPilot but in standard Tcl interpretor
  XSDRAW::RemoveCommand("x");
  XSDRAW::RemoveCommand("exit");

//  if (!getenv("WBHOSTTOP")) XSDRAW::RemoveCommand("xsnew");
  Handle(TColStd_HSequenceOfAsciiString) list =
    IFSelect_Activator::Commands(0);
  TCollection_AsciiString com;
  Standard_Integer i, nb = list->Length();
  for (i = 1; i <= nb; i ++) {
    Handle(IFSelect_Activator) act;
    Standard_Integer nact, num = -1;
    char help[200];
    com = list->Value(i);
    if (!theolds.IsEmpty())
      theolds.Find(com, num);
    if (num == 0) continue;
    if (!IFSelect_Activator::Select(com.ToCString(),nact,act))
      Sprintf (help,"type :  xhelp %s for help",com.ToCString());
    else if (!act.IsNull()) strcpy(help,act->Help(nact));
    if (num < 0) theCommands.Add (com.ToCString(),help,XSTEPDRAWRUN,act->Group());
    else theCommands.Add (thenews->Value(num).ToCString(),help,XSTEPDRAWRUN,act->Group());
  }
}

    Standard_Integer  XSDRAW::Execute
    (const Standard_CString command, const Standard_CString varname)
{
  char mess[100];
  Sprintf (mess,command,varname);
  thepilot->Execute (mess);
  return 1;  // stat ?
}

    Handle(IFSelect_SessionPilot)  XSDRAW::Pilot ()
      {  return thepilot;  }

    Handle(XSControl_WorkSession)  XSDRAW::Session ()
      {  return XSControl::Session(thepilot);  }

    void  XSDRAW::SetController (const Handle(XSControl_Controller)& control)
{
  if (thepilot.IsNull()) XSDRAW::LoadSession();
  if (control.IsNull()) cout<<"XSTEP Controller not defined"<<endl;
  else if (!Session().IsNull()) Session()->SetController (control);
  else cout<<"XSTEP Session badly or not defined"<<endl;
}


    Handle(XSControl_Controller) XSDRAW::Controller ()
      {  return Session()->NormAdaptor();  }

    Standard_Boolean  XSDRAW::SetNorm
  (const Standard_CString norm)
{
  return Session()->SelectNorm (norm);
}

    Handle(Interface_Protocol)   XSDRAW::Protocol ()
      {  return thepilot->Session()->Protocol();  }

    Handle(Interface_InterfaceModel)  XSDRAW::Model ()
      {  return thepilot->Session()->Model();  }

    void  XSDRAW::SetModel
  (const Handle(Interface_InterfaceModel)& model, const Standard_CString file)
{
  thepilot->Session()->SetModel(model);
  if (file && file[0] != '\0') thepilot->Session()->SetLoadedFile(file);
}

    Handle(Interface_InterfaceModel)  XSDRAW::NewModel ()
      {  return Session()->NewModel();  }

    Handle(Standard_Transient)  XSDRAW::Entity (const Standard_Integer num)
      {  return thepilot->Session()->StartingEntity(num);  }

    Standard_Integer  XSDRAW::Number (const Handle(Standard_Transient)& ent)
      {  return thepilot->Session()->StartingNumber(ent);  }

    void  XSDRAW::SetTransferProcess (const Handle(Standard_Transient)& ATP)
{
  DeclareAndCast(Transfer_FinderProcess,FP,ATP);
  DeclareAndCast(Transfer_TransientProcess,TP,ATP);

//   Cas FinderProcess    ==> TransferWriter
  if (!FP.IsNull())  Session()->SetMapWriter(FP);

//   Cas TransientProcess ==> TransferReader
  if (!TP.IsNull()) {
    if (!TP->Model().IsNull() && TP->Model() != Session()->Model())
      Session()->SetModel (TP->Model());
    Session()->SetMapReader(TP);
  }
}

    Handle(Transfer_TransientProcess)  XSDRAW::TransientProcess  ()
      {  return Session()->TransferReader()->TransientProcess();  }

    Handle(Transfer_FinderProcess)  XSDRAW::FinderProcess  ()
      {  return Session()->TransferWriter()->FinderProcess();  }


    void XSDRAW::InitTransferReader (const Standard_Integer mode)
{
//   0 nullify  1 clear
//   2 init TR avec contenu TP (roots)  3 init TP avec contenu TR
//   4 init avec model (debut scratch)
  Session()->InitTransferReader(mode);
}

    Handle(XSControl_TransferReader)  XSDRAW::TransferReader ()
      {  return Session()->TransferReader();  }


//  ############  AUXILIAIRES  #############

    Handle(Standard_Transient)  XSDRAW::GetEntity (const Standard_CString name)
      {  return  IFSelect_Functions::GiveEntity (Session(),name);  }

    Standard_Integer  XSDRAW::GetEntityNumber  (const Standard_CString name)
      {  return  IFSelect_Functions::GiveEntityNumber (Session(),name);  }


    Handle(TColStd_HSequenceOfTransient)  XSDRAW::GetList
  (const Standard_CString first, const Standard_CString second)
{
  Handle(TColStd_HSequenceOfTransient) list;
  if (!first || first[0] == '\0') {
    char ligne[80];  ligne[0] = '\0'; char truc;
//    cin.clear();  cin.get (ligne,79,'\n');
    cin >> ligne;  Standard_Size ln = strlen(ligne);
    char *ff = &ligne[0], *ss = NULL;
    cin.get(truc);  if (truc != '\n') { cin>>&ligne[ln+1]; ss = &ligne[ln+1]; }
    return  XSDRAW::GetList (ff,ss);
  }
//  return IFSelect_Functions::GiveList (Session(),first,second);
  return IFSelect_Functions::GiveList (Session(),first,second);
}


    Standard_Boolean  XSDRAW::FileAndVar
  (const Standard_CString file, const Standard_CString var,
   const Standard_CString def,
   TCollection_AsciiString& resfile,   TCollection_AsciiString& resvar)
{  return XSControl_FuncShape::FileAndVar
     (XSDRAW::Session(),file,var,def,resfile,resvar); }

    Standard_Integer  XSDRAW::MoreShapes
  (Handle(TopTools_HSequenceOfShape)& list, const Standard_CString name)
{  return XSControl_FuncShape::MoreShapes  (XSDRAW::Session(),list,name);  }


//  FONCTION POUR LE DEBUG

Standard_Integer XSDRAW_WHAT (const Handle(Standard_Transient)& ent)
{
  if (ent.IsNull()) { cout<<"(Null Handle)"<<endl; return 0; }
  Handle(Interface_InterfaceModel) model = XSDRAW::Model();
  if (model.IsNull()) { cout<<"(No model)  Type:"<<ent->DynamicType()->Name()<<endl; return 0; }
  cout<<" Num/Id :";  
  model->Print (ent, Message::DefaultMessenger(), 0);
  cout<<"  --  Recorded Type:"<<model->TypeName (ent)<<endl;
  return model->Number(ent);
}
