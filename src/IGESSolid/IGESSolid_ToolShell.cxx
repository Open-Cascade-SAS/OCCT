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
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_Status.hxx>
#include <IGESSolid.hxx>
#include <IGESSolid_Face.hxx>
#include <IGESSolid_HArray1OfFace.hxx>
#include <IGESSolid_Shell.hxx>
#include <IGESSolid_ToolShell.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>
#include <TColStd_HArray1OfInteger.hxx>

// MGE 03/08/98
//=======================================================================
//function : IGESSolid_ToolShell
//purpose  : 
//=======================================================================
IGESSolid_ToolShell::IGESSolid_ToolShell ()
{
}


//=======================================================================
//function : ReadOwnParams
//purpose  : 
//=======================================================================

void IGESSolid_ToolShell::ReadOwnParams(const Handle(IGESSolid_Shell)& ent,
                                        const Handle(IGESData_IGESReaderData)& IR,
                                        IGESData_ParamReader& PR) const
{

  // MGE 03/08/98

  //Standard_Boolean abool; //szv#4:S4163:12Mar99 moved down
  Standard_Integer nbfaces=0;; //szv#4:S4163:12Mar99 `i` moved in for
  //Handle(IGESSolid_Face) aface; //szv#4:S4163:12Mar99 moved down
  Handle(IGESSolid_HArray1OfFace) tempFaces;
  Handle(TColStd_HArray1OfInteger) tempOrientation;

  //st = PR.ReadInteger(PR.Current(), Msg200, nbfaces); //szv#4:S4163:12Mar99 moved in if
  //st = PR.ReadInteger(PR.Current(), "Number of faces", nbfaces);
  Standard_Boolean sb = PR.ReadInteger(PR.Current(), nbfaces);
  if (sb && nbfaces > 0 ) {
    Message_Msg Msg180("XSTEP_180");
    
    Standard_Boolean abool;
    Handle(IGESSolid_Face) aface;
    tempFaces = new IGESSolid_HArray1OfFace(1, nbfaces);
    tempOrientation = new TColStd_HArray1OfInteger(1, nbfaces);
    IGESData_Status aStatus;
    for (Standard_Integer i=1; i<=nbfaces; i++) {
      //st = PR.ReadEntity(IR, PR.Current(),Msg201, STANDARD_TYPE(IGESSolid_Face), aface); //szv#4:S4163:12Mar99 moved in if
      //st = PR.ReadEntity(IR, PR.Current(), "Faces", STANDARD_TYPE(IGESSolid_Face), aface);
      if (PR.ReadEntity(IR, PR.Current(),aStatus, STANDARD_TYPE(IGESSolid_Face), aface))
	tempFaces->SetValue(i, aface);
      else{
	Message_Msg Msg201("XSTEP_201");
	switch(aStatus) {
	case IGESData_ReferenceError: {  
	  Message_Msg Msg216 ("IGES_216");
	  Msg201.Arg(Msg216.Value());
	  PR.SendFail(Msg201);
	  break; }
	case IGESData_EntityError: {
	  Message_Msg Msg217 ("IGES_217");
	  Msg201.Arg(Msg217.Value());
	  PR.SendFail(Msg201);
	  break; }
	case IGESData_TypeError: {
	  Message_Msg Msg218 ("IGES_218");
	  Msg201.Arg(Msg218.Value());
	  PR.SendFail(Msg201);
	  break; }
	default:{
	}
	}
      }
      //st = PR.ReadBoolean(PR.Current(), Msg180, abool); //szv#4:S4163:12Mar99 moved in if
      //st = PR.ReadBoolean(PR.Current(), "Orientation flags", abool);
      if (PR.ReadBoolean(PR.Current(), Msg180, abool))
	tempOrientation->SetValue(i, (abool ? 1 : 0) );
    }
  }
  else {
    Message_Msg Msg200("XSTEP_200");
    PR.SendFail(Msg200);
  }
  
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init (tempFaces, tempOrientation);
}


//=======================================================================
//function : WriteOwnParams
//purpose  : 
//=======================================================================

