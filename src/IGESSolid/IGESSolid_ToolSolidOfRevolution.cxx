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

#include <IGESSolid_ToolSolidOfRevolution.ixx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_IGESEntity.hxx>
#include <gp_XYZ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <IGESData_Dump.hxx>
#include <Interface_Macros.hxx>


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
