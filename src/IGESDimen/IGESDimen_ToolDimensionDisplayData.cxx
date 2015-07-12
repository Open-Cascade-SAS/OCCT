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
#include <IGESDimen_DimensionDisplayData.hxx>
#include <IGESDimen_ToolDimensionDisplayData.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>

IGESDimen_ToolDimensionDisplayData::IGESDimen_ToolDimensionDisplayData ()  {  }


void  IGESDimen_ToolDimensionDisplayData::ReadOwnParams
  (const Handle(IGESDimen_DimensionDisplayData)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 moved down
  Standard_Integer tempDimType;
  Standard_Integer tempLabelPos;
  Standard_Integer tempCharSet;
  Handle(TCollection_HAsciiString) tempLString;
  Standard_Real tempWitLineAng;
  Standard_Integer tempDeciSymb;
  Standard_Integer tempTextAlign;
  Standard_Integer tempTextLevel;
  Standard_Integer tempTextPlace;
  Standard_Integer tempArrHeadOrient;
  Standard_Real tempInitVal;
  Standard_Integer tempNbProps;
  Handle(TColStd_HArray1OfInteger) tempSuppleNotes;
  Handle(TColStd_HArray1OfInteger) tempStartInd;
  Handle(TColStd_HArray1OfInteger) tempEndInd;

  //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadInteger(PR.Current(),"Number of Properties",tempNbProps);
  PR.ReadInteger(PR.Current(),"Dimension Type", tempDimType);
  PR.ReadInteger(PR.Current(),"Label Position", tempLabelPos);
  if (PR.DefinedElseSkip())
    PR.ReadInteger(PR.Current(),"Character Set", tempCharSet); //szv#4:S4163:12Mar99 `st=` not needed
  else
    tempCharSet = 1;

  //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadText(PR.Current(),"L String", tempLString);
  PR.ReadInteger(PR.Current(),"Decimal Symbol",tempDeciSymb);
  if (PR.DefinedElseSkip())
    PR.ReadReal(PR.Current(),"Witness Line Angle",tempWitLineAng); //szv#4:S4163:12Mar99 `st=` not needed
  else
    tempWitLineAng = M_PI / 2;

  PR.ReadInteger(PR.Current(),"Text Alignment",tempTextAlign); //szv#4:S4163:12Mar99 `st=` not needed
  if (PR.DefinedElseSkip())
    PR.ReadInteger(PR.Current(),"Text Level",tempTextLevel); //szv#4:S4163:12Mar99 `st=` not needed
  else
    tempTextLevel = 0;

  if (PR.DefinedElseSkip())
    PR.ReadInteger(PR.Current(),"Text Place",tempTextPlace); //szv#4:S4163:12Mar99 `st=` not needed
  else
    tempTextPlace = 0;

  //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadInteger(PR.Current(),"ArrowHeadOrientation",tempArrHeadOrient);
  PR.ReadReal(PR.Current(),"Initial Value",tempInitVal);
  Standard_Integer tempnbval;
  Standard_Boolean st = PR.ReadInteger( PR.Current(), "No. of supplementary notes", tempnbval);
  if (st && tempnbval > 0)
    {
      tempSuppleNotes = new TColStd_HArray1OfInteger(1,tempnbval);
      tempStartInd = new TColStd_HArray1OfInteger(1,tempnbval);
      tempEndInd = new TColStd_HArray1OfInteger(1,tempnbval);
      for (Standard_Integer i = 1; i <= tempnbval; i++)
	{
          Standard_Integer anote,astart,anend;
          //st = PR.ReadInteger(PR.Current(), "Supplementary Notes", anote); //szv#4:S4163:12Mar99 moved in if
	  if (PR.ReadInteger(PR.Current(), "Supplementary Notes", anote))
	    tempSuppleNotes->SetValue(i,anote);
          //st = PR.ReadInteger(PR.Current(),"Start Index", astart); //szv#4:S4163:12Mar99 moved in if
	  if (PR.ReadInteger(PR.Current(),"Start Index", astart))
	    tempStartInd->SetValue(i,astart);
          //st = PR.ReadInteger(PR.Current(),"End Index",anend); //szv#4:S4163:12Mar99 moved in if
	  if (PR.ReadInteger(PR.Current(),"End Index",anend))
	    tempEndInd->SetValue(i,anend);
	}
    }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init
    (tempNbProps,   tempDimType, tempLabelPos, tempCharSet, tempLString,
     tempDeciSymb,  tempWitLineAng, tempTextAlign, tempTextLevel,
     tempTextPlace, tempArrHeadOrient, tempInitVal, tempSuppleNotes,
     tempStartInd,  tempEndInd );
}

