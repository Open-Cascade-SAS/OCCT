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

#ifndef _StepFEA_ParametricSurface3dElementCoordinateSystem_HeaderFile
#define _StepFEA_ParametricSurface3dElementCoordinateSystem_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <StepFEA_FeaRepresentationItem.hxx>
class TCollection_HAsciiString;


class StepFEA_ParametricSurface3dElementCoordinateSystem;
DEFINE_STANDARD_HANDLE(StepFEA_ParametricSurface3dElementCoordinateSystem, StepFEA_FeaRepresentationItem)

//! Representation of STEP entity ParametricSurface3dElementCoordinateSystem
class StepFEA_ParametricSurface3dElementCoordinateSystem : public StepFEA_FeaRepresentationItem
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepFEA_ParametricSurface3dElementCoordinateSystem();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aRepresentationItem_Name, const Standard_Integer aAxis, const Standard_Real aAngle);
  
  //! Returns field Axis
  Standard_EXPORT Standard_Integer Axis() const;
  
  //! Set field Axis
  Standard_EXPORT void SetAxis (const Standard_Integer Axis);
  
  //! Returns field Angle
  Standard_EXPORT Standard_Real Angle() const;
  
  //! Set field Angle
  Standard_EXPORT void SetAngle (const Standard_Real Angle);




  DEFINE_STANDARD_RTTIEXT(StepFEA_ParametricSurface3dElementCoordinateSystem,StepFEA_FeaRepresentationItem)

protected:




private:


  Standard_Integer theAxis;
  Standard_Real theAngle;


};







#endif // _StepFEA_ParametricSurface3dElementCoordinateSystem_HeaderFile
