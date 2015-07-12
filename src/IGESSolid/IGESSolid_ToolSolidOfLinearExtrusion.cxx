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

#include <gp_XYZ.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESSolid_SolidOfLinearExtrusion.hxx>
#include <IGESSolid_ToolSolidOfLinearExtrusion.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESSolid_ToolSolidOfLinearExtrusion::IGESSolid_ToolSolidOfLinearExtrusion ()
      {  }


void  IGESSolid_ToolSolidOfLinearExtrusion::ReadOwnParams
  (const Handle(IGESSolid_SolidOfLinearExtrusion)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  Handle(IGESData_IGESEntity) tempEntity;
  gp_XYZ tempDirection;
  Standard_Real tempLength;
  Standard_Real tempreal;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  PR.ReadEntity(IR, PR.Current(), "Curve Entity", tempEntity); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadReal(PR.Current(), "Length of extrusion", tempLength); //szv#4:S4163:12Mar99 `st=` not needed

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Extrusion direction (I)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Extrusion direction (I)", tempreal))
	tempDirection.SetX(tempreal);
    }
  else  tempDirection.SetX(0.0);

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Extrusion direction (J)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Extrusion direction (J)", tempreal))
	tempDirection.SetY(tempreal);
    }
  else  tempDirection.SetY(0.0);

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Extrusion direction (K)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Extrusion direction (K)", tempreal))
	tempDirection.SetZ(tempreal);
    }
  else  tempDirection.SetZ(1.0);

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempEntity, tempLength, tempDirection);
  Standard_Real eps = 1.E-05;
  if (!tempDirection.IsEqual(ent->ExtrusionDirection().XYZ(),eps))
    PR.AddWarning("Extrusion Direction poorly unitary, normalized");
}

void  IGESSolid_ToolSolidOfLinearExtrusion::WriteOwnParams
  (const Handle(IGESSolid_SolidOfLinearExtrusion)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->Curve());
  IW.Send(ent->ExtrusionLength());
  IW.Send(ent->ExtrusionDirection().X());
  IW.Send(ent->ExtrusionDirection().Y());
  IW.Send(ent->ExtrusionDirection().Z());
}

void  IGESSolid_ToolSolidOfLinearExtrusion::OwnShared
  (const Handle(IGESSolid_SolidOfLinearExtrusion)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->Curve());
}

void  IGESSolid_ToolSolidOfLinearExtrusion::OwnCopy
  (const Handle(IGESSolid_SolidOfLinearExtrusion)& another,
   const Handle(IGESSolid_SolidOfLinearExtrusion)& ent, Interface_CopyTool& TC) const
{
  DeclareAndCast(IGESData_IGESEntity, tempEntity,
		 TC.Transferred(another->Curve()));
  Standard_Real tempLength = another->ExtrusionLength();
  gp_XYZ tempDirection = another->ExtrusionDirection().XYZ();
  ent->Init(tempEntity, tempLength, tempDirection);
}

IGESData_DirChecker  IGESSolid_ToolSolidOfLinearExtrusion::DirChecker
  (const Handle(IGESSolid_SolidOfLinearExtrusion)& /* ent */ ) const
{
  IGESData_DirChecker DC(164, 0);

  DC.Structure  (IGESData_DefVoid);
  DC.LineFont   (IGESData_DefAny);
  DC.Color      (IGESData_DefAny);

  DC.UseFlagRequired (0);
  DC.HierarchyStatusIgnored ();
  return DC;
}

void  IGESSolid_ToolSolidOfLinearExtrusion::OwnCheck
  (const Handle(IGESSolid_SolidOfLinearExtrusion)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  if (ent->ExtrusionLength() <= 0.0)
    ach->AddFail("Length of extrusion : Not Positive");
}

void  IGESSolid_ToolSolidOfLinearExtrusion::OwnDump
  (const Handle(IGESSolid_SolidOfLinearExtrusion)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESSolid_SolidOfLinearExtrusion" << endl;

  S << "Curve entity        : ";
  dumper.Dump(ent->Curve(),S, (level <= 4) ? 0 : 1);
  S << endl;
  S << "Extrusion length    : " << ent->ExtrusionLength() << endl;
  S << "Extrusion direction : ";
  IGESData_DumpXYZL(S,level, ent->ExtrusionDirection(), ent->VectorLocation());
  S << endl;
}
