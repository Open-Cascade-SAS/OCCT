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
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_Status.hxx>
#include <IGESGeom_CurveOnSurface.hxx>
#include <IGESGeom_HArray1OfCurveOnSurface.hxx>
#include <IGESGeom_ToolTrimmedSurface.hxx>
#include <IGESGeom_TrimmedSurface.hxx>
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
//function : IGESGeom_ToolTrimmedSurface
//purpose  : 
//=======================================================================
IGESGeom_ToolTrimmedSurface::IGESGeom_ToolTrimmedSurface ()
{
}


//=======================================================================
//function : ReadOwnParams
//purpose  : 
//=======================================================================

void IGESGeom_ToolTrimmedSurface::ReadOwnParams
  (const Handle(IGESGeom_TrimmedSurface)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  // MGE 31/07/98
  // Building of messages
  //========================================
  //========================================

  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Integer aFlag;
  Standard_Integer count;
  Handle(IGESData_IGESEntity) aSurface;
  Handle(IGESGeom_CurveOnSurface) anOuter;
  Handle(IGESGeom_HArray1OfCurveOnSurface) anInner;
  IGESData_Status aStatus;

  //szv#4:S4163:12Mar99 `st=` not needed
  if (!PR.ReadEntity(IR, PR.Current(), aStatus, aSurface)){
    Message_Msg Msg169("XSTEP_169");
    switch(aStatus) {
    case IGESData_ReferenceError: {  
      Message_Msg Msg216 ("IGES_216");
      Msg169.Arg(Msg216.Value());
      PR.SendFail(Msg169);
      break; }
    case IGESData_EntityError: {
      Message_Msg Msg217 ("IGES_217");
      Msg169.Arg(Msg217.Value());
      PR.SendFail(Msg169);
      break; }
    default:{
    }
    }
  }
  if (!PR.ReadInteger(PR.Current(), aFlag)){
    Message_Msg Msg170("XSTEP_170");
    PR.SendFail(Msg170);
  }
  //st = PR.ReadInteger(PR.Current(), Msg171, count); //szv#4:S4163:12Mar99 moved in if

/*
  st = PR.ReadEntity(IR, PR.Current(), "Surface to be trimmed", aSurface);
  st = PR.ReadInteger(PR.Current(), "Outer boundary type", aFlag);
  st = PR.ReadInteger(PR.Current(), "Number of inner boundary closed curves", count);
*/
  if (PR.ReadInteger(PR.Current(), count)) {
    if (count < 0){
      Message_Msg Msg171("XSTEP_171");
      PR.SendFail (Msg171);
    }
    else  if (count > 0) anInner =
      new IGESGeom_HArray1OfCurveOnSurface(1, count);
  }
  else {
    Message_Msg Msg171("XSTEP_171");
    PR.SendFail(Msg171);
  }

  if (!PR.ReadEntity(IR, PR.Current(), aStatus,
		STANDARD_TYPE(IGESGeom_CurveOnSurface), anOuter,Standard_True)){
    Message_Msg Msg172("XSTEP_172");
    switch(aStatus) {
    case IGESData_ReferenceError: {  
      Message_Msg Msg216 ("IGES_216");
      Msg172.Arg(Msg216.Value());
      PR.SendFail(Msg172);
      break; }
    case IGESData_EntityError: {
      Message_Msg Msg217 ("IGES_217");
      Msg172.Arg(Msg217.Value());
      PR.SendFail(Msg172);
      break; }
    case IGESData_TypeError: {
      Message_Msg Msg218 ("IGES_218");
      Msg172.Arg(Msg218.Value());
      PR.SendFail(Msg172);
      break; }
    default:{
    }
    }
  } //szv#4:S4163:12Mar99 `st=` not needed
/*  st = PR.ReadEntity(IR, PR.Current(), "Outer boundary curve",
		     STANDARD_TYPE(IGESGeom_CurveOnSurface), anOuter,Standard_True);
*/
  if (! anInner.IsNull()) {
    Standard_Integer I;
    for (I = 1; I <= count; I++ ) {
      Handle(IGESGeom_CurveOnSurface) tempEnt;
      //st = PR.ReadEntity(IR, PR.Current(), Msg173,
			   //STANDARD_TYPE(IGESGeom_CurveOnSurface), tempEnt); //szv#4:S4163:12Mar99 moved in if
/*      st = PR.ReadEntity(IR, PR.Current(), "Inner boundary curves",
			 STANDARD_TYPE(IGESGeom_CurveOnSurface), tempEnt);
*/
      if (PR.ReadEntity(IR, PR.Current(), aStatus, STANDARD_TYPE(IGESGeom_CurveOnSurface), tempEnt))
	anInner->SetValue(I, tempEnt);
      else{
	Message_Msg Msg173("XSTEP_173");
	switch(aStatus) {
	case IGESData_ReferenceError: {  
	  Message_Msg Msg216 ("IGES_216");
	  Msg173.Arg(Msg216.Value());
	  PR.SendFail(Msg173);
	  break; }
	case IGESData_EntityError: {
	  Message_Msg Msg217 ("IGES_217");
	  Msg173.Arg(Msg217.Value());
	  PR.SendFail(Msg173);
	  break; }
	case IGESData_TypeError: {
	  Message_Msg Msg218 ("IGES_218");
	  Msg173.Arg(Msg218.Value());
	  PR.SendFail(Msg173);
	  break; }
	default:{
	}
	}
      }
    }
  }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(aSurface, aFlag, anOuter, anInner);
}


