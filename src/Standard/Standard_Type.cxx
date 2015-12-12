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


#include <Standard_Type.hxx>
#include <Standard_Mutex.hxx>
#include <Standard_Assert.hxx>

#include <NCollection_DataMap.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Standard_Type,Standard_Transient)

//============================================================================

namespace {
static Standard_CString copy_string (const char* theString)
{
  size_t aLength = strlen (theString);
  char* aResult = static_cast<char*> (Standard::Allocate (aLength + 1));
  strncpy (aResult, theString, aLength + 1); //including null-character
  return aResult;
}
}

Standard_Type::Standard_Type (const char* theSystemName,
                              const char* theName,
                              Standard_Size theSize,
                              const Handle(Standard_Type)& theParent) :
  mySystemName(copy_string (theSystemName)),
  myName(copy_string (theName)), 
  mySize(theSize), 
  myParent(theParent)
{
}

//============================================================================

Standard_Boolean Standard_Type::SubType (const Handle(Standard_Type)& theOther) const
{
  return ! theOther.IsNull() && (theOther == this || (! myParent.IsNull() && myParent->SubType (theOther)));
}

//============================================================================

Standard_Boolean Standard_Type::SubType (const Standard_CString theName) const
{
  return theName != 0 && (IsEqual (myName, theName) || (! myParent.IsNull() && myParent->SubType (theName)));
}

// ------------------------------------------------------------------
// Print (me; s: in out OStream) returns OStream;
// ------------------------------------------------------------------
void Standard_Type::Print (Standard_OStream& AStream) const
{
  AStream << hex << (Standard_Address)this << " : " << dec << myName ;
}

//============================================================================
// Registry of types
//============================================================================

namespace {
  // Value-based hasher for plain C string (char*)
  struct CStringHasher 
  {
    static Standard_Integer HashCode (const Standard_CString& theKey, const Standard_Integer Upper)
    {
      return ::HashCode (theKey, Upper);
    }
    static bool IsEqual (const Standard_CString& theKey1, const Standard_CString& theKey2)
    {
      return ! strcmp (theKey1, theKey2);
    }
  };

  // Map of string to type
  typedef NCollection_DataMap<Standard_CString, Standard_Type*, CStringHasher> registry_type;

  // Registry is made static in the function to ensure that it gets
  // initialized by the time of first access
  registry_type& GetRegistry() 
  {
    static registry_type theRegistry;
    return theRegistry;
  }
}

Standard_Type* Standard_Type::Register (const char* theSystemName, const char* theName,
                                        Standard_Size theSize, const Handle(Standard_Type)& theParent)
{
  // Access to registry is protected by mutex; it should not happen often because
  // instances are cached by Standard_Type::Instance() (one per binary module)
  static Standard_Mutex theMutex;
  Standard_Mutex::Sentry aSentry (theMutex);

  // return existing descriptor if already in the registry
  registry_type& aRegistry = GetRegistry();
  Standard_Type* aType = 0;
  if (aRegistry.Find (theSystemName, aType))
    return aType;

  // else create a new descriptor
  aType = new Standard_Type (theSystemName, theName, theSize, theParent);

  // then add it to registry and return (the reference to the handle stored in the registry)
  aRegistry.Bind (aType->mySystemName, aType);

//  cout << "Registering " << theSystemName << ": " << aRegistry.Extent() << endl;

  return aType;
}

Standard_Type::~Standard_Type ()
{
  // remove descriptor from the registry
  registry_type& aRegistry = GetRegistry();
  Standard_ASSERT(aRegistry.UnBind (mySystemName), "Standard_Type::~Standard_Type() cannot find itself in registry",);

//  cout << "Unregistering " << mySystemName << ": " << aRegistry.Extent() << endl;
  Standard::Free (mySystemName);
  Standard::Free (myName);
}
