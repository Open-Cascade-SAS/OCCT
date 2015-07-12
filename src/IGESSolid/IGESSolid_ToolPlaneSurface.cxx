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

#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_Status.hxx>
#include <IGESGeom_Direction.hxx>
#include <IGESGeom_Point.hxx>
#include <IGESSolid_PlaneSurface.hxx>
#include <IGESSolid_ToolPlaneSurface.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>

// MGE 31/07/98
//=======================================================================
//function : IGESSolid_ToolPlaneSurface
//purpose  : 
//=======================================================================
IGESSolid_ToolPlaneSurface::IGESSolid_ToolPlaneSurface ()
{
}


//=======================================================================
//function : ReadOwnParams
//purpose  : 
//=======================================================================

void IGESSolid_ToolPlaneSurface::ReadOwnParams(const Handle(IGESSolid_PlaneSurface)& ent,
                                               const Handle(IGESData_IGESReaderData)& IR,
                                               IGESData_ParamReader& PR) const
{
  Handle(IGESGeom_Point) tempLocation;
  Handle(IGESGeom_Direction) tempNormal;
  Handle(IGESGeom_Direction) tempRefdir;          // default Unparametrised
  IGESData_Status aStatus;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  if (!PR.ReadEntity(IR, PR.Current(),aStatus,STANDARD_TYPE(IGESGeom_Point), tempLocation)){ //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg174("XSTEP_174");
    switch(aStatus) {
    case IGESData_ReferenceError: {  
      Message_Msg Msg216 ("IGES_216");
      Msg174.Arg(Msg216.Value());
      PR.SendFail(Msg174);
      break; }
    case IGESData_EntityError: {
      Message_Msg Msg217 ("IGES_217");
      Msg174.Arg(Msg217.Value());
      PR.SendFail(Msg174);
      break; }
    case IGESData_TypeError: {
      Message_Msg Msg218 ("IGES_218");
      Msg174.Arg(Msg218.Value());
      PR.SendFail(Msg174);
      break; }
    default:{
    }
    }
  }
/*
  st = PR.ReadEntity(IR, PR.Current(), "Point on axis",
		     STANDARD_TYPE(IGESGeom_Point), tempLocation);
*/
  if (!PR.ReadEntity(IR, PR.Current(),aStatus,STANDARD_TYPE(IGESGeom_Direction), tempNormal)){ //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg175("XSTEP_175");
    switch(aStatus) {
    case IGESData_ReferenceError: {  
      Message_Msg Msg216 ("IGES_216");
      Msg175.Arg(Msg216.Value());
      PR.SendFail(Msg175);
      break; }
    case IGESData_EntityError: {
      Message_Msg Msg217 ("IGES_217");
      Msg175.Arg(Msg217.Value());
      PR.SendFail(Msg175);
      break; }
    case IGESData_TypeError: {
      Message_Msg Msg218 ("IGES_218");
      Msg175.Arg(Msg218.Value());
      PR.SendFail(Msg175);
      break; }
    default:{
    }
    }
}
/*
  st = PR.ReadEntity(IR, PR.Current(), "Normal direction",
		     STANDARD_TYPE(IGESGeom_Direction), tempNormal);
*/
  if (ent->FormNumber() == 1){
      // Parametrised surface
    if (!PR.ReadEntity(IR, PR.Current(), aStatus, STANDARD_TYPE(IGESGeom_Direction), tempRefdir)){ //szv#4:S4163:12Mar99 `st=` not needed
      Message_Msg Msg176("XSTEP_176");
      switch(aStatus) {
      case IGESData_ReferenceError: {  
	Message_Msg Msg216 ("IGES_216");
	Msg176.Arg(Msg216.Value());
	PR.SendFail(Msg176);
	break; }
      case IGESData_EntityError: {
	Message_Msg Msg217 ("IGES_217");
	Msg176.Arg(Msg217.Value());
	PR.SendFail(Msg176);
	break; }
      case IGESData_TypeError: {
	Message_Msg Msg218 ("IGES_218");
	Msg176.Arg(Msg218.Value());
	PR.SendFail(Msg176);
	break; }
      default:{
      }
      }
    }
  }
/*
    st = PR.ReadEntity(IR, PR.Current(), "Reference direction",
		       STANDARD_TYPE(IGESGeom_Direction), tempRefdir);
*/
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init (tempLocation, tempNormal, tempRefdir);
}


