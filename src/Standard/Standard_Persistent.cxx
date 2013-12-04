// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <Handle_Standard_Persistent.hxx>
#include <Standard_Persistent.hxx>

#include <Standard_Failure.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_ImmutableObject.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>

// The Initialization of the Standard_Persistent variables
const Handle_Standard_Type& Standard_Persistent_Type_() 
{
  static const Handle_Standard_Type _Ancestors[]={NULL};
  static Handle_Standard_Type _aType 
    = new Standard_Type("Standard_Persistent",
			sizeof(Standard_Persistent),
			0,
     (Standard_Address) _Ancestors,
			NULL);

  return _aType;
}

//
// The Standard_Persistent Methods
//

// The Method This 
//
Handle_Standard_Persistent Standard_Persistent::This() const
{
  Handle_Standard_Persistent aHand(this);

  return aHand;
}

// The Method ShallowCopy 
//
Handle_Standard_Persistent Standard_Persistent::ShallowCopy() const
{
  //the method must be redefined in the every subclass that is expected to use it
  Standard_NotImplemented::Raise ("The ShallowCopy() method must be redefined in the subclass");
  return This(); // for compilation only
}

// Empty Constructor
//
// Standard_Persistent::Standard_Persistent()
// {
// }

// Empty Destructor
//
Standard_Persistent::~Standard_Persistent()
{
}

// Constructor from a Standard_Persistent
//
// Standard_Persistent::Standard_Persistent(const Standard_Persistent& )
// { 
// }

// Operator= with a Standard_Persistent
//
Standard_Persistent& Standard_Persistent::operator=(const Standard_Persistent& )
{ 
  return *this;
}

//
//
Standard_Boolean Standard_Persistent::IsInstance(const Handle(Standard_Type)
						&AType) const
{
  return (Standard_Boolean) (AType ==  DynamicType());
}

//
//
const Handle(Standard_Type)& Standard_Persistent::DynamicType () const
{  
  return  STANDARD_TYPE(Standard_Persistent);
}

//
//
Standard_Boolean Standard_Persistent::IsKind (const Handle(Standard_Type)& aType) const
{
  return DynamicType()->SubType ( aType );
//  return  (aType == STANDARD_TYPE(Standard_Persistent));
}

void Standard_Persistent::ShallowDump(Standard_OStream& AStream) const
{
  Handle(Standard_Persistent) me = this;
  ::ShallowDump(me, AStream);
}


void Standard_Persistent::Delete() const
 { 
   delete((Standard_Persistent *)this); 
 }

const Handle(Standard_Persistent) Handle(Standard_Persistent)::DownCast(const Handle(Standard_Persistent)& AnObject) 
{
  Handle(Standard_Persistent) _anOtherObject;
  
  if (!AnObject.IsNull()) 
    if (AnObject->IsKind(STANDARD_TYPE(Standard_Persistent))) 
      {
	_anOtherObject = Handle(Standard_Persistent)((Handle(Standard_Persistent)&)AnObject);
      }
  
  return _anOtherObject ;
}

Handle(Standard_Persistent)::~Handle(Standard_Persistent)()
{
 EndScope();
}

void Handle_Standard_Persistent::RaiseNullObject(const Standard_CString S) const
{ 
  Standard_NullObject::Raise(S);
}

void Handle(Standard_Persistent)::Dump(Standard_OStream& out) const
{ 
  out << ControlAccess();
}


