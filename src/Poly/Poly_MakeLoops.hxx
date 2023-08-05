// Created on: 2009-10-22
// Created by: Mikhail SAZONOV
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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

#ifndef Poly_MakeLoops_HeaderFile
#define Poly_MakeLoops_HeaderFile

#include <NCollection_Sequence.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_List.hxx>
#include <NCollection_OccAllocator.hxx>
#include <Standard_HashUtils.hxx>


/**
 * Make loops from a set of connected links. A link is represented by 
 * a pair of integer indices of nodes.
 */
class Poly_MakeLoops
{
public:
  //! Orientation flags that can be attached to a link
  enum LinkFlag {
    LF_None  = 0,
    LF_Fwd   = 1,    // forward orientation
    LF_Rev   = 2,    // reversed orientation
    LF_Both  = 3,    // both ways oriented
    LF_Reversed = 4  // means the link is reversed
  };

  //! The Link structure
  struct Link
  {
    Standard_Integer node1, node2;
    Standard_Integer flags;

    Link()
      : node1(0), node2(0), flags(0) {}

    Link(Standard_Integer theNode1, Standard_Integer theNode2)
      : node1(theNode1), node2(theNode2), flags(1) {}

    void Reverse()
    {
      flags ^= Poly_MakeLoops::LF_Reversed;
    }

    Standard_Boolean IsReversed() const
    {
      return (flags & Poly_MakeLoops::LF_Reversed) != 0;
    }

    void Nullify()
    {
      node1 = node2 = 0;
    }

    Standard_Boolean IsNull() const
    {
      return node1 == 0 || node2 == 0;
    }

    bool operator==(const Link& theOther) const
    {
      return (theOther.node1 == node1 && theOther.node2 == node2) ||
        (theOther.node1 == node2 && theOther.node2 == node1);
    }
  };

  struct Hasher
  {
    size_t operator()(const Poly_MakeLoops::Link& theLink) const noexcept
    {
      // Combine two int values into a single hash value.
      int aCombination[2]{ theLink.node1, theLink.node2 };
      if (aCombination[0] > aCombination[1])
      {
        std::swap(aCombination[0], aCombination[1]);
      }
      return opencascade::hashBytes(aCombination, sizeof(aCombination));
    }

    bool operator()(const Poly_MakeLoops::Link& theLink1, const Poly_MakeLoops::Link& theLink2) const noexcept
    {
      return theLink1 == theLink2;
    }
  };

  // Define the Loop as a list of links
  typedef NCollection_List<Link> ListOfLink;
  typedef ListOfLink Loop;

  //! The abstract helper class
  class Helper
  {
  public:
    //! returns the links adjacent to the given node
    virtual const ListOfLink& GetAdjacentLinks (Standard_Integer theNode) const = 0;
    //! hook function called from AddLink in _DEBUG mode
    virtual void OnAddLink (Standard_Integer /*theNum*/, const Link& /*theLink*/) const {}
  };

  //! This class implements a heap of integers. The most effective usage
  //! of it is first to add there all items, and then get top item and remove
  //! any items till it becomes empty.
  class HeapOfInteger
  {
  public:
    HeapOfInteger (const Standard_Integer theNbPreAllocated = 1)
      : myMap (theNbPreAllocated),
        myIterReady (Standard_False) {}

    void Clear()
    {
      myMap.Clear();
      myIterReady = Standard_False;
    }

    void Add (const Standard_Integer theValue)
    {
      myMap.Add (theValue);
      myIterReady = Standard_False;
    }

    Standard_Integer Top()
    {
      if (!myIterReady)
      {
        myIter.Initialize (myMap);
        myIterReady = Standard_True;
      }
      return myIter.Key();
    }

    Standard_Boolean Contains (const Standard_Integer theValue) const
    {
      return myMap.Contains (theValue);
    }

