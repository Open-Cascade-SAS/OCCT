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
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_Status.hxx>
#include <IGESSolid_EdgeList.hxx>
#include <IGESSolid_HArray1OfVertexList.hxx>
#include <IGESSolid_ToolEdgeList.hxx>
#include <IGESSolid_VertexList.hxx>
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
//function : IGESSolid_ToolEdgeList
//purpose  : 
//=======================================================================
IGESSolid_ToolEdgeList::IGESSolid_ToolEdgeList ()
{
}


//=======================================================================
//function : ReadOwnParams
//purpose  : 
//=======================================================================

void IGESSolid_ToolEdgeList::ReadOwnParams(const Handle(IGESSolid_EdgeList)& ent,
                                           const Handle(IGESData_IGESReaderData)& IR,
                                           IGESData_ParamReader& PR) const
{
  // MGE 03/08/98
  //Standard_Boolean st; //szv#4:S4163:12Mar99 moved down
  Standard_Integer length, anint;
  Handle(IGESData_IGESEntity) anent;
  Handle(IGESSolid_VertexList) avert;
  Handle(IGESData_HArray1OfIGESEntity) tempCurves;
  Handle(IGESSolid_HArray1OfVertexList) tempStartVertexList;
  Handle(TColStd_HArray1OfInteger)   tempStartVertexIndex;
  Handle(IGESSolid_HArray1OfVertexList) tempEndVertexList;
  Handle(TColStd_HArray1OfInteger)   tempEndVertexIndex;
  IGESData_Status aStatus;

  Standard_Boolean st = PR.ReadInteger(PR.Current(), length);
  if(!st){
    Message_Msg Msg184("XSTEP_184");
    PR.SendFail(Msg184);
  }
  //st = PR.ReadInteger(PR.Current(), "Number of edges", length);
  if (st && length > 0)
    {
      tempCurves = new IGESData_HArray1OfIGESEntity(1, length);
      tempStartVertexList = new IGESSolid_HArray1OfVertexList(1, length);
      tempStartVertexIndex = new TColStd_HArray1OfInteger(1, length);
      tempEndVertexList = new IGESSolid_HArray1OfVertexList(1, length);
      tempEndVertexIndex = new TColStd_HArray1OfInteger(1, length);
      for (Standard_Integer i=1 ; i<=length ; i++)
	{
          // Curves
          //st = PR.ReadEntity(IR, PR.Current(), Msg185, anent); //szv#4:S4163:12Mar99 moved in if
          //st = PR.ReadEntity(IR, PR.Current(), "Model space curve", anent);
	  if (PR.ReadEntity(IR, PR.Current(), aStatus, anent))
	    tempCurves->SetValue(i, anent);
	  else{
	    Message_Msg Msg185("XSTEP_185");
	    switch(aStatus) {
	    case IGESData_ReferenceError: {  
	      Message_Msg Msg216 ("IGES_216");
	      Msg185.Arg(Msg216.Value());
	      PR.SendFail(Msg185);
	      break; }
	    case IGESData_EntityError: {
	      Message_Msg Msg217 ("IGES_217");
	      Msg185.Arg(Msg217.Value());
	      PR.SendFail(Msg185);
	      break; }
	    default:{
	    }
	    }
	  }
          // Start vertex list
          //st = PR.ReadEntity(IR, PR.Current(), Msg188,
			     //STANDARD_TYPE(IGESSolid_VertexList), avert); //szv#4:S4163:12Mar99 moved in if
          /*
          st = PR.ReadEntity(IR, PR.Current(), "Start vertex list",
			     STANDARD_TYPE(IGESSolid_VertexList), avert);
          */
	  if (PR.ReadEntity(IR, PR.Current(), aStatus, STANDARD_TYPE(IGESSolid_VertexList), avert))
	    tempStartVertexList->SetValue(i, avert);
	  else{
	    Message_Msg Msg188("XSTEP_188");
	    switch(aStatus) {
	    case IGESData_ReferenceError: {  
	      Message_Msg Msg216 ("IGES_216");
	      Msg188.Arg(Msg216.Value());
	      PR.SendFail(Msg188);
	      break; }
	    case IGESData_EntityError: {
	      Message_Msg Msg217 ("IGES_217");
	      Msg188.Arg(Msg217.Value());
	      PR.SendFail(Msg188);
	      break; }
	    case IGESData_TypeError: {
	      Message_Msg Msg218 ("IGES_218");
	      Msg188.Arg(Msg218.Value());
	      PR.SendFail(Msg188);
	      break; }
	    default:{
	    }
	    }
	  }

          // Start vertex index
          //st = PR.ReadInteger(PR.Current(), Msg186, anint); //szv#4:S4163:12Mar99 moved in if
          //st = PR.ReadInteger(PR.Current(), "Start vertex index", anint);
	  if (PR.ReadInteger(PR.Current(), anint))
	    tempStartVertexIndex->SetValue(i, anint);
	  else{
	    Message_Msg Msg186("XSTEP_186");
	    PR.SendFail(Msg186);
	  }

          // End vertex list
          //st = PR.ReadEntity(IR, PR.Current(),Msg189 ,
			     //STANDARD_TYPE(IGESSolid_VertexList), avert); //szv#4:S4163:12Mar99 moved in if
          /*
          st = PR.ReadEntity(IR, PR.Current(), "End vertex list",
			     STANDARD_TYPE(IGESSolid_VertexList), avert);
          */
          if (PR.ReadEntity(IR, PR.Current(), aStatus, STANDARD_TYPE(IGESSolid_VertexList), avert))
	    tempEndVertexList->SetValue(i, avert);
	  else{
	    Message_Msg Msg189("XSTEP_189");
	    switch(aStatus) {
	    case IGESData_ReferenceError: {  
	      Message_Msg Msg216 ("IGES_216");
	      Msg189.Arg(Msg216.Value());
	      PR.SendFail(Msg189);
	      break; }
	    case IGESData_EntityError: {
	      Message_Msg Msg217 ("IGES_217");
	      Msg189.Arg(Msg217.Value());
	      PR.SendFail(Msg189);
	      break; }
	    case IGESData_TypeError: {
	      Message_Msg Msg218 ("IGES_218");
	      Msg189.Arg(Msg218.Value());
	      PR.SendFail(Msg189);
	      break; }
	    default:{
	    }
	    }
	  }
          // End vertex index
          //st = PR.ReadInteger(PR.Current(), Msg187, anint); //szv#4:S4163:12Mar99 moved in if
          //st = PR.ReadInteger(PR.Current(), "End vertex index", anint);
	  if (PR.ReadInteger(PR.Current(), anint))
	    tempEndVertexIndex->SetValue(i, anint);
	  else {
	    Message_Msg Msg187("XSTEP_187");
	    PR.SendFail(Msg187);
	  }
	}
    }
  
  if (st && length <= 0){
    Message_Msg Msg184("XSTEP_184");
    PR.SendFail(Msg184);
  }
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  if (length > 0) ent->Init
    (tempCurves, tempStartVertexList, tempStartVertexIndex,
     tempEndVertexList, tempEndVertexIndex);
}


