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

#include <BRepGraph_CachedValue.hxx>
#include <BRepGraph_UserAttribute.hxx>

#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_DataMap.hxx>

//! Per-node cache bundle.
//!
//! Contains two layers:
//!
//! 1. **Fixed fields** -- BoundingBox, Centroid.
//!    Always present.  Not every node kind uses every field; unused ones
//!    stay dirty forever and are never computed.
//!
//! 2. **User attributes** -- an extensible map keyed by integer.
//!    Downstream algorithms register a unique key via
//!    BRepGraph_UserAttribute::AllocateKey() and attach
//!    BRepGraph_TypedAttribute<T> instances.
//!    The map is allocated lazily on first SetUserAttribute() to save
//!    ~48-64 bytes for the majority of nodes that never use attributes.
struct BRepGraph_NodeCache
{
  //! Fixed cached fields.
  BRepGraph_CachedValue<Bnd_Box> BoundingBox;
  BRepGraph_CachedValue<gp_Pnt>  Centroid;

  ~BRepGraph_NodeCache()
  {
    delete myUserAttributes;
  }

  BRepGraph_NodeCache() = default;

  //! Copy constructor: creates fresh (dirty) cache slots, copies user attributes.
  BRepGraph_NodeCache(const BRepGraph_NodeCache& theOther)
      : myUserAttributes(nullptr)
  {
    if (theOther.myUserAttributes != nullptr)
      myUserAttributes =
        new NCollection_DataMap<int, BRepGraph_UserAttrPtr>(*theOther.myUserAttributes);
  }

  BRepGraph_NodeCache& operator=(const BRepGraph_NodeCache& theOther)
  {
    if (this != &theOther)
    {
      BoundingBox.Invalidate();
      Centroid.Invalidate();
      delete myUserAttributes;
      myUserAttributes = nullptr;
      if (theOther.myUserAttributes != nullptr)
        myUserAttributes =
          new NCollection_DataMap<int, BRepGraph_UserAttrPtr>(*theOther.myUserAttributes);
    }
    return *this;
  }

  //! Invalidate ALL cached data on this node: fixed fields AND every
  //! user attribute in the map.
  void InvalidateAll()
  {
    BoundingBox.Invalidate();
    Centroid.Invalidate();

    if (myUserAttributes != nullptr)
    {
      for (auto anIter = myUserAttributes->cbegin();
           anIter != myUserAttributes->cend(); ++anIter)
      {
        if (*anIter)
          (*anIter)->Invalidate();
      }
    }
  }

  //! Invalidate only a specific user attribute by key.
  void InvalidateUserAttribute(int theKey)
  {
    if (myUserAttributes == nullptr)
      return;
    const BRepGraph_UserAttrPtr* aPtr = myUserAttributes->Seek(theKey);
    if (aPtr != nullptr && *aPtr)
      (*aPtr)->Invalidate();
  }

  //! Attach a user attribute.  Replaces any existing attribute at the same key.
  void SetUserAttribute(int                          theKey,
                        const BRepGraph_UserAttrPtr& theAttr)
  {
    if (myUserAttributes == nullptr)
      myUserAttributes = new NCollection_DataMap<int, BRepGraph_UserAttrPtr>();
    myUserAttributes->Bind(theKey, theAttr);
  }

  //! Retrieve a user attribute by key.  Returns nullptr if absent.
  BRepGraph_UserAttrPtr
    GetUserAttribute(int theKey) const
  {
    if (myUserAttributes == nullptr)
      return nullptr;
    const BRepGraph_UserAttrPtr* aPtr = myUserAttributes->Seek(theKey);
    return (aPtr != nullptr) ? *aPtr : nullptr;
  }

  //! Remove a user attribute by key.  Returns true if something was removed.
  bool RemoveUserAttribute(int theKey)
  {
    if (myUserAttributes == nullptr)
      return false;
    return myUserAttributes->UnBind(theKey);
  }

  //! True if any user attributes are registered on this node.
  bool HasUserAttributes() const
  {
    return myUserAttributes != nullptr && !myUserAttributes->IsEmpty();
  }

private:
  //! Lazily-allocated user attributes map (nullptr until first SetUserAttribute).
  NCollection_DataMap<int, BRepGraph_UserAttrPtr>* myUserAttributes = nullptr;
};

#endif // _BRepGraph_NodeCache_HeaderFile
