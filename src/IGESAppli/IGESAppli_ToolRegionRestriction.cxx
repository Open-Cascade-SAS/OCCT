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

#include <IGESAppli_ToolRegionRestriction.ixx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_LevelListEntity.hxx>


IGESAppli_ToolRegionRestriction::IGESAppli_ToolRegionRestriction ()    {  }


void  IGESAppli_ToolRegionRestriction::ReadOwnParams
  (const Handle(IGESAppli_RegionRestriction)& ent,
   const Handle(IGESData_IGESReaderData)& /*IR*/, IGESData_ParamReader& PR) const
{
  Standard_Integer tempNbPropertyValues;
  Standard_Integer tempElectViasRestrict;
  Standard_Integer tempElectCompRestrict;
  Standard_Integer tempElectCktRestrict;

  PR.ReadInteger(PR.Current(),"No. of Property values",tempNbPropertyValues);
  PR.ReadInteger(PR.Current(),"Electrical vias restriction",tempElectViasRestrict);
  PR.ReadInteger(PR.Current(),"Electrical components restriction", tempElectCompRestrict);
  PR.ReadInteger(PR.Current(), " Electrical circuitary restriction",tempElectCktRestrict);

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempNbPropertyValues,tempElectViasRestrict,tempElectCompRestrict, tempElectCktRestrict);
}

void  IGESAppli_ToolRegionRestriction::WriteOwnParams
  (const Handle(IGESAppli_RegionRestriction)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->NbPropertyValues());
  IW.Send(ent->ElectricalViasRestriction());
  IW.Send(ent->ElectricalComponentRestriction());
  IW.Send(ent->ElectricalCktRestriction());
}

void  IGESAppli_ToolRegionRestriction::OwnShared
  (const Handle(IGESAppli_RegionRestriction)& /*ent*/, Interface_EntityIterator& /*iter*/) const
{
}

void  IGESAppli_ToolRegionRestriction::OwnCopy
  (const Handle(IGESAppli_RegionRestriction)& another,
   const Handle(IGESAppli_RegionRestriction)& ent, Interface_CopyTool& /*TC*/) const
{
  ent->Init
    (3,another->ElectricalViasRestriction(),
     another->ElectricalComponentRestriction(),
     another->ElectricalCktRestriction());    // nbprops = 3
}

Standard_Boolean  IGESAppli_ToolRegionRestriction::OwnCorrect
  (const Handle(IGESAppli_RegionRestriction)& ent) const
{
  Standard_Boolean res = (ent->NbPropertyValues() != 3);
  if (res) ent->Init
    (3,ent->ElectricalViasRestriction(),ent->ElectricalComponentRestriction(),
     ent->ElectricalCktRestriction());    // nbprops = 3
  if (ent->SubordinateStatus() != 0) {
    Handle(IGESData_LevelListEntity) nulevel;
    ent->InitLevel(nulevel,0);
    res = Standard_True;
  }
  return res;    // + RAZ level selon subordibate
}

IGESData_DirChecker  IGESAppli_ToolRegionRestriction::DirChecker
  (const Handle(IGESAppli_RegionRestriction)& /*ent*/ ) const
{
  IGESData_DirChecker DC(406,2);  //Form no = 2 & Type = 406
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void  IGESAppli_ToolRegionRestriction::OwnCheck
  (const Handle(IGESAppli_RegionRestriction)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  if (ent->SubordinateStatus() != 0)
    if (ent->DefLevel() != IGESData_DefOne &&
	ent->DefLevel() != IGESData_DefSeveral)
      ach->AddFail("Level type: Not value/reference");
  if (ent->NbPropertyValues() != 3)
    ach->AddFail("Number of Property Values != 3");
  if (ent->ElectricalViasRestriction() < 0 || ent->ElectricalViasRestriction() > 2)
    ach->AddFail("Incorrect value for Electrical Vias Restriction");
  if (ent->ElectricalComponentRestriction() < 0 || ent->ElectricalComponentRestriction() > 2)
    ach->AddFail("Incorrect value for Electrical Component Restriction");
  if (ent->ElectricalCktRestriction() < 0 || ent->ElectricalCktRestriction() > 2)
    ach->AddFail("Incorrect value for Electrical Circuit Restriction");
  //UNFINISHED
  //level ignored if this property is subordinate -- queried
}

void  IGESAppli_ToolRegionRestriction::OwnDump
  (const Handle(IGESAppli_RegionRestriction)& ent, const IGESData_IGESDumper& /*dumper*/,
   const Handle(Message_Messenger)& S, const Standard_Integer /*level*/) const
{
  S << "IGESAppli_RegionRestriction" << endl;
  S << "Number of property values : " << ent->NbPropertyValues() << endl;
  S << "Electrical vias restriction       : "
    << ent->ElectricalViasRestriction() << endl;
  S << "Electrical components restriction : "
    << ent->ElectricalComponentRestriction() << endl;
  S << "Electrical circuitary restriction : "
    << ent->ElectricalCktRestriction()  << endl;
}
