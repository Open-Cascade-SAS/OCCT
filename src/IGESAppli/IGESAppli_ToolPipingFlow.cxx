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

#include <IGESAppli_PipingFlow.hxx>
#include <IGESAppli_ToolPipingFlow.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDraw_ConnectPoint.hxx>
#include <IGESDraw_HArray1OfConnectPoint.hxx>
#include <IGESGraph_HArray1OfTextDisplayTemplate.hxx>
#include <IGESGraph_TextDisplayTemplate.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TCollection_HAsciiString.hxx>

IGESAppli_ToolPipingFlow::IGESAppli_ToolPipingFlow ()    {  }


void  IGESAppli_ToolPipingFlow::ReadOwnParams
  (const Handle(IGESAppli_PipingFlow)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 moved down
  Standard_Integer tempNbContextFlags;
  Standard_Integer tempTypeOfFlow;
  Standard_Integer i, num;
  Handle(IGESData_HArray1OfIGESEntity) tempFlowAssocs;
  Handle(IGESDraw_HArray1OfConnectPoint) tempConnectPoints;
  Handle(IGESData_HArray1OfIGESEntity) tempJoins;
  Handle(Interface_HArray1OfHAsciiString) tempFlowNames;
  Handle(IGESGraph_HArray1OfTextDisplayTemplate) tempTextDisplayTemplates;
  Handle(IGESData_HArray1OfIGESEntity) tempContFlowAssocs;

  //szv#4:S4163:12Mar99 `st=` not needed
  if (PR.DefinedElseSkip())
    PR.ReadInteger(PR.Current(), "Number of Context Flags", tempNbContextFlags);
  else
    tempNbContextFlags = 1;

  if (!PR.ReadInteger(PR.Current(), "Number of Flow Associativities", num)) num = 0;
  if (num > 0) tempFlowAssocs = new IGESData_HArray1OfIGESEntity(1, num);
  else  PR.AddFail("Number of Flow Associativities: Not Positive");

  if (!PR.ReadInteger(PR.Current(), "Number of Connect Points", num)) num = 0;
  if (num > 0) tempConnectPoints = new IGESDraw_HArray1OfConnectPoint(1, num);
  else  PR.AddFail("Number of Connect Points: Not Positive");

  if (!PR.ReadInteger(PR.Current(), "Number of Joins", num)) num = 0;
  if (num > 0) tempJoins = new IGESData_HArray1OfIGESEntity(1, num);
  else  PR.AddFail("Number of Joins: Not Positive");

  if (!PR.ReadInteger(PR.Current(), "Number of Flow Names", num)) num = 0;
  if (num > 0) tempFlowNames = new Interface_HArray1OfHAsciiString(1, num);
  else  PR.AddFail("Number of Flow Names: Not Positive");

  if (!PR.ReadInteger(PR.Current(), "Number of Text Displays", num)) num = 0;
  if (num > 0) tempTextDisplayTemplates = new IGESGraph_HArray1OfTextDisplayTemplate(1, num);
  else  PR.AddFail("Number of Text Displays: Not Positive");

  if (!PR.ReadInteger(PR.Current(), "Number of Continuation Flows", num)) num = 0;
  if (num > 0) tempContFlowAssocs = new IGESData_HArray1OfIGESEntity(1, num);
  else  PR.AddFail("Number of Continuation Flows: Not Positive");

  if (PR.DefinedElseSkip())
    PR.ReadInteger(PR.Current(), "Type of Flow", tempTypeOfFlow);
  else
    tempTypeOfFlow = 0;

  if (!tempFlowAssocs.IsNull())
    for ( num = tempFlowAssocs->Length(), i = 1; i <= num; i++ ) {
      Handle(IGESData_IGESEntity) tempEntity;
      //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadEntity (IR, PR.Current(), "Flow Associativity", STANDARD_TYPE(IGESData_IGESEntity),tempEntity))
	tempFlowAssocs->SetValue(i, tempEntity);
    }

  if (!tempConnectPoints.IsNull())
    for ( num = tempConnectPoints->Length(), i = 1; i <= num; i++ ) {
      Handle(IGESDraw_ConnectPoint) tempEntity;
      if (PR.ReadEntity(IR, PR.Current(), "Connect Point", STANDARD_TYPE(IGESDraw_ConnectPoint),tempEntity))
	tempConnectPoints->SetValue(i, tempEntity);
    }

  if (!tempJoins.IsNull())
    for ( num = tempJoins->Length(), i = 1; i <= num; i++ ) {
      Handle(IGESData_IGESEntity) tempEntity;
      if (PR.ReadEntity(IR, PR.Current(), "Join", tempEntity))
	tempJoins->SetValue(i, tempEntity);
    }

  if (!tempFlowNames.IsNull())
    for ( num = tempFlowNames->Length(), i = 1; i <= num; i++ ) {
      Handle(TCollection_HAsciiString) tempString;
      if (PR.ReadText(PR.Current(), "Flow Name", tempString))
	tempFlowNames->SetValue(i, tempString);
    }

  if (!tempTextDisplayTemplates.IsNull())
    for ( num = tempTextDisplayTemplates->Length(), i = 1; i <= num; i++ ) {
      Handle(IGESGraph_TextDisplayTemplate) tempEntity;
      if (PR.ReadEntity (IR, PR.Current(), "Text Display Template",
			 STANDARD_TYPE(IGESGraph_TextDisplayTemplate),tempEntity))
	tempTextDisplayTemplates->SetValue(i, tempEntity);
    }

  if (!tempContFlowAssocs.IsNull())
    for ( num = tempContFlowAssocs->Length(), i = 1; i <= num; i++ ) {
      Handle(IGESData_IGESEntity) tempEntity;
      if (PR.ReadEntity (IR, PR.Current(), "Continuation Flow Assocs", tempEntity))
	tempContFlowAssocs->SetValue(i, tempEntity);
    }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempNbContextFlags, tempTypeOfFlow, tempFlowAssocs,
	    tempConnectPoints, tempJoins, tempFlowNames,
	    tempTextDisplayTemplates, tempContFlowAssocs);
}

