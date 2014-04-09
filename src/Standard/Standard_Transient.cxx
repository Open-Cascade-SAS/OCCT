// Copyright (c) 1998-1999 Matra Datavision
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

#include <Standard_Transient.hxx>

// The Initialization of the Standard_Transient variables
IMPLEMENT_STANDARD_TYPE(Standard_Transient)
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY()
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_END()
IMPLEMENT_STANDARD_TYPE_END(Standard_Transient)

IMPLEMENT_STANDARD_RTTI(Standard_Transient)

//
// The Standard_Transient Methods
//

// The Method This 
//
Handle(Standard_Transient) Standard_Transient::This() const
{
  return Handle(Standard_Transient)(this);
}

// Empty Destructor
//
Standard_Transient::~Standard_Transient()
{
}

//
//
Standard_Boolean Standard_Transient::IsInstance(const Handle(Standard_Type) &AType) const
{
  return (Standard_Boolean) (AType ==  DynamicType());
}

//
//
Standard_Boolean Standard_Transient::IsInstance(const Standard_CString theTypeName) const
{
  return IsSimilar ( DynamicType()->Name(), theTypeName );
}

//
//
Standard_Boolean Standard_Transient::IsKind (const Handle(Standard_Type)& aType) const
{
  return DynamicType()->SubType ( aType );
}

//
//
Standard_Boolean Standard_Transient::IsKind (const Standard_CString theTypeName) const
{
  return DynamicType()->SubType ( theTypeName );
}

void Standard_Transient::Delete() const
{ 
  delete((Standard_Transient *)this); 
}
