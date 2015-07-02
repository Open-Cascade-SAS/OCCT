// Created on: 2015-06-17
// Created by: Alexander Malyshev
// Copyright (c) 2007-2015 OPEN CASCADE SAS
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

#ifndef NCollection_CellFilterNDim_HeaderFile
#define NCollection_CellFilterNDim_HeaderFile

#include <NCollection_CellFilter.hxx>
#include <Standard_Real.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_TypeDef.hxx>
#include <NCollection_Array1.hxx>

/**
 * A data structure for sorting geometric objects (called targets) in 
 * n-dimensional space into cells, with associated algorithm for fast checking 
 * of coincidence (overlapping, intersection, etc.) with other objects 
 * (called here bullets).
 *
 * Purpose of this class is to add possibility to work with CellFilter with unknown
   dimension count at compilation time.
 *
 * For more details look at base class NCollection_CellFilter.
 *
 */

template <class Inspector> class NCollection_CellFilterNDim
{
public:
  typedef TYPENAME Inspector::Target Target;
  typedef TYPENAME Inspector::Point  Point;

public:

  //! Constructor; initialized by dimension count and cell size.
  //!
  //! Note: the cell size must be ensured to be greater than
  //! maximal co-ordinate of the involved points divided by INT_MAX,
  //! in order to avoid integer overflow of cell index.
  //! 
  //! By default cell size is 0, which is invalid; thus if default
  //! constructor is used, the tool must be initialized later with
  //! appropriate cell size by call to Reset()
  NCollection_CellFilterNDim (const Standard_Integer theDim,
                              const Standard_Real theCellSize = 0,
                              const Handle(NCollection_IncAllocator)& theAlloc = 0)
  : myCellSize(0, theDim - 1)
  {
    myDim = theDim;
    Reset (theCellSize, theAlloc);
  }

  //! Constructor; initialized by dimension count and cell sizes along each dimension.
  //! Note: the cell size in each dimension must be ensured to be greater than 
  //! maximal co-ordinate of the involved points by this dimension divided by INT_MAX,
  //! in order to avoid integer overflow of cell index.
  NCollection_CellFilterNDim (const Standard_Integer theDim,
                              const NCollection_Array1<Standard_Real> theCellSize,
                              const Handle(NCollection_IncAllocator)& theAlloc = 0)
  : myCellSize(0, theDim - 1)
  {
    myDim = theDim;
    Reset (theCellSize, theAlloc);
  }

  //! Clear the data structures, set new cell size and allocator
  void Reset (Standard_Real theCellSize, 
              const Handle(NCollection_IncAllocator)& theAlloc=0)
  {
    for (int i=0; i < myDim; i++)
      myCellSize(i) = theCellSize;
    resetAllocator ( theAlloc );
  }

  //! Clear the data structures and set new cell sizes and allocator
  void Reset (NCollection_Array1<Standard_Real> theCellSize, 
              const Handle(NCollection_IncAllocator)& theAlloc=0)
  {
    myCellSize = theCellSize;
    resetAllocator ( theAlloc );
  }
  
  //! Adds a target object for further search at a point (into only one cell)
  void Add (const Target& theTarget, const Point &thePnt)
  {
    Cell aCell (thePnt, myCellSize);
    add (aCell, theTarget);
  }

  //! Adds a target object for further search in the range of cells 
  //! defined by two points (the first point must have all co-ordinates equal or
  //! less than the same co-ordinate of the second point)
  void Add (const Target& theTarget, 
	    const Point &thePntMin, const Point &thePntMax)
  {
    // get cells range by minimal and maximal co-ordinates
    Cell aCellMin (thePntMin, myCellSize);
    Cell aCellMax (thePntMax, myCellSize);
    Cell aCell = aCellMin;
    // add object recursively into all cells in range
    iterateAdd (myDim-1, aCell, aCellMin, aCellMax, theTarget);
  }

  //! Find a target object at a point and remove it from the structures.
  //! For usage of this method "operator ==" should be defined for Target.
  void Remove (const Target& theTarget, const Point &thePnt)
  {
    Cell aCell (thePnt, myCellSize);
    remove (aCell, theTarget);
  }

  //! Find a target object in the range of cells defined by two points and
  //! remove it from the structures
  //! (the first point must have all co-ordinates equal or
  //! less than the same co-ordinate of the second point).
  //! For usage of this method "operator ==" should be defined for Target.
  void Remove (const Target& theTarget, 
               const Point &thePntMin, const Point &thePntMax)
  {
    // get cells range by minimal and maximal co-ordinates
    Cell aCellMin (thePntMin, myCellSize);
    Cell aCellMax (thePntMax, myCellSize);
    Cell aCell = aCellMin;
    // remove object recursively from all cells in range
    iterateRemove (myDim-1, aCell, aCellMin, aCellMax, theTarget);
  }

  //! Inspect all targets in the cell corresponding to the given point
  void Inspect (const Point& thePnt, Inspector &theInspector) 
  {
    Cell aCell (thePnt, myCellSize);
    inspect (aCell, theInspector);
  }

  //! Inspect all targets in the cells range limited by two given points
  //! (the first point must have all co-ordinates equal or
  //! less than the same co-ordinate of the second point)
  void Inspect (const Point& thePntMin, const Point& thePntMax, 
                Inspector &theInspector) 
  {
    // get cells range by minimal and maximal co-ordinates
    Cell aCellMin (thePntMin, myCellSize);
    Cell aCellMax (thePntMax, myCellSize);
    Cell aCell = aCellMin;
    // inspect object recursively into all cells in range
    iterateInspect (myDim-1, aCell, 
                    aCellMin, aCellMax, theInspector);
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
      Standard_NoSuchObject::Raise("NCollection_CellFilterNDim::ListNode()");
    }

    Target Object;
    ListNode *Next;
  };

  /**
   * Auxilary structure representing a cell in the space. 
   * Cells are stored in the map, each cell contains list of objects 
   * that belong to that cell.
   */
  struct Cell
  {
  public:

    //! Constructor; computes cell indices
    Cell (const Point& thePnt, 
          const NCollection_Array1<Standard_Real> theCellSize)
      : index(0, theCellSize.Size() - 1),
        Objects(0)
    {
      for (int i=0; i < theCellSize.Size(); i++)
      {
          Standard_Real val = (Standard_Real)(Inspector::Coord(i, thePnt) / theCellSize(i));
          //If the value of index is greater than
          //INT_MAX it is decreased correspondingly for the value of INT_MAX. If the value
          //of index is less than INT_MIN it is increased correspondingly for the absolute
          //value of INT_MIN.
          index(i) = long((val > INT_MAX - 1) ? fmod(val, (Standard_Real) INT_MAX) 
                                               : (val < INT_MIN + 1) ? fmod(val, (Standard_Real) INT_MIN)
                                                                     : val);
      }
    }

    //! Copy constructor: ensure that list is not deleted twice
    Cell (const Cell& theOther)
      : index(0, theOther.index.Size() - 1)
    {
      (*this) = theOther;
    }

    //! Assignment operator: ensure that list is not deleted twice
    void operator = (const Cell& theOther)
    {
      index = theOther.index;
      Objects = theOther.Objects;
      ((Cell&)theOther).Objects = 0;
    }

    //! Destructor; calls destructors for targets contained in the list
    ~Cell ()
    {
      for ( ListNode* aNode = Objects; aNode; aNode = aNode->Next )
        aNode->Object.~Target();
      // note that list nodes need not to be freed, since IncAllocator is used
      Objects = 0;
    }

    //! Compare cell with other one
    Standard_Boolean IsEqual (const Cell& theOther) const
    {
      Standard_Integer myDim = theOther.index.Size();
      for (int i=0; i < myDim; i++) 
        if ( index(i) != theOther.index(i) ) return Standard_False;
      return Standard_True;
    }

    //! Compute hash code
    Standard_Integer HashCode (const Standard_Integer theUpper) const
    {
      // number of bits per each dimension in the hash code
      Standard_Integer myDim = index.Size();
      const Standard_Size aShiftBits = (BITS(long)-1) / myDim;
      long aCode=0;
      for (int i=0; i < myDim; i++)
        aCode = ( aCode << aShiftBits ) ^ index(i);
      return (unsigned)aCode % theUpper;
    }

  public:
    NCollection_Array1<long> index;
    ListNode *Objects;
  };

  // definition of global functions is needed for map
  friend Standard_Integer HashCode (const Cell &aCell, const Standard_Integer theUpper)
  { return aCell.HashCode(theUpper); }
  friend Standard_Boolean IsEqual (const Cell &aCell1, const Cell &aCell2)
  { return aCell1.IsEqual(aCell2); }

