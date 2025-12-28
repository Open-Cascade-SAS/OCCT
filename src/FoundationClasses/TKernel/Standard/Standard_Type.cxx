// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2025 OPEN CASCADE SAS
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

#include <NCollection_DataMap.hxx>
#include <Standard_HashUtils.hxx>
#include <Standard_Assert.hxx>

#include <mutex>

IMPLEMENT_STANDARD_RTTIEXT(Standard_Type, Standard_Transient)

Standard_Type::Standard_Type(const char*                       theSystemName,
                             const char*                       theName,
                             size_t                            theSize,
                             const occ::handle<Standard_Type>& theParent)
    : mySystemName(theSystemName),
      myName(theName),
      mySize(theSize),
      myParent(theParent)
{
}

bool Standard_Type::SubType(const occ::handle<Standard_Type>& theOther) const
{
  if (theOther.IsNull())
  {
    return false;
  }
  const Standard_Type* aTypeIter = this;
  while (aTypeIter && theOther->mySize <= aTypeIter->mySize)
  {
    if (theOther.get() == aTypeIter)
    {
      return true;
    }
    aTypeIter = aTypeIter->Parent().get();
  }
  return false;
}

bool Standard_Type::SubType(const char* theName) const
{
  if (!theName)
  {
    return false;
  }
  const Standard_Type* aTypeIter = this;
  while (aTypeIter)
  {
    if (IsEqual(theName, aTypeIter->Name()))
    {
      return true;
    }
    aTypeIter = aTypeIter->Parent().get();
  }
  return false;
}

void Standard_Type::Print(Standard_OStream& AStream) const
{
  AStream << std::hex << (void*)this << " : " << std::dec << myName;
}

//============================================================================
// Registry of types
//============================================================================

namespace
{

struct typeNameHasher
{
  size_t operator()(const char* theType) const noexcept
  {
    const int aLen = static_cast<int>(strlen(theType));
    return opencascade::hashBytes(theType, aLen);
  }

  bool operator()(const char* theType1, const char* theType2) const noexcept
  {
    return strcmp(theType1, theType2) == 0;
  }
};

using registry_type = NCollection_DataMap<const char*, Standard_Type*, typeNameHasher>;

// Registry is made static in the function to ensure that it gets
// initialized by the time of first access
registry_type& GetRegistry()
{
  static registry_type theRegistry(2048, NCollection_BaseAllocator::CommonBaseAllocator());
  return theRegistry;
}

// To initialize theRegistry map as soon as possible to be destroyed the latest
occ::handle<Standard_Type> theType = STANDARD_TYPE(Standard_Transient);
} // namespace

Standard_Type* Standard_Type::Register(const std::type_info&             theInfo,
                                       const char*                       theName,
                                       size_t                            theSize,
                                       const occ::handle<Standard_Type>& theParent)
{
  // Access to registry is protected by mutex; it should not happen often because
  // instances are cached by Standard_Type::Instance() (one per binary module)
  static std::mutex           aMutex;
  std::lock_guard<std::mutex> aLock(aMutex);

  // return existing descriptor if already in the registry
  registry_type& aRegistry = GetRegistry();
  Standard_Type* aType;
  if (aRegistry.Find(theInfo.name(), aType))
  {
    return aType;
  }

  // Calculate sizes for deep copies
  const size_t anInfoNameLen = strlen(theInfo.name()) + 1;
  const size_t aNameLen      = strlen(theName) + 1;

  // Allocate memory block for Standard_Type and the two strings
  char* aMemoryBlock =
    static_cast<char*>(Standard::AllocateOptimal(sizeof(Standard_Type) + anInfoNameLen + aNameLen));

  // Pointers to the locations for the deep copies of the strings
  char* anInfoNameCopy = aMemoryBlock + sizeof(Standard_Type);
  char* aNameCopy      = anInfoNameCopy + anInfoNameLen;

  // Deep copy the strings using strncpy
  strncpy(anInfoNameCopy, theInfo.name(), anInfoNameLen);
  strncpy(aNameCopy, theName, aNameLen);

  aType = new (aMemoryBlock) Standard_Type(anInfoNameCopy, aNameCopy, theSize, theParent);

  // Insert the descriptor into the registry
  aRegistry.Bind(anInfoNameCopy, aType);
  return aType;
}

Standard_Type::~Standard_Type()
{
  // remove descriptor from the registry
  registry_type& aRegistry = GetRegistry();
  Standard_ASSERT(aRegistry.UnBind(mySystemName),
                  "Standard_Type::~Standard_Type() cannot find itself in registry", Standard_VOID_RETURN);
}