void IGESSolid_ToolShell::WriteOwnParams(const Handle(IGESSolid_Shell)& ent,
                                         IGESData_IGESWriter& IW) const
{
  Standard_Integer i, nbfaces = ent->NbFaces();

  IW.Send(nbfaces);
  for (i = 1; i <= nbfaces; i ++)
    {
      IW.Send(ent->Face(i));
      IW.SendBoolean(ent->Orientation(i));
    }
}


//=======================================================================
//function : OwnShared
//purpose  : 
//=======================================================================

void IGESSolid_ToolShell::OwnShared(const Handle(IGESSolid_Shell)& ent,
                                    Interface_EntityIterator& iter) const
{
  Standard_Integer nbfaces = ent->NbFaces();
  for (Standard_Integer i = 1; i <= nbfaces; i ++)
    iter.GetOneItem(ent->Face(i));
}


//=======================================================================
//function : OwnCopy
//purpose  : 
//=======================================================================

void IGESSolid_ToolShell::OwnCopy(const Handle(IGESSolid_Shell)& another,
                                  const Handle(IGESSolid_Shell)& ent,
                                  Interface_CopyTool& TC) const
{
  Standard_Integer nbfaces = another->NbFaces();

  Handle(IGESSolid_HArray1OfFace) tempFaces = new
    IGESSolid_HArray1OfFace(1, nbfaces);
  Handle(TColStd_HArray1OfInteger) tempOrientation = new
    TColStd_HArray1OfInteger(1, nbfaces);
  for (Standard_Integer i=1; i<=nbfaces; i++)
    {
      DeclareAndCast(IGESSolid_Face, face,
		     TC.Transferred(another->Face(i)));
      tempFaces->SetValue(i, face);
      tempOrientation->SetValue(i, (another->Orientation(i) ? 1 : 0) );
    }
  ent->Init (tempFaces, tempOrientation);
}


//=======================================================================
//function : DirChecker
//purpose  : 
//=======================================================================

IGESData_DirChecker IGESSolid_ToolShell::DirChecker
  (const Handle(IGESSolid_Shell)& /* ent */ ) const
{
  IGESData_DirChecker DC(514, 1,2);

  DC.Structure  (IGESData_DefVoid);
  DC.LineFont   (IGESData_DefVoid);
  DC.LineWeight (IGESData_DefVoid);
  DC.Color      (IGESData_DefVoid);

  DC.SubordinateStatusRequired(1);
  return DC;
}


//=======================================================================
//function : OwnCheck
//purpose  : 
//=======================================================================

void IGESSolid_ToolShell::OwnCheck(const Handle(IGESSolid_Shell)& ent,
                                   const Interface_ShareTool&,
                                   Handle(Interface_Check)& ach) const
{
  // MGE 03/08/98
  // Building of messages
  //========================================
  //Message_Msg Msg200("XSTEP_200");
  //========================================

  if (ent->NbFaces() <= 0) {
    Message_Msg Msg200("XSTEP_200");
    ach->SendFail(Msg200);
  }
}


//=======================================================================
//function : OwnDump
//purpose  : 
//=======================================================================

void IGESSolid_ToolShell::OwnDump(const Handle(IGESSolid_Shell)& ent,
                                  const IGESData_IGESDumper& dumper,
                                  const Handle(Message_Messenger)& S,
                                  const Standard_Integer level) const
{
  S << "IGESSolid_Shell" << endl;
  Standard_Integer upper = ent->NbFaces();
  Standard_Integer sublevel = (level <= 4) ? 0 : 1;

  S << "Faces : " << endl << "Orientation flags : ";
  IGESData_DumpEntities(S,dumper,-level,1, ent->NbFaces(),ent->Face);
  S << endl;
  if (level > 4)
    {
      S << "[" << endl;
      for (Standard_Integer i = 1; i <= upper; i ++)
	{
          S << "[" << i << "]:  ";
          S << "Face : ";
          dumper.Dump (ent->Face(i),S, sublevel);
          S << "  - Orientation flag : ";
          if (ent->Orientation(i)) S << "True" << endl;
          else                     S << "False" << endl;
	}
    }
  S << endl;
}