void  IGESDimen_ToolDimensionDisplayData::WriteOwnParams
  (const Handle(IGESDimen_DimensionDisplayData)& ent, IGESData_IGESWriter&  IW)
const
{
  IW.Send(ent->NbPropertyValues());
  IW.Send(ent->DimensionType());
  IW.Send(ent->LabelPosition());
  IW.Send(ent->CharacterSet());
  IW.Send(ent->LString());
  IW.Send(ent->DecimalSymbol());
  IW.Send(ent->WitnessLineAngle());
  IW.Send(ent->TextAlignment());
  IW.Send(ent->TextLevel());
  IW.Send(ent->TextPlacement());
  IW.Send(ent->ArrowHeadOrientation());
  IW.Send(ent->InitialValue());

  Standard_Integer nbval = ent->NbSupplementaryNotes();
  IW.Send(nbval);
  for (Standard_Integer i = 1; i <= nbval; i++)
    {
      IW.Send(ent->SupplementaryNote(i));
      IW.Send(ent->StartIndex(i));
      IW.Send(ent->EndIndex(i));
    }
}

void  IGESDimen_ToolDimensionDisplayData::OwnShared
  (const Handle(IGESDimen_DimensionDisplayData)& /* ent */, Interface_EntityIterator& /* iter */) const
{
}

void  IGESDimen_ToolDimensionDisplayData::OwnCopy
  (const Handle(IGESDimen_DimensionDisplayData)& another,
   const Handle(IGESDimen_DimensionDisplayData)& ent, Interface_CopyTool& /* TC */) const
{
  Handle(TColStd_HArray1OfInteger) EndList;
  Handle(TColStd_HArray1OfInteger) StartList;
  Handle(TColStd_HArray1OfInteger) NotesList;

  Standard_Integer upper = another->NbSupplementaryNotes();
  if (upper > 0)
    {
      EndList = new TColStd_HArray1OfInteger(1,upper);
      StartList = new TColStd_HArray1OfInteger(1,upper);
      NotesList = new TColStd_HArray1OfInteger(1,upper);
      for (Standard_Integer i = 1;i <= upper;i++)
	{
          EndList->SetValue(i,another->EndIndex(i));
          StartList->SetValue(i,another->StartIndex(i));
          NotesList->SetValue(i,another->SupplementaryNote(i));
	}
    }
  Standard_Integer tempNbPropertyValues = another->NbPropertyValues();
  Standard_Integer tempDimensionType    = another->DimensionType();
  Standard_Integer tempLabelPos         = another->LabelPosition();
  Standard_Integer tempCharSet          = another->CharacterSet();
  Handle(TCollection_HAsciiString) tempLS =
    new TCollection_HAsciiString(another->LString());
  Standard_Integer tempSymbol           = another->DecimalSymbol();
  Standard_Real    tempAngle            = another->WitnessLineAngle();
  Standard_Integer tempAlign            = another->TextAlignment();
  Standard_Integer tempLevel            = another->TextLevel();
  Standard_Integer tempPlacement        = another->TextPlacement();
  Standard_Integer tempArrowHead        = another->ArrowHeadOrientation();
  Standard_Real    tempInitial          = another->InitialValue();

  ent->Init(tempNbPropertyValues, tempDimensionType,tempLabelPos,tempCharSet,
	    tempLS,tempSymbol,tempAngle,tempAlign,tempLevel,tempPlacement,
	    tempArrowHead,tempInitial,NotesList,StartList,EndList);
}

