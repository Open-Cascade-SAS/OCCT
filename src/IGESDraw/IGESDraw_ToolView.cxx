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

#include <IGESDraw_ToolView.ixx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_TransfEntity.hxx>
#include <IGESGeom_Plane.hxx>
#include <Interface_Macros.hxx>


IGESDraw_ToolView::IGESDraw_ToolView ()    {  }


void IGESDraw_ToolView::ReadOwnParams
  (const Handle(IGESDraw_View)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  Standard_Integer tempViewNumber;
  Standard_Real tempScaleFactor;
  Handle(IGESGeom_Plane) tempLeftPlane,   tempTopPlane,  tempRightPlane;
  Handle(IGESGeom_Plane) tempBottomPlane, tempBackPlane, tempFrontPlane;

  PR.ReadInteger(PR.Current(), "View Number", tempViewNumber); //szv#4:S4163:12Mar99 `st=` not needed

  if (PR.DefinedElseSkip())
    PR.ReadReal(PR.Current(), "Scale Factor", tempScaleFactor); //szv#4:S4163:12Mar99 `st=` not needed
  else
    tempScaleFactor = 1.0;      // Setting to default value of 1.0

  PR.ReadEntity(IR, PR.Current(), "Left Side Of View Volume",
		STANDARD_TYPE(IGESGeom_Plane), tempLeftPlane,   Standard_True); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity(IR, PR.Current(), "Top Side Of View Volume",
		STANDARD_TYPE(IGESGeom_Plane), tempTopPlane,    Standard_True); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity(IR, PR.Current(), "Right Side Of View Volume",
		STANDARD_TYPE(IGESGeom_Plane), tempRightPlane,  Standard_True); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity(IR, PR.Current(), "Bottom Side Of View Volume",
		STANDARD_TYPE(IGESGeom_Plane), tempBottomPlane, Standard_True); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity(IR, PR.Current(), "Back Side Of View Volume",
		STANDARD_TYPE(IGESGeom_Plane), tempBackPlane,   Standard_True); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity(IR, PR.Current(), "Front Side Of View Volume",
		STANDARD_TYPE(IGESGeom_Plane), tempFrontPlane,  Standard_True); //szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init
    (tempViewNumber, tempScaleFactor, tempLeftPlane, tempTopPlane,
     tempRightPlane, tempBottomPlane, tempBackPlane, tempFrontPlane);
}

void IGESDraw_ToolView::WriteOwnParams
  (const Handle(IGESDraw_View)& ent, IGESData_IGESWriter& IW)  const
{
  IW.Send(ent->ViewNumber());
  IW.Send(ent->ScaleFactor());
  IW.Send(ent->LeftPlane());
  IW.Send(ent->TopPlane());
  IW.Send(ent->RightPlane());
  IW.Send(ent->BottomPlane());
  IW.Send(ent->BackPlane());
  IW.Send(ent->FrontPlane());
}

void  IGESDraw_ToolView::OwnShared
  (const Handle(IGESDraw_View)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->LeftPlane());
  iter.GetOneItem(ent->TopPlane());
  iter.GetOneItem(ent->RightPlane());
  iter.GetOneItem(ent->BottomPlane());
  iter.GetOneItem(ent->BackPlane());
  iter.GetOneItem(ent->FrontPlane());
}

void IGESDraw_ToolView::OwnCopy
  (const Handle(IGESDraw_View)& another,
   const Handle(IGESDraw_View)& ent, Interface_CopyTool& TC) const
{
  Standard_Integer tempViewNumber = another->ViewNumber();
  Standard_Real tempScaleFactor = another->ScaleFactor();
  DeclareAndCast(IGESGeom_Plane, tempLeftPlane,
                 TC.Transferred(another->LeftPlane()));
  DeclareAndCast(IGESGeom_Plane, tempTopPlane,
                 TC.Transferred(another->TopPlane()));
  DeclareAndCast(IGESGeom_Plane, tempRightPlane,
                 TC.Transferred(another->RightPlane()));
  DeclareAndCast(IGESGeom_Plane, tempBottomPlane,
                 TC.Transferred(another->BottomPlane()));
  DeclareAndCast(IGESGeom_Plane, tempBackPlane,
                 TC.Transferred(another->BackPlane()));
  DeclareAndCast(IGESGeom_Plane, tempFrontPlane,
                 TC.Transferred(another->FrontPlane()));

  ent->Init(tempViewNumber, tempScaleFactor, tempLeftPlane, tempTopPlane,
	    tempRightPlane, tempBottomPlane, tempBackPlane, tempFrontPlane);
}

IGESData_DirChecker IGESDraw_ToolView::DirChecker
  (const Handle(IGESDraw_View)& /*ent*/)  const
{
  IGESData_DirChecker DC(410, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.UseFlagRequired(1);
  DC.HierarchyStatusIgnored();

  return DC;
}

void IGESDraw_ToolView::OwnCheck
  (const Handle(IGESDraw_View)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach)  const
{
  if (ent->HasTransf()) {
    if (ent->Transf()->FormNumber() != 0)
      ach->AddFail("Associated Matrix has not Form Number 0");
  }
}

void IGESDraw_ToolView::OwnDump
  (const Handle(IGESDraw_View)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level)  const
{
  Standard_Integer tempSubLevel = (level <= 4) ? 0 : 1;

  S << "IGESDraw_View" << endl;

  S << "View Number  : " << ent->ViewNumber()  << endl;
  S << "Scale Factor : " << ent->ScaleFactor() << endl;
  S << "Left Plane Of View Volume   : ";
  dumper.Dump(ent->LeftPlane(),S, tempSubLevel);    S << endl;
  S << "Top Plane Of View Volume    : ";
  dumper.Dump(ent->TopPlane(),S, tempSubLevel);     S << endl;
  S << "Right Plane Of View Volume  : ";
  dumper.Dump(ent->RightPlane(),S, tempSubLevel);   S << endl;
  S << "Bottom Plane Of View Volume : ";
  dumper.Dump(ent->BottomPlane(),S, tempSubLevel);  S << endl;
  S << "Back Plane Of View Volume   : ";
  dumper.Dump(ent->BackPlane(),S, tempSubLevel);    S << endl;
  S << "Front Plane Of View Volume  : ";
  dumper.Dump(ent->FrontPlane(),S, tempSubLevel);   S << endl;
}
