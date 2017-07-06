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

#ifndef _StepFEA_CurveElementLocation_HeaderFile
#define _StepFEA_CurveElementLocation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class StepFEA_FeaParametricPoint;


class StepFEA_CurveElementLocation;
DEFINE_STANDARD_HANDLE(StepFEA_CurveElementLocation, Standard_Transient)

//! Representation of STEP entity CurveElementLocation
class StepFEA_CurveElementLocation : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepFEA_CurveElementLocation();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(StepFEA_FeaParametricPoint)& aCoordinate);
  
  //! Returns field Coordinate
  Standard_EXPORT Handle(StepFEA_FeaParametricPoint) Coordinate() const;
  
  //! Set field Coordinate
  Standard_EXPORT void SetCoordinate (const Handle(StepFEA_FeaParametricPoint)& Coordinate);




  DEFINE_STANDARD_RTTIEXT(StepFEA_CurveElementLocation,Standard_Transient)

protected:




private:


  Handle(StepFEA_FeaParametricPoint) theCoordinate;


};







#endif // _StepFEA_CurveElementLocation_HeaderFile