Standard_Boolean  IGESDimen_ToolDimensionDisplayData::OwnCorrect
  (const Handle(IGESDimen_DimensionDisplayData)& ent) const
{
  Standard_Boolean res = (ent->NbPropertyValues() != 14);
  if (!res) return res;
  Handle(TColStd_HArray1OfInteger) EndList;
  Handle(TColStd_HArray1OfInteger) StartList;
  Handle(TColStd_HArray1OfInteger) NotesList;

  Standard_Integer upper = ent->NbSupplementaryNotes();
  if (upper > 0)
    {
      EndList   = new TColStd_HArray1OfInteger(1,upper);
      StartList = new TColStd_HArray1OfInteger(1,upper);
      NotesList = new TColStd_HArray1OfInteger(1,upper);
      for (Standard_Integer i = 1;i <= upper;i++)
	{
          EndList->SetValue  (i,ent->EndIndex(i));
          StartList->SetValue(i,ent->StartIndex(i));
          NotesList->SetValue(i,ent->SupplementaryNote(i));
	}
    }
  ent->Init
    (14, ent->DimensionType(),ent->LabelPosition(),ent->CharacterSet(),
     ent->LString(),      ent->DecimalSymbol(),ent->WitnessLineAngle(),
     ent->TextAlignment(),ent->TextLevel(),    ent->TextPlacement(),
     ent->ArrowHeadOrientation(),ent->InitialValue(),
     NotesList,StartList,EndList);
  return res;    // nbpropertyvalues=14
}

IGESData_DirChecker  IGESDimen_ToolDimensionDisplayData::DirChecker
  (const Handle(IGESDimen_DimensionDisplayData)& /* ent */ ) const
{
  IGESData_DirChecker DC(406,30); // type=406, Form no. = 30
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.BlankStatusIgnored();
  DC.SubordinateStatusRequired(02);
  DC.UseFlagRequired(02);
  DC.HierarchyStatusIgnored();
  return DC;
}

void  IGESDimen_ToolDimensionDisplayData::OwnCheck
  (const Handle(IGESDimen_DimensionDisplayData)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  if (ent->NbPropertyValues() != 14)
    ach->AddFail("The No. of property values != 14 ");
  if (ent->DimensionType() < 0 || ent->DimensionType() > 2)
    ach->AddFail("Incorrect Dimension Type");
  if (ent->LabelPosition() < 0 || ent->LabelPosition() > 4)
    ach->AddFail("Incorrect Preferred Label Position");
  if (!(ent->CharacterSet() == 1 || ent->CharacterSet() == 1001 ||
        ent->CharacterSet() == 1002 || ent->CharacterSet() == 1003))
    ach->AddFail("Incorrect Character Set");
  if (ent->DecimalSymbol() != 0 && ent->DecimalSymbol() != 1)
    ach->AddFail("Incorrect Decimal Symbol");
  if (ent->TextAlignment() != 0 && ent->TextAlignment() != 1)
    ach->AddFail("Incorrect Text Alignment");
  if (ent->TextLevel() < 0 || ent->TextLevel() > 2)
    ach->AddFail("Incorrect Text Level");
  if (ent->TextPlacement() < 0 || ent->TextPlacement() > 2)
    ach->AddFail("Incorrect Text Placement");
  if (ent->ArrowHeadOrientation() != 0 && ent->ArrowHeadOrientation() != 1)
    ach->AddFail("Incorrect ArrowHead Orientation");
  for (Standard_Integer upper = ent->NbSupplementaryNotes(), i = 1;
       i <= upper; i++) {
    if (ent->SupplementaryNote(i) < 1 ||
	ent->SupplementaryNote(i) > 4)
      ach->AddFail("Incorrect First supplement note");
  }
}

