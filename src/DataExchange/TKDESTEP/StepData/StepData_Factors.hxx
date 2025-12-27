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

#ifndef _StepData_Factors_HeaderFile
#define _StepData_Factors_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>

//! Class for using units variables
class StepData_Factors
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor
  Standard_EXPORT StepData_Factors();

  //! Initializes the 3 factors for the conversion of units
  Standard_EXPORT void InitializeFactors(const double theLengthFactor,
                                         const double thePlaneAngleFactor,
                                         const double theSolidAngleFactor);

  //! Sets length unit for current transfer process
  Standard_EXPORT void SetCascadeUnit(const double theUnit);

  //! Returns length unit for current transfer process (mm by default)
  Standard_EXPORT double CascadeUnit() const;

  //! Returns transient length factor for scaling of shapes
  //! at one stage of transfer process
  Standard_EXPORT double LengthFactor() const;

  //! Returns transient plane angle factor for conversion of angles
  //! at one stage of transfer process
  Standard_EXPORT double PlaneAngleFactor() const;

  //! Returns transient solid angle factor for conversion of angles
  //! at one stage of transfer process
  Standard_EXPORT double SolidAngleFactor() const;

  //! Returns transient factor radian degree for conversion of angles
  //! at one stage of transfer process
  Standard_EXPORT double FactorRadianDegree() const;

  //! Returns transient factor degree radian for conversion of angles
  //! at one stage of transfer process
  Standard_EXPORT double FactorDegreeRadian() const;

private:
  double myLengthFactor;
  double myPlaneAngleFactor;
  double mySolidAngleFactor;
  double myFactRD;
  double myFactDR;
  double myCascadeUnit;
};

#endif // _StepData_Factors_HeaderFile
