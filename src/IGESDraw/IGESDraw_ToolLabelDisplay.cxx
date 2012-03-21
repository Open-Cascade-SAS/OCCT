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

#include <IGESDraw_ToolLabelDisplay.ixx>
#include <IGESData_ParamCursor.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <IGESDraw_HArray1OfViewKindEntity.hxx>
#include <TColgp_HArray1OfXYZ.hxx>
#include <IGESDimen_HArray1OfLeaderArrow.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_Dump.hxx>
#include <IGESDraw_View.hxx>
#include <IGESDraw_PerspectiveView.hxx>
#include <Interface_Macros.hxx>


IGESDraw_ToolLabelDisplay::IGESDraw_ToolLabelDisplay ()    {  }


void IGESDraw_ToolLabelDisplay::ReadOwnParams
  (const Handle(IGESDraw_LabelDisplay)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{ 
  //Standard_Boolean st; //szv#4:S4163:12Mar99 moved down
  Standard_Integer nbval;

  Handle(IGESDraw_HArray1OfViewKindEntity) views; 
  Handle(TColgp_HArray1OfXYZ)                   textLocations; 
  Handle(IGESDimen_HArray1OfLeaderArrow)    leaderEntities; 
  Handle(TColStd_HArray1OfInteger)       labelLevels;
  Handle(IGESData_HArray1OfIGESEntity)     displayedEntities; 

  // Reading nbval(No. of Label placements)
  Standard_Boolean st = PR.ReadInteger(PR.Current(), "No. of Label placements", nbval);
  if (st && nbval > 0)
    {
      views             = new IGESDraw_HArray1OfViewKindEntity(1, nbval);
      textLocations     = new TColgp_HArray1OfXYZ(1, nbval);
      leaderEntities    = new IGESDimen_HArray1OfLeaderArrow(1, nbval);
      labelLevels       = new TColStd_HArray1OfInteger(1, nbval);
      displayedEntities = new IGESData_HArray1OfIGESEntity(1, nbval);
      
      Handle(IGESData_ViewKindEntity) tempView;
      gp_XYZ                          tempXYZ;
      Handle(IGESDimen_LeaderArrow)   tempLeaderArrow;
      Standard_Integer                tempLabel;
      Handle(IGESData_IGESEntity)     tempDisplayedEntity;
      
      for (Standard_Integer i = 1; i <= nbval; i++)
	{
          // Reading views(HArray1OfView)
          //st = PR.ReadEntity (IR, PR.Current(), "Instance of views",
				//STANDARD_TYPE(IGESData_ViewKindEntity), tempView); //szv#4:S4163:12Mar99 moved in if
	  if (PR.ReadEntity (IR, PR.Current(), "Instance of views",
			     STANDARD_TYPE(IGESData_ViewKindEntity), tempView))
	    views->SetValue(i, tempView);
	  
          // Reading textLocations(HArray1OfXYZ)
          //st = PR.ReadXYZ(PR.CurrentList(1, 3), "array textLocations", tempXYZ); //szv#4:S4163:12Mar99 moved in if
	  if (PR.ReadXYZ(PR.CurrentList(1, 3), "array textLocations", tempXYZ))
	    textLocations->SetValue(i, tempXYZ);

          // Reading leaderEntities(HArray1OfLeaderArrow)
          //st = PR.ReadEntity (IR, PR.Current(), "Instance of LeaderArrow",
				//STANDARD_TYPE(IGESDimen_LeaderArrow), tempLeaderArrow); //szv#4:S4163:12Mar99 moved in if
	  if (PR.ReadEntity (IR, PR.Current(), "Instance of LeaderArrow",
			     STANDARD_TYPE(IGESDimen_LeaderArrow), tempLeaderArrow))
	    leaderEntities->SetValue(i, tempLeaderArrow);
	  
          // Reading labelLevels(HArray1OfInteger)
          //st = PR.ReadInteger(PR.Current(), "array labelLevels", tempLabel); //szv#4:S4163:12Mar99 moved in if
	  if (PR.ReadInteger(PR.Current(), "array labelLevels", tempLabel))
	    labelLevels->SetValue(i, tempLabel);
	  
          // Reading displayedEntities(HArray1OfIGESEntity)
          //st = PR.ReadEntity (IR, PR.Current(), "displayedEntities entity",
				//tempDisplayedEntity); //szv#4:S4163:12Mar99 moved in if
	  if (PR.ReadEntity (IR, PR.Current(), "displayedEntities entity", tempDisplayedEntity))
	    displayedEntities->SetValue(i, tempDisplayedEntity);
	}
    }
  else  PR.AddFail("No. of Label placements : Not Positive");

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init
    (views, textLocations, leaderEntities, labelLevels, displayedEntities);
}

void IGESDraw_ToolLabelDisplay::WriteOwnParams
  (const Handle(IGESDraw_LabelDisplay)& ent, IGESData_IGESWriter& IW)  const
{
  Standard_Integer Up  = ent->NbLabels();
  IW.Send( Up );
  for ( Standard_Integer i = 1; i <= Up; i++)
    {
      IW.Send( ent->ViewItem(i) );
      IW.Send( (ent->TextLocation(i)).X() );
      IW.Send( (ent->TextLocation(i)).Y() );
      IW.Send( (ent->TextLocation(i)).Z() );
      IW.Send( ent->LeaderEntity(i) );
      IW.Send( ent->LabelLevel(i) );
      IW.Send( ent->DisplayedEntity(i) );
    }
}

void  IGESDraw_ToolLabelDisplay::OwnShared
  (const Handle(IGESDraw_LabelDisplay)& ent, Interface_EntityIterator& iter) const
{
  Standard_Integer Up  = ent->NbLabels();
  for ( Standard_Integer i = 1; i <= Up; i++)
    {
      iter.GetOneItem( ent->ViewItem(i) );
      iter.GetOneItem( ent->LeaderEntity(i) );
      iter.GetOneItem( ent->DisplayedEntity(i) );
    }
}

void IGESDraw_ToolLabelDisplay::OwnCopy
  (const Handle(IGESDraw_LabelDisplay)& another,
   const Handle(IGESDraw_LabelDisplay)& ent, Interface_CopyTool& TC) const
{
  Standard_Integer                              nbval;
  Handle(IGESDraw_HArray1OfViewKindEntity) views;
  Handle(TColgp_HArray1OfXYZ)                   textLocations;
  Handle(IGESDimen_HArray1OfLeaderArrow)    leaderEntities;
  Handle(TColStd_HArray1OfInteger)       labelLevels;
  Handle(IGESData_HArray1OfIGESEntity)     displayedEntities;
 
  nbval             = another->NbLabels();
  views             = new IGESDraw_HArray1OfViewKindEntity(1, nbval);
  textLocations     = new TColgp_HArray1OfXYZ(1, nbval);
  leaderEntities    = new IGESDimen_HArray1OfLeaderArrow(1, nbval);
  labelLevels       = new TColStd_HArray1OfInteger(1, nbval);
  displayedEntities = new IGESData_HArray1OfIGESEntity(1, nbval);
 
  for (Standard_Integer i = 1; i <= nbval; i++)
    {
      DeclareAndCast(IGESData_ViewKindEntity, tempView,
                     TC.Transferred(another->ViewItem(i)));
      views->SetValue( i, tempView );
 
      textLocations->SetValue( i, (another->TextLocation(i)).XYZ() );
 
      DeclareAndCast(IGESDimen_LeaderArrow, tempArrow, 
                     TC.Transferred(another->LeaderEntity(i)));
      leaderEntities->SetValue( i, tempArrow );
 
      labelLevels->SetValue( i, another->LabelLevel(i) );
 
      DeclareAndCast(IGESData_IGESEntity, tempEntity, 
                     TC.Transferred(another->DisplayedEntity(i)));
      displayedEntities->SetValue( i, tempEntity );
    }

  ent->Init(views, textLocations, leaderEntities, 
	    labelLevels, displayedEntities);
}

IGESData_DirChecker IGESDraw_ToolLabelDisplay::DirChecker
  (const Handle(IGESDraw_LabelDisplay)& /*ent*/)  const
{ 
  IGESData_DirChecker DC (402, 5);
  DC.Structure(IGESData_DefVoid);
  DC.HierarchyStatusIgnored();
  DC.BlankStatusIgnored();
  return DC;
}

void IGESDraw_ToolLabelDisplay::OwnCheck
  (const Handle(IGESDraw_LabelDisplay)& /*ent*/,
   const Interface_ShareTool& , Handle(Interface_Check)& /*ach*/)  const
{
}

void IGESDraw_ToolLabelDisplay::OwnDump
  (const Handle(IGESDraw_LabelDisplay)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level)  const
{
  Standard_Integer sublevel = (level <= 4) ? 0 : 1;

  S << "IGESDraw_LabelDisplay" << endl;

  S << "View Entities       : " << endl
    << "Text Locations      : " << endl
    << "Leader Entities     : " << endl
    << "Label Level Numbers : " << endl
    << "Displayed Entities  : ";
  S << "Count = "      << ent->NbLabels() << endl;
  if (level > 4)  // Level = 4 : no Dump. Level = 5 & 6 : same Dump
    {
      Standard_Integer I;
      Standard_Integer up  = ent->NbLabels();
      for (I = 1; I <= up; I ++)
	{
	  S << "[" << I << "]:" << endl;
	  S << "View Entity : ";
	  dumper.Dump (ent->ViewItem(I),S, sublevel);
	  S << endl;
	  S << "Text Location in View : ";
	  IGESData_DumpXYZL(S,level, ent->TextLocation(I), ent->Location());
	  S << "  Leader Entity in View : ";
	  dumper.Dump (ent->LeaderEntity(I),S, sublevel);
	  S << endl;
	  S << "Entity Label Level Number : ";
	  S << ent->LabelLevel(I) << "  ";
	  S << "Displayed Entity : ";
	  dumper.Dump (ent->DisplayedEntity(I),S, sublevel);
	  S << endl;
	}
    }
  S << endl;
}
