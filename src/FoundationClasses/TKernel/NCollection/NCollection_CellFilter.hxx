// Created on: 2007-05-26
// Created by: Andrey BETENEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef NCollection_CellFilter_HeaderFile
#define NCollection_CellFilter_HeaderFile

#include <NCollection_LocalArray.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_HashUtils.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_IncAllocator.hxx>

//! Auxiliary enumeration serving as response from method Inspect
enum NCollection_CellFilter_Action
{
  CellFilter_Keep  = 0, //!< Target is needed and should be kept
  CellFilter_Purge = 1  //!< Target is not needed and can be removed from the current cell
};

/**
 * A data structure for sorting geometric objects (called targets) in
 * n-dimensional space into cells, with associated algorithm for fast checking
 * of coincidence (overlapping, intersection, etc.) with other objects
 * (called here bullets).
 *
 * Description
 *
 * The algorithm is based on hash map, thus it has linear time of initialization
 * (O(N) where N is number of cells covered by added targets) and constant-time
 * search for one bullet (more precisely, O(M) where M is number of cells covered
 * by the bullet).
 *
 * The idea behind the algorithm is to separate each coordinate of the space
 * into equal-size cells. Note that this works well when cell size is
 * approximately equal to the characteristic size of the involved objects
 * (targets and bullets; including tolerance eventually used for coincidence
 * check).
 *
 * Usage
 *
 * The target objects to be searched are added to the tool by methods Add();
 * each target is classified as belonging to some cell(s). The data on cells
 * (list of targets found in each one) are stored in the hash map with key being
 * cumulative index of the cell by all coordinates.
 * Thus the time needed to find targets in some cell is O(1) * O(number of
 * targets in the cell).
 *
 * As soon as all the targets are added, the algorithm is ready to check for
 * coincidence.
 * To find the targets coincident with any given bullet, it checks all the
 * candidate targets in the cell(s) covered by the bullet object
 * (methods Inspect()).
 *
 * The methods Add() and Inspect() have two flavours each: one accepts
 * single point identifying one cell, another accept two points specifying
 * the range of cells. It should be noted that normally at least one of these
 * methods is called as for range of cells: either due to objects having non-
 * zero size, or in order to account for the tolerance when objects are points.
 *
 * The set of targets can be modified during the process: new targets can be
 * added by Add(), existing targets can be removed by Remove().
 *
 * Implementation
 *
 * The algorithm is implemented as template class, thus it is capable to
 * work with objects of any type. The only argument of the template should be
 * the specific class providing all necessary features required by the
 * algorithm:
 *
 * - typedef "Target" defining type of target objects.
 *   This type must have copy constructor
 *
 * - typedef "Point" defining type of geometrical points used
 *
 * - enum Dimension whose value must be dimension of the point
 *
 * - method Coord() returning value of the i-th coordinate of the point:
 *
 *   static Standard_Real Coord (int i, const Point& thePnt);
 *
 *   Note that index i is from 0 to Dimension-1.
 *
 * - method IsEqual() used by Remove() to identify objects to be removed:
 *
 *   Standard_Boolean IsEqual (const Target& theT1, const Target& theT2);
 *
 * - method Inspect() performing necessary actions on the candidate target
 *   object (usially comparison with the currently checked bullet object):
 *
 *   NCollection_CellFilter_Action Inspect (const Target& theObject);
 *
 *   The returned value can be used to command CellFilter
 *   to remove the inspected item from the current cell; this allows
 *   to exclude the items that has been processed and are not needed any
 *   more in further search (for better performance).
 *
 *   Note that method Inspect() can be const and/or virtual.
 */

template <class Inspector>
class NCollection_CellFilter
{
public:
  typedef typename Inspector::Target Target;
  typedef typename Inspector::Point  Point;

public:
  //! Constructor; initialized by dimension count and cell size.
  //!
  //! Note: the cell size must be ensured to be greater than
  //! maximal coordinate of the involved points divided by INT_MAX,
  //! in order to avoid integer overflow of cell index.
  //!
  //! By default cell size is 0, which is invalid; thus if default
  //! constructor is used, the tool must be initialized later with
  //! appropriate cell size by call to Reset()
  //! Constructor when dimension count is unknown at compilation time.
  NCollection_CellFilter(const Standard_Integer                  theDim,
                         const Standard_Real                     theCellSize = 0,
                         const Handle(NCollection_IncAllocator)& theAlloc    = 0)
      : myCellSize(0, theDim - 1)
  {
    myDim = theDim;
    Reset(theCellSize, theAlloc);
  }

