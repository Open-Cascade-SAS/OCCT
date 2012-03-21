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

#include <IGESSolid_ToolConeFrustum.ixx>
#include <IGESData_ParamCursor.hxx>
#include <gp_XYZ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <IGESData_Dump.hxx>


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
