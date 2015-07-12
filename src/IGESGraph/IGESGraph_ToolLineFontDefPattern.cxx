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
#include <IGESGraph_LineFontDefPattern.hxx>
#include <IGESGraph_ToolLineFontDefPattern.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfReal.hxx>

IGESGraph_ToolLineFontDefPattern::IGESGraph_ToolLineFontDefPattern ()    {  }


void IGESGraph_ToolLineFontDefPattern::ReadOwnParams
  (const Handle(IGESGraph_LineFontDefPattern)& ent,
   const Handle(IGESData_IGESReaderData)& /*IR*/, IGESData_ParamReader& PR) const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  Standard_Integer                 tempNbSeg;
  Handle(TCollection_HAsciiString) tempDisplayPattern;
  Handle(TColStd_HArray1OfReal)    tempSegmentLengths;

  if (PR.ReadInteger(PR.Current(), "Number of Visible-Blank Segments", tempNbSeg)) { //szv#4:S4163:12Mar99 `st=` not needed
    // Initialise HArray1 only if there is no error reading its Length
    if (tempNbSeg <= 0)  PR.AddFail("Number of Visible-Blank Segments : Not Positive");
    else  tempSegmentLengths = new TColStd_HArray1OfReal(1, tempNbSeg);
  }

  // Read the HArray1 only if its Length was read without any Error
  if (! tempSegmentLengths.IsNull()) {
    Standard_Integer I;
    for (I = 1; I <= tempNbSeg; I++) {
      Standard_Real tempReal;
      if (PR.ReadReal(PR.Current(), "Length of Segment", tempReal)) //szv#4:S4163:12Mar99 `st=` not needed
	tempSegmentLengths->SetValue(I, tempReal);
    }
  }

  PR.ReadText(PR.Current(), "Visible-Blank Display Pattern", tempDisplayPattern); //szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempSegmentLengths, tempDisplayPattern);
}

void IGESGraph_ToolLineFontDefPattern::WriteOwnParams
  (const Handle(IGESGraph_LineFontDefPattern)& ent, IGESData_IGESWriter& IW)  const
{
  Standard_Integer up  = ent->NbSegments();
  IW.Send(up);
  Standard_Integer I;
  for (I = 1; I <= up; I++)
    IW.Send(ent->Length(I));
  IW.Send(ent->DisplayPattern());
}

void  IGESGraph_ToolLineFontDefPattern::OwnShared
  (const Handle(IGESGraph_LineFontDefPattern)& /*ent*/, Interface_EntityIterator& /*iter*/) const
{
}

void IGESGraph_ToolLineFontDefPattern::OwnCopy
  (const Handle(IGESGraph_LineFontDefPattern)& another,
   const Handle(IGESGraph_LineFontDefPattern)& ent, Interface_CopyTool& /*TC*/) const
{
  Handle(TColStd_HArray1OfReal) tempSegmentLengths =
    new TColStd_HArray1OfReal(1, another->NbSegments());
  Standard_Integer I;
  Standard_Integer up  = another->NbSegments();
  for (I = 1; I <= up; I++)
    tempSegmentLengths->SetValue(I, another->Length(I));
  Handle(TCollection_HAsciiString) tempDisplayPattern =
    new TCollection_HAsciiString(another->DisplayPattern());

  ent->Init(tempSegmentLengths, tempDisplayPattern);
}

IGESData_DirChecker IGESGraph_ToolLineFontDefPattern::DirChecker
  (const Handle(IGESGraph_LineFontDefPattern)& /*ent*/)  const
{
  IGESData_DirChecker DC(304, 2);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefValue);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusRequired(0);
  DC.UseFlagRequired(2);
  DC.HierarchyStatusIgnored();

  return DC;
}

void IGESGraph_ToolLineFontDefPattern::OwnCheck
  (const Handle(IGESGraph_LineFontDefPattern)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach)  const
{
  if (ent->RankLineFont() == 0)
    ach->AddWarning("Line Font Rank is zero");
  else if (ent->RankLineFont() < 1 || ent->RankLineFont() > 5)
    ach->AddWarning("Invalid Value As Line Font Rank(Valid Range 1 to 5)");
}

void IGESGraph_ToolLineFontDefPattern::OwnDump
  (const Handle(IGESGraph_LineFontDefPattern)& ent, const IGESData_IGESDumper& /*dumper*/,
   const Handle(Message_Messenger)& S, const Standard_Integer level)  const
{
  S << "IGESGraph_LineFontDefPattern" << endl;

  S << "Visible-Blank Segments : ";
  Standard_Integer nb = ent->NbSegments();
  IGESData_DumpVals(S ,level,1, nb,ent->Length);
  S <<endl << "Display Pattern : ";
  IGESData_DumpString(S,ent->DisplayPattern());
  S << endl;
  if (level > 4) {
    S << " -> Which Segments are Visible (the others are Blank) : " << endl;
    for (Standard_Integer I = 1; I <= nb; I ++) {
      if (ent->IsVisible(I)) S << "  " << I;
    }
    S << endl;
  }
}
