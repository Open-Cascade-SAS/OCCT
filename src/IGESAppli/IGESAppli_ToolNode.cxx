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

#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <IGESAppli_Node.hxx>
#include <IGESAppli_ToolNode.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESGeom_TransformationMatrix.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESAppli_ToolNode::IGESAppli_ToolNode ()    {  }


void  IGESAppli_ToolNode::ReadOwnParams
  (const Handle(IGESAppli_Node)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  gp_XYZ tempCoordinates;
  Handle(IGESGeom_TransformationMatrix) tempSystem;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadXYZ(PR.CurrentList(1, 3),"Coordinates of Node (XYZ)",tempCoordinates);

  if (PR.DefinedElseSkip())
    PR.ReadEntity(IR,PR.Current(),"Transformation Matrix",
		  STANDARD_TYPE(IGESGeom_TransformationMatrix), tempSystem,Standard_True);

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempCoordinates,tempSystem);
}

void  IGESAppli_ToolNode::WriteOwnParams
  (const Handle(IGESAppli_Node)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->Coord().X());
  IW.Send(ent->Coord().Y());
  IW.Send(ent->Coord().Z());
  IW.Send(ent->System());
}

void  IGESAppli_ToolNode::OwnShared
  (const Handle(IGESAppli_Node)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->System());
}

void  IGESAppli_ToolNode::OwnCopy
  (const Handle(IGESAppli_Node)& another,
   const Handle(IGESAppli_Node)& ent, Interface_CopyTool& TC) const
{
  gp_XYZ aCoord = (another->Coord()).XYZ();
  DeclareAndCast(IGESGeom_TransformationMatrix,aSystem,
		 TC.Transferred(another->System()));

  ent->Init(aCoord,aSystem);
}

IGESData_DirChecker  IGESAppli_ToolNode::DirChecker
  (const Handle(IGESAppli_Node)& /*ent*/ ) const
{
  IGESData_DirChecker DC(134,0);  //Form no = 0 & Type = 134
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefAny);
  DC.UseFlagRequired(04);
  return DC;
}

void  IGESAppli_ToolNode::OwnCheck
  (const Handle(IGESAppli_Node)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  if (!ent->HasSubScriptNumber())
    ach->AddFail("SubScript Number expected (for Node Number) not present");
  if (!ent->HasTransf())
    ach->AddFail("Transformation Matrix expected, not present");
  if (!ent->System().IsNull())
    if (ent->System()->FormNumber() < 10)
      ach->AddFail("System : Incorrect FormNumber (not 10-11-12)");
}

void  IGESAppli_ToolNode::OwnDump
  (const Handle(IGESAppli_Node)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESAppli_Node" << endl;
  S << " Nodal Coords : 1st " << ent->Coord().X()
    << "  2nd : " << ent->Coord().Y() << "  3rd : " << ent->Coord().Z() <<endl;
  S << "Nodal Displacement Coordinate System : ";
  if (!ent->System().IsNull())
    dumper.Dump(ent->System(),S, level);
  else
    S << "Global Cartesian Coordinate System (default)";
  S << endl;
}
