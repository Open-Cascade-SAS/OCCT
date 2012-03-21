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

#include <IGESAppli_ToolPartNumber.ixx>
#include <IGESData_ParamCursor.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <IGESData_Dump.hxx>


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