void  IGESAppli_ToolPipingFlow::WriteOwnParams
  (const Handle(IGESAppli_PipingFlow)& ent, IGESData_IGESWriter& IW) const
{
  Standard_Integer i, num;
  IW.Send(ent->NbContextFlags());
  IW.Send(ent->NbFlowAssociativities());
  IW.Send(ent->NbConnectPoints());
  IW.Send(ent->NbJoins());
  IW.Send(ent->NbFlowNames());
  IW.Send(ent->NbTextDisplayTemplates());
  IW.Send(ent->NbContFlowAssociativities());
  IW.Send(ent->TypeOfFlow());
  for ( num = ent->NbFlowAssociativities(), i = 1; i <= num; i++ )
    IW.Send(ent->FlowAssociativity(i));
  for ( num = ent->NbConnectPoints(), i = 1; i <= num; i++ )
    IW.Send(ent->ConnectPoint(i));
  for ( num = ent->NbJoins(), i = 1; i <= num; i++ )
    IW.Send(ent->Join(i));
  for ( num = ent->NbFlowNames(), i = 1; i <= num; i++ )
    IW.Send(ent->FlowName(i));
  for ( num = ent->NbTextDisplayTemplates(), i = 1; i <= num; i++ )
    IW.Send(ent->TextDisplayTemplate(i));
  for ( num = ent->NbContFlowAssociativities(), i = 1; i <= num; i++ )
    IW.Send(ent->ContFlowAssociativity(i));
}

void  IGESAppli_ToolPipingFlow::OwnShared
  (const Handle(IGESAppli_PipingFlow)& ent, Interface_EntityIterator& iter) const
{
  Standard_Integer i, num;
  for ( num = ent->NbFlowAssociativities(), i = 1; i <= num; i++ )
    iter.GetOneItem(ent->FlowAssociativity(i));
  for ( num = ent->NbConnectPoints(), i = 1; i <= num; i++ )
    iter.GetOneItem(ent->ConnectPoint(i));
  for ( num = ent->NbJoins(), i = 1; i <= num; i++ )
    iter.GetOneItem(ent->Join(i));
  for ( num = ent->NbTextDisplayTemplates(), i = 1; i <= num; i++ )
    iter.GetOneItem(ent->TextDisplayTemplate(i));
  for ( num = ent->NbContFlowAssociativities(), i = 1; i <= num; i++ )
    iter.GetOneItem(ent->ContFlowAssociativity(i));
}

