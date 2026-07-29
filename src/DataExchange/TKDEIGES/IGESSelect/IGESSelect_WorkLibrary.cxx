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
#include <IGESAppli.hxx>
#include <IGESAppli_Protocol.hxx>
#include <IGESData_FileProtocol.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESDefs.hxx>
#include <IGESFile_Read.hxx>
#include <IGESSelect_Dumper.hxx>
#include <IGESSelect_FileModifier.hxx>
#include <IGESSelect_WorkLibrary.hxx>
#include <IGESSolid.hxx>
#include <IGESSolid_Protocol.hxx>
#include <Interface_Check.hxx>
#include <Interface_InterfaceModel.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_ReportEntity.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <OSD_FileSystem.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Macro.hxx>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

#include <cerrno>
IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_WorkLibrary, IFSelect_WorkLibrary)

static int                                deja = 0;
static occ::handle<IGESData_FileProtocol> IGESProto;

IGESSelect_WorkLibrary::IGESSelect_WorkLibrary(const bool modefnes)
    : themodefnes(modefnes)
{
  IGESSolid::Init();
  IGESAppli::Init();
  IGESDefs::Init();

  if (!deja)
  {
    occ::handle<IGESSelect_Dumper> sesdump = new IGESSelect_Dumper; // ainsi,cestfait
    deja                                   = 1;
  }
  SetDumpLevels(4, 6);
  SetDumpHelp(0, "Only DNum");
  SetDumpHelp(1, "DNum, IGES Type & Form");
  SetDumpHelp(2, "Main Directory Information");
  SetDumpHelp(3, "Complete Directory Part");
  SetDumpHelp(4, "Directory + Fields (except list contents)");
  SetDumpHelp(5, "Complete (with list contents)");
  SetDumpHelp(6, "Complete + Transformed data");
}

//==================================================================================================

int IGESSelect_WorkLibrary::ReadFile(const char* const                      theName,
                                     occ::handle<Interface_InterfaceModel>& theModel,
                                     const occ::handle<Interface_Protocol>& theProtocol) const
{
  Message_Messenger::StreamBuffer    aLog        = Message::SendInfo();
  const occ::handle<OSD_FileSystem>& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::istream>      aStream =
    aFileSystem->OpenIStream(theName, std::ios::in | std::ios::binary);
  if (aStream.get() == nullptr)
  {
    aLog << "File not found : " << theName << '\n';
    theModel.Nullify();
    return -1;
  }
  return ReadStream(theName, *aStream, theModel, theProtocol);
}

//==================================================================================================

int IGESSelect_WorkLibrary::ReadStream(const char* const                      theName,
                                       std::istream&                          theIStream,
                                       occ::handle<Interface_InterfaceModel>& theModel,
                                       const occ::handle<Interface_Protocol>& theProtocol) const
{
  Message_Messenger::StreamBuffer      aLog      = Message::SendInfo();
  occ::handle<IGESData_IGESModel>      aModel    = new IGESData_IGESModel;
  const occ::handle<IGESData_Protocol> aProtocol = occ::down_cast<IGESData_Protocol>(theProtocol);

  const int aStatus = IGESFile_Read(theName, theIStream, aModel, aProtocol);

  if (aStatus < 0)
  {
    aLog << "Stream read failed : " << theName << '\n';
  }
  else if (aStatus > 0)
  {
    aLog << "Error when reading stream : " << theName << '\n';
  }
  if (aStatus == 0)
  {
    theModel = aModel;
  }
  else
  {
    theModel.Nullify();
  }
  return aStatus;
}

//==================================================================================================

