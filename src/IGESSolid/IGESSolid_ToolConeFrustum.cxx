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
#include <IGESSolid_ConeFrustum.hxx>
#include <IGESSolid_ToolConeFrustum.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESSolid_ToolConeFrustum::IGESSolid_ToolConeFrustum ()    {  }


void  IGESSolid_ToolConeFrustum::ReadOwnParams
  (const Handle(IGESSolid_ConeFrustum)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{
  Standard_Real tempHeight, tempR1, tempR2, tempreal;
  gp_XYZ tempCenter, tempAxis;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  PR.ReadReal(PR.Current(), "Height", tempHeight); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadReal(PR.Current(), "Larger face radius", tempR1); //szv#4:S4163:12Mar99 `st=` not needed

  if (PR.DefinedElseSkip())
    PR.ReadReal(PR.Current(), "Smaller face radius", tempR2); //szv#4:S4163:12Mar99 `st=` not needed
  else
    tempR2 = 0.0;

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Face center (X)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Face center (X)", tempreal))
	tempCenter.SetX(tempreal);
    }
  else  tempCenter.SetX(0.0);

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Face center (Y)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Face center (Y)", tempreal))
	tempCenter.SetY(tempreal);
    }
  else  tempCenter.SetY(0.0);

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Face center (Z)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Face center (Z)", tempreal))
	tempCenter.SetZ(tempreal);
    }
  else  tempCenter.SetZ(0.0);

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
  ent->Init
    (tempHeight, tempR1, tempR2, tempCenter, tempAxis);
  Standard_Real eps = 1.E-05;
  if (!tempAxis.IsEqual(ent->Axis().XYZ(),eps)) PR.AddWarning
    ("Axis poorly unitary, normalized");
}

void  IGESSolid_ToolConeFrustum::WriteOwnParams
  (const Handle(IGESSolid_ConeFrustum)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->Height());
  IW.Send(ent->LargerRadius());
  IW.Send(ent->SmallerRadius());
  IW.Send(ent->FaceCenter().X());
  IW.Send(ent->FaceCenter().Y());
  IW.Send(ent->FaceCenter().Z());
  IW.Send(ent->Axis().X());
  IW.Send(ent->Axis().Y());
  IW.Send(ent->Axis().Z());
}

void  IGESSolid_ToolConeFrustum::OwnShared
  (const Handle(IGESSolid_ConeFrustum)& /* ent */, Interface_EntityIterator& /* iter */) const
{
}

void  IGESSolid_ToolConeFrustum::OwnCopy
  (const Handle(IGESSolid_ConeFrustum)& another,
   const Handle(IGESSolid_ConeFrustum)& ent, Interface_CopyTool& /* TC */) const
{
  ent->Init
    (another->Height(), another->LargerRadius(), another->SmallerRadius(),
     another->FaceCenter().XYZ(), another->Axis().XYZ());
}

IGESData_DirChecker  IGESSolid_ToolConeFrustum::DirChecker
  (const Handle(IGESSolid_ConeFrustum)& /* ent */ ) const
{
  IGESData_DirChecker DC(156, 0);

  DC.Structure  (IGESData_DefVoid);
  DC.LineFont   (IGESData_DefAny);
  DC.Color      (IGESData_DefAny);

  DC.UseFlagRequired (0);
  DC.HierarchyStatusIgnored ();
  return DC;
}

void  IGESSolid_ToolConeFrustum::OwnCheck
  (const Handle(IGESSolid_ConeFrustum)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  if (ent->Height() <= 0.0)
    ach->AddFail("Height : Value Not Positive");
  if (ent->LargerRadius() <= 0.0)
    ach->AddFail("Larger face radius : Value Not Positive");
  if (ent->SmallerRadius() < 0.0)
    ach->AddFail("Smaller face radius : Value Not Positive");
  if (ent->SmallerRadius() > ent->LargerRadius())
    ach->AddFail("Smaller face radius : is greater than Larger face radius");
}

void  IGESSolid_ToolConeFrustum::OwnDump
  (const Handle(IGESSolid_ConeFrustum)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESSolid_ConeFrustum" << endl;

  S << "Height : " << ent->Height() << "  ";
  S << "Larger face Radius : "  << ent->LargerRadius() << "  ";
  S << "Smaller face Radius : " << ent->SmallerRadius() << endl;
  S << "Face Center : ";
  IGESData_DumpXYZL(S,level, ent->FaceCenter(), ent->Location());
  S << endl << "Axis : ";
  IGESData_DumpXYZL(S,level, ent->Axis(), ent->VectorLocation());
  S << endl;
}
