// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDefs_MacroDef.hxx>
#include <IGESDefs_ToolMacroDef.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>

IGESDefs_ToolMacroDef::IGESDefs_ToolMacroDef() {}

void IGESDefs_ToolMacroDef::ReadOwnParams(const occ::handle<IGESDefs_MacroDef>& ent,
                                          const occ::handle<IGESData_IGESReaderData>& /* IR */,
                                          IGESData_ParamReader& PR) const
{
  // bool st; //szv#4:S4163:12Mar99 not needed

  occ::handle<TCollection_HAsciiString>                                   macro;
  int                                                                     entityTypeID;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> langStatements;
  occ::handle<TCollection_HAsciiString>                                   endMacro;

  PR.ReadText(PR.Current(), "MACRO", macro); // szv#4:S4163:12Mar99 `st=` not needed

  // clang-format off
  PR.ReadInteger(PR.Current(), "Entity Type ID", entityTypeID); //szv#4:S4163:12Mar99 `st=` not needed
  // clang-format on

  int tempCurrent = PR.CurrentNumber();
  // Counting the no. of language statements.
  int nbval; // svv Jan 10 2000 : porting on DEC
  for (nbval = 0; PR.CurrentNumber() != PR.NbParams();
       nbval++, PR.SetCurrentNumber(PR.CurrentNumber() + 1))
    ;

  PR.SetCurrentNumber(tempCurrent);
  if (nbval > 0)
    langStatements = new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, nbval);
  else
    PR.AddFail("Number of Lang. Stats. : Not Positive");

  if (!langStatements.IsNull())
  {
    for (int i = 1; i <= nbval; i++)
    {
      occ::handle<TCollection_HAsciiString> langStat;
      // st = PR.ReadText(PR.Current(), "Language Statement", langStat); //szv#4:S4163:12Mar99 moved
      // in if
      if (PR.ReadText(PR.Current(), "Language Statement", langStat))
        langStatements->SetValue(i, langStat);
    }
  }

  PR.ReadText(PR.Current(), "END MACRO", endMacro); // szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(macro, entityTypeID, langStatements, endMacro);
}

void IGESDefs_ToolMacroDef::WriteOwnParams(const occ::handle<IGESDefs_MacroDef>& ent,
                                           IGESData_IGESWriter&                  IW) const
{
  IW.Send(ent->MACRO());
  IW.Send(ent->EntityTypeID());
  int upper = ent->NbStatements();
  for (int i = 1; i <= upper; i++)
    IW.Send(ent->LanguageStatement(i));
  IW.Send(ent->ENDMACRO());
}

void IGESDefs_ToolMacroDef::OwnShared(const occ::handle<IGESDefs_MacroDef>& /* ent */,
                                      Interface_EntityIterator& /* iter */) const
{
}

void IGESDefs_ToolMacroDef::OwnCopy(const occ::handle<IGESDefs_MacroDef>& another,
                                    const occ::handle<IGESDefs_MacroDef>& ent,
                                    Interface_CopyTool& /* TC */) const
{

  occ::handle<TCollection_HAsciiString> macro = new TCollection_HAsciiString(another->MACRO());
  int                                   entityTypeID = another->EntityTypeID();
  occ::handle<TCollection_HAsciiString> endMacro =
    new TCollection_HAsciiString(another->ENDMACRO());
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> langStatements;
  int nbval      = another->NbStatements();
  langStatements = new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, nbval);

  for (int i = 1; i <= nbval; i++)
  {
    occ::handle<TCollection_HAsciiString> langStat =
      new TCollection_HAsciiString(another->LanguageStatement(i));
    langStatements->SetValue(i, langStat);
  }
  ent->Init(macro, entityTypeID, langStatements, endMacro);
}

IGESData_DirChecker IGESDefs_ToolMacroDef::DirChecker(
  const occ::handle<IGESDefs_MacroDef>& /* ent */) const
{
  IGESData_DirChecker DC(306, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusRequired(0);
  DC.UseFlagRequired(2);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESDefs_ToolMacroDef::OwnCheck(const occ::handle<IGESDefs_MacroDef>& /* ent */,
                                     const Interface_ShareTool&,
                                     occ::handle<Interface_Check>& /* ach */) const
{
}

void IGESDefs_ToolMacroDef::OwnDump(const occ::handle<IGESDefs_MacroDef>& ent,
                                    const IGESData_IGESDumper& /* dumper */,
                                    Standard_OStream& S,
                                    const int         level) const
{
  S << "IGESDefs_MacroDef\n"
    << "MACRO : ";
  IGESData_DumpString(S, ent->MACRO());
  S << "\n"
    << "Entity Type ID : " << ent->EntityTypeID() << "\n"
    << "Language Statement : ";
  IGESData_DumpStrings(S, level, 1, ent->NbStatements(), ent->LanguageStatement);
  S << "END MACRO : ";
  IGESData_DumpString(S, ent->ENDMACRO());
  S << std::endl;
}