//=======================================================================
//function : WriteOwnParams
//purpose  : 
//=======================================================================

void IGESSolid_ToolPlaneSurface::WriteOwnParams
  (const Handle(IGESSolid_PlaneSurface)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->LocationPoint());
  IW.Send(ent->Normal());
  if (ent->IsParametrised())    IW.Send(ent->ReferenceDir());
}


//=======================================================================
//function : OwnShared
//purpose  : 
//=======================================================================

void IGESSolid_ToolPlaneSurface::OwnShared(const Handle(IGESSolid_PlaneSurface)& ent,
                                           Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->LocationPoint());
  iter.GetOneItem(ent->Normal());
  iter.GetOneItem(ent->ReferenceDir());
}


//=======================================================================
//function : OwnCopy
//purpose  : 
//=======================================================================

void IGESSolid_ToolPlaneSurface::OwnCopy
  (const Handle(IGESSolid_PlaneSurface)& another,
   const Handle(IGESSolid_PlaneSurface)& ent, Interface_CopyTool& TC) const
{
  DeclareAndCast(IGESGeom_Point, tempLocation,
		 TC.Transferred(another->LocationPoint()));
  DeclareAndCast(IGESGeom_Direction, tempNormal,
		 TC.Transferred(another->Normal()));
  if (another->IsParametrised())
    {
      DeclareAndCast(IGESGeom_Direction, tempRefdir,
		     TC.Transferred(another->ReferenceDir()));
      ent->Init (tempLocation, tempNormal, tempRefdir);
    }
  else
    {
      Handle(IGESGeom_Direction) tempRefdir;
      ent->Init (tempLocation, tempNormal, tempRefdir);
    }
}


//=======================================================================
//function : DirChecker
//purpose  : 
//=======================================================================

IGESData_DirChecker IGESSolid_ToolPlaneSurface::DirChecker
  (const Handle(IGESSolid_PlaneSurface)& /*ent*/) const
{
  IGESData_DirChecker DC(190, 0, 1);

  DC.Structure  (IGESData_DefVoid);
  DC.LineFont   (IGESData_DefAny);
  DC.Color      (IGESData_DefAny);

  DC.BlankStatusIgnored ();
  DC.HierarchyStatusIgnored ();
  return DC;
}


//=======================================================================
//function : OwnCheck
//purpose  : 
//=======================================================================

void IGESSolid_ToolPlaneSurface::OwnCheck(const Handle(IGESSolid_PlaneSurface)& ent,
                                          const Interface_ShareTool&,
                                          Handle(Interface_Check)& ach) const
{

  // MGE 31/07/98
  // Building of messages
  //========================================
  //Message_Msg Msg177("XSTEP_177");
  //========================================

  Standard_Integer fn = 0;
  if (ent->IsParametrised()) fn = 1;
  if (fn != ent->FormNumber()) {
    Message_Msg Msg177("XSTEP_177");
    ach->SendFail (Msg177);
  }
}


//=======================================================================
//function : OwnDump
//purpose  : 
//=======================================================================

void IGESSolid_ToolPlaneSurface::OwnDump(const Handle(IGESSolid_PlaneSurface)& ent,
                                         const IGESData_IGESDumper& dumper,
                                         const Handle(Message_Messenger)& S,
                                         const Standard_Integer level) const
{
  S << "IGESSolid_PlaneSurface" << endl;

  Standard_Integer sublevel = (level <= 4) ? 0 : 1;
  S << "Point on axis    : ";
  dumper.Dump(ent->LocationPoint(),S, sublevel);
  S << endl;
  S << "Normal direction : ";
  dumper.Dump(ent->Normal(),S, sublevel);
  S << endl;
  if (ent->IsParametrised())
    {
      S << "Surface is Parametrised  -  Reference direction : ";
      dumper.Dump(ent->ReferenceDir(),S, sublevel);
      S << endl;
    }
  else S << "Surface is UnParametrised" << endl;
}
