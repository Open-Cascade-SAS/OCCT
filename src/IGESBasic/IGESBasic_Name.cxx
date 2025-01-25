// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESBasic_Name.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESBasic_Name, IGESData_NameEntity)

IGESBasic_Name::IGESBasic_Name() {}

void IGESBasic_Name::Init(const Standard_Integer                  nbPropVal,
                          const Handle(TCollection_HAsciiString)& aName)
{
  theName             = aName;
  theNbPropertyValues = nbPropVal;
  InitTypeAndForm(406, 15);
}

Standard_Integer IGESBasic_Name::NbPropertyValues() const
{
  return theNbPropertyValues;
}

Handle(TCollection_HAsciiString) IGESBasic_Name::Value() const
{
  return theName;
}
