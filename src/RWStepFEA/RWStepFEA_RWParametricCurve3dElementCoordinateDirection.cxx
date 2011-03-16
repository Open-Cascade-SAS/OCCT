// File:	RWStepFEA_RWParametricCurve3dElementCoordinateDirection.cxx
// Created:	Thu Dec 12 17:51:07 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepFEA_RWParametricCurve3dElementCoordinateDirection.ixx>

//=======================================================================
//function : RWStepFEA_RWParametricCurve3dElementCoordinateDirection
//purpose  : 
//=======================================================================

RWStepFEA_RWParametricCurve3dElementCoordinateDirection::RWStepFEA_RWParametricCurve3dElementCoordinateDirection ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWParametricCurve3dElementCoordinateDirection::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                        const Standard_Integer num,
                                                                        Handle(Interface_Check)& ach,
                                                                        const Handle(StepFEA_ParametricCurve3dElementCoordinateDirection) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"parametric_curve3d_element_coordinate_direction") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of ParametricCurve3dElementCoordinateDirection

  Handle(StepGeom_Direction) aOrientation;
  data->ReadEntity (num, 2, "orientation", ach, STANDARD_TYPE(StepGeom_Direction), aOrientation);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aOrientation);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWParametricCurve3dElementCoordinateDirection::WriteStep (StepData_StepWriter& SW,
                                                                         const Handle(StepFEA_ParametricCurve3dElementCoordinateDirection) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Own fields of ParametricCurve3dElementCoordinateDirection

  SW.Send (ent->Orientation());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWParametricCurve3dElementCoordinateDirection::Share (const Handle(StepFEA_ParametricCurve3dElementCoordinateDirection) &ent,
                                                                     Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of ParametricCurve3dElementCoordinateDirection

  iter.AddItem (ent->Orientation());
}
