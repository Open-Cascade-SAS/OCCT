// File:	RWStepFEA_RWAlignedCurve3dElementCoordinateSystem.cxx
// Created:	Thu Dec 12 17:51:03 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepFEA_RWAlignedCurve3dElementCoordinateSystem.ixx>

//=======================================================================
//function : RWStepFEA_RWAlignedCurve3dElementCoordinateSystem
//purpose  : 
//=======================================================================

RWStepFEA_RWAlignedCurve3dElementCoordinateSystem::RWStepFEA_RWAlignedCurve3dElementCoordinateSystem ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWAlignedCurve3dElementCoordinateSystem::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                  const Standard_Integer num,
                                                                  Handle(Interface_Check)& ach,
                                                                  const Handle(StepFEA_AlignedCurve3dElementCoordinateSystem) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"aligned_curve3d_element_coordinate_system") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of AlignedCurve3dElementCoordinateSystem

  Handle(StepFEA_FeaAxis2Placement3d) aCoordinateSystem;
  data->ReadEntity (num, 2, "coordinate_system", ach, STANDARD_TYPE(StepFEA_FeaAxis2Placement3d), aCoordinateSystem);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aCoordinateSystem);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWAlignedCurve3dElementCoordinateSystem::WriteStep (StepData_StepWriter& SW,
                                                                   const Handle(StepFEA_AlignedCurve3dElementCoordinateSystem) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Own fields of AlignedCurve3dElementCoordinateSystem

  SW.Send (ent->CoordinateSystem());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWAlignedCurve3dElementCoordinateSystem::Share (const Handle(StepFEA_AlignedCurve3dElementCoordinateSystem) &ent,
                                                               Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of AlignedCurve3dElementCoordinateSystem

  iter.AddItem (ent->CoordinateSystem());
}
