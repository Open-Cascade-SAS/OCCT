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

#ifndef _BRepGraphSupInc_StoreRegistry_HeaderFile
#define _BRepGraphSupInc_StoreRegistry_HeaderFile

#include <BRepGraphSupInc_Store.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <NCollection_LinearVector.hxx>
#include <Standard_DefineAlloc.hxx>

#include <shared_mutex>
#include <utility>

//! Thread-safe registry of independently owned supplemental stores.
//! Iterators retain a shared registry lock. Store GUIDs select store types;
//! StoreIds identify one registration and are never reused by the process.
class BRepGraphSupInc_StoreRegistry
{
public:
  DEFINE_STANDARD_ALLOC

  //! Lock-owning iterator over registered stores.
  //!
  //! Registration and unregistration remain blocked until this iterator is
  //! destroyed. The current handle remains valid until Next() is called.
  class Iterator
  {
  public:
    explicit Iterator(const BRepGraphSupInc_StoreRegistry& theRegistry)
        : myRegistry(&theRegistry),
          myLock(theRegistry.myMutex)
    {
    }

    Iterator(const Iterator&)            = delete;
    Iterator& operator=(const Iterator&) = delete;

    Iterator(Iterator&& theOther) noexcept
        : myRegistry(theOther.myRegistry),
          myLock(std::move(theOther.myLock)),
          myIndex(theOther.myIndex)
    {
      theOther.myRegistry = nullptr;
      theOther.myIndex    = 0;
    }

    Iterator& operator=(Iterator&& theOther) noexcept
    {
      if (this != &theOther)
      {
        myLock              = std::move(theOther.myLock);
        myRegistry          = theOther.myRegistry;
        myIndex             = theOther.myIndex;
        theOther.myRegistry = nullptr;
        theOther.myIndex    = 0;
      }
      return *this;
    }

    [[nodiscard]] bool More() const noexcept
    {
      return myRegistry != nullptr && myIndex < myRegistry->myStores.Size();
    }

    void Next() noexcept { ++myIndex; }

    [[nodiscard]] const occ::handle<BRepGraphSupInc_Store>& Value() const
    {
      return myRegistry->myStores.Value(myIndex);
    }

  private:
    const BRepGraphSupInc_StoreRegistry* myRegistry = nullptr;
    std::shared_lock<std::shared_mutex>  myLock;
    size_t                               myIndex = 0;
  };

  BRepGraphSupInc_StoreRegistry() = default;
  Standard_EXPORT ~BRepGraphSupInc_StoreRegistry();
  BRepGraphSupInc_StoreRegistry(const BRepGraphSupInc_StoreRegistry&)            = delete;
  BRepGraphSupInc_StoreRegistry& operator=(const BRepGraphSupInc_StoreRegistry&) = delete;

  //! Move registered stores from another registry.
  //! @param[in,out] theOther registry to empty
  Standard_EXPORT BRepGraphSupInc_StoreRegistry(BRepGraphSupInc_StoreRegistry&& theOther) noexcept;
  //! Replace this registry's stores by moving registrations from another registry.
  //! @param[in,out] theOther registry to empty
  //! @return this registry
  Standard_EXPORT BRepGraphSupInc_StoreRegistry& operator=(
    BRepGraphSupInc_StoreRegistry&& theOther) noexcept;

  //! Register a non-null store when its GUID is not already registered.
  //! @param[in] theStore store service to register
  //! @return false for null, already bound, or duplicate-GUID stores.
  Standard_EXPORT bool RegisterStore(const occ::handle<BRepGraphSupInc_Store>& theStore);

  //! Find a registered store by GUID, or return a null handle.
  //! @param[in] theGUID stable store-type GUID
  //! @return matching store or a null handle
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraphSupInc_Store> FindStore(
    const Standard_GUID& theGUID) const;

  //! Find a registered store by runtime registration ID, or return a null handle.
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraphSupInc_Store> FindStore(
    const uint32_t theStoreId) const;

  //! Return the runtime registration ID for a store GUID, or zero when absent.
  [[nodiscard]] Standard_EXPORT uint32_t StoreId(const Standard_GUID& theGUID) const;

  //! Find a registered store by its static store GUID.
  //! @return matching typed store or a null handle
  template <typename StoreT>
  [[nodiscard]] occ::handle<StoreT> FindStore() const
  {
    return occ::down_cast<StoreT>(FindStore(StoreT::GetID()));
  }

  //! Return a registered store of this type, creating it on first use.
  //! @return existing or newly registered typed store
  template <typename StoreT>
  [[nodiscard]] occ::handle<StoreT> EnsureStore()
  {
    occ::handle<StoreT> aStore = FindStore<StoreT>();
    if (!aStore.IsNull())
    {
      return aStore;
    }

    aStore = new StoreT();
    if (RegisterStore(aStore))
    {
      return aStore;
    }
    return FindStore<StoreT>();
  }

  //! Unregister a store only after all of its records have been cleared.
  //! @param[in] theGUID stable store-type GUID
  //! @return true if a registered empty store was removed.
  Standard_EXPORT bool UnregisterStore(const Standard_GUID& theGUID);

  //! Return the number of currently registered stores.
  //! @return registered store count
  [[nodiscard]] uint32_t Count() const;

private:
  friend class BRepGraphSupInc_Storage;

  //! Register a detached compacted store in the source store's runtime identity space.
  //! The target registry must remain private until it replaces the source registry.
  [[nodiscard]] Standard_EXPORT bool registerCompactedStore(
    const occ::handle<BRepGraphSupInc_Store>& theStore,
    const uint32_t                            theStoreId);

  //! Clear every registered store without unregistering it.
  Standard_EXPORT void ClearAll() noexcept;

  NCollection_LinearVector<occ::handle<BRepGraphSupInc_Store>> myStores;
  NCollection_FlatDataMap<Standard_GUID, uint32_t>             myGUIDToSlot;
  NCollection_FlatDataMap<uint32_t, uint32_t>                  myStoreIdToSlot;
  mutable std::shared_mutex                                    myMutex;
};

#endif // _BRepGraphSupInc_StoreRegistry_HeaderFile