  //! Constructor when dimension count is known at compilation time.
  NCollection_CellFilter(const Standard_Real                     theCellSize = 0,
                         const Handle(NCollection_IncAllocator)& theAlloc    = 0)
      : myCellSize(0, Inspector::Dimension - 1)
  {
    myDim = Inspector::Dimension;
    Reset(theCellSize, theAlloc);
  }

  //! Clear the data structures, set new cell size and allocator
  void Reset(Standard_Real theCellSize, const Handle(NCollection_IncAllocator)& theAlloc = 0)
  {
    for (int i = 0; i < myDim; i++)
      myCellSize(i) = theCellSize;
    resetAllocator(theAlloc);
  }

  //! Clear the data structures and set new cell sizes and allocator
  void Reset(NCollection_Array1<Standard_Real>&      theCellSize,
             const Handle(NCollection_IncAllocator)& theAlloc = 0)
  {
    myCellSize = theCellSize;
    resetAllocator(theAlloc);
  }

  //! Adds a target object for further search at a point (into only one cell)
  void Add(const Target& theTarget, const Point& thePnt)
  {
    Cell aCell(thePnt, myCellSize);
    add(aCell, theTarget);
  }

  //! Adds a target object for further search in the range of cells
  //! defined by two points (the first point must have all coordinates equal or
  //! less than the same coordinate of the second point)
  void Add(const Target& theTarget, const Point& thePntMin, const Point& thePntMax)
  {
    // get cells range by minimal and maximal coordinates
    Cell aCellMin(thePntMin, myCellSize);
    Cell aCellMax(thePntMax, myCellSize);
    Cell aCell = aCellMin;
    // add object recursively into all cells in range
    iterateAdd(myDim - 1, aCell, aCellMin, aCellMax, theTarget);
  }

  //! Find a target object at a point and remove it from the structures.
  //! For usage of this method "operator ==" should be defined for Target.
  void Remove(const Target& theTarget, const Point& thePnt)
  {
    Cell aCell(thePnt, myCellSize);
    remove(aCell, theTarget);
  }

  //! Find a target object in the range of cells defined by two points and
  //! remove it from the structures
  //! (the first point must have all coordinates equal or
  //! less than the same coordinate of the second point).
  //! For usage of this method "operator ==" should be defined for Target.
  void Remove(const Target& theTarget, const Point& thePntMin, const Point& thePntMax)
  {
    // get cells range by minimal and maximal coordinates
    Cell aCellMin(thePntMin, myCellSize);
    Cell aCellMax(thePntMax, myCellSize);
    Cell aCell = aCellMin;
    // remove object recursively from all cells in range
    iterateRemove(myDim - 1, aCell, aCellMin, aCellMax, theTarget);
  }

  //! Inspect all targets in the cell corresponding to the given point
  void Inspect(const Point& thePnt, Inspector& theInspector)
  {
    Cell aCell(thePnt, myCellSize);
    inspect(aCell, theInspector);
  }

  //! Inspect all targets in the cells range limited by two given points
  //! (the first point must have all coordinates equal or
  //! less than the same coordinate of the second point)
  void Inspect(const Point& thePntMin, const Point& thePntMax, Inspector& theInspector)
  {
    // get cells range by minimal and maximal coordinates
    Cell aCellMin(thePntMin, myCellSize);
    Cell aCellMax(thePntMax, myCellSize);
    Cell aCell = aCellMin;
    // inspect object recursively into all cells in range
    iterateInspect(myDim - 1, aCell, aCellMin, aCellMax, theInspector);
  }

#if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x530)
public: // work-around against obsolete SUN WorkShop 5.3 compiler
#else
protected:
#endif

  /**
   * Auxiliary class for storing points belonging to the cell as the list
   */
  struct ListNode
  {
    ListNode()
    {
      // Empty constructor is forbidden.
      throw Standard_NoSuchObject("NCollection_CellFilter::ListNode()");
    }

    Target    Object;
    ListNode* Next;
  };

  //! Cell index type.
  typedef Standard_Integer Cell_IndexType;

