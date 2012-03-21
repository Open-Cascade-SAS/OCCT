// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <RWStepElement_RWUniformSurfaceSection.ixx>

//=======================================================================
//function : RWStepElement_RWUniformSurfaceSection
//purpose  : 
//=======================================================================

RWStepElement_RWUniformSurfaceSection::RWStepElement_RWUniformSurfaceSection ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepElement_RWUniformSurfaceSection::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                      const Standard_Integer num,
                                                      Handle(Interface_Check)& ach,
                                                      const Handle(StepElement_UniformSurfaceSection) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,6,ach,"uniform_surface_section") ) return;

  // Inherited fields of SurfaceSection

  StepElement_MeasureOrUnspecifiedValue aSurfaceSection_Offset;
  data->ReadEntity (num, 1, "surface_section.offset", ach, aSurfaceSection_Offset);

  StepElement_MeasureOrUnspecifiedValue aSurfaceSection_NonStructuralMass;
  data->ReadEntity (num, 2, "surface_section.non_structural_mass", ach, aSurfaceSection_NonStructuralMass);

  StepElement_MeasureOrUnspecifiedValue aSurfaceSection_NonStructuralMassOffset;
  data->ReadEntity (num, 3, "surface_section.non_structural_mass_offset", ach, aSurfaceSection_NonStructuralMassOffset);

  // Own fields of UniformSurfaceSection

  Standard_Real aThickness;
  data->ReadReal (num, 4, "thickness", ach, aThickness);

  StepElement_MeasureOrUnspecifiedValue aBendingThickness;
  data->ReadEntity (num, 5, "bending_thickness", ach, aBendingThickness);

  StepElement_MeasureOrUnspecifiedValue aShearThickness;
  data->ReadEntity (num, 6, "shear_thickness", ach, aShearThickness);

  // Initialize entity
  ent->Init(aSurfaceSection_Offset,
            aSurfaceSection_NonStructuralMass,
            aSurfaceSection_NonStructuralMassOffset,
            aThickness,
            aBendingThickness,
            aShearThickness);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepElement_RWUniformSurfaceSection::WriteStep (StepData_StepWriter& SW,
                                                       const Handle(StepElement_UniformSurfaceSection) &ent) const
{

  // Inherited fields of SurfaceSection

  SW.Send (ent->StepElement_SurfaceSection::Offset().Value());

  SW.Send (ent->StepElement_SurfaceSection::NonStructuralMass().Value());

  SW.Send (ent->StepElement_SurfaceSection::NonStructuralMassOffset().Value());

  // Own fields of UniformSurfaceSection

  SW.Send (ent->Thickness());

  SW.Send (ent->BendingThickness().Value());

  SW.Send (ent->ShearThickness().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepElement_RWUniformSurfaceSection::Share (const Handle(StepElement_UniformSurfaceSection) &ent,
                                                   Interface_EntityIterator& iter) const
{

  // Inherited fields of SurfaceSection
/*  CKY 17JUN04. Content is made of REAL and ENUM. No entity !
  iter.AddItem (ent->StepElement_SurfaceSection::Offset().Value());

  iter.AddItem (ent->StepElement_SurfaceSection::NonStructuralMass().Value());

  iter.AddItem (ent->StepElement_SurfaceSection::NonStructuralMassOffset().Value());

  // Own fields of UniformSurfaceSection

  iter.AddItem (ent->BendingThickness().Value());

  iter.AddItem (ent->ShearThickness().Value());
*/
}
