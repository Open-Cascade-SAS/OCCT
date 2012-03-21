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

#include <IGESSolid_ToolTorus.ixx>
#include <IGESData_ParamCursor.hxx>
#include <gp_XYZ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <IGESData_Dump.hxx>


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
