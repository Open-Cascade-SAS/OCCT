// File:	RWStepFEA_RWCurveElementLocation.cxx
// Created:	Thu Dec 12 17:51:04 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepFEA_RWCurveElementLocation.ixx>

//=======================================================================
//function : RWStepFEA_RWCurveElementLocation
//purpose  : 
//=======================================================================

RWStepFEA_RWCurveElementLocation::RWStepFEA_RWCurveElementLocation ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWCurveElementLocation::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                 const Standard_Integer num,
                                                 Handle(Interface_Check)& ach,
                                                 const Handle(StepFEA_CurveElementLocation) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"curve_element_location") ) return;

  // Own fields of CurveElementLocation

  Handle(StepFEA_FeaParametricPoint) aCoordinate;
  data->ReadEntity (num, 1, "coordinate", ach, STANDARD_TYPE(StepFEA_FeaParametricPoint), aCoordinate);

  // Initialize entity
  ent->Init(aCoordinate);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWCurveElementLocation::WriteStep (StepData_StepWriter& SW,
                                                  const Handle(StepFEA_CurveElementLocation) &ent) const
{

  // Own fields of CurveElementLocation

  SW.Send (ent->Coordinate());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWCurveElementLocation::Share (const Handle(StepFEA_CurveElementLocation) &ent,
                                              Interface_EntityIterator& iter) const
{

  // Own fields of CurveElementLocation

  iter.AddItem (ent->Coordinate());
}
