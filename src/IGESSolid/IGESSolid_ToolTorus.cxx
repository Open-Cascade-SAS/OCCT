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
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESSolid_ToolTorus.hxx>
#include <IGESSolid_Torus.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESSolid_ToolTorus::IGESSolid_ToolTorus ()    {  }


void  IGESSolid_ToolTorus::ReadOwnParams
  (const Handle(IGESSolid_Torus)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{
  Standard_Real r1, r2;
  Standard_Real tempreal;
  gp_XYZ tempPoint, tempAxis;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  PR.ReadReal(PR.Current(), "Radius of revolution", r1); //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadReal(PR.Current(), "Radius of disc", r2); //szv#4:S4163:12Mar99 `st=` not needed

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Center Point (X)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Center Point (X)", tempreal))
	tempPoint.SetX(tempreal);
    }
  else  tempPoint.SetX(0.0);

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Center Point (Y)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Center Point (Y)", tempreal))
	tempPoint.SetY(tempreal);
    }
  else  tempPoint.SetY(0.0);

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Center Point (Z)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Center Point (Z)", tempreal))
	tempPoint.SetZ(tempreal);
    }
  else  tempPoint.SetZ(0.0);

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
  ent->Init (r1, r2, tempPoint, tempAxis);
  Standard_Real eps = 1.E-05;
  if (!tempAxis.IsEqual(ent->Axis().XYZ(),eps))
    PR.AddWarning("Axis poorly unitary, normalized");
}

void  IGESSolid_ToolTorus::WriteOwnParams
  (const Handle(IGESSolid_Torus)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->MajorRadius());
  IW.Send(ent->DiscRadius());
  IW.Send(ent->AxisPoint().X());
  IW.Send(ent->AxisPoint().Y());
  IW.Send(ent->AxisPoint().Z());
  IW.Send(ent->Axis().X());
  IW.Send(ent->Axis().Y());
  IW.Send(ent->Axis().Z());
}

void  IGESSolid_ToolTorus::OwnShared
  (const Handle(IGESSolid_Torus)& /* ent */, Interface_EntityIterator& /* iter */) const
{
}

void  IGESSolid_ToolTorus::OwnCopy
  (const Handle(IGESSolid_Torus)& another,
   const Handle(IGESSolid_Torus)& ent, Interface_CopyTool& /* TC */) const
{
  ent->Init (another->MajorRadius(), another->DiscRadius(),
	     another->AxisPoint().XYZ(), another->Axis().XYZ());
}

IGESData_DirChecker  IGESSolid_ToolTorus::DirChecker
  (const Handle(IGESSolid_Torus)& /* ent */ ) const
{
  IGESData_DirChecker DC(160, 0);

  DC.Structure  (IGESData_DefVoid);
  DC.LineFont   (IGESData_DefAny);
  DC.Color      (IGESData_DefAny);

  DC.UseFlagRequired (0);
  DC.HierarchyStatusIgnored ();
  return DC;
}

void  IGESSolid_ToolTorus::OwnCheck
  (const Handle(IGESSolid_Torus)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  if (ent->MajorRadius() <= 0.0)
    ach->AddFail("Radius of revolution : Not Positive");
  if (ent->DiscRadius() <= 0.0)
    ach->AddFail("Radius of disc : Not Positive");
  if (ent->DiscRadius() >= ent->MajorRadius())
    ach->AddFail("Radius of disc : is not Less than Radius of revolution");
}

void  IGESSolid_ToolTorus::OwnDump
  (const Handle(IGESSolid_Torus)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESSolid_Torus" << endl;

  S << "Radius of revolution : " << ent->MajorRadius() << "  ";
  S << "Radius of the disc   : " << ent->DiscRadius()  << endl;
  S << "Center Point   : ";
  IGESData_DumpXYZL(S,level, ent->AxisPoint(), ent->Location());
  S << endl << "Axis direction : ";
  IGESData_DumpXYZL(S,level, ent->Axis(), ent->VectorLocation());
  S << endl;
}
