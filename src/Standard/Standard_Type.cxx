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
#include <Standard_Mutex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Standard_Type,Standard_Transient)

Standard_Type::Standard_Type (const char* theSystemName,
                              const char* theName,
                              Standard_Size theSize,
                              const Handle(Standard_Type)& theParent) :
  mySystemName(theSystemName),
  myName(theName),
  mySize(theSize),
  myParent(theParent)
{}

Standard_Boolean Standard_Type::SubType(const Handle(Standard_Type)& theOther) const
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

Standard_Boolean Standard_Type::SubType(const Standard_CString theName) const
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

void Standard_Type::Print (Standard_OStream& AStream) const
{
  AStream << std::hex << (Standard_Address)this << " : " << std::dec << myName ;
}

Standard_Type* Standard_Type::Register (const std::type_info& theInfo, const char* theName,
                                        Standard_Size theSize, const Handle(Standard_Type)& theParent)
{
  return new Standard_Type (theInfo.name(), theName, theSize, theParent);
}

Standard_Type::~Standard_Type ()
{}
