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

#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESSolid_Sphere.hxx>
#include <IGESSolid_ToolSphere.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESSolid_ToolSphere::IGESSolid_ToolSphere ()    {  }


void  IGESSolid_ToolSphere::ReadOwnParams
  (const Handle(IGESSolid_Sphere)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{
  Standard_Real tempRadius, tempreal;
  gp_XYZ tempCenter;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  PR.ReadReal(PR.Current(), "Radius", tempRadius); //szv#4:S4163:12Mar99 `st=` not needed

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Center (X)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Center (X)", tempreal))
	tempCenter.SetX(tempreal);
    }
  else  tempCenter.SetX(0.0);

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Center (Y)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Center (Y)", tempreal))
	tempCenter.SetY(tempreal);
    }
  else  tempCenter.SetY(0.0);

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Center (Z)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Center (Z)", tempreal))
	tempCenter.SetZ(tempreal);
    }
  else  tempCenter.SetZ(0.0);

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init (tempRadius, tempCenter);
}

void  IGESSolid_ToolSphere::WriteOwnParams
  (const Handle(IGESSolid_Sphere)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->Radius());
  IW.Send(ent->Center().X());
  IW.Send(ent->Center().Y());
  IW.Send(ent->Center().Z());
}

void  IGESSolid_ToolSphere::OwnShared
  (const Handle(IGESSolid_Sphere)& /* ent */, Interface_EntityIterator& /* iter */) const
{
}

void  IGESSolid_ToolSphere::OwnCopy
  (const Handle(IGESSolid_Sphere)& another,
   const Handle(IGESSolid_Sphere)& ent, Interface_CopyTool& /* TC */) const
{
  ent->Init (another->Radius(), another->Center().XYZ());
}

IGESData_DirChecker  IGESSolid_ToolSphere::DirChecker
  (const Handle(IGESSolid_Sphere)& /* ent */ ) const
{
  IGESData_DirChecker DC(158, 0);

  DC.Structure  (IGESData_DefVoid);
  DC.LineFont   (IGESData_DefAny);
  DC.Color      (IGESData_DefAny);

  DC.UseFlagRequired (0);
  DC.HierarchyStatusIgnored ();
  return DC;
}

void  IGESSolid_ToolSphere::OwnCheck
  (const Handle(IGESSolid_Sphere)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  if (ent->Radius() <= 0.0)
    ach->AddFail("Radius : Not Positive");
}

void  IGESSolid_ToolSphere::OwnDump
  (const Handle(IGESSolid_Sphere)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESSolid_Sphere" << endl;

  S << "Radius : " << ent->Radius() << endl;
  S << "Center : ";
  IGESData_DumpXYZL(S,level, ent->Center(), ent->Location());
  S << endl;
}
