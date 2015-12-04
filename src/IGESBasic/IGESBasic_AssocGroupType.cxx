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

#include <IGESBasic_AssocGroupType.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESBasic_AssocGroupType,IGESData_IGESEntity)

IGESBasic_AssocGroupType::IGESBasic_AssocGroupType ()    {  }


    void  IGESBasic_AssocGroupType::Init
  (const Standard_Integer nbDataFields,
   const Standard_Integer aType, const Handle(TCollection_HAsciiString)& aName)
{
  theNbData = nbDataFields;
  theType   = aType;
  theName   = aName;
  InitTypeAndForm(406,23);
}


    Standard_Integer  IGESBasic_AssocGroupType::NbData () const
{
  return theNbData;
}

    Standard_Integer  IGESBasic_AssocGroupType::AssocType () const
{
  return theType;
}

    Handle(TCollection_HAsciiString)  IGESBasic_AssocGroupType::Name () const
{
  return theName;
}
