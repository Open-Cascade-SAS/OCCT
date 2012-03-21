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

#include <IGESDraw_ToolConnectPoint.ixx>
#include <IGESData_ParamCursor.hxx>
#include <gp_XYZ.hxx>
#include <gp_Pnt.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESGraph_TextDisplayTemplate.hxx>
#include <TCollection_HAsciiString.hxx>
#include <IGESData_Dump.hxx>
#include <Interface_Macros.hxx>


IGESDraw_ToolConnectPoint::IGESDraw_ToolConnectPoint ()    {  }


void IGESDraw_ToolConnectPoint::ReadOwnParams
  (const Handle(IGESDraw_ConnectPoint)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  gp_XYZ tempPoint;
  Standard_Integer tempTypeFlag, tempFunctionFlag;
  Standard_Integer tempPointIdentifier, tempFunctionCode, tempSwapFlag;
  Handle(IGESData_IGESEntity) tempDisplaySymbol, tempOwnerSubfigure;
  Handle(TCollection_HAsciiString) tempFunctionIdentifier, tempFunctionName;
  Handle(IGESGraph_TextDisplayTemplate) tempFunctionTemplate;
  Handle(IGESGraph_TextDisplayTemplate) tempIdentifierTemplate;

  PR.ReadXYZ(PR.CurrentList(1, 3), "Connect Point Coordinate", tempPoint); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity(IR, PR.Current(), "Display Symbol Geometry Entity",
		tempDisplaySymbol,Standard_True); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadInteger(PR.Current(), "Type Flag", tempTypeFlag); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadInteger(PR.Current(), "Function Flag", tempFunctionFlag); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadText(PR.Current(), "Function Identifier",
	      tempFunctionIdentifier); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity(IR, PR.Current(), "Text Display Identifier Template",
		STANDARD_TYPE(IGESGraph_TextDisplayTemplate), tempIdentifierTemplate,
		Standard_True); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadText(PR.Current(), "Connect Point Function Name",
	      tempFunctionName); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity(IR, PR.Current(), "Text Display Function Template",
		STANDARD_TYPE(IGESGraph_TextDisplayTemplate), tempFunctionTemplate,
		Standard_True); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadInteger(PR.Current(), "Unique Connect Point Identifier",
		 tempPointIdentifier); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadInteger(PR.Current(), "Connect Point Function Code",
		 tempFunctionCode); //szv#4:S4163:12Mar99 `st=` not needed

  if (PR.DefinedElseSkip())
    PR.ReadInteger(PR.Current(), "Swap Flag", tempSwapFlag); //szv#4:S4163:12Mar99 `st=` not needed
  else tempSwapFlag = 0;  // default

  PR.ReadEntity(IR, PR.Current(), "Owner Network Subfigure Entity",
		tempOwnerSubfigure,Standard_True); //szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init
    (tempPoint, tempDisplaySymbol, tempTypeFlag, tempFunctionFlag,
     tempFunctionIdentifier, tempIdentifierTemplate, tempFunctionName,
     tempFunctionTemplate, tempPointIdentifier, tempFunctionCode,
     tempSwapFlag, tempOwnerSubfigure);
}

void IGESDraw_ToolConnectPoint::WriteOwnParams
  (const Handle(IGESDraw_ConnectPoint)& ent, IGESData_IGESWriter& IW)  const
{
  IW.Send(ent->Point().X());
  IW.Send(ent->Point().Y());
  IW.Send(ent->Point().Z());
  IW.Send(ent->DisplaySymbol());
  IW.Send(ent->TypeFlag());
  IW.Send(ent->FunctionFlag());
  IW.Send(ent->FunctionIdentifier());
  IW.Send(ent->IdentifierTemplate());
  IW.Send(ent->FunctionName());
  IW.Send(ent->FunctionTemplate());
  IW.Send(ent->PointIdentifier());
  IW.Send(ent->FunctionCode());
  IW.SendBoolean(ent->SwapFlag());
  IW.Send(ent->OwnerSubfigure());
}

void  IGESDraw_ToolConnectPoint::OwnShared
  (const Handle(IGESDraw_ConnectPoint)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->DisplaySymbol());
  iter.GetOneItem(ent->IdentifierTemplate());
  iter.GetOneItem(ent->FunctionTemplate());
  iter.GetOneItem(ent->OwnerSubfigure());
}

