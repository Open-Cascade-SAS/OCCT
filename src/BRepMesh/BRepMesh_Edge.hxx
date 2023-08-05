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

#ifndef _BRepMesh_Edge_HeaderFile
#define _BRepMesh_Edge_HeaderFile

#include <Standard.hxx>
#include <BRepMesh_DegreeOfFreedom.hxx>
#include <BRepMesh_OrientedEdge.hxx>
#include <Standard_HashUtils.hxx>

//! Light weighted structure representing link of the mesh.
class BRepMesh_Edge : public BRepMesh_OrientedEdge
{
public:

    //! Default constructor.
  BRepMesh_Edge()
    : BRepMesh_OrientedEdge(),
      myMovability(BRepMesh_Deleted)
  {
  }

  //! Constructs a link between two vertices.
  BRepMesh_Edge(
    const Standard_Integer         theFirstNode,
    const Standard_Integer         theLastNode,
    const BRepMesh_DegreeOfFreedom theMovability)
    : BRepMesh_OrientedEdge(theFirstNode, theLastNode),
      myMovability(theMovability)
  {
  }

  //! Returns movability flag of the Link.
  BRepMesh_DegreeOfFreedom Movability() const
  {
    return myMovability;
  }

  //! Sets movability flag of the Link.
  //! @param theMovability flag to be set.
  void SetMovability(const BRepMesh_DegreeOfFreedom theMovability)
  {
    myMovability = theMovability;
  }

  //! Checks if the given edge and this one have the same orientation.
  //! @param theOther edge to be checked against this one.
  //! \return TRUE if edges have the same orientation, FALSE if not.
  Standard_Boolean IsSameOrientation(const BRepMesh_Edge& theOther) const
  {
    return BRepMesh_OrientedEdge::IsEqual(theOther);
  }

  //! Checks for equality with another edge.
  //! @param theOther edge to be checked against this one.
  //! @return TRUE if equal, FALSE if not.
  Standard_Boolean IsEqual(const BRepMesh_Edge& theOther) const
  {
    if (myMovability == BRepMesh_Deleted || theOther.myMovability == BRepMesh_Deleted)
      return Standard_False;

    return IsSameOrientation(theOther) ||
      (FirstNode() == theOther.LastNode() && LastNode() == theOther.FirstNode());
  }

  //! Alias for IsEqual.
  Standard_Boolean operator ==(const BRepMesh_Edge& Other) const
  {
    return IsEqual(Other);
  }

private:

  BRepMesh_DegreeOfFreedom  myMovability;
};

namespace std
{
  template <>
  struct hash<BRepMesh_Edge>
  {
    size_t operator()(const BRepMesh_Edge& theEdge) const noexcept
    {
      union Combination
      {
        unsigned short Arr[2]; // Node can be represented as a short
        uint32_t Hash;

      } aCombination;
      aCombination.Arr[0] = static_cast<unsigned short>(theEdge.FirstNode());
      aCombination.Arr[1] = static_cast<unsigned short>(theEdge.LastNode());
      if (aCombination.Arr[0] > aCombination.Arr[1])
      {
        std::swap(aCombination.Arr[0], aCombination.Arr[1]);
      }
      return static_cast<size_t>(aCombination.Hash);
    }
  };
}

#endif
