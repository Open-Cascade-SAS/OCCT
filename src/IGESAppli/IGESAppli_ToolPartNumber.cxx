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

#include <IGESAppli_PartNumber.hxx>
#include <IGESAppli_ToolPartNumber.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TCollection_HAsciiString.hxx>

IGESAppli_ToolPartNumber::IGESAppli_ToolPartNumber ()    {  }


void  IGESAppli_ToolPartNumber::ReadOwnParams
  (const Handle(IGESAppli_PartNumber)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */,  IGESData_ParamReader& PR) const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Integer tempNbPropertyValues;
  Handle(TCollection_HAsciiString) tempGenericNumber;
  Handle(TCollection_HAsciiString) tempMilitaryNumber;
  Handle(TCollection_HAsciiString) tempVendorNumber;
  Handle(TCollection_HAsciiString) tempInternalNumber;

  //szv#4:S4163:12Mar99 `st=` not needed
  if (PR.DefinedElseSkip())
    PR.ReadInteger(PR.Current(), "Number of property values", tempNbPropertyValues);
  else
    tempNbPropertyValues = 4;

  PR.ReadText(PR.Current(), "Generic Number or Name", tempGenericNumber);
  PR.ReadText(PR.Current(), "Military Number or Name", tempMilitaryNumber);
  PR.ReadText(PR.Current(), "Vendor Number or Name", tempVendorNumber);
  PR.ReadText(PR.Current(), "Internal Number or Name", tempInternalNumber);
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempNbPropertyValues, tempGenericNumber,
	    tempMilitaryNumber, tempVendorNumber, tempInternalNumber);
}

void  IGESAppli_ToolPartNumber::WriteOwnParams
  (const Handle(IGESAppli_PartNumber)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->NbPropertyValues());
  IW.Send(ent->GenericNumber());
  IW.Send(ent->MilitaryNumber());
  IW.Send(ent->VendorNumber());
  IW.Send(ent->InternalNumber());
}

void  IGESAppli_ToolPartNumber::OwnShared
  (const Handle(IGESAppli_PartNumber)& /* ent */, Interface_EntityIterator& /* iter */) const
{
}

void  IGESAppli_ToolPartNumber::OwnCopy
  (const Handle(IGESAppli_PartNumber)& another,
   const Handle(IGESAppli_PartNumber)& ent, Interface_CopyTool& /* TC */) const
{
  Standard_Integer tempNbPropertyValues = another->NbPropertyValues();
  Handle(TCollection_HAsciiString) tempGenericNumber  =
    new TCollection_HAsciiString(another->GenericNumber());
  Handle(TCollection_HAsciiString) tempMilitaryNumber =
    new TCollection_HAsciiString(another->MilitaryNumber());
  Handle(TCollection_HAsciiString) tempVendorNumber   =
    new TCollection_HAsciiString(another->VendorNumber());
  Handle(TCollection_HAsciiString) tempInternalNumber =
    new TCollection_HAsciiString(another->InternalNumber());
  ent->Init(tempNbPropertyValues, tempGenericNumber, tempMilitaryNumber,
	    tempVendorNumber,     tempInternalNumber);
}

Standard_Boolean  IGESAppli_ToolPartNumber::OwnCorrect
  (const Handle(IGESAppli_PartNumber)& ent) const
{
  Standard_Boolean res = (ent->NbPropertyValues() != 4);
  if (res) ent->Init
    (4,ent->GenericNumber(),ent->MilitaryNumber(),ent->VendorNumber(),
     ent->InternalNumber());    // nbpropertyvalues= 4
  return res;
}

IGESData_DirChecker  IGESAppli_ToolPartNumber::DirChecker
  (const Handle(IGESAppli_PartNumber)& /* ent */ ) const
{
  IGESData_DirChecker DC(406, 9);
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void  IGESAppli_ToolPartNumber::OwnCheck
  (const Handle(IGESAppli_PartNumber)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  if (ent->NbPropertyValues() != 4)
    ach->AddFail("Number of property values != 4");
}

void  IGESAppli_ToolPartNumber::OwnDump
  (const Handle(IGESAppli_PartNumber)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer /* level */) const
{
  //Standard_Integer sublevel = (level > 4) ? 1 : 0; //szv#4:S4163:12Mar99 unused
  S << "IGESAppli_PartNumber" << endl;
  S << "Number of property values : " << ent->NbPropertyValues() << endl;
  S << "Generic  Number or Name : ";
  IGESData_DumpString(S,ent->GenericNumber());
  S << endl;
  S << "Military Number or Name : ";
  IGESData_DumpString(S,ent->MilitaryNumber());
  S << endl;
  S << "Vendor   Number or Name : ";
  IGESData_DumpString(S,ent->VendorNumber());
  S << endl;
  S << "Internal Number or Name : ";
  IGESData_DumpString(S,ent->InternalNumber());
  S << endl;
}
