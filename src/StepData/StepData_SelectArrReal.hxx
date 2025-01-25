// Created on: 2002-12-18
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

#ifndef _StepData_SelectArrReal_HeaderFile
#define _StepData_SelectArrReal_HeaderFile

#include <Standard.hxx>

#include <TColStd_HArray1OfReal.hxx>
#include <StepData_SelectNamed.hxx>
#include <Standard_Integer.hxx>

class StepData_SelectArrReal;
DEFINE_STANDARD_HANDLE(StepData_SelectArrReal, StepData_SelectNamed)

class StepData_SelectArrReal : public StepData_SelectNamed
{

public:
  Standard_EXPORT StepData_SelectArrReal();

  Standard_EXPORT virtual Standard_Integer Kind() const Standard_OVERRIDE;

  Standard_EXPORT Handle(TColStd_HArray1OfReal) ArrReal() const;

  Standard_EXPORT void SetArrReal(const Handle(TColStd_HArray1OfReal)& arr);

  DEFINE_STANDARD_RTTIEXT(StepData_SelectArrReal, StepData_SelectNamed)

protected:
private:
  Handle(TColStd_HArray1OfReal) theArr;
};

#endif // _StepData_SelectArrReal_HeaderFile
