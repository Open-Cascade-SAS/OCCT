// Created on: 1993-05-04
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

#include <MAT_Arc.hxx>
#include <MAT_Node.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MAT_Arc, Standard_Transient)

//=================================================================================================

MAT_Arc::MAT_Arc(const Standard_Integer      ArcIndex,
                 const Standard_Integer      GeomIndex,
                 const Handle(MAT_BasicElt)& FirstElement,
                 const Handle(MAT_BasicElt)& SecondElement)
    : arcIndex(ArcIndex),
      geomIndex(GeomIndex),
      firstArcLeft(0),
      firstArcRight(0),
      secondArcRight(0),
      secondArcLeft(0)
{
  firstElement  = FirstElement;
  secondElement = SecondElement;
}

//=================================================================================================

Standard_Integer MAT_Arc::Index() const
{
  return arcIndex;
}

//=================================================================================================

Standard_Integer MAT_Arc::GeomIndex() const
{
  return geomIndex;
}

//=================================================================================================

Handle(MAT_BasicElt) MAT_Arc::FirstElement() const
{
  return firstElement;
}

//=================================================================================================

Handle(MAT_BasicElt) MAT_Arc::SecondElement() const
{
  return secondElement;
}

//=================================================================================================

Handle(MAT_Node) MAT_Arc::FirstNode() const
{
  return firstNode;
}

//=================================================================================================

Handle(MAT_Node) MAT_Arc::SecondNode() const
{
  return secondNode;
}

//=================================================================================================

Handle(MAT_Node) MAT_Arc::TheOtherNode(const Handle(MAT_Node)& aNode) const
{
  if (FirstNode() == aNode)
    return SecondNode();
  else if (SecondNode() == aNode)
    return FirstNode();
  else
  {
    throw Standard_DomainError("MAT_Arc::TheOtherNode");
  }
}

//=================================================================================================

Standard_Boolean MAT_Arc::HasNeighbour(const Handle(MAT_Node)& aNode, const MAT_Side aSide) const
{
  if (aSide == MAT_Left)
  {
    //    if (aNode == FirstNode())  return (!firstArcLeft  == NULL);
    if (aNode == FirstNode())
      return (firstArcLeft != 0);
    //    if (aNode == SecondNode()) return (!secondArcLeft == NULL);
    if (aNode == SecondNode())
      return (secondArcLeft != 0);
  }
  else
  {
    //    if (aNode == FirstNode())  return (!firstArcRight  == NULL);
    if (aNode == FirstNode())
      return (firstArcRight != 0);
    //    if (aNode == SecondNode()) return (!secondArcRight == NULL);
    if (aNode == SecondNode())
      return (secondArcRight != 0);
  }
  throw Standard_DomainError("MAT_Arc::HasNeighbour");
}

//=================================================================================================

Handle(MAT_Arc) MAT_Arc::Neighbour(const Handle(MAT_Node)& aNode, const MAT_Side aSide) const

{
  if (aSide == MAT_Left)
  {
    if (aNode == FirstNode())
      return (MAT_Arc*)firstArcLeft;
    if (aNode == SecondNode())
      return (MAT_Arc*)secondArcLeft;
  }
  else
  {
    if (aNode == FirstNode())
      return (MAT_Arc*)firstArcRight;
    if (aNode == SecondNode())
      return (MAT_Arc*)secondArcRight;
  }
  throw Standard_DomainError("MAT_Arc::Neighbour");
}

//=================================================================================================

void MAT_Arc::SetIndex(const Standard_Integer anInteger)
{
  arcIndex = anInteger;
}

//=================================================================================================

void MAT_Arc::SetGeomIndex(const Standard_Integer anInteger)
{
  geomIndex = anInteger;
}

//=================================================================================================

void MAT_Arc::SetFirstElement(const Handle(MAT_BasicElt)& aBasicElt)
{
  firstElement = aBasicElt;
}

//=================================================================================================

void MAT_Arc::SetSecondElement(const Handle(MAT_BasicElt)& aBasicElt)
{
  secondElement = aBasicElt;
}

//=================================================================================================

void MAT_Arc::SetFirstNode(const Handle(MAT_Node)& aNode)
{
  firstNode = aNode;
}

//=================================================================================================

void MAT_Arc::SetSecondNode(const Handle(MAT_Node)& aNode)
{
  secondNode = aNode;
}

//=================================================================================================

void MAT_Arc::SetFirstArc(const MAT_Side aSide, const Handle(MAT_Arc)& anArc)
{
  if (aSide == MAT_Left)
    firstArcLeft = anArc.get();
  else
    firstArcRight = anArc.get();
}

//=================================================================================================

void MAT_Arc::SetSecondArc(const MAT_Side aSide, const Handle(MAT_Arc)& anArc)
{
  if (aSide == MAT_Left)
    secondArcLeft = anArc.get();
  else
    secondArcRight = anArc.get();
}

//=================================================================================================

void MAT_Arc::SetNeighbour(const MAT_Side          aSide,
                           const Handle(MAT_Node)& aNode,
                           const Handle(MAT_Arc)&  anArc)
{
  if (aSide == MAT_Left)
  {
    if (aNode == FirstNode())
      firstArcLeft = anArc.get();
    else if (aNode == SecondNode())
      secondArcLeft = anArc.get();
    else
      throw Standard_DomainError("MAT_Arc::SetNeighbour");
  }
  else
  {
    if (aNode == FirstNode())
      firstArcRight = anArc.get();
    else if (aNode == SecondNode())
      secondArcRight = anArc.get();
    else
      throw Standard_DomainError("MAT_Arc::SetNeighbour");
  }
}
