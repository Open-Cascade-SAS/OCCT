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

#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESSolid_SolidOfRevolution.hxx>
#include <IGESSolid_ToolSolidOfRevolution.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESSolid_ToolSolidOfRevolution::IGESSolid_ToolSolidOfRevolution ()    {  }


void  IGESSolid_ToolSolidOfRevolution::ReadOwnParams
  (const Handle(IGESSolid_SolidOfRevolution)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  Handle(IGESData_IGESEntity) tempEntity;
  gp_XYZ tempAxisPoint;
  gp_XYZ tempAxis;
  Standard_Real tempFraction;
  Standard_Real tempreal;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  PR.ReadEntity(IR, PR.Current(), "Curve Entity", tempEntity); //szv#4:S4163:12Mar99 `st=` not needed

  if (PR.DefinedElseSkip())
    PR.ReadReal(PR.Current(), "Fraction of rotation", tempFraction); //szv#4:S4163:12Mar99 `st=` not needed
  else
    tempFraction = 1.0;
  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Axis Point (X)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Axis Point (X)", tempreal))
	tempAxisPoint.SetX(tempreal);
    }
  else  tempAxisPoint.SetX(0.0);

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Axis Point (Y)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Axis Point (Y)", tempreal))
	tempAxisPoint.SetY(tempreal);
    }
  else  tempAxisPoint.SetY(0.0);

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Axis Point (Z)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Axis Point (Z)", tempreal))
	tempAxisPoint.SetZ(tempreal);
    }
  else  tempAxisPoint.SetZ(0.0);

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Axis direction (I)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Axis direction (I)", tempreal))
	tempAxis.SetX(tempreal);
    }
  else  tempAxis.SetX(0.0);

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Axis direction (J)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Axis direction (J)", tempreal))
	tempAxis.SetY(tempreal);
    }
  else  tempAxis.SetY(0.0);

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Axis direction (K)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Axis direction (K)", tempreal))
	tempAxis.SetZ(tempreal);
    }
  else  tempAxis.SetZ(1.0);

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init (tempEntity, tempFraction, tempAxisPoint, tempAxis);
  Standard_Real eps = 1.E-05;
  if (!tempAxis.IsEqual(ent->Axis().XYZ(),eps))
    PR.AddWarning("Axis poorly unitary, normalized");
}

void IGESSolid_ToolSolidOfRevolution::WriteOwnParams
  (const Handle(IGESSolid_SolidOfRevolution)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->Curve());
  IW.Send(ent->Fraction());
  IW.Send(ent->AxisPoint().X());
  IW.Send(ent->AxisPoint().Y());
  IW.Send(ent->AxisPoint().Z());
  IW.Send(ent->Axis().X());
  IW.Send(ent->Axis().Y());
  IW.Send(ent->Axis().Z());
}

void  IGESSolid_ToolSolidOfRevolution::OwnShared
  (const Handle(IGESSolid_SolidOfRevolution)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->Curve());
}

void  IGESSolid_ToolSolidOfRevolution::OwnCopy
  (const Handle(IGESSolid_SolidOfRevolution)& another,
   const Handle(IGESSolid_SolidOfRevolution)& ent, Interface_CopyTool& TC) const
{
  DeclareAndCast(IGESData_IGESEntity, tempEntity,
		 TC.Transferred(another->Curve()));
  Standard_Real tempFraction = another->Fraction();
  gp_XYZ tempAxisPoint = another->AxisPoint().XYZ();
  gp_XYZ tempAxis= another->Axis().XYZ();
  ent->Init(tempEntity, tempFraction, tempAxisPoint, tempAxis);
}

IGESData_DirChecker  IGESSolid_ToolSolidOfRevolution::DirChecker
  (const Handle(IGESSolid_SolidOfRevolution)& /* ent */ ) const
{
  IGESData_DirChecker DC(162, 0, 1);

  DC.Structure  (IGESData_DefVoid);
  DC.LineFont   (IGESData_DefAny);
  DC.Color      (IGESData_DefAny);

  DC.UseFlagRequired (0);
  DC.HierarchyStatusIgnored ();
  return DC;
}

void  IGESSolid_ToolSolidOfRevolution::OwnCheck
  (const Handle(IGESSolid_SolidOfRevolution)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  if (ent->Fraction() <= 0 || ent->Fraction() > 1.0)
    ach->AddFail("Fraction of rotation : Incorrect value");
}

void  IGESSolid_ToolSolidOfRevolution::OwnDump
  (const Handle(IGESSolid_SolidOfRevolution)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESSolid_SolidOfRevolution" << endl;

  S << "Curve entity   :";
  dumper.Dump(ent->Curve(),S, (level <= 4) ? 0 : 1);
  S << endl;
  S << "Fraction of rotation : " << ent->Fraction() << endl;
  S << "Axis Point     : ";
  IGESData_DumpXYZL(S,level, ent->AxisPoint(), ent->Location());
  S << endl << "Axis direction : ";
  IGESData_DumpXYZL(S,level, ent->Axis(), ent->VectorLocation());
  S << endl;
}
