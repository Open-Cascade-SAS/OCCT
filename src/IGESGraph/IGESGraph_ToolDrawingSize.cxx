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
#include <IGESGraph_DrawingSize.hxx>
#include <IGESGraph_ToolDrawingSize.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESGraph_ToolDrawingSize::IGESGraph_ToolDrawingSize ()    {  }


void IGESGraph_ToolDrawingSize::ReadOwnParams
  (const Handle(IGESGraph_DrawingSize)& ent,
   const Handle(IGESData_IGESReaderData)& /*IR*/, IGESData_ParamReader& PR) const
{ 
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  Standard_Integer nbPropertyValues;
  Standard_Real    xSize;
  Standard_Real    ySize; 

  // Reading nbPropertyValues(Integer)
  PR.ReadInteger(PR.Current(), "No. of property values", nbPropertyValues); //szv#4:S4163:12Mar99 `st=` not needed
  if (nbPropertyValues != 2)
    PR.AddFail("No. of Property values : Value is not 2");

  // Reading xSize(Real)
  PR.ReadReal (PR.Current(), "Drawing extent along +ve XD axis", xSize); //szv#4:S4163:12Mar99 `st=` not needed

  // Reading ySize(Real)
  PR.ReadReal (PR.Current(), "Drawing extent along +ve YD axis", ySize); //szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(nbPropertyValues, xSize, ySize);
}

void IGESGraph_ToolDrawingSize::WriteOwnParams
  (const Handle(IGESGraph_DrawingSize)& ent, IGESData_IGESWriter& IW)  const
{ 
  IW.Send( ent->NbPropertyValues() );
  IW.Send( ent->XSize() );
  IW.Send( ent->YSize() );
}

void  IGESGraph_ToolDrawingSize::OwnShared
  (const Handle(IGESGraph_DrawingSize)& /*ent*/, Interface_EntityIterator& /*iter*/) const
{
}

void IGESGraph_ToolDrawingSize::OwnCopy
  (const Handle(IGESGraph_DrawingSize)& another,
   const Handle(IGESGraph_DrawingSize)& ent, Interface_CopyTool& /*TC*/) const
{
  ent->Init(2,another->XSize(),another->YSize());
}

Standard_Boolean  IGESGraph_ToolDrawingSize::OwnCorrect
  (const Handle(IGESGraph_DrawingSize)& ent) const
{
  Standard_Boolean res = (ent->NbPropertyValues() != 2);
  if (res) ent->Init(2,ent->XSize(),ent->YSize());    // nbpropertyvalues=2
  return res;
}

IGESData_DirChecker IGESGraph_ToolDrawingSize::DirChecker
  (const Handle(IGESGraph_DrawingSize)& /*ent*/)  const
{ 
  IGESData_DirChecker DC (406, 16);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESGraph_ToolDrawingSize::OwnCheck
  (const Handle(IGESGraph_DrawingSize)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach)  const
{
  if (ent->NbPropertyValues() != 2)
    ach->AddFail("No. of Property values : Value != 2");
}

void IGESGraph_ToolDrawingSize::OwnDump
  (const Handle(IGESGraph_DrawingSize)& ent, const IGESData_IGESDumper& /*dumper*/,
   const Handle(Message_Messenger)& S, const Standard_Integer /*level*/)  const
{
  S << "IGESGraph_DrawingSize" << endl;

  S << "No. of property values : " << ent->NbPropertyValues() << endl;
  S << "Drawing extent along positive X-axis : " << ent->XSize() << endl;
  S << "Drawing extent along positive Y-axis : " << ent->YSize() << endl;
  S << endl;
}
