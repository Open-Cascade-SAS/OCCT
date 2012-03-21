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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

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
Standard_Boolean  Standard_Type::SubType(const Handle_Standard_Type& anOther)const 
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

//============================================================================
void Standard_Type::ShallowDump() const
{
  ShallowDump(cout) ;
}

//============================================================================
void Standard_Type::ShallowDump(Standard_OStream& AStream) const
{
  Standard_Integer i;
  Handle(Standard_Type) aType;

  Handle(Standard_Type) *allAncestors;

//  AStream << "Standard_Type " << hex << (Standard_Address )this << dec << " " ;

  allAncestors = (Handle(Standard_Type) *)myAncestors;

  if(myKind == Standard_IsEnumeration) {
    AStream << "enumeration " << myName << endl;
  }

  if(myKind == Standard_IsPrimitive) {
    AStream << "primitive " << myName << endl;
  }

  if(myKind == Standard_IsImported) {
    AStream << "imported " << myName << endl;
  }

  if(myKind == Standard_IsClass) {
    AStream << "class " << myName << endl;
    if(SubType(STANDARD_TYPE(Standard_Transient))) {
      AStream << "      -- manipulated by 'Handle'" << endl;
    }
    else if(SubType(STANDARD_TYPE(Standard_Persistent))) {
      AStream << "      -- manipulated by 'Handle' and is 'persistent'"<< endl;
    }
  }

  if(myNumberOfParent > 0) {
    AStream << "      inherits ";
    for(i=0; i<myNumberOfParent; i++){
      aType = allAncestors[i];
      if (i>1) AStream <<", ";
      if ( !aType.IsNull() )
        AStream << aType->Name();
      else
        AStream << " ??? (TypeIsNull)" ;
    }
    AStream << endl;
  }
 
  if(myNumberOfAncestor > myNumberOfParent) {
    AStream << "      -- Ancestors: ";
    for(i=myNumberOfParent; i<myNumberOfAncestor; i++){
      aType = allAncestors[i];
      if (i>1) AStream <<", ";
      if ( !aType.IsNull() )
        AStream << aType->Name();
      else
        AStream << " ??? (TypeIsNull)" ;
    }
    AStream << endl;
  }
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
