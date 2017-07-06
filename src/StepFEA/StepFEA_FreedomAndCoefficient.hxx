// Created on: 2002-12-14
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

#ifndef _StepFEA_FreedomAndCoefficient_HeaderFile
#define _StepFEA_FreedomAndCoefficient_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepFEA_DegreeOfFreedom.hxx>
#include <StepElement_MeasureOrUnspecifiedValue.hxx>
#include <Standard_Transient.hxx>
class StepFEA_DegreeOfFreedom;
class StepElement_MeasureOrUnspecifiedValue;


class StepFEA_FreedomAndCoefficient;
DEFINE_STANDARD_HANDLE(StepFEA_FreedomAndCoefficient, Standard_Transient)

//! Representation of STEP entity FreedomAndCoefficient
class StepFEA_FreedomAndCoefficient : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepFEA_FreedomAndCoefficient();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const StepFEA_DegreeOfFreedom& aFreedom, const StepElement_MeasureOrUnspecifiedValue& aA);
  
  //! Returns field Freedom
  Standard_EXPORT StepFEA_DegreeOfFreedom Freedom() const;
  
  //! Set field Freedom
  Standard_EXPORT void SetFreedom (const StepFEA_DegreeOfFreedom& Freedom);
  
  //! Returns field A
  Standard_EXPORT StepElement_MeasureOrUnspecifiedValue A() const;
  
  //! Set field A
  Standard_EXPORT void SetA (const StepElement_MeasureOrUnspecifiedValue& A);




  DEFINE_STANDARD_RTTIEXT(StepFEA_FreedomAndCoefficient,Standard_Transient)

protected:




private:


  StepFEA_DegreeOfFreedom theFreedom;
  StepElement_MeasureOrUnspecifiedValue theA;


};







#endif // _StepFEA_FreedomAndCoefficient_HeaderFile