  /**
   * Auxiliary structure representing a cell in the space.
   * Cells are stored in the map, each cell contains list of objects
   * that belong to that cell.
   */
  struct Cell
  {
  public:
    //! Constructor; computes cell indices
    Cell(const Point& thePnt, const NCollection_Array1<Standard_Real>& theCellSize)
        : index(theCellSize.Size()),
          Objects(0)
    {
      for (int i = 0; i < theCellSize.Size(); i++)
      {
        Standard_Real aVal =
          (Standard_Real)(Inspector::Coord(i, thePnt) / theCellSize(theCellSize.Lower() + i));
        // If the value of index is greater than
        // INT_MAX it is decreased correspondingly for the value of INT_MAX. If the value
        // of index is less than INT_MIN it is increased correspondingly for the absolute
        // value of INT_MIN.
        index[i] = Cell_IndexType((aVal > INT_MAX - 1)   ? fmod(aVal, (Standard_Real)INT_MAX)
                                  : (aVal < INT_MIN + 1) ? fmod(aVal, (Standard_Real)INT_MIN)
                                                         : aVal);
      }
    }

    //! Copy constructor: ensure that list is not deleted twice
    Cell(const Cell& theOther)
        : index(theOther.index.Size())
    {
      (*this) = theOther;
    }

    //! Assignment operator: ensure that list is not deleted twice
    void operator=(const Cell& theOther)
    {
      Standard_Integer aDim = Standard_Integer(theOther.index.Size());
      for (Standard_Integer anIdx = 0; anIdx < aDim; anIdx++)
        index[anIdx] = theOther.index[anIdx];

      Objects                   = theOther.Objects;
      ((Cell&)theOther).Objects = 0;
    }

    //! Destructor; calls destructors for targets contained in the list
    ~Cell()
    {
      for (ListNode* aNode = Objects; aNode; aNode = aNode->Next)
        aNode->Object.~Target();
      // note that list nodes need not to be freed, since IncAllocator is used
      Objects = 0;
    }

    //! Compare cell with other one
    Standard_Boolean IsEqual(const Cell& theOther) const
    {
      Standard_Integer aDim = Standard_Integer(theOther.index.Size());
      for (int i = 0; i < aDim; i++)
        if (index[i] != theOther.index[i])
          return Standard_False;
      return Standard_True;
    }

    bool operator==(const Cell& theOther) const { return IsEqual(theOther); }

  public:
    NCollection_LocalArray<Cell_IndexType, 10> index;
    ListNode*                                  Objects;
  };

  struct CellHasher
  {
    size_t operator()(const Cell& theCell) const noexcept
    {
      // number of bits per each dimension in the hash code
      const std::size_t aDim = theCell.index.Size();
      return opencascade::hashBytes(&theCell.index[0],
                                    static_cast<int>(aDim * sizeof(Cell_IndexType)));
    }

    bool operator()(const Cell& theCell1, const Cell& theCell2) const noexcept
    {
      return theCell1 == theCell2;
    }
  };

  typedef NCollection_Map<Cell, CellHasher> CellMap;

protected:
  //! Reset allocator to the new one
  void resetAllocator(const Handle(NCollection_IncAllocator)& theAlloc)
  {
    if (theAlloc.IsNull())
      myAllocator = new NCollection_IncAllocator;
    else
      myAllocator = theAlloc;
    myCells.Clear(myAllocator);
  }

  //! Add a new target object into the specified cell
  void add(const Cell& theCell, const Target& theTarget)
  {
    // add a new cell or get reference to existing one
    Cell& aMapCell = (Cell&)myCells.Added(theCell);

    // create a new list node and add it to the beginning of the list
    ListNode* aNode = (ListNode*)myAllocator->Allocate(sizeof(ListNode));
    new (&aNode->Object) Target(theTarget);
    aNode->Next      = aMapCell.Objects;
    aMapCell.Objects = aNode;
  }

  //! Internal addition function, performing iteration for adjacent cells
  //! by one dimension; called recursively to cover all dimensions
  void iterateAdd(int           idim,
                  Cell&         theCell,
                  const Cell&   theCellMin,
                  const Cell&   theCellMax,
                  const Target& theTarget)
  {
    const Cell_IndexType aStart = theCellMin.index[idim];
    const Cell_IndexType anEnd  = theCellMax.index[idim];
    for (Cell_IndexType i = aStart; i <= anEnd; ++i)
    {
      theCell.index[idim] = i;
      if (idim) // recurse
      {
        iterateAdd(idim - 1, theCell, theCellMin, theCellMax, theTarget);
      }
      else // add to this cell
      {
        add(theCell, theTarget);
      }
    }
  }

  //! Remove the target object from the specified cell
  void remove(const Cell& theCell, const Target& theTarget)
  {
    // check if any objects are recorded in that cell
    if (!myCells.Contains(theCell))
      return;

    // iterate by objects in the cell and check each
    Cell&     aMapCell = (Cell&)myCells.Added(theCell);
    ListNode* aNode    = aMapCell.Objects;
    ListNode* aPrev    = NULL;
    while (aNode)
    {
      ListNode* aNext = aNode->Next;
      if (Inspector::IsEqual(aNode->Object, theTarget))
      {
        aNode->Object.~Target();
        (aPrev ? aPrev->Next : aMapCell.Objects) = aNext;
        // note that aNode itself need not to be freed, since IncAllocator is used
      }
      else
        aPrev = aNode;
      aNode = aNext;
    }
  }

