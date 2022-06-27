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

#ifndef _StepData_FieldList_HeaderFile
#define _StepData_FieldList_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
class StepData_Field;
class Interface_EntityIterator;


//! Describes a list of fields, in a general way
//! This basic class is for a null size list
//! Subclasses are for 1, N (fixed) or Dynamic sizes
class StepData_FieldList 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Destructor is virtual
  Standard_EXPORT virtual ~StepData_FieldList();
  
  //! Creates a FieldList of 0 Field
  Standard_EXPORT StepData_FieldList();
  
  //! Returns the count of fields. Here, returns 0
  Standard_EXPORT virtual Standard_Integer NbFields() const;
  
  //! Returns the field n0 <num> between 1 and NbFields (read only)
  Standard_EXPORT virtual const StepData_Field& Field (const Standard_Integer num) const;
  
  //! Returns the field n0 <num> between 1 and NbFields, in order to
  //! modify its content
  Standard_EXPORT virtual StepData_Field& CField (const Standard_Integer num);
  
  //! Fills an iterator with the entities shared by <me>
  Standard_EXPORT void FillShared (Interface_EntityIterator& iter) const;
};

#endif // _StepData_FieldList_HeaderFile
