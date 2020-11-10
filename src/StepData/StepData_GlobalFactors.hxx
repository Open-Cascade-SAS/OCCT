// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _StepData_GlobalFactors_HeaderFile
#define _StepData_GlobalFactors_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>

//! Class for using global units variables
class StepData_GlobalFactors
{

private:

  Standard_EXPORT StepData_GlobalFactors();

public:

  DEFINE_STANDARD_ALLOC
 
  //! Returns a global static object
  Standard_EXPORT static StepData_GlobalFactors& Intance();

  //! Initializes the 3 factors for the conversion of units
  Standard_EXPORT void InitializeFactors(
    const Standard_Real theLengthFactor,
    const Standard_Real thePlaneAngleFactor,
    const Standard_Real theSolidAngleFactor);

  //! Sets length unit for current transfer process
  Standard_EXPORT void SetCascadeUnit(const Standard_Real theUnit);

  //! Returns length unit for current transfer process (mm by default)
  Standard_EXPORT Standard_Real CascadeUnit();

  //! Returns transient length factor for scaling of shapes
  //! at one stage of transfer process
  Standard_EXPORT Standard_Real LengthFactor();

  //! Returns transient plane angle factor for conversion of angles
  //! at one stage of transfer process
  Standard_EXPORT Standard_Real PlaneAngleFactor();

  //! Returns transient solid angle factor for conversion of angles
  //! at one stage of transfer process
  Standard_EXPORT Standard_Real SolidAngleFactor();

  //! Returns transient factor radian degree for conversion of angles
  //! at one stage of transfer process
  Standard_EXPORT Standard_Real FactorRadianDegree();

  //! Returns transient factor degree radian for conversion of angles
  //! at one stage of transfer process
  Standard_EXPORT Standard_Real FactorDegreeRadian();

private:

  Standard_Real myLengthFactor;
  Standard_Real myPlaneAngleFactor;
  Standard_Real mySolidAngleFactor;
  Standard_Real myFactRD;
  Standard_Real myFactDR;
  Standard_Real myCascadeUnit;
};

#endif // _StepData_GlobalFactors_HeaderFile