void  IGESAppli_ToolPipingFlow::OwnCopy
  (const Handle(IGESAppli_PipingFlow)& another,
   const Handle(IGESAppli_PipingFlow)& ent, Interface_CopyTool& TC) const
{
  Standard_Integer tempNbContextFlags = another->NbContextFlags();
  Standard_Integer tempTypeOfFlow = another->TypeOfFlow();
  Standard_Integer i, num;
  num = another->NbFlowAssociativities();
  Handle(IGESData_HArray1OfIGESEntity) tempFlowAssocs =
    new IGESData_HArray1OfIGESEntity(1, num);
  for ( i = 1; i <= num; i++ )
    {
      DeclareAndCast(IGESData_IGESEntity, new_item,
		     TC.Transferred(another->FlowAssociativity(i)));
      tempFlowAssocs->SetValue(i, new_item);
    }
  num = another->NbConnectPoints();
  Handle(IGESDraw_HArray1OfConnectPoint) tempConnectPoints =
    new IGESDraw_HArray1OfConnectPoint(1, num);
  for ( i = 1; i <= num; i++ )
    {
      DeclareAndCast(IGESDraw_ConnectPoint, new_item,
		     TC.Transferred(another->ConnectPoint(i)));
      tempConnectPoints->SetValue(i, new_item);
    }
  num = another->NbJoins();
  Handle(IGESData_HArray1OfIGESEntity) tempJoins =
    new IGESData_HArray1OfIGESEntity(1, num);
  for ( i = 1; i <= num; i++ )
    {
      DeclareAndCast(IGESData_IGESEntity, new_item,
		     TC.Transferred(another->Join(i)));
      tempJoins->SetValue(i, new_item);
    }
  num = another->NbFlowNames();
  Handle(Interface_HArray1OfHAsciiString) tempFlowNames =
    new Interface_HArray1OfHAsciiString(1, num);
  for ( i = 1; i <= num; i++ )
    tempFlowNames->SetValue
      (i, new TCollection_HAsciiString(another->FlowName(i)));
  num = another->NbTextDisplayTemplates();
  Handle(IGESGraph_HArray1OfTextDisplayTemplate) tempTextDisplayTemplates =
    new IGESGraph_HArray1OfTextDisplayTemplate(1, num);
  for ( i = 1; i <= num; i++ )
    {
      DeclareAndCast(IGESGraph_TextDisplayTemplate, new_item,
		     TC.Transferred(another->TextDisplayTemplate(i)));
      tempTextDisplayTemplates->SetValue(i, new_item);
    }
  num = another->NbContFlowAssociativities();
  Handle(IGESData_HArray1OfIGESEntity) tempContFlowAssocs =
    new IGESData_HArray1OfIGESEntity(1, num);
  for ( i = 1; i <= num; i++ )
    {
      DeclareAndCast(IGESData_IGESEntity, new_item,
		     TC.Transferred(another->ContFlowAssociativity(i)));
      tempContFlowAssocs->SetValue(i, new_item);
    }
  ent->Init (tempNbContextFlags, tempTypeOfFlow, tempFlowAssocs,
	     tempConnectPoints,  tempJoins, tempFlowNames,
	     tempTextDisplayTemplates, tempContFlowAssocs);
}

Standard_Boolean  IGESAppli_ToolPipingFlow::OwnCorrect
  (const Handle(IGESAppli_PipingFlow)& ent) const
{
  return ent->OwnCorrect();    // nbcontextflags = 1
}

IGESData_DirChecker  IGESAppli_ToolPipingFlow::DirChecker
  (const Handle(IGESAppli_PipingFlow)& /* ent  */) const
{
  IGESData_DirChecker DC(402, 20);
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.UseFlagRequired(3);
  DC.HierarchyStatusIgnored();
  return DC;
}

void  IGESAppli_ToolPipingFlow::OwnCheck
  (const Handle(IGESAppli_PipingFlow)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  if (ent->NbContextFlags() != 1)
    ach->AddFail("Number of Context Flags != 1");
  if ((ent->TypeOfFlow() < 0) || (ent->TypeOfFlow() > 2))
    ach->AddFail("Type of Flow != 0,1,2");
}

void  IGESAppli_ToolPipingFlow::OwnDump
  (const Handle(IGESAppli_PipingFlow)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESAppli_PipingFlow" << endl;
  S << "Number of Context Flags : " << ent->NbContextFlags() << endl;
  S << "Type of Flow : " << ent->TypeOfFlow() << endl;
  S << "Flow Associativities : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbFlowAssociativities(),
			ent->FlowAssociativity);
  S << "Connect Points : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbConnectPoints()
			,ent->ConnectPoint);
  S << "Joins : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbJoins(),ent->Join);
  S << "Flow Names : ";
  IGESData_DumpStrings(S ,level,1, ent->NbFlowNames(),ent->FlowName);
  S << "Text Display Templates : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbTextDisplayTemplates(),
			ent->TextDisplayTemplate);
  S << "Continuation Flow Associativities : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbContFlowAssociativities(),
			ent->ContFlowAssociativity);
  S << endl;
}

