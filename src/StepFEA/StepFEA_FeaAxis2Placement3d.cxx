// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Standard_Type.hxx>
#include <StepFEA_FeaAxis2Placement3d.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Direction.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepFEA_FeaAxis2Placement3d,StepGeom_Axis2Placement3d)

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
