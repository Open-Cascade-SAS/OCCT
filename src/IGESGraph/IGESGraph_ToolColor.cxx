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
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESGraph_Color.hxx>
#include <IGESGraph_ToolColor.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TCollection_HAsciiString.hxx>

IGESGraph_ToolColor::IGESGraph_ToolColor ()    {  }


void IGESGraph_ToolColor::ReadOwnParams
  (const Handle(IGESGraph_Color)& ent,
   const Handle(IGESData_IGESReaderData)& /*IR*/, IGESData_ParamReader& PR) const
{
  Standard_Real tempRed, tempGreen, tempBlue;
  Handle(TCollection_HAsciiString) tempColorName;

  PR.ReadReal(PR.Current(), "RED as % Of Full Intensity", tempRed);

  PR.ReadReal(PR.Current(), "GREEN as % Of Full Intensity", tempGreen);

  PR.ReadReal(PR.Current(), "BLUE as % Of Full Intensity", tempBlue);

  if ((PR.CurrentNumber() <= PR.NbParams()) &&
      (PR.ParamType(PR.CurrentNumber()) == Interface_ParamText))
    PR.ReadText(PR.Current(), "Color Name", tempColorName);

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempRed, tempGreen, tempBlue, tempColorName);
}

void IGESGraph_ToolColor::WriteOwnParams
  (const Handle(IGESGraph_Color)& ent, IGESData_IGESWriter& IW)  const
{
  Standard_Real Red,Green,Blue;
  ent->RGBIntensity(Red,Green,Blue);
  IW.Send(Red);
  IW.Send(Green);
  IW.Send(Blue);
//  ATTENTION  place a reserver (Null) silya des pointeurs additionnels
  if (ent->HasColorName())
    IW.Send(ent->ColorName());
  else IW.SendVoid();    // placekeeper to be reserved for additional pointers
}

void  IGESGraph_ToolColor::OwnShared
  (const Handle(IGESGraph_Color)& /*ent*/, Interface_EntityIterator& /*iter*/) const
{
}

void IGESGraph_ToolColor::OwnCopy
  (const Handle(IGESGraph_Color)& another,
   const Handle(IGESGraph_Color)& ent, Interface_CopyTool& /*TC*/) const
{
  Standard_Real tempRed, tempGreen, tempBlue;
  Handle(TCollection_HAsciiString) tempColorName;
  another->RGBIntensity(tempRed, tempGreen, tempBlue);
  if (another->HasColorName())
    tempColorName = new TCollection_HAsciiString(another->ColorName());

  ent->Init(tempRed, tempGreen, tempBlue, tempColorName);
}

IGESData_DirChecker  IGESGraph_ToolColor::DirChecker
  (const Handle(IGESGraph_Color)& /*ent*/ )  const
{
  IGESData_DirChecker DC(314, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefAny);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusRequired(0);
  DC.UseFlagRequired(2);
  DC.HierarchyStatusIgnored();

  return DC;
}

void IGESGraph_ToolColor::OwnCheck
  (const Handle(IGESGraph_Color)& /*ent*/,
   const Interface_ShareTool& , Handle(Interface_Check)& /*ach*/)  const
{
//  if (ent->RankColor() == 0)
//    ach.AddFail("Color Rank is zero");
//  else if (ent->RankColor() < 1 || ent->RankColor() > 8)
//    ach.AddFail("Color Rank not between 1 to 8");
}

void IGESGraph_ToolColor::OwnDump
  (const Handle(IGESGraph_Color)& ent, const IGESData_IGESDumper& /*dumper*/,
   const Handle(Message_Messenger)& S, const Standard_Integer /*level*/)  const
{
  S << "IGESGraph_Color" << endl;

  Standard_Real Red,Green,Blue;
  ent->RGBIntensity(Red,Green,Blue);
  S << "Red   (in % Of Full Intensity) : " << Red   << endl;
  S << "Green (in % Of Full Intensity) : " << Green << endl;
  S << "Blue  (in % Of Full Intensity) : " << Blue  << endl;
  S << "Color Name : ";
  IGESData_DumpString(S,ent->ColorName());
  S << endl;
}