//=======================================================================
//function : WriteOwnParams
//purpose  : 
//=======================================================================

void IGESSolid_ToolEdgeList::WriteOwnParams(const Handle(IGESSolid_EdgeList)& ent,
                                            IGESData_IGESWriter& IW) const
{
  Standard_Integer length = ent->NbEdges();

  IW.Send(length);
  for (Standard_Integer i = 1; i <= length; i ++)
    {
      IW.Send(ent->Curve(i));
      IW.Send(ent->StartVertexList(i));
      IW.Send(ent->StartVertexIndex(i));
      IW.Send(ent->EndVertexList(i));
      IW.Send(ent->EndVertexIndex(i));
    }
}


//=======================================================================
//function : OwnShared
//purpose  : 
//=======================================================================

void IGESSolid_ToolEdgeList::OwnShared(const Handle(IGESSolid_EdgeList)& ent,
                                       Interface_EntityIterator& iter) const
{
  Standard_Integer length = ent->NbEdges();
  for (Standard_Integer i = 1; i <= length; i ++)
    {
      iter.GetOneItem(ent->Curve(i));
      iter.GetOneItem(ent->StartVertexList(i));
      iter.GetOneItem(ent->EndVertexList(i));
    }
}


//=======================================================================
//function : OwnCopy
//purpose  : 
//=======================================================================

