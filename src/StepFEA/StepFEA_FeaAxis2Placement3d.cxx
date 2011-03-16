// File:	StepFEA_FeaAxis2Placement3d.cxx
// Created:	Thu Dec 12 17:51:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_FeaAxis2Placement3d.ixx>

//=======================================================================
//function : StepFEA_FeaAxis2Placement3d
//purpose  : 
//=======================================================================

StepFEA_FeaAxis2Placement3d::StepFEA_FeaAxis2Placement3d ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_FeaAxis2Placement3d::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                                        const Handle(StepGeom_CartesianPoint) &aPlacement_Location,
                                        const Standard_Boolean hasAxis2Placement3d_Axis,
                                        const Handle(StepGeom_Direction) &aAxis2Placement3d_Axis,
                                        const Standard_Boolean hasAxis2Placement3d_RefDirection,
                                        const Handle(StepGeom_Direction) &aAxis2Placement3d_RefDirection,
                                        const StepFEA_CoordinateSystemType aSystemType,
                                        const Handle(TCollection_HAsciiString) &aDescription)
{
  StepGeom_Axis2Placement3d::Init(aRepresentationItem_Name,
                                  aPlacement_Location,
                                  hasAxis2Placement3d_Axis,
                                  aAxis2Placement3d_Axis,
                                  hasAxis2Placement3d_RefDirection,
                                  aAxis2Placement3d_RefDirection);

  theSystemType = aSystemType;

  theDescription = aDescription;
}

//=======================================================================
//function : SystemType
//purpose  : 
//=======================================================================

StepFEA_CoordinateSystemType StepFEA_FeaAxis2Placement3d::SystemType () const
{
  return theSystemType;
}

//=======================================================================
//function : SetSystemType
//purpose  : 
//=======================================================================

void StepFEA_FeaAxis2Placement3d::SetSystemType (const StepFEA_CoordinateSystemType aSystemType)
{
  theSystemType = aSystemType;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepFEA_FeaAxis2Placement3d::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepFEA_FeaAxis2Placement3d::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}
