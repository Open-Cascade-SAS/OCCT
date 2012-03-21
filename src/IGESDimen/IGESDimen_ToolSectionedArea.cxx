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

#include <IGESDimen_ToolSectionedArea.ixx>
#include <IGESData_ParamCursor.hxx>
#include <gp_XYZ.hxx>
#include <gp_Pnt.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_Dump.hxx>
#include <Interface_Macros.hxx>


IGESDimen_ToolSectionedArea::IGESDimen_ToolSectionedArea ()    {  }


void IGESDimen_ToolSectionedArea::ReadOwnParams
  (const Handle(IGESDimen_SectionedArea)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  Handle(IGESData_IGESEntity) extCurve;
  Standard_Integer tempPattern, nbislands;
  gp_XYZ passPnt;
  Standard_Real tempDistance, tempAngle;
  Handle(IGESData_HArray1OfIGESEntity) tempIslands;
  Handle(IGESData_IGESEntity) anent;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 moved down

  //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadEntity(IR, PR.Current(), "Exterior curve", extCurve, (ent->FormNumber() == 1));
  PR.ReadInteger(PR.Current(), "Fill pattern", tempPattern);
  PR.ReadXYZ(PR.CurrentList(1, 3), "Passing point", passPnt);
  PR.ReadReal(PR.Current(), "Distance between lines", tempDistance);

  if (PR.DefinedElseSkip())
    PR.ReadReal(PR.Current(), "Angle between line and X axis", tempAngle); //szv#4:S4163:12Mar99 `st=` not needed
  else
    tempAngle = M_PI / 4.0;

  Standard_Boolean st = PR.ReadInteger(PR.Current(), "Number of island curves", nbislands);
  if (st && nbislands > 0)
    PR.ReadEnts (IR,PR.CurrentList(nbislands),"Island curves",tempIslands); //szv#4:S4163:12Mar99 `st=` not needed
/*
    {
      tempIslands = new IGESData_HArray1OfIGESEntity(1, nbislands);
      for (Standard_Integer i=1; i<=nbislands; i++)
	{
          st = PR.ReadEntity(IR, PR.Current(), "Island curves", anent);
	  if (st) tempIslands->SetValue(i, anent);
	}
    }
*/

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init
    (extCurve, tempPattern, passPnt, tempDistance, tempAngle, tempIslands);
}

void IGESDimen_ToolSectionedArea::WriteOwnParams
  (const Handle(IGESDimen_SectionedArea)& ent, IGESData_IGESWriter& IW) const
{
  Standard_Integer i, length = ent->NbIslands();
  IW.Send(ent->ExteriorCurve());
  IW.Send(ent->Pattern());
  IW.Send(ent->PassingPoint().X());
  IW.Send(ent->PassingPoint().Y());
  IW.Send(ent->PassingPoint().Z());
  IW.Send(ent->Distance());
  IW.Send(ent->Angle());
  IW.Send(length);
  for (i = 1; i <= length; i ++)
    IW.Send(ent->IslandCurve(i));
}

void  IGESDimen_ToolSectionedArea::OwnShared
  (const Handle(IGESDimen_SectionedArea)& ent, Interface_EntityIterator& iter) const
{
  Standard_Integer i, length = ent->NbIslands();
  iter.GetOneItem(ent->ExteriorCurve());
  for (i = 1; i <= length; i ++)
    iter.GetOneItem(ent->IslandCurve(i));
}

void IGESDimen_ToolSectionedArea::OwnCopy
  (const Handle(IGESDimen_SectionedArea)& another,
   const Handle(IGESDimen_SectionedArea)& ent, Interface_CopyTool& TC) const
{
  DeclareAndCast(IGESData_IGESEntity, extCurve,
		 TC.Transferred(another->ExteriorCurve()));
  Standard_Integer tempPattern = another->Pattern();
  gp_XYZ passPnt = (another->PassingPoint()).XYZ();
  Standard_Real tempDistance = another->Distance();
  Standard_Real tempAngle = another->Angle();
  Handle(IGESData_HArray1OfIGESEntity) tempIslands;
  Standard_Integer nbislands = another->NbIslands();
  if (nbislands > 0)
    {
      tempIslands = new IGESData_HArray1OfIGESEntity(1, nbislands);
      for (Standard_Integer i=1; i<=nbislands; i++)
	{
          DeclareAndCast(IGESData_IGESEntity, anent,
			 TC.Transferred(another->IslandCurve(i)));
          tempIslands->SetValue(i, anent);
	}
    }
  ent->Init(extCurve, tempPattern, passPnt,
	    tempDistance, tempAngle, tempIslands);
  ent->SetInverted (another->IsInverted());
}

IGESData_DirChecker IGESDimen_ToolSectionedArea::DirChecker
  (const Handle(IGESDimen_SectionedArea)& /* ent */ ) const
{
  IGESData_DirChecker DC(230, 0,1);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);

  DC.UseFlagRequired (1);

  return DC;
}

void IGESDimen_ToolSectionedArea::OwnCheck
  (const Handle(IGESDimen_SectionedArea)& /* ent */,
   const Interface_ShareTool& , Handle(Interface_Check)& /* ach */) const
{
}

void IGESDimen_ToolSectionedArea::OwnDump
  (const Handle(IGESDimen_SectionedArea)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  Standard_Integer sublevel = (level <= 4) ? 0 : 1;

  S << "IGESDimen_SectionedArea" << endl;
  S << (ent->IsInverted() ? "Inverted Cross Hatches" : "Standard Cross Hatches");

  S << " - Exterior curve : ";
  dumper.Dump(ent->ExteriorCurve(),S, sublevel);
  S << endl;
  S << "Fill pattern code : " << ent->Pattern() << endl;
  S << "Passing point : ";
  IGESData_DumpXYZL(S,level, ent->PassingPoint(), ent->Location());  S << endl;
  S << "Distance between lines : " << ent->Distance() << endl;
  S << "Angle between lines and X axis : " << ent->Angle() << endl;
  S << "Island Curve : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbIslands(),ent->IslandCurve);
  S << endl;
}
