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
#include <IGESDraw_ConnectPoint.hxx>
#include <IGESDraw_HArray1OfConnectPoint.hxx>
#include <IGESDraw_NetworkSubfigureDef.hxx>
#include <IGESDraw_ToolNetworkSubfigureDef.hxx>
#include <IGESGraph_TextDisplayTemplate.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TCollection_HAsciiString.hxx>

IGESDraw_ToolNetworkSubfigureDef::IGESDraw_ToolNetworkSubfigureDef ()    {  }


void IGESDraw_ToolNetworkSubfigureDef::ReadOwnParams
  (const Handle(IGESDraw_NetworkSubfigureDef)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  //Standard_Boolean  st; //szv#4:S4163:12Mar99 not needed

  Standard_Integer tempDepth, tempNbEntities1, tempTypeFlag, tempNbEntities2;
  Handle(TCollection_HAsciiString) tempName, tempDesignator;
  Handle(IGESGraph_TextDisplayTemplate) tempTemplate;
  Handle(IGESData_HArray1OfIGESEntity)  tempEntities;
  Handle(IGESDraw_HArray1OfConnectPoint) tempPointEntities;

  //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadInteger(PR.Current(), "Depth Of Subfigure", tempDepth);
  PR.ReadText(PR.Current(), "Subfigure Name", tempName);

  //st = PR.ReadInteger(PR.Current(), "Number Of Child Entities", tempNbEntities1); //szv#4:S4163:12Mar99 moved in if
  if (PR.ReadInteger(PR.Current(), "Number Of Child Entities", tempNbEntities1)) {
    // Initialize HArray1 only if there is no error reading its Length
    if (tempNbEntities1 < 0)
      PR.AddFail("Number Of Child Entities : Not Positive");
    else if (tempNbEntities1 > 0)
      PR.ReadEnts(IR,PR.CurrentList(tempNbEntities1),"Child Entities",tempEntities); //szv#4:S4163:12Mar99 `st=` not needed
//      tempEntities = new IGESData_HArray1OfIGESEntity (1,tempNbEntities1);
  }

  // Read the HArray1 only if its Length was read without any Error
/*
  if (! tempEntities.IsNull()) {
    Handle(IGESData_IGESEntity) tempEntity1;
    Standard_Integer I;
    for (I = 1; I <= tempNbEntities1; I++) {
      st = PR.ReadEntity(IR, PR.Current(), "Associated Entity",
			 tempEntity1);
      if (st) tempEntities->SetValue(I, tempEntity1);
    }
  }
*/
  PR.ReadInteger(PR.Current(), "Type Flag", tempTypeFlag); //szv#4:S4163:12Mar99 `st=` not needed

  if (PR.DefinedElseSkip())
    PR.ReadText(PR.Current(), "Primary Reference Designator", tempDesignator); //szv#4:S4163:12Mar99 `st=` not needed
  else PR.AddWarning("Primary Reference Designator : Null");

  Standard_Boolean st = PR.ReadEntity(IR, PR.Current(), "Primary Reference Designator",
				      STANDARD_TYPE(IGESGraph_TextDisplayTemplate), tempTemplate,
				      Standard_True);

  if (PR.DefinedElseSkip())
    st = PR.ReadInteger(PR.Current(), "Number Of Connect Points", tempNbEntities2);
  else tempNbEntities2 = 0;
  if (st) {
    // Initialise HArray1 only if there is no error reading its Length
    if (tempNbEntities2 < 0)
      PR.AddFail("Number Of Connect Points : Less Than Zero");
    else if (tempNbEntities2 > 0) tempPointEntities =
      new IGESDraw_HArray1OfConnectPoint (1, tempNbEntities2);
  }

  // Read the HArray1 only if its Length was read without any Error
  if (! tempPointEntities.IsNull()) {
    Handle(IGESDraw_ConnectPoint) tempConnectPoint;
    Standard_Integer I;
    for (I = 1; I <= tempNbEntities2; I++) {
      //st = PR.ReadEntity(IR, PR.Current(),"Associated Connect Point Entity",
			   //STANDARD_TYPE(IGESDraw_ConnectPoint), tempConnectPoint,
			   //Standard_True); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadEntity(IR, PR.Current(),"Associated Connect Point Entity",
			STANDARD_TYPE(IGESDraw_ConnectPoint), tempConnectPoint, Standard_True))
	tempPointEntities->SetValue(I, tempConnectPoint);
    }
  }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init
    (tempDepth, tempName, tempEntities, tempTypeFlag, tempDesignator,
     tempTemplate, tempPointEntities);
}

void IGESDraw_ToolNetworkSubfigureDef::WriteOwnParams
  (const Handle(IGESDraw_NetworkSubfigureDef)& ent, IGESData_IGESWriter& IW)  const
{
  Standard_Integer up  = ent->NbEntities();
  IW.Send(ent->Depth());
  IW.Send(ent->Name());
  IW.Send(up);
  Standard_Integer I;
  for (I = 1; I <= up; I++)
    IW.Send(ent->Entity(I));
  IW.Send(ent->TypeFlag());
  IW.Send(ent->Designator());
  IW.Send(ent->DesignatorTemplate());
  up  = ent->NbPointEntities();
  IW.Send(up);
  for (I = 1; I <= up; I++)
    IW.Send(ent->PointEntity(I));
}

