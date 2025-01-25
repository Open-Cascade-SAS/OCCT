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

#ifndef _StepBasic_Date_HeaderFile
#define _StepBasic_Date_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>

class StepBasic_Date;
DEFINE_STANDARD_HANDLE(StepBasic_Date, Standard_Transient)

class StepBasic_Date : public Standard_Transient
{

public:
  //! Returns a Date
  Standard_EXPORT StepBasic_Date();

  Standard_EXPORT void Init(const Standard_Integer aYearComponent);

  Standard_EXPORT void SetYearComponent(const Standard_Integer aYearComponent);

  Standard_EXPORT Standard_Integer YearComponent() const;

  DEFINE_STANDARD_RTTIEXT(StepBasic_Date, Standard_Transient)

protected:
private:
  Standard_Integer yearComponent;
};

#endif // _StepBasic_Date_HeaderFile
