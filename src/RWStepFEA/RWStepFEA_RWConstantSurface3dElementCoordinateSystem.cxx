// File:	RWStepFEA_RWConstantSurface3dElementCoordinateSystem.cxx
// Created:	Thu Dec 26 15:06:35 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepFEA_RWConstantSurface3dElementCoordinateSystem.ixx>

//=======================================================================
//function : RWStepFEA_RWConstantSurface3dElementCoordinateSystem
//purpose  : 
//=======================================================================

RWStepFEA_RWConstantSurface3dElementCoordinateSystem::RWStepFEA_RWConstantSurface3dElementCoordinateSystem ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWConstantSurface3dElementCoordinateSystem::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                     const Standard_Integer num,
                                                                     Handle(Interface_Check)& ach,
                                                                     const Handle(StepFEA_ConstantSurface3dElementCoordinateSystem) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"constant_surface3d_element_coordinate_system") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of ConstantSurface3dElementCoordinateSystem

  Standard_Integer aAxis;
  data->ReadInteger (num, 2, "axis", ach, aAxis);

  Standard_Real aAngle;
  data->ReadReal (num, 3, "angle", ach, aAngle);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aAxis,
            aAngle);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWConstantSurface3dElementCoordinateSystem::WriteStep (StepData_StepWriter& SW,
                                                                      const Handle(StepFEA_ConstantSurface3dElementCoordinateSystem) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Own fields of ConstantSurface3dElementCoordinateSystem

  SW.Send (ent->Axis());

  SW.Send (ent->Angle());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWConstantSurface3dElementCoordinateSystem::Share (const Handle(StepFEA_ConstantSurface3dElementCoordinateSystem) &ent,
                                                                  Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of ConstantSurface3dElementCoordinateSystem
}