//=======================================================================
//function : WriteOwnParams
//purpose  : 
//=======================================================================

void IGESGeom_ToolTrimmedSurface::WriteOwnParams
  (const Handle(IGESGeom_TrimmedSurface)& ent, IGESData_IGESWriter& IW)  const
{
  Standard_Integer up  = ent->NbInnerContours();
  IW.Send(ent->Surface());
  IW.Send(ent->OuterBoundaryType());
  IW.Send(up);

  if (ent->OuterBoundaryType())
    IW.Send(ent->OuterContour());
  else 
    IW.Send(0);
  Standard_Integer I;
  for (I = 1; I <= up; I ++)
    IW.Send(ent->InnerContour(I));
}


//=======================================================================
//function : OwnShared
//purpose  : 
//=======================================================================

void IGESGeom_ToolTrimmedSurface::OwnShared(const Handle(IGESGeom_TrimmedSurface)& ent,
                                            Interface_EntityIterator& iter) const
{
  Standard_Integer up  = ent->NbInnerContours();
  iter.GetOneItem(ent->Surface());
  iter.GetOneItem(ent->OuterContour());
  Standard_Integer I;
  for (I = 1; I <= up; I ++)  iter.GetOneItem(ent->InnerContour(I));
}


//=======================================================================
//function : OwnCopy
//purpose  : 
//=======================================================================

void IGESGeom_ToolTrimmedSurface::OwnCopy(const Handle(IGESGeom_TrimmedSurface)& another,
                                          const Handle(IGESGeom_TrimmedSurface)& ent,
                                          Interface_CopyTool& TC) const
{
  Handle(IGESGeom_HArray1OfCurveOnSurface) anInner;

  DeclareAndCast(IGESData_IGESEntity, aSurface,
                 TC.Transferred(another->Surface()));
  Standard_Integer aFlag = another->OuterBoundaryType();
  DeclareAndCast(IGESGeom_CurveOnSurface, anOuter,
                 TC.Transferred(another->OuterContour()));
  Standard_Integer count = another->NbInnerContours();

  if (count > 0) {
    anInner = new IGESGeom_HArray1OfCurveOnSurface(1, count);
    Standard_Integer I;
    for (I = 1; I <= count; I++) {
      DeclareAndCast(IGESGeom_CurveOnSurface, temp,
		     TC.Transferred(another->InnerContour(I)));
      anInner->SetValue(I, temp);
    }
  }

  ent->Init(aSurface, aFlag, anOuter, anInner);
}


//=======================================================================
//function : DirChecker
//purpose  : 
//=======================================================================

IGESData_DirChecker IGESGeom_ToolTrimmedSurface::DirChecker
  (const Handle(IGESGeom_TrimmedSurface)& /* ent */ )   const
{
  IGESData_DirChecker DC(144, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
//  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.UseFlagRequired (0);
  DC.HierarchyStatusIgnored();

  return DC;
}


//=======================================================================
//function : OwnCheck
//purpose  : 
//=======================================================================

void IGESGeom_ToolTrimmedSurface::OwnCheck(const Handle(IGESGeom_TrimmedSurface)& ent,
                                           const Interface_ShareTool&,
                                           Handle(Interface_Check)& ach)  const
{
  // MGE 31/07/98
  // Building of messages
  //========================================
  //Message_Msg Msg172("XSTEP_172");
  //========================================

  if (ent->OuterBoundaryType() != 0 &&   ent->OuterBoundaryType() != 1) {
    Message_Msg Msg172("XSTEP_172");
    ach->SendFail(Msg172);
  }
}


//=======================================================================
//function : OwnDump
//purpose  : 
//=======================================================================

void IGESGeom_ToolTrimmedSurface::OwnDump(const Handle(IGESGeom_TrimmedSurface)& ent,
                                          const IGESData_IGESDumper& dumper,
                                          const Handle(Message_Messenger)& S,
                                          const Standard_Integer level)  const
{
  Standard_Integer tempSubLevel = (level <= 4) ? 0 : 1;

  S << "IGESGeom_TrimmedSurface" << endl;

  S << "Surface to be trimmed : ";
  dumper.Dump(ent->Surface(),S, tempSubLevel);
  S << endl;
  S << "Boundary type    : " << ent->OuterBoundaryType() << endl;
  S << "Outer Boundary   : ";
  dumper.Dump(ent->OuterContour(),S, tempSubLevel);
  S << endl;
  S << "Inner Boundaries : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbInnerContours(),ent->InnerContour);
  S << endl;
}