void IGESSolid_ToolEdgeList::OwnCopy(const Handle(IGESSolid_EdgeList)& another,
                                     const Handle(IGESSolid_EdgeList)& ent,
                                     Interface_CopyTool& TC) const
{
  Standard_Integer length;

  length = another->NbEdges();
  Handle(IGESData_HArray1OfIGESEntity) tempCurves =
    new IGESData_HArray1OfIGESEntity(1, length);
  Handle(IGESSolid_HArray1OfVertexList) tempStartVertexList =
    new IGESSolid_HArray1OfVertexList(1, length);
  Handle(TColStd_HArray1OfInteger)   tempStartVertexIndex =
    new TColStd_HArray1OfInteger(1, length);
  Handle(IGESSolid_HArray1OfVertexList) tempEndVertexList =
    new IGESSolid_HArray1OfVertexList(1, length);
  Handle(TColStd_HArray1OfInteger)   tempEndVertexIndex =
    new TColStd_HArray1OfInteger(1, length);

  for (Standard_Integer i=1 ; i<=length ; i++)
    {
      // Curves
      DeclareAndCast(IGESData_IGESEntity, curve,
		     TC.Transferred(another->Curve(i)));
      tempCurves->SetValue(i, curve);

      // Start vertex list
      DeclareAndCast(IGESSolid_VertexList, start,
		     TC.Transferred(another->StartVertexList(i)));
      tempStartVertexList->SetValue(i, start);

      // Start vertex index
      tempStartVertexIndex->SetValue(i, another->StartVertexIndex(i));

      // End vertex list
      DeclareAndCast(IGESSolid_VertexList, end,
		     TC.Transferred(another->EndVertexList(i)));
      tempEndVertexList->SetValue(i, end);

      // End vertex index
      tempEndVertexIndex->SetValue(i, another->EndVertexIndex(i));
    }

  ent->Init (tempCurves, tempStartVertexList, tempStartVertexIndex,
	     tempEndVertexList, tempEndVertexIndex);
}


//=======================================================================
//function : DirChecker
//purpose  : 
//=======================================================================

IGESData_DirChecker IGESSolid_ToolEdgeList::DirChecker
  (const Handle(IGESSolid_EdgeList)& /* ent */ ) const
{
  IGESData_DirChecker DC(504, 1);

  DC.Structure  (IGESData_DefVoid);
  DC.LineFont   (IGESData_DefVoid);
  DC.LineWeight (IGESData_DefVoid);
  DC.Color      (IGESData_DefVoid);

  DC.SubordinateStatusRequired (1);
  DC.HierarchyStatusRequired (1);
  return DC;
}


//=======================================================================
//function : OwnCheck
//purpose  : 
//=======================================================================

void IGESSolid_ToolEdgeList::OwnCheck(const Handle(IGESSolid_EdgeList)& ent,
                                      const Interface_ShareTool&,
                                      Handle(Interface_Check)& ach) const
{
  // MGE 03/08/98
  // Building of messages
  //========================================
  //Message_Msg Msg184("XSTEP_184");
  //========================================

  if (ent->NbEdges() <= 0) {
    Message_Msg Msg184("XSTEP_184");
    ach->SendFail(Msg184);
  }
}


//=======================================================================
//function : OwnDump
//purpose  : 
//=======================================================================

void IGESSolid_ToolEdgeList::OwnDump(const Handle(IGESSolid_EdgeList)& ent,
                                     const IGESData_IGESDumper& dumper,
                                     const Handle(Message_Messenger)& S,
                                     const Standard_Integer level) const
{
  Standard_Integer i, length = ent->NbEdges();

  S << "IGESSolid_EdgeList" << endl;
  S << "Number of edge tuples : " << length << endl;
  switch (level)
    {
    case 4 :
      S << "Curves : ";
      S << "Start Vertex List : ";
      S << "Start Vertex Index : ";
      IGESData_DumpVals(S ,level,1, length,ent->StartVertexIndex);
      S << endl;
      S << "End Vertex List : ";
      S << "End Vertex Index : ";
      IGESData_DumpVals(S ,level,1, length,ent->EndVertexIndex);
      S << endl;
      break;
    case 5 :
    case 6 :
      S<<" Curve - Vertices. Start : (VertexList,Index)  End : (VertexList,Index)"<<endl;
      for (i = 1; i <= length; i ++)
	{
//[123]:Curve : #1234 - Vertices. Start = (#5678 , 3467)  End = (#1234 , 4664)
	  S << "[" << i << "]:Curve : ";
	  dumper.Dump (ent->Curve(i),S, level-5);
	  S <<" - Vertices. Start : (";
	  dumper.Dump (ent->StartVertexList(i),S, 0);
	  S << " , " << ent->StartVertexIndex(i);
	  S << ")  End : (";
	  dumper.Dump (ent->EndVertexList(i),S, 0);
	  S << " , " << ent->EndVertexIndex(i);
	  S << ")"   << endl;
	}
      break;
      default :
	break;
    }
  S << endl;
}
