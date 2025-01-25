// Created on: 1997-04-01
// Created by: Christian CAILLET
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _StepData_FieldList1_HeaderFile
#define _StepData_FieldList1_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <StepData_Field.hxx>
#include <StepData_FieldList.hxx>
#include <Standard_Integer.hxx>

//! Describes a list of ONE field
class StepData_FieldList1 : public StepData_FieldList
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a FieldList of 1 Field
  Standard_EXPORT StepData_FieldList1();

  //! Returns the count of fields. Here, returns 1
  Standard_EXPORT virtual Standard_Integer NbFields() const Standard_OVERRIDE;

  //! Returns the field n0 <num> between 1 and NbFields (read only)
  Standard_EXPORT virtual const StepData_Field& Field(const Standard_Integer num) const
    Standard_OVERRIDE;

  //! Returns the field n0 <num> between 1 and NbFields, in order to
  //! modify its content
  Standard_EXPORT virtual StepData_Field& CField(const Standard_Integer num) Standard_OVERRIDE;

private:
  StepData_Field thefield;
};

#endif // _StepData_FieldList1_HeaderFile