  //! Internal removal function, performing iteration for adjacent cells
  //! by one dimension; called recursively to cover all dimensions
  void iterateRemove(int           idim,
                     Cell&         theCell,
                     const Cell&   theCellMin,
                     const Cell&   theCellMax,
                     const Target& theTarget)
  {
    const Cell_IndexType aStart = theCellMin.index[idim];
    const Cell_IndexType anEnd  = theCellMax.index[idim];
    for (Cell_IndexType i = aStart; i <= anEnd; ++i)
    {
      theCell.index[idim] = i;
      if (idim) // recurse
      {
        iterateRemove(idim - 1, theCell, theCellMin, theCellMax, theTarget);
      }
      else // remove from this cell
      {
        remove(theCell, theTarget);
      }
    }
  }

  //! Inspect the target objects in the specified cell.
  void inspect(const Cell& theCell, Inspector& theInspector)
  {
    // check if any objects are recorded in that cell
    if (!myCells.Contains(theCell))
      return;

    // iterate by objects in the cell and check each
    Cell&     aMapCell = (Cell&)myCells.Added(theCell);
    ListNode* aNode    = aMapCell.Objects;
    ListNode* aPrev    = NULL;
    while (aNode)
    {
      ListNode*                     aNext    = aNode->Next;
      NCollection_CellFilter_Action anAction = theInspector.Inspect(aNode->Object);
      // delete items requested to be purged
      if (anAction == CellFilter_Purge)
      {
        aNode->Object.~Target();
        (aPrev ? aPrev->Next : aMapCell.Objects) = aNext;
        // note that aNode itself need not to be freed, since IncAllocator is used
      }
      else
        aPrev = aNode;
      aNode = aNext;
    }
  }

  //! Inspect the target objects in the specified range of the cells
  void iterateInspect(int         idim,
                      Cell&       theCell,
                      const Cell& theCellMin,
                      const Cell& theCellMax,
                      Inspector&  theInspector)
  {
    const Cell_IndexType aStart = theCellMin.index[idim];
    const Cell_IndexType anEnd  = theCellMax.index[idim];
    for (Cell_IndexType i = aStart; i <= anEnd; ++i)
    {
      theCell.index[idim] = i;
      if (idim) // recurse
      {
        iterateInspect(idim - 1, theCell, theCellMin, theCellMax, theInspector);
      }
      else // inspect this cell
      {
        inspect(theCell, theInspector);
      }
    }
  }

protected:
  Standard_Integer                  myDim;
  Handle(NCollection_BaseAllocator) myAllocator;
  CellMap                           myCells;
  NCollection_Array1<Standard_Real> myCellSize;
};

/**
 * Base class defining part of the Inspector interface
 * for CellFilter algorithm, working with gp_XYZ points in 3d space
 */

class gp_XYZ;

struct NCollection_CellFilter_InspectorXYZ
{
  //! Points dimension
  enum
  {
    Dimension = 3
  };

  //! Points type
  typedef gp_XYZ Point;

  //! Access to coordinate
  static Standard_Real Coord(int i, const Point& thePnt) { return thePnt.Coord(i + 1); }

  //! Auxiliary method to shift point by each coordinate on given value;
  //! useful for preparing a points range for Inspect with tolerance
  Point Shift(const Point& thePnt, Standard_Real theTol) const
  {
    return Point(thePnt.X() + theTol, thePnt.Y() + theTol, thePnt.Z() + theTol);
  }
};

/**
 * Base class defining part of the Inspector interface
 * for CellFilter algorithm, working with gp_XY points in 2d space
 */

class gp_XY;

struct NCollection_CellFilter_InspectorXY
{
  //! Points dimension
  enum
  {
    Dimension = 2
  };

  //! Points type
  typedef gp_XY Point;

  //! Access to coordinate
  static Standard_Real Coord(int i, const Point& thePnt) { return thePnt.Coord(i + 1); }

  //! Auxiliary method to shift point by each coordinate on given value;
  //! useful for preparing a points range for Inspect with tolerance
  Point Shift(const Point& thePnt, Standard_Real theTol) const
  {
    return Point(thePnt.X() + theTol, thePnt.Y() + theTol);
  }
};

#endif