protected:

  //! Reset allocator to the new one
  void resetAllocator (const Handle(NCollection_IncAllocator)& theAlloc)
  {
    if ( theAlloc.IsNull() )
      myAllocator = new NCollection_IncAllocator;
    else 
      myAllocator = theAlloc;
    myCells.Clear ( myAllocator );
  }
  
  //! Add a new target object into the specified cell
  void add (const Cell& theCell, const Target& theTarget)
  {
    // add a new cell or get reference to existing one
    Cell& aMapCell = (Cell&)myCells.Added (theCell);

    // create a new list node and add it to the beginning of the list
    ListNode* aNode = (ListNode*)myAllocator->Allocate(sizeof(ListNode));
    new (&aNode->Object) Target (theTarget);
    aNode->Next = aMapCell.Objects;
    aMapCell.Objects = aNode;
  }

  //! Internal addition function, performing iteration for adjacent cells
  //! by one dimension; called recursively to cover all dimensions
  void iterateAdd (int idim, Cell &theCell, 
		   const Cell& theCellMin, const Cell& theCellMax, 
                   const Target& theTarget)
  {
    int start = theCellMin.index(idim);
    int end   = theCellMax.index(idim);
    for (int i=start; i <= end; i++) {
      theCell.index(idim) = i;
      if ( idim ) // recurse
        iterateAdd (idim-1, theCell, theCellMin, theCellMax, theTarget);
      else // add to this cell
        add (theCell, theTarget);
    }
  }
  
  //! Remove the target object from the specified cell
  void remove (const Cell& theCell, const Target& theTarget)
  {
    // check if any objects are recorded in that cell
    if ( ! myCells.Contains (theCell) ) 
      return;

    // iterate by objects in the cell and check each
    Cell& aMapCell = (Cell&)myCells.Added (theCell);
    ListNode* aNode = aMapCell.Objects;
    ListNode* aPrev = NULL;
    while (aNode)
    {
      ListNode* aNext = aNode->Next;
      if (Inspector::IsEqual (aNode->Object, theTarget))
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
  void iterateRemove (int idim, Cell &theCell, 
                      const Cell& theCellMin, const Cell& theCellMax, 
                      const Target& theTarget)
  {
    int start = theCellMin.index(idim);
    int end   = theCellMax.index(idim);
    for (int i=start; i <= end; i++) {
      theCell.index(idim) = i;
      if ( idim ) // recurse
        iterateRemove (idim-1, theCell, theCellMin, theCellMax, theTarget);
      else // remove from this cell
        remove (theCell, theTarget);
    }
  }

  //! Inspect the target objects in the specified cell.
  void inspect (const Cell& theCell, Inspector& theInspector) 
  {
    // check if any objects are recorded in that cell
    if ( ! myCells.Contains (theCell) ) 
      return;

    // iterate by objects in the cell and check each
    Cell& aMapCell = (Cell&)myCells.Added (theCell);
    ListNode* aNode = aMapCell.Objects;
    ListNode* aPrev = NULL;
    while(aNode) {
      ListNode* aNext = aNode->Next;
      NCollection_CellFilter_Action anAction = 
        theInspector.Inspect (aNode->Object);
      // delete items requested to be purged
      if ( anAction == CellFilter_Purge ) {
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
  void iterateInspect (int idim, Cell &theCell, 
	               const Cell& theCellMin, const Cell& theCellMax, 
                       Inspector& theInspector) 
  {
    int start = theCellMin.index(idim);
    int end   = theCellMax.index(idim);
    for (int i=start; i <= end; i++) {
      theCell.index(idim) = i;
      if ( idim ) // recurse
        iterateInspect (idim-1, theCell, theCellMin, theCellMax, theInspector);
      else // inspect this cell
        inspect (theCell, theInspector);
    }
  }

protected:
  Standard_Integer myDim;
  Handle(NCollection_BaseAllocator) myAllocator;
  NCollection_Map<Cell>             myCells;
  NCollection_Array1<Standard_Real> myCellSize;
};

#endif