    void Remove (const Standard_Integer theValue)
    {
      if (myIterReady && myIter.More() && myIter.Key() == theValue)
        myIter.Next();
      myMap.Remove (theValue);
    }

    Standard_Boolean IsEmpty()
    {
      if (!myIterReady)
      {
        myIter.Initialize (myMap);
        myIterReady = Standard_True;
      }
      return !myIter.More();
    }

  private:
    TColStd_PackedMapOfInteger              myMap;
    TColStd_MapIteratorOfPackedMapOfInteger myIter;
    Standard_Boolean                        myIterReady;
  };

public:
  // PUBLIC METHODS

  //! Constructor. If helper is NULL then the algorithm will
  //! probably return a wrong result
  Standard_EXPORT Poly_MakeLoops(const Helper* theHelper,
                                 const Handle(NCollection_BaseAllocator)& theAlloc = 0L);

  //! It is to reset the algorithm to the initial state.
  Standard_EXPORT void Reset
                   (const Helper* theHelper,
                    const Handle(NCollection_BaseAllocator)& theAlloc = 0L);

  //! Adds a link to the set. theOrient defines which orientations of the link
  //! are allowed.
  Standard_EXPORT void AddLink(const Link& theLink);

  //! Replace one link with another (e.g. to change order of nodes)
  Standard_EXPORT void ReplaceLink(const Link& theLink, const Link& theNewLink);

  //! Set a new value of orientation of a link already added earlier.
  //! It can be used with LF_None to exclude the link from consideration.
  //! Returns the old value of orientation.
  Standard_EXPORT LinkFlag SetLinkOrientation
                   (const Link& theLink,
                    const LinkFlag theOrient);

  //! Find the link stored in algo by value
  Standard_EXPORT Link FindLink(const Link& theLink) const;

  enum ResultCode
  {
    RC_LoopsDone    = 1,
    RC_HangingLinks = 2,
    RC_Failure      = 4
  };

  //! Does the work. Returns the collection of result codes
  Standard_EXPORT Standard_Integer Perform();

  //! Returns the number of loops in the result
  Standard_Integer GetNbLoops() const
  {
    return myLoops.Length();
  }

  //! Returns the loop of the given index
  const Loop& GetLoop(Standard_Integer theIndex) const
  {
    return myLoops.Value(theIndex);
  }

  //! Returns the number of detected hanging chains
  Standard_Integer GetNbHanging() const
  {
    return myHangIndices.Extent();
  }

  //! Fills in the list of hanging links
  Standard_EXPORT void GetHangingLinks(ListOfLink& theLinks) const;

protected:
  virtual Standard_Integer chooseLeftWay
                   (const Standard_Integer theNode,
                    const Standard_Integer theSegIndex,
                    const NCollection_List<Standard_Integer>& theLstIndS) const = 0;

  const Helper* getHelper () const
  {
    return myHelper;
  }

  Link getLink (const Standard_Integer theSegIndex) const
  {
    Link aLink = myMapLink(Abs(theSegIndex));
    if (theSegIndex < 0)
      aLink.Reverse();
    return aLink;
  }
#ifdef OCCT_DEBUG
  void showBoundaryBreaks() const;
#endif

private:
  int findContour(Standard_Integer theIndexS, NCollection_IndexedMap<Standard_Integer>& theContour,
                  const Handle(NCollection_BaseAllocator)& theTempAlloc,
                  const Handle(NCollection_IncAllocator)& theTempAlloc1) const;
  void acceptContour(const NCollection_IndexedMap<Standard_Integer>& theContour,
                     Standard_Integer theStartNumber);
  Standard_Integer getFirstNode(Standard_Integer theIndexS) const;
  Standard_Integer getLastNode(Standard_Integer theIndexS) const;
  void markHangChain(Standard_Integer theNode, Standard_Integer theIndexS);
  Standard_Boolean canLinkBeTaken(Standard_Integer theIndexS) const;

