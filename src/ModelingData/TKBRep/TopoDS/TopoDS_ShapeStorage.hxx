// Copyright (c) 2024 OPEN CASCADE SAS
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

#ifndef _TopoDS_ShapeStorage_HeaderFile
#define _TopoDS_ShapeStorage_HeaderFile

#include <NCollection_DynamicArray.hxx>
#include <TopoDS_Shape.hxx>

#include <array>
#include <variant>

//! @brief Local storage for a fixed number of sub-shapes.
//!
//! This class provides inline storage for a small number of TopoDS_Shape objects.
//! It is used as the primary storage when the number of children is within
//! the local capacity, avoiding heap allocation overhead.
//!
//! @tparam LocalCapacity Maximum number of shapes that can be stored locally
template <size_t LocalCapacity>
class TopoDS_LocalShapeStorage
{
public:
  //! Default constructor - creates empty storage
  TopoDS_LocalShapeStorage()
      : myCount(0)
  {
  }

  //! Returns the number of stored shapes
  int Size() const { return myCount; }

  //! Returns true if storage is empty
  bool IsEmpty() const { return myCount == 0; }

  //! Returns true if local capacity is full
  bool IsFull() const { return myCount >= static_cast<int>(LocalCapacity); }

  //! Returns the local capacity
  static constexpr size_t Capacity() { return LocalCapacity; }

  //! Returns the shape at index (0-based)
  //! @param theIndex index of the shape (0 <= theIndex < Size())
  const TopoDS_Shape& Value(int theIndex) const { return myShapes[theIndex]; }

  //! Returns the shape at index for modification
  //! @param theIndex index of the shape (0 <= theIndex < Size())
  TopoDS_Shape& ChangeValue(int theIndex) { return myShapes[theIndex]; }

  //! Adds a shape to local storage
  //! @param theShape shape to add
  //! @pre !IsFull()
  void Append(const TopoDS_Shape& theShape)
  {
    myShapes[myCount] = theShape;
    ++myCount;
  }

  //! Removes shape at index
  //! @param theIndex index of the shape to remove (0 <= theIndex < Size())
  void Remove(int theIndex)
  {
    // Shift remaining elements left
    for (int i = theIndex; i < myCount - 1; ++i)
    {
      myShapes[i] = myShapes[i + 1];
    }
    // Clear the last slot
    myShapes[myCount - 1].Nullify();
    --myCount;
  }

  //! Clears all shapes
  void Clear()
  {
    for (int i = 0; i < myCount; ++i)
    {
      myShapes[i].Nullify();
    }
    myCount = 0;
  }

private:
  std::array<TopoDS_Shape, LocalCapacity> myShapes; //!< Fixed array of shapes
  int                                     myCount;  //!< Current number of shapes
};

//! @brief Dynamic storage for sub-shapes using NCollection_DynamicArray.
//!
//! This class wraps NCollection_DynamicArray for use in variant storage.
//! It provides the same interface as TopoDS_LocalShapeStorage for
//! seamless switching between local and dynamic storage.
//!
//! @tparam BucketSize Number of items per allocation bucket
template <int BucketSize>
class TopoDS_DynamicShapeStorage
{
public:
  //! Default constructor - creates empty storage with template BucketSize
  TopoDS_DynamicShapeStorage()
      : myShapes(BucketSize)
  {
  }

  //! Constructor with explicit bucket size (ignored, uses template parameter)
  //! @param theBucketSize ignored, provided for compatibility
  explicit TopoDS_DynamicShapeStorage(int theBucketSize)
      : myShapes(theBucketSize > 0 ? theBucketSize : BucketSize)
  {
  }

  //! Returns the number of stored shapes
  int Size() const { return myShapes.Size(); }

  //! Returns true if storage is empty
  bool IsEmpty() const { return myShapes.IsEmpty(); }

  //! Returns the bucket size
  static constexpr int GetBucketSize() { return BucketSize; }

  //! Returns the shape at index (0-based)
  //! @param theIndex index of the shape (0 <= theIndex < Size())
  const TopoDS_Shape& Value(int theIndex) const { return myShapes.Value(theIndex); }

  //! Returns the shape at index for modification
  //! @param theIndex index of the shape (0 <= theIndex < Size())
  TopoDS_Shape& ChangeValue(int theIndex) { return myShapes.ChangeValue(theIndex); }

  //! Adds a shape to storage
  //! @param theShape shape to add
  void Append(const TopoDS_Shape& theShape) { myShapes.Append(theShape); }