bool IGESSelect_WorkLibrary::WriteFile(IFSelect_ContextWrite& theContext) const
{
  if (occ::down_cast<IGESData_IGESModel>(theContext.Model()).IsNull()
      || occ::down_cast<IGESData_Protocol>(theContext.Protocol()).IsNull())
  {
    return false;
  }

  Message_Messenger::StreamBuffer    aLog        = Message::SendInfo();
  const occ::handle<OSD_FileSystem>& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::ostream>      aStream =
    aFileSystem->OpenOStream(theContext.FileName(), std::ios::out | std::ios::binary);
  if (aStream.get() == nullptr)
  {
    theContext.CCheck(0)->AddFail("IGES File could not be created");
    aLog << " - IGES File could not be created : " << theContext.FileName() << '\n';
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

bool IGESSelect_WorkLibrary::WriteStream(IFSelect_ContextWrite& theContext,
                                         Standard_OStream&      theOStream) const
{
  Message_Messenger::StreamBuffer       aLog = Message::SendInfo();
  const occ::handle<IGESData_IGESModel> aModel =
    occ::down_cast<IGESData_IGESModel>(theContext.Model());
  const occ::handle<IGESData_Protocol> aProtocol =
    occ::down_cast<IGESData_Protocol>(theContext.Protocol());

  if (aModel.IsNull() || aProtocol.IsNull())
  {
    return false;
  }
  aLog << " IGES File Name : " << theContext.FileName();
  IGESData_IGESWriter aWriter(aModel);
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
    const occ::handle<IGESSelect_FileModifier> aFileModifier =
      occ::down_cast<IGESSelect_FileModifier>(aModifier);
    if (aFileModifier.IsNull())
    {
      theContext.CCheck(0)->AddFail("IGES file modifier has an incompatible type");
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
  aLog << " Write ";
  if (themodefnes)
  {
    aWriter.WriteMode() = 10;
  }
  const bool isPrinted = aWriter.Print(theOStream);
  aLog << " Done" << '\n';
  return isPrinted && theOStream.good();
}

occ::handle<IGESData_Protocol> IGESSelect_WorkLibrary::DefineProtocol()
{
  if (!IGESProto.IsNull())
  {
    return IGESProto;
  }
  occ::handle<IGESData_Protocol> IGESProto1 = IGESSolid::Protocol();
  occ::handle<IGESData_Protocol> IGESProto2 = IGESAppli::Protocol();
  //  occ::handle<IGESData_FileProtocol> IGESProto  = new IGESData_FileProtocol;
  IGESProto = new IGESData_FileProtocol;
  IGESProto->Add(IGESProto1);
  IGESProto->Add(IGESProto2);
  return IGESProto;
}

void IGESSelect_WorkLibrary::DumpEntity(const occ::handle<Interface_InterfaceModel>& model,
                                        const occ::handle<Interface_Protocol>&       protocol,
                                        const occ::handle<Standard_Transient>&       entity,
                                        Standard_OStream&                            S,
                                        const int                                    level) const
{
  DeclareAndCast(IGESData_IGESModel, igesmod, model);
  DeclareAndCast(IGESData_Protocol, igespro, protocol);
  DeclareAndCast(IGESData_IGESEntity, igesent, entity);
  if (igesmod.IsNull() || igespro.IsNull() || igesent.IsNull())
  {
    return;
  }
  int num = igesmod->Number(igesent);
  if (num == 0)
  {
    return;
  }

  S << " --- Entity " << num;
  bool                            iserr = model->IsRedefinedContent(num);
  occ::handle<Standard_Transient> con;
  if (iserr)
  {
    con = model->ReportEntity(num)->Content();
  }
  if (entity.IsNull())
  {
    S << " Null" << '\n';
    return;
  }

  //  We start the dump : first the Error case
  if (iserr)
  {
    S << " ERRONEOUS, Content, Type cdl : ";
    if (!con.IsNull())
    {
      S << con->DynamicType()->Name();
    }
    else
    {
      S << "(undefined)" << '\n';
    }
    igesent = GetCasted(IGESData_IGESEntity, con);
    con.Nullify();
    occ::handle<Interface_Check> check = model->ReportEntity(num)->Check();
    Interface_CheckIterator      chlist;
    chlist.Add(check, num);
    chlist.Print(S, igesmod, false);
    if (igesent.IsNull())
    {
      return;
    }
  }
  else
  {
    S << " Type cdl : " << igesent->DynamicType()->Name();
  }

  IGESData_IGESDumper dump(igesmod, igespro);
  try
  {
    OCC_CATCH_SIGNALS
    dump.Dump(igesent, S, level, (level - 1) / 3);
  }
  catch (Standard_Failure const&)
  {
    S << " **  Dump Interrupt **" << '\n';
  }
}
