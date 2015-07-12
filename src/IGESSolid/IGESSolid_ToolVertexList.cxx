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
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESSolid_ToolVertexList.hxx>
#include <IGESSolid_VertexList.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>
#include <TColgp_HArray1OfXYZ.hxx>

// MGE 03/08/98
//=======================================================================
//function : IGESSolid_ToolVertexList
//purpose  : 
//=======================================================================
IGESSolid_ToolVertexList::IGESSolid_ToolVertexList ()
{
}


//=======================================================================
//function : ReadOwnParams
//purpose  : 
//=======================================================================

void IGESSolid_ToolVertexList::ReadOwnParams(const Handle(IGESSolid_VertexList)& ent,
                                             const Handle(IGESData_IGESReaderData)& /* IR */,
                                             IGESData_ParamReader& PR) const
{
  // MGE 03/08/98
  // Building of messages
  //========================================
//  Message_Msg Msg182("XSTEP_182");
//  Message_Msg Msg183("XSTEP_183");
  //========================================

  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Integer nbitems = 0; //szv#4:S4163:12Mar99 `i` moved in for
  //gp_XYZ anXYZ; //szv#4:S4163:12Mar99 moved down
  Handle(TColgp_HArray1OfXYZ) tempVertices;

  //st = PR.ReadInteger(PR.Current(), Msg182, nbitems); //szv#4:S4163:12Mar99 moved in if
  //st = PR.ReadInteger(PR.Current(), "Number of Vertices", nbitems);
  Standard_Boolean sb = PR.ReadInteger(PR.Current(), nbitems);
  if (sb && (nbitems > 0)) {
    
    Message_Msg Msg183("XSTEP_183");
    
    tempVertices = new TColgp_HArray1OfXYZ(1, nbitems);
    
    gp_XYZ anXYZ;
    for (Standard_Integer i = 1; i <= nbitems; i++)
      {
	//st = PR.ReadXYZ(PR.CurrentList(1, 3), Msg183, anXYZ); //szv#4:S4163:12Mar99 moved in if
	//st = PR.ReadXYZ(PR.CurrentList(1, 3), "Vertices", anXYZ);
	if (PR.ReadXYZ(PR.CurrentList(1, 3), Msg183, anXYZ))
	  tempVertices->SetValue(i, anXYZ);
      }
  }
  else {
    Message_Msg Msg182("XSTEP_182");
    PR.SendFail(Msg182);
  }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  if (nbitems > 0) ent->Init (tempVertices);
}


//=======================================================================
//function : WriteOwnParams
//purpose  : 
//=======================================================================

void IGESSolid_ToolVertexList::WriteOwnParams(const Handle(IGESSolid_VertexList)& ent,
                                              IGESData_IGESWriter& IW) const
{
  Standard_Integer nbitems = ent->NbVertices();
  Standard_Integer i;

  IW.Send(nbitems);
  for (i = 1; i <= nbitems; i ++)
    {
      IW.Send(ent->Vertex(i).X());
      IW.Send(ent->Vertex(i).Y());
      IW.Send(ent->Vertex(i).Z());
    }
}


//=======================================================================
//function : OwnShared
//purpose  : 
//=======================================================================

void IGESSolid_ToolVertexList::OwnShared(const Handle(IGESSolid_VertexList)& /* ent */,
                                         Interface_EntityIterator& /* iter */) const
{
}


//=======================================================================
//function : OwnCopy
//purpose  : 
//=======================================================================

void IGESSolid_ToolVertexList::OwnCopy(const Handle(IGESSolid_VertexList)& another,
                                       const Handle(IGESSolid_VertexList)& ent,
                                       Interface_CopyTool& /* TC */) const
{
  Standard_Integer nbitems, i;

  nbitems = another->NbVertices();
  Handle(TColgp_HArray1OfXYZ) tempVertices = new
    TColgp_HArray1OfXYZ(1, nbitems);

  for (i=1; i<=nbitems; i++)
    {
      tempVertices->SetValue(i, another->Vertex(i).XYZ());
    }
  ent->Init(tempVertices);
}


//=======================================================================
//function : DirChecker
//purpose  : 
//=======================================================================

IGESData_DirChecker IGESSolid_ToolVertexList::DirChecker
  (const Handle(IGESSolid_VertexList)& /* ent */ ) const
{
  IGESData_DirChecker DC(502, 1);

  DC.Structure  (IGESData_DefVoid);
  DC.LineFont   (IGESData_DefVoid);
  DC.LineWeight (IGESData_DefVoid);
  DC.Color      (IGESData_DefAny);

  DC.SubordinateStatusRequired (1);
  DC.HierarchyStatusIgnored ();
  return DC;
}


//=======================================================================
//function : OwnCheck
//purpose  : 
//=======================================================================

void IGESSolid_ToolVertexList::OwnCheck(const Handle(IGESSolid_VertexList)& ent,
                                        const Interface_ShareTool&,
                                        Handle(Interface_Check)& ach) const
{
  // MGE 03/08/98
  // Building of messages
  //========================================
  //Message_Msg Msg182("XSTEP_182");
  //========================================

  if (ent->NbVertices() <= 0) {
    Message_Msg Msg182("XSTEP_182");
    ach->SendFail(Msg182);
  }
}


//=======================================================================
//function : OwnDump
//purpose  : 
//=======================================================================

void IGESSolid_ToolVertexList::OwnDump(const Handle(IGESSolid_VertexList)& ent,
                                       const IGESData_IGESDumper& /* dumper */,
                                       const Handle(Message_Messenger)& S,
                                       const Standard_Integer level) const
{
  S << "IGESSolid_VertexList" << endl;

  S << "Vertices : ";
  IGESData_DumpListXYZL(S ,level,1, ent->NbVertices(),ent->Vertex,ent->Location());
  S << endl;
}
