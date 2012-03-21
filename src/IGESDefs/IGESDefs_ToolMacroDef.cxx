// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESDefs_ToolMacroDef.ixx>
#include <IGESData_ParamCursor.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <IGESData_Dump.hxx>
#include <Interface_Macros.hxx>


IGESDefs_ToolMacroDef::IGESDefs_ToolMacroDef ()    {  }


void  IGESDefs_ToolMacroDef::ReadOwnParams
  (const Handle(IGESDefs_MacroDef)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{ 
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  Handle(TCollection_HAsciiString) macro;
  Standard_Integer entityTypeID;
  Handle(Interface_HArray1OfHAsciiString) langStatements;
  Handle(TCollection_HAsciiString) endMacro;

  PR.ReadText(PR.Current(), "MACRO", macro); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadInteger(PR.Current(), "Entity Type ID", entityTypeID); //szv#4:S4163:12Mar99 `st=` not needed

  Standard_Integer tempCurrent = PR.CurrentNumber();
  // Counting the no. of language statements.
  Standard_Integer nbval; // svv Jan 10 2000 : porting on DEC
  for (nbval = 0; PR.CurrentNumber() != PR.NbParams();
       nbval++, PR.SetCurrentNumber(PR.CurrentNumber() + 1));

  PR.SetCurrentNumber(tempCurrent);
  if (nbval > 0) langStatements =
    new Interface_HArray1OfHAsciiString(1, nbval);
  else  PR.AddFail("Number of Lang. Stats. : Not Positive");

  if (! langStatements.IsNull())
    {
      for (Standard_Integer i = 1; i <= nbval; i++)
	{
          Handle(TCollection_HAsciiString) langStat;
          //st = PR.ReadText(PR.Current(), "Language Statement", langStat); //szv#4:S4163:12Mar99 moved in if
	  if (PR.ReadText(PR.Current(), "Language Statement", langStat))
	    langStatements->SetValue(i, langStat);
	}
    }

  PR.ReadText(PR.Current(), "END MACRO", endMacro); //szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init (macro, entityTypeID, langStatements, endMacro);
}

void  IGESDefs_ToolMacroDef::WriteOwnParams
  (const Handle(IGESDefs_MacroDef)& ent, IGESData_IGESWriter& IW) const 
{ 
  IW.Send(ent->MACRO());
  IW.Send(ent->EntityTypeID());
  Standard_Integer upper = ent->NbStatements();
  for (Standard_Integer i = 1; i <= upper; i++)
    IW.Send(ent->LanguageStatement(i));
  IW.Send(ent->ENDMACRO());
}

void  IGESDefs_ToolMacroDef::OwnShared
  (const Handle(IGESDefs_MacroDef)& /* ent */, Interface_EntityIterator& /* iter */) const
{
}

void  IGESDefs_ToolMacroDef::OwnCopy
  (const Handle(IGESDefs_MacroDef)& another,
   const Handle(IGESDefs_MacroDef)& ent, Interface_CopyTool& /* TC */) const
{ 

  Handle(TCollection_HAsciiString) macro =
    new TCollection_HAsciiString(another->MACRO());
  Standard_Integer entityTypeID = another->EntityTypeID();
  Handle(TCollection_HAsciiString) endMacro =
    new TCollection_HAsciiString(another->ENDMACRO());
  Handle(Interface_HArray1OfHAsciiString) langStatements;
  Standard_Integer nbval = another->NbStatements();
  langStatements = new Interface_HArray1OfHAsciiString(1, nbval);

  for (Standard_Integer i = 1; i <= nbval; i++)
    {
      Handle(TCollection_HAsciiString) langStat =
	new TCollection_HAsciiString(another->LanguageStatement(i));
      langStatements->SetValue(i, langStat);
    }
  ent->Init(macro, entityTypeID, langStatements, endMacro);
}

IGESData_DirChecker  IGESDefs_ToolMacroDef::DirChecker
  (const Handle(IGESDefs_MacroDef)& /* ent */ ) const 
{ 
  IGESData_DirChecker DC (306, 0);
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

void  IGESDefs_ToolMacroDef::OwnCheck
  (const Handle(IGESDefs_MacroDef)& /* ent */,
   const Interface_ShareTool& , Handle(Interface_Check)& /* ach */) const 
{
}

void  IGESDefs_ToolMacroDef::OwnDump
  (const Handle(IGESDefs_MacroDef)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const 
{ 
  S << "IGESDefs_MacroDef" << endl;
  S << "MACRO : ";
  IGESData_DumpString(S,ent->MACRO());
  S << endl;
  S << "Entity Type ID : " << ent->EntityTypeID() << endl;
  S << "Language Statement : ";
  IGESData_DumpStrings(S ,level,1, ent->NbStatements(),ent->LanguageStatement);
  S << "END MACRO : ";
  IGESData_DumpString(S,ent->ENDMACRO());
  S << endl;
}
