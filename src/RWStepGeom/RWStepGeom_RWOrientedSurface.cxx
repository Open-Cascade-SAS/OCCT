// File:	RWStepGeom_RWOrientedSurface.cxx
// Created:	Fri Jan  4 17:42:44 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <RWStepGeom_RWOrientedSurface.ixx>

//=======================================================================
//function : RWStepGeom_RWOrientedSurface
//purpose  : 
//=======================================================================

RWStepGeom_RWOrientedSurface::RWStepGeom_RWOrientedSurface ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepGeom_RWOrientedSurface::ReadStep (const Handle(StepData_StepReaderData)& data,
                                             const Standard_Integer num,
                                             Handle(Interface_Check)& ach,
                                             const Handle(StepGeom_OrientedSurface) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"oriented_surface") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of OrientedSurface

  Standard_Boolean aOrientation;
  data->ReadBoolean (num, 2, "orientation", ach, aOrientation);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aOrientation);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepGeom_RWOrientedSurface::WriteStep (StepData_StepWriter& SW,
                                              const Handle(StepGeom_OrientedSurface) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Own fields of OrientedSurface

  SW.SendBoolean (ent->Orientation());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepGeom_RWOrientedSurface::Share (const Handle(StepGeom_OrientedSurface) &/*ent*/,
                                          Interface_EntityIterator& /*iter*/) const
{

  // Inherited fields of RepresentationItem

  // Own fields of OrientedSurface
}