void  IGESDimen_ToolDimensionDisplayData::OwnDump
  (const Handle(IGESDimen_DimensionDisplayData)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESDimen_DimensionDisplayData" << endl;

  S << "No. of property values : " << ent->NbPropertyValues() << endl;

  S << "DimensionType : "  << ent->DimensionType() ;
  switch (ent->DimensionType()) {
    case 0  :  S << " (Ordinary)"  << endl;  break;
    case 1  :  S << " (Reference)" << endl;  break;
    case 2  :  S << " (Basic)"     << endl;  break;
    default :  S << " (Incorrect Value)" << endl;  break;
  }

  S << "Preferred Label Position : "  << ent->LabelPosition();
  switch (ent->LabelPosition()) {
    case 0  :  S << " (Does not exist)"     << endl;  break;
    case 1  :  S << " (Before Measurement)" << endl;  break;
    case 2  :  S << " (After Measurement)"  << endl;  break;
    case 3  :  S << " (Above Measurement)"  << endl;  break;
    case 4  :  S << " (Below Measurement)"  << endl;  break;
    default :  S << " (Incorrect Value)"    << endl;  break;
  }

  S << "Character set interpretation : " << ent->CharacterSet() ;
  switch (ent->CharacterSet()) 
    {
    case 1    : S << " (Standard ASCII)" << endl; break;
    case 1001 : S << " (Symbol Font 1)" << endl; break;
    case 1002 : S << " (Symbol Font 2)" << endl; break;
    case 1003 : S << " (Drafting Font)" << endl; break;
    default   : S << " (Not meaningful)" << endl; break;
  }

  S << "LString : ";
  IGESData_DumpString(S,ent->LString());
  S << endl;

  S << "Decimal Symbol : ";
  if (ent->DecimalSymbol() == 0)  S << "0 (.)" << endl;
  else                            S << "1 (,)" << endl;

  S << "Witness Line Angle : " << ent->WitnessLineAngle() << endl;
  S << "Text Alignment : " ;
  if      (ent->TextAlignment() == 0 )    S << "0 (Horizontal)" << endl;
  else if (ent->TextAlignment() == 1 )    S << "1 (Parallel)" << endl;
  else     S <<  ent->TextAlignment() << " (Incorrect Value)" << endl;

  S << "Text Level : " << ent->TextLevel();
  switch (ent->TextLevel()) {
    case 0  :  S << " (Neither above nor below)" << endl;  break;
    case 1  :  S << " (Above)"                   << endl;  break;
    case 2  :  S << " (Below)"                   << endl;  break;
    default :  S << " (Incorrect Value)"         << endl;  break;
  }

  S << "Preferred Text placement : " << ent->TextPlacement();
  switch (ent->TextPlacement()) {
    case 0  :  S << " (Between witness lines)"               << endl;  break;
    case 1  :  S << " (Outside near the first witness line)" << endl;  break;
    case 2  :  S << " (Outside near second witness line)"    << endl;  break;
    default :  S << " (Incorrect Value)"                     << endl;  break;
  }

  S << "Arrow Head Orientation : "  << ent->ArrowHeadOrientation();
  if      (ent->ArrowHeadOrientation() == 0) S << " (In, pointing out)" <<endl;
  else if (ent->ArrowHeadOrientation() == 1) S << " (Out, pointing in)" <<endl;
  else                                       S << " (Incorrect Value)" << endl;

  Standard_Integer nbnotes = ent->NbSupplementaryNotes();
  S << " Primary Dimension Value : " << ent->InitialValue() << endl;
  S << " Number of Supplementary Notes : " << nbnotes <<endl;
  S << "Supplementary Notes , ";
  S << " Start Index , ";
  S << " End   Index : " << endl;
  IGESData_DumpVals(S,-level,1, nbnotes,ent->EndIndex);
  S << endl;
  if (level > 4)
    for (Standard_Integer i = 1; i <= nbnotes; i ++)
      {
	S << "[" << i << "]: " << endl;
	S << "Supplementary Note : " << ent->SupplementaryNote(i);
	S << ", Start Index : " << ent->StartIndex(i);
	S << ", End Index : "   << ent->EndIndex(i) << endl;
      }
  S << endl;
}
