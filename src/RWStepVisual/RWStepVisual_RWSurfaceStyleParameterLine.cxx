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


#include <RWStepVisual_RWSurfaceStyleParameterLine.ixx>
#include <StepVisual_CurveStyle.hxx>
#include <Standard_Integer.hxx>

#include <StepVisual_HArray1OfDirectionCountSelect.hxx>

#include <Interface_EntityIterator.hxx>


#include <StepVisual_SurfaceStyleParameterLine.hxx>


RWStepVisual_RWSurfaceStyleParameterLine::RWStepVisual_RWSurfaceStyleParameterLine () {}

void RWStepVisual_RWSurfaceStyleParameterLine::ReadStep
(const Handle(StepData_StepReaderData)& data,
 const Standard_Integer num,
 Handle(Interface_Check)& ach,
 const Handle(StepVisual_SurfaceStyleParameterLine)& ent) const
{
  
  
  // --- Number of Parameter Control ---
  
  if (!data->CheckNbParams(num,2,ach,"surface_style_parameter_line")) return;
  
  // --- own field : styleOfParameterLines ---
  
  Handle(StepVisual_CurveStyle) aStyleOfParameterLines;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
  data->ReadEntity(num, 1,"style_of_parameter_lines", ach, STANDARD_TYPE(StepVisual_CurveStyle), aStyleOfParameterLines);
  
  // --- own field : directionCounts ---
  // DirectionCount : select de UDirectionCount et VDirectionCount qui sont
  //                  des type INTEGER;
  // Par consequent, on doit trouver dans le fichier :
  //     ... , (U_DIRECTION_COUNT(10), V_DIRECTION_COUNT(1)) );

  Standard_Integer numr, numpr;
  TCollection_AsciiString UType("U_DIRECTION_COUNT");
  TCollection_AsciiString VType("V_DIRECTION_COUNT");
  TCollection_AsciiString TrueType;

  Handle(StepVisual_HArray1OfDirectionCountSelect) aDirectionCounts;
  Standard_Integer aDirectionCountsItem;
  StepVisual_DirectionCountSelect aDirectionCountSelect;

  Standard_Integer nsub2;
  if (data->ReadSubList (num,2,"direction_counts",ach,nsub2)) {
    Standard_Integer nb2 = data->NbParams(nsub2);
    aDirectionCounts = new StepVisual_HArray1OfDirectionCountSelect(1, nb2);
    for (Standard_Integer i2 = 1; i2 <= nb2; i2 ++) {
      // looks for true type :
      //szv#4:S4163:12Mar99 `Standard_Boolean statUV =` not needed
      if (data->ReadTypedParam(nsub2,i2,Standard_True,"direction_count", ach,numr,numpr,TrueType)) {
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	if (data->ReadInteger (numr,numpr,"direction_counts",ach,aDirectionCountsItem)) {
	  if (TrueType == UType) {
	    aDirectionCountSelect.SetUDirectionCount(aDirectionCountsItem);
	    aDirectionCounts->SetValue(i2,aDirectionCountSelect);
	  }
	  else if (TrueType == VType) {
	    aDirectionCountSelect.SetVDirectionCount(aDirectionCountsItem);
	    aDirectionCounts->SetValue(i2,aDirectionCountSelect);
	  }
	  else {
	    ach->AddFail("Parameter #2 (direction_counts) item has illegal TYPE");
	  }
	}
	else {
	  ach->AddFail("Parameter #2 (direction_counts) item is not an INTEGER");
	}
      }
      else {
	ach->AddFail("Parameter #2 (direction_counts) item is not TYPED");
      }
    }
  }
  
  //--- Initialisation of the read entity ---
  
  ent->Init(aStyleOfParameterLines, aDirectionCounts);
}


void RWStepVisual_RWSurfaceStyleParameterLine::WriteStep
(StepData_StepWriter& SW,
 const Handle(StepVisual_SurfaceStyleParameterLine)& ent) const
{
  
  // --- own field : styleOfParameterLines ---
  
  SW.Send(ent->StyleOfParameterLines());
  
  // --- own field : directionCounts ---
  // Attention : a modifier avant utilisation

  SW.Send(ent->DirectionCounts());
}


void RWStepVisual_RWSurfaceStyleParameterLine::Share(const Handle(StepVisual_SurfaceStyleParameterLine)& ent, Interface_EntityIterator& iter) const
{
  
  iter.GetOneItem(ent->StyleOfParameterLines());
}

