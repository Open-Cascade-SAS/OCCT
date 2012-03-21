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


#include <RWStepGeom_RWSurfacePatch.ixx>
#include <StepGeom_BoundedSurface.hxx>
#include <StepGeom_TransitionCode.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepGeom_SurfacePatch.hxx>

#include <TCollection_AsciiString.hxx>

#include <TCollection_AsciiString.hxx>


// --- Enum : TransitionCode ---
static TCollection_AsciiString tcDiscontinuous(".DISCONTINUOUS.");
static TCollection_AsciiString tcContSameGradientSameCurvature(".CONT_SAME_GRADIENT_SAME_CURVATURE.");
static TCollection_AsciiString tcContSameGradient(".CONT_SAME_GRADIENT.");
static TCollection_AsciiString tcContinuous(".CONTINUOUS.");

RWStepGeom_RWSurfacePatch::RWStepGeom_RWSurfacePatch () {}

void RWStepGeom_RWSurfacePatch::ReadStep
(const Handle(StepData_StepReaderData)& data,
 const Standard_Integer num,
 Handle(Interface_Check)& ach,
 const Handle(StepGeom_SurfacePatch)& ent) const
{
  
  
  // --- Number of Parameter Control ---
  
  if (!data->CheckNbParams(num,5,ach,"surface_patch")) return;
  
  // --- own field : parentSurface ---
  
  Handle(StepGeom_BoundedSurface) aParentSurface;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
  data->ReadEntity(num, 1,"parent_surface", ach, STANDARD_TYPE(StepGeom_BoundedSurface), aParentSurface);
  
  // --- own field : uTransition ---
  
  StepGeom_TransitionCode aUTransition = StepGeom_tcDiscontinuous;
  if (data->ParamType(num,2) == Interface_ParamEnum) {
    Standard_CString text = data->ParamCValue(num,2);
    if      (tcDiscontinuous.IsEqual(text)) aUTransition = StepGeom_tcDiscontinuous;
    else if (tcContSameGradientSameCurvature.IsEqual(text)) aUTransition = StepGeom_tcContSameGradientSameCurvature;
    else if (tcContSameGradient.IsEqual(text)) aUTransition = StepGeom_tcContSameGradient;
    else if (tcContinuous.IsEqual(text)) aUTransition = StepGeom_tcContinuous;
    else ach->AddFail("Enumeration transition_code has not an allowed value");
  }
  else ach->AddFail("Parameter #2 (u_transition) is not an enumeration");
  
  // --- own field : vTransition ---
  
  StepGeom_TransitionCode aVTransition = StepGeom_tcDiscontinuous;
  if (data->ParamType(num,3) == Interface_ParamEnum) {
    Standard_CString text = data->ParamCValue(num,3);
    if      (tcDiscontinuous.IsEqual(text)) aVTransition = StepGeom_tcDiscontinuous;
    else if (tcContSameGradientSameCurvature.IsEqual(text)) aVTransition = StepGeom_tcContSameGradientSameCurvature;
    else if (tcContSameGradient.IsEqual(text)) aVTransition = StepGeom_tcContSameGradient;
    else if (tcContinuous.IsEqual(text)) aVTransition = StepGeom_tcContinuous;
    else ach->AddFail("Enumeration transition_code has not an allowed value");
  }
  else ach->AddFail("Parameter #3 (v_transition) is not an enumeration");
  
  // --- own field : uSense ---
  
  Standard_Boolean aUSense;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
  data->ReadBoolean (num,4,"u_sense",ach,aUSense);
  
  // --- own field : vSense ---
  
  Standard_Boolean aVSense;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat5 =` not needed
  data->ReadBoolean (num,5,"v_sense",ach,aVSense);
  
  //--- Initialisation of the read entity ---
  
  
  ent->Init(aParentSurface, aUTransition, aVTransition, aUSense, aVSense);
}


void RWStepGeom_RWSurfacePatch::WriteStep
(StepData_StepWriter& SW,
 const Handle(StepGeom_SurfacePatch)& ent) const
{
  
  // --- own field : parentSurface ---
  
  SW.Send(ent->ParentSurface());
  
  // --- own field : uTransition ---
  
  switch(ent->UTransition()) {
  case StepGeom_tcDiscontinuous : SW.SendEnum (tcDiscontinuous); break;
			   case StepGeom_tcContSameGradientSameCurvature : SW.SendEnum (tcContSameGradientSameCurvature); break;
			   case StepGeom_tcContSameGradient : SW.SendEnum (tcContSameGradient); break;
			   case StepGeom_tcContinuous : SW.SendEnum (tcContinuous); break;
			   }
  
	// --- own field : vTransition ---

	switch(ent->VTransition()) {
	  case StepGeom_tcDiscontinuous : SW.SendEnum (tcDiscontinuous); break;
	  case StepGeom_tcContSameGradientSameCurvature : SW.SendEnum (tcContSameGradientSameCurvature); break;
	  case StepGeom_tcContSameGradient : SW.SendEnum (tcContSameGradient); break;
	  case StepGeom_tcContinuous : SW.SendEnum (tcContinuous); break;
	}

	// --- own field : uSense ---

	SW.SendBoolean(ent->USense());

	// --- own field : vSense ---

	SW.SendBoolean(ent->VSense());
}


void RWStepGeom_RWSurfacePatch::Share(const Handle(StepGeom_SurfacePatch)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->ParentSurface());
}