void  IGESDraw_ToolNetworkSubfigureDef::OwnShared
  (const Handle(IGESDraw_NetworkSubfigureDef)& ent, Interface_EntityIterator& iter) const
{
  Standard_Integer I;
  Standard_Integer up  = ent->NbEntities();
  for (I = 1; I <= up; I++)
    iter.GetOneItem(ent->Entity(I));
  up  = ent->NbPointEntities();
  for (I = 1; I <= up; I++)
    iter.GetOneItem(ent->PointEntity(I));
}

void IGESDraw_ToolNetworkSubfigureDef::OwnCopy
  (const Handle(IGESDraw_NetworkSubfigureDef)& another,
   const Handle(IGESDraw_NetworkSubfigureDef)& ent, Interface_CopyTool& TC) const
{
  Standard_Integer tempDepth = another->Depth();
  Handle(TCollection_HAsciiString) tempName =
    new TCollection_HAsciiString(another->Name());
  Handle(IGESData_HArray1OfIGESEntity) tempEntities;
  Standard_Integer up  = another->NbEntities();
  if (up > 0) tempEntities =  new IGESData_HArray1OfIGESEntity (1,up);
  Standard_Integer I;
  for (I = 1; I <= up; I++) {
    DeclareAndCast(IGESData_IGESEntity, tempEntity,
		   TC.Transferred(another->Entity(I)));
    tempEntities->SetValue(I, tempEntity);
  }
  Standard_Integer tempTypeFlag = another->TypeFlag();
  Handle(TCollection_HAsciiString) tempDesignator;
  if (!another->Designator().IsNull()) tempDesignator =
    new TCollection_HAsciiString(another->Designator());
  up  = another->NbPointEntities();
  Handle(IGESDraw_HArray1OfConnectPoint) tempPointEntities;
  if (up > 0) tempPointEntities = new IGESDraw_HArray1OfConnectPoint (1,up);
  for (I = 1; I <= up; I++) {
    if (another->HasPointEntity(I)) {
      DeclareAndCast(IGESDraw_ConnectPoint, tempPointEntity,
		     TC.Transferred(another->PointEntity(I)));
      tempPointEntities->SetValue(I, tempPointEntity);
    }
  }
  if (another->HasDesignatorTemplate()) {
    DeclareAndCast(IGESGraph_TextDisplayTemplate, tempDesignatorTemplate,
		   TC.Transferred(another->DesignatorTemplate()));

    ent->Init(tempDepth, tempName, tempEntities, tempTypeFlag,
	      tempDesignator, tempDesignatorTemplate, tempPointEntities);
  }
  else {
    Handle(IGESGraph_TextDisplayTemplate) tempDesignatorTemplate;

    ent->Init(tempDepth, tempName, tempEntities, tempTypeFlag,
	      tempDesignator, tempDesignatorTemplate, tempPointEntities);
  }
}

IGESData_DirChecker IGESDraw_ToolNetworkSubfigureDef::DirChecker
  (const Handle(IGESDraw_NetworkSubfigureDef)& /*ent*/)  const
{
  IGESData_DirChecker DC(320, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.LineWeight(IGESData_DefValue);
  DC.BlankStatusIgnored();
  DC.UseFlagRequired(2);
  DC.GraphicsIgnored(1);

  return DC;
}

void IGESDraw_ToolNetworkSubfigureDef::OwnCheck
  (const Handle(IGESDraw_NetworkSubfigureDef)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach)  const
{
  if ((ent->TypeFlag() < 0) || (ent->TypeFlag() > 2))
    ach->AddFail("TypeFlag has Invalid value");
  if (ent->Designator().IsNull())
    ach->AddFail("Primary Reference Designator : not defined");
}

void IGESDraw_ToolNetworkSubfigureDef::OwnDump
  (const Handle(IGESDraw_NetworkSubfigureDef)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level)  const
{
  Standard_Integer tempSubLevel = (level <= 4) ? 0 : 1;

  S << "IGESDraw_NetworkSubfigureDef" << endl;

  S << "Depth Of Subfigure(Nesting)  : " << ent->Depth() << endl;
  S << "Name Of Subfigure            : ";
  IGESData_DumpString(S,ent->Name());
  S << endl << "Associated Entities          : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbEntities(),ent->Entity);
  S << endl << "Type Flag : " << ent->TypeFlag() << endl;
  S << "Primary Reference Designator : ";
  IGESData_DumpString(S,ent->Designator());
  S << endl << "Text Display Template Entity : ";
  dumper.Dump(ent->DesignatorTemplate(),S, tempSubLevel);
  S << endl << "Connect Point Entities       : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbPointEntities(),ent->PointEntity);
  S << endl;
}
