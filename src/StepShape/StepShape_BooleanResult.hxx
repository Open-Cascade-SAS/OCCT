// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _StepShape_BooleanResult_HeaderFile
#define _StepShape_BooleanResult_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_BooleanOperator.hxx>
#include <StepShape_BooleanOperand.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
class TCollection_HAsciiString;
class StepShape_BooleanOperand;


class StepShape_BooleanResult;
DEFINE_STANDARD_HANDLE(StepShape_BooleanResult, StepGeom_GeometricRepresentationItem)


class StepShape_BooleanResult : public StepGeom_GeometricRepresentationItem
{

public:

  
  //! Returns a BooleanResult
  Standard_EXPORT StepShape_BooleanResult();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const StepShape_BooleanOperator aOperator, const StepShape_BooleanOperand& aFirstOperand, const StepShape_BooleanOperand& aSecondOperand);
  
  Standard_EXPORT void SetOperator (const StepShape_BooleanOperator aOperator);
  
  Standard_EXPORT StepShape_BooleanOperator Operator() const;
  
  Standard_EXPORT void SetFirstOperand (const StepShape_BooleanOperand& aFirstOperand);
  
  Standard_EXPORT StepShape_BooleanOperand FirstOperand() const;
  
  Standard_EXPORT void SetSecondOperand (const StepShape_BooleanOperand& aSecondOperand);
  
  Standard_EXPORT StepShape_BooleanOperand SecondOperand() const;




  DEFINE_STANDARD_RTTIEXT(StepShape_BooleanResult,StepGeom_GeometricRepresentationItem)

protected:




private:


  StepShape_BooleanOperator anOperator;
  StepShape_BooleanOperand firstOperand;
  StepShape_BooleanOperand secondOperand;


};







#endif // _StepShape_BooleanResult_HeaderFile
