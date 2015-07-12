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

#include <gp_Vec.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESGeom_Direction.hxx>
#include <IGESGeom_ToolDirection.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESGeom_ToolDirection::IGESGeom_ToolDirection ()    {  }


void IGESGeom_ToolDirection::ReadOwnParams
  (const Handle(IGESGeom_Direction)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{
  gp_XYZ aDirection;
  gp_XY  tmpXY;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Real tmpReal;

  //st = PR.ReadXY(PR.CurrentList(1, 2), "Direction", tmpXY); //szv#4:S4163:12Mar99 moved in if
  if (PR.ReadXY(PR.CurrentList(1, 2), "Direction", tmpXY)) {
    aDirection.SetX(tmpXY.X());
    aDirection.SetY(tmpXY.Y());
  }

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Direction", tmpReal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Direction", tmpReal))
	aDirection.SetZ(tmpReal);
    }
  else
    {
      aDirection.SetZ(0.0);   // Default value.
    }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(aDirection);
}

void IGESGeom_ToolDirection::WriteOwnParams
  (const Handle(IGESGeom_Direction)& ent, IGESData_IGESWriter& IW)  const
{
  IW.Send(ent->Value().X());
  IW.Send(ent->Value().Y());
  IW.Send(ent->Value().Z());
}

void  IGESGeom_ToolDirection::OwnShared
  (const Handle(IGESGeom_Direction)& /* ent */, Interface_EntityIterator& /* iter */) const
{
}

void IGESGeom_ToolDirection::OwnCopy
  (const Handle(IGESGeom_Direction)& another,
   const Handle(IGESGeom_Direction)& ent, Interface_CopyTool& /* TC */) const
{
  ent->Init(another->Value().XYZ());
}

IGESData_DirChecker IGESGeom_ToolDirection::DirChecker
  (const Handle(IGESGeom_Direction)& /* ent */ )  const
{
  IGESData_DirChecker DC(123, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
//  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);

  DC.BlankStatusIgnored ();
  DC.SubordinateStatusRequired (1);
  DC.UseFlagRequired (2);
  DC.HierarchyStatusIgnored ();
  return DC;
}

void IGESGeom_ToolDirection::OwnCheck
  (const Handle(IGESGeom_Direction)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach)  const
{
  if (ent->Value().XYZ().SquareModulus() <= 0.0)
    ach->AddFail("Direction : The values indicate no direction");
}

void IGESGeom_ToolDirection::OwnDump
  (const Handle(IGESGeom_Direction)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer level)  const
{
  S << "IGESGeom_Direction" << endl << endl;

  S << "Value : ";
  IGESData_DumpXYZL(S,level, ent->Value(), ent->VectorLocation());
  S << endl;
}
