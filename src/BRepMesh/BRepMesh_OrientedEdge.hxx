// Copyright (c) 2013 OPEN CASCADE SAS
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

#ifndef _BRepMesh_OrientedEdge_HeaderFile
#define _BRepMesh_OrientedEdge_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <BRepMesh_DegreeOfFreedom.hxx>

//! Light weighted structure representing simple link.
class BRepMesh_OrientedEdge
{
public:

  DEFINE_STANDARD_ALLOC

  //! Default constructor.
  BRepMesh_OrientedEdge()
    : myFirstNode(-1),
      myLastNode(-1)
  {
  }

  //! Constructs a link between two vertices.
  BRepMesh_OrientedEdge(
    const Standard_Integer theFirstNode,
    const Standard_Integer theLastNode)
    : myFirstNode(theFirstNode),
      myLastNode(theLastNode)
  {
  }

  //! Returns index of first node of the Link.
  Standard_Integer FirstNode() const
  {
    return myFirstNode;
  }

  //! Returns index of last node of the Link.
  Standard_Integer LastNode() const
  {
    return myLastNode;
  }

  //! Computes a hash code for this oriented edge, in the range [1, theUpperBound]
  //! @param theUpperBound the upper bound of the range a computing hash code must be within
  //! @return a computed hash code, in the range [1, theUpperBound]
  Standard_Integer HashCode (const Standard_Integer theUpperBound) const
  {
    return ::HashCode (myFirstNode + myLastNode, theUpperBound);
  }

  //! Checks this and other edge for equality.
  //! @param theOther edge to be checked against this one.
  //! @return TRUE if edges have the same orientation, FALSE if not.
  Standard_Boolean IsEqual(const BRepMesh_OrientedEdge& theOther) const
  {
    return (myFirstNode == theOther.myFirstNode && myLastNode == theOther.myLastNode);
  }

  //! Alias for IsEqual.
  Standard_Boolean operator ==(const BRepMesh_OrientedEdge& Other) const
  {
    return IsEqual(Other);
  }

private:

  Standard_Integer myFirstNode;
  Standard_Integer myLastNode;
};

//! Computes a hash code for the given oriented edge, in the range [1, theUpperBound]
//! @param theOrientedEdge the oriented edge which hash code is to be computed
//! @param theUpperBound the upper bound of the range a computing hash code must be within
//! @return a computed hash code, in the range [1, theUpperBound]
inline Standard_Integer HashCode (const BRepMesh_OrientedEdge& theOrientedEdge, const Standard_Integer theUpperBound)
{
  return theOrientedEdge.HashCode (theUpperBound);
}

#endif
