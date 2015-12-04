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


#include <IFSelect_ContextWrite.hxx>
#include <IFSelect_GeneralModifier.hxx>
#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ParamType.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_ReportEntity.hxx>
#include <Interface_UndefinedContent.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <OSD_OpenFile.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <StepData_Protocol.hxx>
#include <StepData_StepDumper.hxx>
#include <StepData_StepModel.hxx>
#include <StepData_StepWriter.hxx>
#include <StepData_UndefinedEntity.hxx>
#include <StepFile_Read.hxx>
#include <StepSelect_FileModifier.hxx>
#include <StepSelect_WorkLibrary.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfInteger.hxx>

#include <errno.h>
#include <sys/stat.h>
IMPLEMENT_STANDARD_RTTIEXT(StepSelect_WorkLibrary,IFSelect_WorkLibrary)

StepSelect_WorkLibrary::StepSelect_WorkLibrary
  (const Standard_Boolean copymode)
  : thecopymode (copymode) , thelabmode (0)
{
  SetDumpLevels (1,2);
  SetDumpHelp (0,"#id + Step Type");
  SetDumpHelp (1,"Entity as in file");
  SetDumpHelp (2,"Entity + shareds (level 1) as in file");
}
// rq : les init sont faits par ailleurs, pas de souci a se faire


void  StepSelect_WorkLibrary::SetDumpLabel (const Standard_Integer mode)
{
  thelabmode = mode;
}


Standard_Integer  StepSelect_WorkLibrary::ReadFile
  (const Standard_CString name,
   Handle(Interface_InterfaceModel)& model,
   const Handle(Interface_Protocol)& protocol) const
{
  long status = 1;
  DeclareAndCast(StepData_Protocol,stepro,protocol);
  if (stepro.IsNull()) return 1;
  Handle(StepData_StepModel) stepmodel  = new StepData_StepModel;
  model  = stepmodel;
  StepFile_ReadTrace (0);
  char *pName=(char *)name;
  status = StepFile_Read (pName,stepmodel,stepro);
  return status;
}


Standard_Boolean  StepSelect_WorkLibrary::WriteFile
  (IFSelect_ContextWrite& ctx) const
{
//  Preparation
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  DeclareAndCast(StepData_StepModel,stepmodel,ctx.Model());
  DeclareAndCast(StepData_Protocol,stepro,ctx.Protocol());
  if (stepmodel.IsNull() || stepro.IsNull()) return Standard_False;

  ofstream fout;
  OSD_OpenStream(fout,ctx.FileName(),ios::out|ios::trunc);

  if (!fout || !fout.rdbuf()->is_open()) {
    ctx.CCheck(0)->AddFail("Step File could not be created");
    sout<<" Step File could not be created : " << ctx.FileName() << endl; return 0;
  }
  sout << " Step File Name : "<<ctx.FileName();
  StepData_StepWriter SW(stepmodel);
  sout<<"("<<stepmodel->NbEntities()<<" ents) ";

//  File Modifiers
  Standard_Integer nbmod = ctx.NbModifiers();
  for (Standard_Integer numod = 1; numod <= nbmod; numod ++) {
    ctx.SetModifier (numod);
    DeclareAndCast(StepSelect_FileModifier,filemod,ctx.FileModifier());
    if (!filemod.IsNull()) filemod->Perform(ctx,SW);
//   (impressions de mise au point)
    sout << " .. FileMod." << numod << filemod->Label();
    if (ctx.IsForAll()) sout << " (all model)";
    else  sout << " (" << ctx.NbEntities() << " entities)";
//    sout << flush;
  }

//  Envoi
  SW.SendModel(stepro);
  Interface_CheckIterator chl = SW.CheckList();
  for (chl.Start(); chl.More(); chl.Next())
    ctx.CCheck(chl.Number())->GetMessages(chl.Value());
  sout<<" Write ";
  Standard_Boolean isGood = SW.Print(fout);                 
  sout<<" Done"<<endl;
      
  errno = 0;
  fout.close();
  isGood = fout.good() && isGood && !errno;
  if(errno)
    sout << strerror(errno) << endl;
  return isGood;  
}


Standard_Boolean  StepSelect_WorkLibrary::CopyModel
  (const Handle(Interface_InterfaceModel)& original,
   const Handle(Interface_InterfaceModel)& newmodel,
   const Interface_EntityIterator& list,
   Interface_CopyTool& TC) const
{
  if (thecopymode) return
    IFSelect_WorkLibrary::CopyModel (original,newmodel,list,TC);
  return thecopymode;
}


void  StepSelect_WorkLibrary::DumpEntity
  (const Handle(Interface_InterfaceModel)& model,
   const Handle(Interface_Protocol)& protocol,
   const Handle(Standard_Transient)& entity,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  Standard_Integer nument = model->Number(entity);
  if (nument <= 0 || nument > model->NbEntities()) return;
  Standard_Boolean iserr = model->IsRedefinedContent(nument);
  Handle(Standard_Transient) ent, con;  ent = entity;
  S<<" --- (STEP) Entity ";  model->Print(entity,S);
  if (iserr) con = model->ReportEntity(nument)->Content();
  if (entity.IsNull()) {  S<<" Null"<<endl; return;  }

//  On attaque le dump : d abord cas de l Erreur
  S << " Type cdl : " << entity->DynamicType()->Name() << endl;
  if (iserr)
    S<<" ***  NOT WELL LOADED : CONTENT FROM FILE  ***"<<endl;
  else if (model->IsUnknownEntity(nument))
    S<<" ***  UNKNOWN TYPE  ***"<<endl;

  StepData_StepDumper dump(GetCasted(StepData_StepModel,model),
                           GetCasted(StepData_Protocol,protocol),thelabmode);
  dump.Dump(S,ent,level);
}