  //! Removes shape at index by shifting elements
  //! @param theIndex index of the shape to remove (0 <= theIndex < Size())
  void Remove(int theIndex)
  {
    // Shift remaining elements left
    for (int i = theIndex; i < myShapes.Size() - 1; ++i)
    {
      myShapes.ChangeValue(i) = myShapes.Value(i + 1);
    }
    myShapes.EraseLast();
  }

  //! Clears all shapes
  void Clear() { myShapes.Clear(); }

  //! Move shapes from local storage to this dynamic storage
  //! @param theLocal local storage to move from
  template <size_t LocalCapacity>
  void MoveFrom(TopoDS_LocalShapeStorage<LocalCapacity>& theLocal)
  {
    for (int i = 0; i < theLocal.Size(); ++i)
    {
      myShapes.Append(theLocal.Value(i));
    }
    theLocal.Clear();
  }

private:
  NCollection_DynamicArray<TopoDS_Shape> myShapes; //!< Dynamic array of shapes
};

//! @brief Variant-based storage combining local and dynamic storage.
//!
//! This class uses std::variant to provide either local (inline) storage
//! for small numbers of children, or dynamic (heap) storage when the
//! local capacity is exceeded. This optimizes memory usage and performance
//! for the common case of few children while supporting arbitrary counts.
//!
//! @tparam LocalCapacity Maximum number of shapes stored inline (must be > 0)
//! @tparam BucketSize Number of items per allocation bucket for dynamic storage
template <size_t LocalCapacity, int BucketSize>
class TopoDS_VariantShapeStorage
{
  static_assert(
    LocalCapacity > 0,
    "LocalCapacity must be > 0. Use TopoDS_DynamicShapeStorage directly for 0 capacity.");

public:
  using LocalStorage   = TopoDS_LocalShapeStorage<LocalCapacity>;
  using DynamicStorage = TopoDS_DynamicShapeStorage<BucketSize>;
  using StorageVariant = std::variant<LocalStorage, DynamicStorage>;

  //! Default constructor - starts with local storage
  TopoDS_VariantShapeStorage()
      : myStorage(LocalStorage())
  {
  }

  //! Returns the number of stored shapes
  int Size() const
  {
    return std::visit([](const auto& theStorage) { return theStorage.Size(); }, myStorage);
  }

  //! Returns true if storage is empty
  bool IsEmpty() const
  {
    return std::visit([](const auto& theStorage) { return theStorage.IsEmpty(); }, myStorage);
  }

  //! Returns true if currently using local storage
  bool IsLocal() const { return std::holds_alternative<LocalStorage>(myStorage); }

  //! Returns the shape at index (0-based)
  //! @param theIndex index of the shape (0 <= theIndex < Size())
  const TopoDS_Shape& Value(int theIndex) const
  {
    return std::visit(
      [theIndex](const auto& theStorage) -> const TopoDS_Shape& {
        return theStorage.Value(theIndex);
      },
      myStorage);
  }

  //! Returns the shape at index for modification
  //! @param theIndex index of the shape (0 <= theIndex < Size())
  TopoDS_Shape& ChangeValue(int theIndex)
  {
    return std::visit(
      [theIndex](auto& theStorage) -> TopoDS_Shape& { return theStorage.ChangeValue(theIndex); },
      myStorage);
  }

  //! Adds a shape to storage, switching to dynamic if needed
  //! @param theShape shape to add
  void Append(const TopoDS_Shape& theShape)
  {
    if (std::holds_alternative<LocalStorage>(myStorage))
    {
      auto& aLocal = std::get<LocalStorage>(myStorage);
      if (aLocal.IsFull())
      {
        // Switch to dynamic storage
        DynamicStorage aDynamic;
        aDynamic.MoveFrom(aLocal);
        aDynamic.Append(theShape);
        myStorage = std::move(aDynamic);
      }
      else
      {
        aLocal.Append(theShape);
      }
    }
    else
    {
      std::get<DynamicStorage>(myStorage).Append(theShape);
    }
  }

  //! Removes shape at index
  //! @param theIndex index of the shape to remove (0 <= theIndex < Size())
  void Remove(int theIndex)
  {
    std::visit([theIndex](auto& theStorage) { theStorage.Remove(theIndex); }, myStorage);
  }

  //! Clears all shapes
  void Clear()
  {
    // Reset to local storage
    myStorage = LocalStorage();
  }

private:
  StorageVariant myStorage; //!< Variant holding either local or dynamic storage
};

#endif // _TopoDS_ShapeStorage_HeaderFile
