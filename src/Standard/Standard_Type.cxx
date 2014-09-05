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

#include <Standard_Type.ixx>
#include <Standard_Persistent.hxx>

//============================================================================
// The constructor for a imported Type 
//============================================================================

Standard_Type::Standard_Type(const Standard_CString aName, 
			     const Standard_Integer aSize)
{
  myName = aName;
  mySize = aSize;
  myKind = Standard_IsImported;

  //==== Just for be clean with the class and the enumeration ================
  myNumberOfParent   = 0; 
  myNumberOfAncestor = 0;

  myAncestors = NULL;
}

//============================================================================
// The constructor for a primitive Type 
//============================================================================

Standard_Type::Standard_Type(const Standard_CString aName, 
			     const Standard_Integer aSize, 
			     const Standard_Integer aNumberOfParent , 
			     const Standard_Address aAncestors)
{
  myName = aName;
  mySize = aSize;
  myKind = Standard_IsPrimitive;

  myNumberOfParent   = aNumberOfParent;

  //==== Calculate the number of ancestor ====================================
  myNumberOfAncestor = 0;
  myAncestors = aAncestors;
  Handle(Standard_Type) *allAncestors = (Handle(Standard_Type) *)myAncestors;
  for(Standard_Integer i=0; allAncestors && !allAncestors[i].IsNull(); i++) {
    myNumberOfAncestor++;
  }
}

//============================================================================
// The constructor for an enumeration.
//============================================================================

Standard_Type::Standard_Type(const Standard_CString aName, 
			     const Standard_Integer aSize, 
			     const Standard_Integer , 
			     const Standard_Integer aNumberOfParent, 
			     const Standard_Address aAncestors, 
			     const Standard_Address )
{
  myName = aName;
  mySize = aSize;
  myKind = Standard_IsEnumeration;

  myNumberOfParent  = aNumberOfParent;

  //==== Calculate the number of ancestor ====================================
  myNumberOfAncestor = 0;
  myAncestors = aAncestors;
  Handle(Standard_Type) *allAncestors = (Handle(Standard_Type) *)myAncestors;
  for(Standard_Integer i=0; allAncestors && !allAncestors[i].IsNull(); i++) {
    myNumberOfAncestor++;
  }
}

//============================================================================
// The constructor for a class.
//============================================================================

Standard_Type::Standard_Type(const Standard_CString aName, 
			     const Standard_Integer aSize, 
			     const Standard_Integer aNumberOfParent , 
			     const Standard_Address aAncestors , 
			     const Standard_Address )
{
  myName = aName;
  mySize = aSize;
  myKind = Standard_IsClass;

  myNumberOfParent    = aNumberOfParent;

  //==== Calculate the number of ancestors ===================================
  myNumberOfAncestor = 0;
  myAncestors = aAncestors;
  Handle(Standard_Type) *allAncestors = (Handle(Standard_Type) *)myAncestors;
  for(Standard_Integer i=0; allAncestors && !allAncestors[i].IsNull(); i++) {
    myNumberOfAncestor++;
  }
}

//============================================================================
Standard_CString  Standard_Type::Name() const 
{
  return myName;
}

//============================================================================
Standard_Integer  Standard_Type::Size()const 
{
  return mySize;
}

//============================================================================
Standard_Boolean  Standard_Type::IsImported()const 
{
  return myKind == Standard_IsImported;
}

//============================================================================
Standard_Boolean  Standard_Type::IsPrimitive()const 
{
  return myKind == Standard_IsPrimitive;
}

//============================================================================
Standard_Boolean  Standard_Type::IsEnumeration()const 
{
  return myKind == Standard_IsEnumeration;
}

//============================================================================
Standard_Boolean  Standard_Type::IsClass()const 
{
  return myKind == Standard_IsClass;
}

//============================================================================
Standard_Integer  Standard_Type::NumberOfParent()const 
{
  return myNumberOfParent;
}

//============================================================================
Standard_Integer  Standard_Type::NumberOfAncestor()const 
{
  return myNumberOfAncestor;
}

//============================================================================
Standard_Address  Standard_Type::Ancestors()const 
{
  return myAncestors;
}

//============================================================================
Standard_Boolean  Standard_Type::SubType(const Handle(Standard_Type)& anOther)const 
{
  // Among ancestors of the type of this, there is the type of anOther
  return anOther == this || 
         ( myNumberOfAncestor &&
           (*(Handle(Standard_Type)*)myAncestors)->SubType(anOther) );
}

//============================================================================
Standard_Boolean  Standard_Type::SubType(const Standard_CString theName)const 
{
  // Among ancestors of the type of this, there is the type of anOther
  if ( ! theName )
    return Standard_False;
  if ( IsSimilar(myName, theName) )
    return Standard_True;
  if (myNumberOfAncestor) 
    return (*(Handle(Standard_Type) *)myAncestors)->SubType(theName);
  return Standard_False;
}

// ------------------------------------------------------------------
// Print (me; s: in out OStream) returns OStream;
// ------------------------------------------------------------------
void Standard_Type::Print (Standard_OStream& AStream) const
{
//  AStream <<  myName ;
  AStream << hex << (Standard_Address ) myName << " : " << dec << myName ;
}

Standard_OStream& operator << (Standard_OStream& AStream
			      ,const Handle(Standard_Type)& AType) 
{
  AType->Print(AStream);
  return AStream;
}
