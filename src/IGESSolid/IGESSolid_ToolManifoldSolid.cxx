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
#include <IGESSolid_HArray1OfShell.hxx>
#include <IGESSolid_ManifoldSolid.hxx>
#include <IGESSolid_Shell.hxx>
#include <IGESSolid_ToolManifoldSolid.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>
#include <TColStd_HArray1OfInteger.hxx>

// MGE 31/07/98
IGESSolid_ToolManifoldSolid::IGESSolid_ToolManifoldSolid ()    {  }


void  IGESSolid_ToolManifoldSolid::ReadOwnParams
  (const Handle(IGESSolid_ManifoldSolid)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{

  // MGE 31/07/98
  // Building of messages
  //========================================
  Message_Msg Msg180("XSTEP_180");
  //========================================

  Standard_Boolean abool, shellFlag; //szv#4:S4163:12Mar99 `st` moved down
  Standard_Integer nbshells, i;
  Handle(TColStd_HArray1OfInteger) voidShellFlags;
  Handle(IGESData_IGESEntity) shell;
  Handle(IGESSolid_Shell) ashell;
  Handle(IGESSolid_HArray1OfShell) voidShells;
  IGESData_Status aStatus;

  if (!PR.ReadEntity(IR, PR.Current(), aStatus, shell)){ //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg178("XSTEP_178");
    switch(aStatus) {
    case IGESData_ReferenceError: {  
      Message_Msg Msg216 ("IGES_216");
      Msg178.Arg(Msg216.Value());
      PR.SendFail(Msg178);
      break; }
    case IGESData_EntityError: {
      Message_Msg Msg217 ("IGES_217");
      Msg178.Arg(Msg217.Value());
      PR.SendFail(Msg178);
      break; }
    default:{
    }
    }
  }

  PR.ReadBoolean(PR.Current(), Msg180, shellFlag); //szv#4:S4163:12Mar99 `st=` not needed
  Standard_Boolean st = PR.ReadInteger(PR.Current(), nbshells);
  if(!st){
    Message_Msg Msg181("XSTEP_181");
    PR.SendFail(Msg181);
  }
/*
  st = PR.ReadEntity(IR, PR.Current(), "Shell", shell);
  st = PR.ReadBoolean(PR.Current(), "Shell orientation", shellFlag);
  st = PR.ReadInteger(PR.Current(), "Number of shells", nbshells);
*/  
  if (st && nbshells > 0)
    {
      voidShells = new IGESSolid_HArray1OfShell(1, nbshells);
      voidShellFlags = new TColStd_HArray1OfInteger(1, nbshells); voidShellFlags->Init(0);
      for (i=1; i<=nbshells; i++)
	{
          //st = PR.ReadEntity(IR, PR.Current(), Msg179,
			     //STANDARD_TYPE(IGESSolid_Shell), ashell); //szv#4:S4163:12Mar99 moved in if
          /*
          st = PR.ReadEntity(IR, PR.Current(), "Void shells",
			     STANDARD_TYPE(IGESSolid_Shell), ashell);
          */
          if (PR.ReadEntity(IR, PR.Current(), aStatus, STANDARD_TYPE(IGESSolid_Shell), ashell))
	    voidShells->SetValue(i, ashell);
	  else {
	    Message_Msg Msg179("XSTEP_179");
	    switch(aStatus) {
	    case IGESData_ReferenceError: {  
	      Message_Msg Msg216 ("IGES_216");
	      Msg179.Arg(Msg216.Value());
	      PR.SendFail(Msg179);
	      break; }
	    case IGESData_EntityError: {
	      Message_Msg Msg217 ("IGES_217");
	      Msg179.Arg(Msg217.Value());
	      PR.SendFail(Msg179);
	      break; }
	    case IGESData_TypeError: {
	      Message_Msg Msg218 ("IGES_218");
	      Msg179.Arg(Msg218.Value());
	      PR.SendFail(Msg179);
	      break; }
	    default:{
	    }
	    }
	  }

          PR.ReadBoolean(PR.Current(), Msg180, abool); //szv#4:S4163:12Mar99 `st=` not needed
	  if (abool) voidShellFlags->SetValue(i, 1);
	}
    }
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init (Handle(IGESSolid_Shell)::DownCast (shell), shellFlag, voidShells, voidShellFlags);
}

void  IGESSolid_ToolManifoldSolid::WriteOwnParams
  (const Handle(IGESSolid_ManifoldSolid)& ent, IGESData_IGESWriter& IW) const
{
  Standard_Integer i;
  Standard_Integer nbshells = ent->NbVoidShells();

  IW.Send(ent->Shell());
  IW.SendBoolean(ent->OrientationFlag());
  IW.Send(nbshells);
  for (i = 1; i <= nbshells; i ++)
    {
      IW.Send(ent->VoidShell(i));
      IW.SendBoolean(ent->VoidOrientationFlag(i));
    }
}

void  IGESSolid_ToolManifoldSolid::OwnShared
  (const Handle(IGESSolid_ManifoldSolid)& ent, Interface_EntityIterator& iter) const
{
  Standard_Integer i;
  Standard_Integer nbshells = ent->NbVoidShells();

  iter.GetOneItem(ent->Shell());
  for (i = 1; i <= nbshells; i ++)    iter.GetOneItem(ent->VoidShell(i));
}

void  IGESSolid_ToolManifoldSolid::OwnCopy
  (const Handle(IGESSolid_ManifoldSolid)& another,
   const Handle(IGESSolid_ManifoldSolid)& ent, Interface_CopyTool& TC) const
{
  DeclareAndCast(IGESSolid_Shell, shell, TC.Transferred(another->Shell()));
  Standard_Boolean shellFlag = another->OrientationFlag();

  Standard_Integer nbshells = another->NbVoidShells();
  Handle(IGESSolid_HArray1OfShell) voidShells;
  Handle(TColStd_HArray1OfInteger) voidFlags;
  if (nbshells > 0)
    {
      voidShells = new IGESSolid_HArray1OfShell(1, nbshells);
      voidFlags  = new TColStd_HArray1OfInteger(1, nbshells);
      for (Standard_Integer i = 1; i <= nbshells; i ++)
	{
          DeclareAndCast(IGESSolid_Shell, voidshell,
			 TC.Transferred(another->VoidShell(i)));
          voidShells->SetValue(i, voidshell);
          voidFlags->SetValue(i, (another->VoidOrientationFlag(i) ? 1 : 0));
	}
    }
  ent->Init (shell, shellFlag, voidShells, voidFlags);
}

IGESData_DirChecker  IGESSolid_ToolManifoldSolid::DirChecker
  (const Handle(IGESSolid_ManifoldSolid)& /* ent */ ) const
{
  IGESData_DirChecker DC(186, 0);

  DC.Structure  (IGESData_DefVoid);
  DC.LineFont   (IGESData_DefVoid);
  DC.LineWeight (IGESData_DefVoid);
  DC.Color      (IGESData_DefAny);

  return DC;
}

void  IGESSolid_ToolManifoldSolid::OwnCheck
  (const Handle(IGESSolid_ManifoldSolid)& /* ent */,
   const Interface_ShareTool& , Handle(Interface_Check)& /* ach */) const
{
}

void  IGESSolid_ToolManifoldSolid::OwnDump
  (const Handle(IGESSolid_ManifoldSolid)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESSolid_ManifoldSolid" << endl;

  Standard_Integer sublevel = (level <= 4) ? 0 : 1;
  S << "Shell : ";
  dumper.Dump(ent->Shell(),S, sublevel);
  S << endl;
  if (ent->OrientationFlag())
    S << "Orientation agrees with the underlying surface" << endl;
  else
    S << "Orientation does not agrees with the underlying surface" << endl;
  S << "Void shells :" << endl << "Orientation flags : ";
  IGESData_DumpEntities(S,dumper,-level,1, ent->NbVoidShells(),ent->VoidShell);
  S << endl;
  if (level > 4)
    {
      S << "[ " << endl;
      if (ent->NbVoidShells() > 0)
	{
          Standard_Integer upper = ent->NbVoidShells();
          for (Standard_Integer i = 1; i <= upper; i ++)
	    {
              S << "[" << i << "]:  ";
              S << "Void shell : ";
              dumper.Dump (ent->VoidShell(i),S, sublevel);
              S << "  - Orientation flag : ";
              if (ent->VoidOrientationFlag(i)) S << "True"  << endl;
              else		               S << "False" << endl;
	    }
	}
      S << " ]" << endl;
    }
  S << endl;
}

