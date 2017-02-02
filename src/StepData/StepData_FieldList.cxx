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


#include <Interface_EntityIterator.hxx>
#include <Standard_OutOfRange.hxx>
#include <StepData_Field.hxx>
#include <StepData_FieldList.hxx>

static StepData_Field nulfild;

StepData_FieldList::~StepData_FieldList()
{
}

StepData_FieldList::StepData_FieldList  ()    {  }

Standard_Integer  StepData_FieldList::NbFields () const
{  return 0;  }

const StepData_Field&  StepData_FieldList::Field (const Standard_Integer) const
{
  throw Standard_OutOfRange("StepData_FieldList : Field");
}

StepData_Field&  StepData_FieldList::CField (const Standard_Integer)
{
  throw Standard_OutOfRange("StepData_FieldList : CField");
}


void  StepData_FieldList::FillShared (Interface_EntityIterator& iter) const
{
  Standard_Integer i, nb = NbFields();
  for (i = 1; i <= nb; i ++) {
    const StepData_Field& fi = Field(i);
    if (fi.Kind() != 7) continue;  // KindEntity
    Standard_Integer i1,i2, nb1 = 1, nb2 = 1, ari = fi.Arity();
    if (ari == 1)   nb1 = fi.Length();
    if (ari == 2) { nb1 = fi.Length(1); nb2 = fi.Length(2); }
    for (i1 = 1; i1 <= nb1; i1 ++)
      for (i2 = 1; i2 <= nb2; i2 ++)
	iter.AddItem (fi.Entity(i1,i2));
  }
}
