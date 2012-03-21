// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <RWStepVisual_RWCurveStyle.ixx>
#include <StepVisual_CurveStyleFontSelect.hxx>
#include <StepBasic_SizeSelect.hxx>
#include <StepVisual_Colour.hxx>
#include <Interface_EntityIterator.hxx>
#include <StepVisual_CurveStyle.hxx>
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

