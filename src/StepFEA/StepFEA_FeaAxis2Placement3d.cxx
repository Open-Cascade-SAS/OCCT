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