  // FIELDS
  const Helper*                           myHelper;
  Handle(NCollection_BaseAllocator)       myAlloc;
  NCollection_IndexedMap<Link, Hasher>    myMapLink;
  NCollection_Sequence<Loop>              myLoops;
  HeapOfInteger                           myStartIndices;
  TColStd_PackedMapOfInteger              myHangIndices;
};

/**
 * Implementation for 3D space
 */
class gp_Dir;
class Poly_MakeLoops3D: public Poly_MakeLoops
{
public:
  //! The abstract helper class
  class Helper: public Poly_MakeLoops::Helper
  {
  public:
    // all the following methods should return False if 
    // it is impossible to return a valid direction

    //! returns the tangent vector at the first node of a link
    virtual Standard_Boolean GetFirstTangent(const Link& theLink,
                                             gp_Dir& theDir) const = 0;

    //! returns the tangent vector at the last node of a link
    virtual Standard_Boolean GetLastTangent(const Link& theLink,
                                            gp_Dir& theDir) const = 0;

    //! returns the normal to the surface at a given node
    virtual Standard_Boolean GetNormal(Standard_Integer theNode,
                                       gp_Dir& theDir) const = 0;
  };

  //! Constructor. If helper is NULL then the algorithm will
  //! probably return a wrong result
  Standard_EXPORT Poly_MakeLoops3D(const Helper* theHelper,
                                      const Handle(NCollection_BaseAllocator)& theAlloc);

protected:
  Standard_EXPORT virtual Standard_Integer chooseLeftWay
                   (const Standard_Integer theNode,
                    const Standard_Integer theSegIndex,
                    const NCollection_List<Standard_Integer>& theLstIndS) const;
  const Helper* getHelper () const
  {
    return static_cast<const Poly_MakeLoops3D::Helper*>
      (Poly_MakeLoops::getHelper());
  }
};

/**
 * Implementation for 2D space
 */
class gp_Dir2d;
class Poly_MakeLoops2D: public Poly_MakeLoops
{
public:
  //! The abstract helper class
  class Helper: public Poly_MakeLoops::Helper
  {
  public:
    // all the following methods should return False if 
    // it is impossible to return a valid direction

    //! returns the tangent vector at the first node of a link
    virtual Standard_Boolean GetFirstTangent(const Link& theLink,
                                             gp_Dir2d& theDir) const = 0;

    //! returns the tangent vector at the last node of a link
    virtual Standard_Boolean GetLastTangent(const Link& theLink,
                                            gp_Dir2d& theDir) const = 0;
  };

  //! Constructor. If helper is NULL then the algorithm will
  //! probably return a wrong result
  Standard_EXPORT Poly_MakeLoops2D(const Standard_Boolean theLeftWay,
                                      const Helper* theHelper,
                                      const Handle(NCollection_BaseAllocator)& theAlloc);

protected:
  Standard_EXPORT virtual Standard_Integer chooseLeftWay
                   (const Standard_Integer theNode,
                    const Standard_Integer theSegIndex,
                    const NCollection_List<Standard_Integer>& theLstIndS) const;
  const Helper* getHelper () const
  {
    return static_cast<const Poly_MakeLoops2D::Helper*>
      (Poly_MakeLoops::getHelper());
  }

private:
  //! this flag says that chooseLeftWay must choose the right way instead
  Standard_Boolean myRightWay;
};


namespace std
{
  template <>
  struct hash<Poly_MakeLoops::Link>
  {
    size_t operator()(const Poly_MakeLoops::Link& theLink) const noexcept
    {
      return Poly_MakeLoops::Hasher{}(theLink);
    }
  };

  template<>
  struct equal_to<Poly_MakeLoops::Link>
  {
    bool operator()(const Poly_MakeLoops::Link& theLink1,
                    const Poly_MakeLoops::Link& theLink2) const noexcept
    {
      return theLink1 == theLink2;
    }
  };
}

#endif
