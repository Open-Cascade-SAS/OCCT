// Created on: 1993-04-30
// Created by: Yves FRICAUD
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _MAT_Arc_HeaderFile
#define _MAT_Arc_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <MAT_Side.hxx>
class MAT_BasicElt;
class MAT_Node;

//! An Arc is associated to each Bisecting of the mat.
class MAT_Arc : public Standard_Transient
{

public:
  Standard_EXPORT MAT_Arc(const int                        ArcIndex,
                          const int                        GeomIndex,
                          const occ::handle<MAT_BasicElt>& FirstElement,
                          const occ::handle<MAT_BasicElt>& SecondElement);

  //! Returns the index of <me> in Graph.theArcs.
  Standard_EXPORT int Index() const;

  //! Returns the index associated of the geometric
  //! representation of <me>.
  Standard_EXPORT int GeomIndex() const;

  //! Returns one of the BasicElt equidistant from <me>.
  Standard_EXPORT occ::handle<MAT_BasicElt> FirstElement() const;

  //! Returns the other BasicElt equidistant from <me>.
  Standard_EXPORT occ::handle<MAT_BasicElt> SecondElement() const;

  //! Returns one Node extremity of <me>.
  Standard_EXPORT occ::handle<MAT_Node> FirstNode() const;

  //! Returns the other Node extremity of <me>.
  Standard_EXPORT occ::handle<MAT_Node> SecondNode() const;

  //! An Arc has two Node, if <aNode> equals one
  //! Returns the other.
  //!
  //! if <aNode> is not oh <me>
  Standard_EXPORT occ::handle<MAT_Node> TheOtherNode(const occ::handle<MAT_Node>& aNode) const;

  //! Returns True if there is an arc linked to
  //! the Node <aNode> located on the side <aSide> of <me>;
  //! if <aNode> is not on <me>
  Standard_EXPORT bool HasNeighbour(const occ::handle<MAT_Node>& aNode, const MAT_Side aSide) const;

  //! Returns the first arc linked to the Node <aNode>
  //! located on the side <aSide> of <me>;
  //! if HasNeighbour() returns FALSE.
  Standard_EXPORT occ::handle<MAT_Arc> Neighbour(const occ::handle<MAT_Node>& aNode,
                                                 const MAT_Side               aSide) const;

  Standard_EXPORT void SetIndex(const int anInteger);

  Standard_EXPORT void SetGeomIndex(const int anInteger);

  Standard_EXPORT void SetFirstElement(const occ::handle<MAT_BasicElt>& aBasicElt);

  Standard_EXPORT void SetSecondElement(const occ::handle<MAT_BasicElt>& aBasicElt);

  Standard_EXPORT void SetFirstNode(const occ::handle<MAT_Node>& aNode);

  Standard_EXPORT void SetSecondNode(const occ::handle<MAT_Node>& aNode);

  Standard_EXPORT void SetFirstArc(const MAT_Side aSide, const occ::handle<MAT_Arc>& anArc);

  Standard_EXPORT void SetSecondArc(const MAT_Side aSide, const occ::handle<MAT_Arc>& anArc);

  Standard_EXPORT void SetNeighbour(const MAT_Side               aSide,
                                    const occ::handle<MAT_Node>& aNode,
                                    const occ::handle<MAT_Arc>&  anArc);

  DEFINE_STANDARD_RTTIEXT(MAT_Arc, Standard_Transient)

private:
  int                       arcIndex;
  int                       geomIndex;
  occ::handle<MAT_BasicElt> firstElement;
  occ::handle<MAT_BasicElt> secondElement;
  occ::handle<MAT_Node>     firstNode;
  occ::handle<MAT_Node>     secondNode;
  void*                     firstArcLeft;
  void*                     firstArcRight;
  void*                     secondArcRight;
  void*                     secondArcLeft;
};

#endif // _MAT_Arc_HeaderFile
