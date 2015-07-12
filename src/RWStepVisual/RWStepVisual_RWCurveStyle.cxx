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


#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepVisual_RWCurveStyle.hxx>
#include <StepBasic_SizeSelect.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_Colour.hxx>
#include <StepVisual_CurveStyle.hxx>
#include <StepVisual_CurveStyleFontSelect.hxx>

//#include <StepBasic_SizeMember.hxx>
//=======================================================================
//function : RWStepVisual_RWCurveStyle
//purpose  : 
//=======================================================================
RWStepVisual_RWCurveStyle::RWStepVisual_RWCurveStyle()
{
}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepVisual_RWCurveStyle::ReadStep(const Handle(StepData_StepReaderData)& data,
					 const Standard_Integer num,
					 Handle(Interface_Check)& ach,
					 const Handle(StepVisual_CurveStyle)& ent) const
{

  // --- Number of Parameter Control ---
  if (!data->CheckNbParams(num,4,ach,"curve_style")) return;

  // --- own field : name ---
  Handle(TCollection_HAsciiString) aName;
  
  //szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
  data->ReadString (num,1,"name",ach,aName);

  // --- own field : curveFont ---
  // idem RWStepVisual_BooleanOperand.
  // doit etre remis a niveau avant utilisation
  StepVisual_CurveStyleFontSelect aCurveFont;
  data->ReadEntity(num,2,"curve_font",ach,aCurveFont);

  // --- own field : curveWidth ---
  StepBasic_SizeSelect aCurveWidth;
  data->ReadEntity(num,3,"curve_width",ach,aCurveWidth);
//  Handle(StepBasic_SizeMember) memb = new StepBasic_SizeMember;
//  data->ReadMember(num,3,"curve_width",ach,memb);
//  if ( ! memb->HasName() ) {
//    ach->AddWarning("Parameter #3 (curve_width) is not a POSITIVE_LENGTH_MEASURE");
//    memb->SetName ( "POSITIVE_LENGTH_MEASURE" );
//  }
//  aCurveWidth.SetValue(memb);

  // --- own field : curveColour ---
  Handle(StepVisual_Colour) aCurveColour;
  
  //szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
  data->ReadEntity(num, 4,"curve_colour", ach, STANDARD_TYPE(StepVisual_Colour), aCurveColour);

  //--- Initialisation of the read entity ---
  ent->Init(aName, aCurveFont, aCurveWidth, aCurveColour);
}


//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepVisual_RWCurveStyle::WriteStep(StepData_StepWriter& SW,
					  const Handle(StepVisual_CurveStyle)& ent) const
{

  // --- own field : name ---
  SW.Send(ent->Name());

  // --- own field : curveFont ---
  SW.Send(ent->CurveFont().Value());

  // --- own field : curveWidth ---
  SW.Send(ent->CurveWidth().Value());

  // --- own field : curveColour ---
  SW.Send(ent->CurveColour());
}


//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepVisual_RWCurveStyle::Share(const Handle(StepVisual_CurveStyle)& ent, 
				      Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->CurveFont().Value());
  iter.GetOneItem(ent->CurveColour());
}

