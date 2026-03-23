// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef _BRepGraph_NodeCache_HeaderFile
#define _BRepGraph_NodeCache_HeaderFile

#include <BRepGraph_UserAttribute.hxx>

#include <NCollection_DataMap.hxx>

class BRepGraph_UserAttribute;

//! Per-node cache bundle.
//!
//! Provides an extensible map of **user attributes** keyed by integer.
//! Downstream algorithms register a unique key via
//! BRepGraph_UserAttribute::AllocateKey() and attach
//! BRepGraph_TypedAttribute<T> instances.
//! The map is kept as a value member and remains empty until first bind.
struct BRepGraph_NodeCache
{
  ~BRepGraph_NodeCache() = default;

  BRepGraph_NodeCache() = default;

  //! Copy constructor: copies user attributes.
  BRepGraph_NodeCache(const BRepGraph_NodeCache& theOther)
      : myUserAttributes(theOther.myUserAttributes)
  {
  }

  BRepGraph_NodeCache& operator=(const BRepGraph_NodeCache& theOther)
  {
    if (this != &theOther)
    {
      myUserAttributes = theOther.myUserAttributes;
    }
    return *this;
  }

  //! Invalidate ALL cached data on this node: every user attribute in the map.
  void InvalidateAll()
  {
    if (!myUserAttributes.IsEmpty())
    {
      for (auto anIter = myUserAttributes.cbegin();
           anIter != myUserAttributes.cend(); ++anIter)
      {
        if (!(*anIter).IsNull())
          (*anIter)->Invalidate();
      }
    }
  }

  //! Invalidate only a specific user attribute by key.
  void InvalidateUserAttribute(int theKey)
  {
    const occ::handle<BRepGraph_UserAttribute>* aPtr = myUserAttributes.Seek(theKey);
    if (aPtr != nullptr && !(*aPtr).IsNull())
      (*aPtr)->Invalidate();
  }

  //! Attach a user attribute.  Replaces any existing attribute at the same key.
  void SetUserAttribute(int                                   theKey,
                        const occ::handle<BRepGraph_UserAttribute>& theAttr)
  {
    myUserAttributes.Bind(theKey, theAttr);
  }

  //! Retrieve a user attribute by key.  Returns null handle if absent.
  occ::handle<BRepGraph_UserAttribute> GetUserAttribute(int theKey) const
  {
    const occ::handle<BRepGraph_UserAttribute>* aPtr = myUserAttributes.Seek(theKey);
    return (aPtr != nullptr) ? *aPtr : occ::handle<BRepGraph_UserAttribute>();
  }

  //! Remove a user attribute by key.  Returns true if something was removed.
  bool RemoveUserAttribute(int theKey)
  {
    return myUserAttributes.UnBind(theKey);
  }

  //! True if any user attributes are registered on this node.
  bool HasUserAttributes() const
  {
    return !myUserAttributes.IsEmpty();
  }

  //! Return all registered user attribute keys.
  NCollection_Vector<int> UserAttributeKeys() const
  {
    NCollection_Vector<int> aKeys;
    for (NCollection_DataMap<int, occ::handle<BRepGraph_UserAttribute>>::Iterator anIter(myUserAttributes);
         anIter.More(); anIter.Next())
    {
      aKeys.Append(anIter.Key());
    }
    return aKeys;
  }

private:
  //! User attributes map (empty until first SetUserAttribute).
  NCollection_DataMap<int, occ::handle<BRepGraph_UserAttribute>> myUserAttributes;
};

#endif // _BRepGraph_NodeCache_HeaderFile
