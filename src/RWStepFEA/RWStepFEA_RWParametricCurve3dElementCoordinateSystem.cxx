// File:	RWStepFEA_RWParametricCurve3dElementCoordinateSystem.cxx
// Created:	Thu Dec 12 17:51:07 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepFEA_RWParametricCurve3dElementCoordinateSystem.ixx>

//=======================================================================
//function : RWStepFEA_RWParametricCurve3dElementCoordinateSystem
//purpose  : 
//=======================================================================

RWStepFEA_RWParametricCurve3dElementCoordinateSystem::RWStepFEA_RWParametricCurve3dElementCoordinateSystem ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWParametricCurve3dElementCoordinateSystem::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                     const Standard_Integer num,
                                                                     Handle(Interface_Check)& ach,
                                                                     const Handle(StepFEA_ParametricCurve3dElementCoordinateSystem) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"parametric_curve3d_element_coordinate_system") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of ParametricCurve3dElementCoordinateSystem

  Handle(StepFEA_ParametricCurve3dElementCoordinateDirection) aDirection;
  data->ReadEntity (num, 2, "direction", ach, STANDARD_TYPE(StepFEA_ParametricCurve3dElementCoordinateDirection), aDirection);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aDirection);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWParametricCurve3dElementCoordinateSystem::WriteStep (StepData_StepWriter& SW,
                                                                      const Handle(StepFEA_ParametricCurve3dElementCoordinateSystem) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Own fields of ParametricCurve3dElementCoordinateSystem

  SW.Send (ent->Direction());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWParametricCurve3dElementCoordinateSystem::Share (const Handle(StepFEA_ParametricCurve3dElementCoordinateSystem) &ent,
                                                                  Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of ParametricCurve3dElementCoordinateSystem

  iter.AddItem (ent->Direction());
}
