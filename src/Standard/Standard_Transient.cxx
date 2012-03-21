// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
Handle_Standard_Transient Standard_Transient::This() const
{
  return Handle_Standard_Transient(this);
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

void Standard_Transient::ShallowDump(Standard_OStream& theStream) const
{
  theStream << "class " << DynamicType()->Name() << " at " << this << endl;
}


void Standard_Transient::Delete() const
{ 
  delete((Standard_Transient *)this); 
}