void IGESDraw_ToolConnectPoint::OwnCopy
  (const Handle(IGESDraw_ConnectPoint)& another,
   const Handle(IGESDraw_ConnectPoint)& ent, Interface_CopyTool& TC) const
{
  gp_XYZ tempPoint = (another->Point()).XYZ();
  DeclareAndCast(IGESData_IGESEntity, tempDisplaySymbol,
                 TC.Transferred(another->DisplaySymbol()));
  Standard_Integer tempTypeFlag     = another->TypeFlag();
  Standard_Integer tempFunctionFlag = another->FunctionFlag();
  Handle(TCollection_HAsciiString) tempFunctionIdentifier =
    new TCollection_HAsciiString(another->FunctionIdentifier());
  DeclareAndCast(IGESGraph_TextDisplayTemplate, tempIdentifierTemplate,
                 TC.Transferred(another->FunctionTemplate()));
  Handle(TCollection_HAsciiString) tempFunctionName =
    new TCollection_HAsciiString(another->FunctionName());
  DeclareAndCast(IGESGraph_TextDisplayTemplate, tempFunctionTemplate,
                 TC.Transferred(another->FunctionTemplate()));
  Standard_Integer tempPointIdentifier = another->PointIdentifier();
  Standard_Integer tempFunctionCode    = another->FunctionCode();
  Standard_Integer tempSwapFlag        = (another->SwapFlag() ? 1 : 0);
  DeclareAndCast(IGESData_IGESEntity, tempOwnerSubfigure,
                 TC.Transferred(another->OwnerSubfigure()));

  ent->Init(tempPoint, tempDisplaySymbol, tempTypeFlag, tempFunctionFlag,
	    tempFunctionIdentifier, tempIdentifierTemplate, tempFunctionName,
	    tempFunctionTemplate, tempPointIdentifier, tempFunctionCode,
	    tempSwapFlag, tempOwnerSubfigure);
}

IGESData_DirChecker IGESDraw_ToolConnectPoint::DirChecker
  (const Handle(IGESDraw_ConnectPoint)& ent )  const
{
  IGESData_DirChecker DC(132, 0);
  DC.Structure(IGESData_DefVoid);
  DC.Color(IGESData_DefAny);
  DC.UseFlagRequired(4);
  if (!ent->DisplaySymbol().IsNull()) {
    DC.LineFont(IGESData_DefAny);
    DC.LineWeight(IGESData_DefAny);
  }
  else {
    // Note : If ent->DisplaySymbol() is NULL Handle; ignore Line Font, Weight
    //        and Hierarchy Status
    DC.LineFont(IGESData_DefVoid);
    DC.LineWeight(IGESData_DefVoid);
    DC.HierarchyStatusIgnored();
  }

  return DC;
}

void IGESDraw_ToolConnectPoint::OwnCheck
  (const Handle(IGESDraw_ConnectPoint)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach)  const
{
  if ((ent->TypeFlag() <   0) || (ent->TypeFlag() > 9999)  ||
      ((ent->TypeFlag() >   2) && (ent->TypeFlag() <  101)) ||
      ((ent->TypeFlag() > 104) && (ent->TypeFlag() <  201)) ||
      ((ent->TypeFlag() > 203) && (ent->TypeFlag() < 5001)))
    ach->AddFail("TypeFlag has Invalid value");

  if ((ent->FunctionFlag() < 0) || (ent->FunctionFlag() > 2))
    ach->AddFail("FunctionFlag has Invalid value");

  if ((ent->FunctionCode() <  0) || (ent->FunctionCode() > 9999)  ||
      ((ent->FunctionCode() > 49) && (ent->FunctionCode() <   98)) ||
      ((ent->FunctionCode() > 99) && (ent->FunctionCode() < 5001)))
    ach->AddFail("FunctionCode has Invalid value");

  //if ((ent->SwapFlag() < 0) || (ent->SwapFlag() > 1)) //szv#4:S4163:12Mar99 SGI warns
  if ((ent->SwapFlag() != 0) && (ent->SwapFlag() != 1))
    ach->AddFail("SwapFlag has Invalid value");
}

void IGESDraw_ToolConnectPoint::OwnDump
  (const Handle(IGESDraw_ConnectPoint)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level)  const
{
  Standard_Integer tempSubLevel = (level <= 4) ? 0 : 1;

  S << "IGESDraw_ConnectPoint" << endl;

  S << "Connection Point Coordinate : ";
  IGESData_DumpXYZL(S, level, ent->Point(), ent->Location());
  S << "Display Symbol Geometry Entity : ";
  dumper.Dump(ent->DisplaySymbol(),S, tempSubLevel);
  S << endl;
  S << "Type Flag : "     << ent->TypeFlag() << "  "
    << "Function Flag : " << ent->FunctionFlag() << endl;
  S << "Function Identifier : ";
  IGESData_DumpString(S,ent->FunctionIdentifier());
  S << endl << "Text Display Template Entity for CID : ";
  dumper.Dump(ent->IdentifierTemplate(),S, tempSubLevel);
  S << endl << "Function Name : ";
  IGESData_DumpString(S,ent->FunctionName());
  S << endl << "Text Display Template Entity for CFN : ";
  dumper.Dump(ent->FunctionTemplate(),S, tempSubLevel);
  S << endl;
  S << "Point Identifier : " << ent->PointIdentifier() << endl
    << "Function Code : "    << ent->FunctionCode()
    << "Swap Flag : "        << ( ent->SwapFlag() ? "True" : "False" ) << endl;
  S << "Owner Subfigure Entity : ";
  dumper.Dump(ent->OwnerSubfigure(),S, tempSubLevel);
  S << endl;
}
