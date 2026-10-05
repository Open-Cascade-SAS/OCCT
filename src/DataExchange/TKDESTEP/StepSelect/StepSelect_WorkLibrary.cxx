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
#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ReportEntity.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <OSD_FileSystem.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <StepData_Protocol.hxx>
#include <StepData_StepDumper.hxx>
#include <StepData_StepModel.hxx>
#include <StepData_StepWriter.hxx>
#include <StepFile_Read.hxx>
#include <StepSelect_FileModifier.hxx>
#include <StepSelect_WorkLibrary.hxx>

#include <cerrno>
IMPLEMENT_STANDARD_RTTIEXT(StepSelect_WorkLibrary, IFSelect_WorkLibrary)

StepSelect_WorkLibrary::StepSelect_WorkLibrary(const bool copymode)
    : thecopymode(copymode),
      thelabmode(0)
{
  SetDumpLevels(1, 2);
  SetDumpHelp(0, "#id + Step Type");
  SetDumpHelp(1, "Entity as in file");
  SetDumpHelp(2, "Entity + shareds (level 1) as in file");
}

// rq : les init sont faits par ailleurs, pas de souci a se faire

void StepSelect_WorkLibrary::SetDumpLabel(const int mode)
{
  thelabmode = mode;
}

int StepSelect_WorkLibrary::ReadFile(const char* const                      name,
                                     occ::handle<Interface_InterfaceModel>& model,
                                     const occ::handle<Interface_Protocol>& protocol) const
{
  DeclareAndCast(StepData_Protocol, stepro, protocol);
  if (stepro.IsNull())
  {
    return 1;
  }
  int aStatus = StepFile_Read(name, nullptr, occ::down_cast<StepData_StepModel>(model), stepro);
  return aStatus;
}

int StepSelect_WorkLibrary::ReadStream(const char* const                      theName,
                                       std::istream&                          theIStream,
                                       occ::handle<Interface_InterfaceModel>& model,
                                       const occ::handle<Interface_Protocol>& protocol) const
{
  DeclareAndCast(StepData_Protocol, stepro, protocol);
  if (stepro.IsNull())
  {
    return 1;
  }
  int aStatus =
    StepFile_Read(theName, &theIStream, occ::down_cast<StepData_StepModel>(model), stepro);
  return aStatus;
}

//==================================================================================================

bool StepSelect_WorkLibrary::WriteFile(IFSelect_ContextWrite& theContext) const
{
  if (occ::down_cast<StepData_StepModel>(theContext.Model()).IsNull()
      || occ::down_cast<StepData_Protocol>(theContext.Protocol()).IsNull())
  {
    return false;
  }

  Message_Messenger::StreamBuffer    aLog        = Message::SendInfo();
  const occ::handle<OSD_FileSystem>& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::ostream>      aStream =
    aFileSystem->OpenOStream(theContext.FileName(),
                             std::ios::out | std::ios::binary | std::ios::trunc);

  if (aStream.get() == nullptr)
  {
    theContext.CCheck(0)->AddFail("Step File could not be created");
    aLog << " Step File could not be created : " << theContext.FileName() << '\n';
    return false;
  }
  const bool isWritten = WriteStream(theContext, *aStream);

  errno = 0;
  aStream->flush();
  const bool isFlushed = aStream->good() && errno == 0;
  aStream.reset();
  if (errno)
  {
    aLog << strerror(errno) << '\n';
  }
  return isWritten && isFlushed;
}

//==================================================================================================

bool StepSelect_WorkLibrary::WriteStream(IFSelect_ContextWrite& theContext,
                                         Standard_OStream&      theOStream) const
{
  Message_Messenger::StreamBuffer       aLog = Message::SendInfo();
  const occ::handle<StepData_StepModel> aModel =
    occ::down_cast<StepData_StepModel>(theContext.Model());
  const occ::handle<StepData_Protocol> aProtocol =
    occ::down_cast<StepData_Protocol>(theContext.Protocol());
  if (aModel.IsNull() || aProtocol.IsNull())
  {
    return false;
  }

  aLog << " Step File Name : " << theContext.FileName();
  StepData_StepWriter aWriter(aModel);
  aLog << "(" << aModel->NbEntities() << " ents) ";

  const int aNbModifiers = theContext.NbModifiers();
  for (int aModifierIndex = 1; aModifierIndex <= aNbModifiers; ++aModifierIndex)
  {
    theContext.SetModifier(aModifierIndex);
    const occ::handle<IFSelect_GeneralModifier> aModifier = theContext.FileModifier();
    if (aModifier.IsNull())
    {
      continue;
    }
    const occ::handle<StepSelect_FileModifier> aFileModifier =
      occ::down_cast<StepSelect_FileModifier>(aModifier);
    if (aFileModifier.IsNull())
    {
      theContext.CCheck(0)->AddFail("STEP file modifier has an incompatible type");
      return false;
    }
    aFileModifier->Perform(theContext, aWriter);
    aLog << " .. FileMod." << aModifierIndex << " " << aFileModifier->Label();
    if (theContext.IsForAll())
    {
      aLog << " (all model)";
    }
    else
    {
      aLog << " (" << theContext.NbEntities() << " entities)";
    }
  }

  aWriter.SendModel(aProtocol);
  Interface_CheckIterator aChecks = aWriter.CheckList();
  for (aChecks.Start(); aChecks.More(); aChecks.Next())
  {
    theContext.CCheck(aChecks.Number())->GetMessages(aChecks.Value());
  }
  aLog << " Write ";
  const bool isPrinted = aWriter.Print(theOStream);
  aLog << " Done" << '\n';
  return isPrinted && theOStream.good();
}

bool StepSelect_WorkLibrary::CopyModel(const occ::handle<Interface_InterfaceModel>& original,
                                       const occ::handle<Interface_InterfaceModel>& newmodel,
                                       const Interface_EntityIterator&              list,
                                       Interface_CopyTool&                          TC) const
{
  if (thecopymode)
  {
    return IFSelect_WorkLibrary::CopyModel(original, newmodel, list, TC);
  }
  return thecopymode;
}

void StepSelect_WorkLibrary::DumpEntity(const occ::handle<Interface_InterfaceModel>& model,
                                        const occ::handle<Interface_Protocol>&       protocol,
                                        const occ::handle<Standard_Transient>&       entity,
                                        Standard_OStream&                            S,
                                        const int                                    level) const
{
  int nument = model->Number(entity);
  if (nument <= 0 || nument > model->NbEntities())
  {
    return;
  }
  bool                            iserr = model->IsRedefinedContent(nument);
  occ::handle<Standard_Transient> ent, con;
  ent = entity;
  S << " --- (STEP) Entity ";
  model->Print(entity, S);
  if (iserr)
  {
    con = model->ReportEntity(nument)->Content();
  }
  if (entity.IsNull())
  {
    S << " Null" << '\n';
    return;
  }

  //  On attaque le dump : d abord cas de l Erreur
  S << " Type cdl : " << entity->DynamicType()->Name() << '\n';
  if (iserr)
  {
    S << " ***  NOT WELL LOADED : CONTENT FROM FILE  ***" << '\n';
  }
  else if (model->IsUnknownEntity(nument))
  {
    S << " ***  UNKNOWN TYPE  ***" << '\n';
  }

  StepData_StepDumper dump(GetCasted(StepData_StepModel, model),
                           GetCasted(StepData_Protocol, protocol),
                           thelabmode);
  dump.Dump(S, ent, level);
}
