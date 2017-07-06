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
#include <Standard_Address.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
#include <MAT_Side.hxx>
class MAT_BasicElt;
class MAT_Node;
class Standard_DomainError;


class MAT_Arc;
DEFINE_STANDARD_HANDLE(MAT_Arc, Standard_Transient)

//! An Arc is associated to each Bisecting of the mat.
class MAT_Arc : public Standard_Transient
{

public:

  
  Standard_EXPORT MAT_Arc(const Standard_Integer ArcIndex, const Standard_Integer GeomIndex, const Handle(MAT_BasicElt)& FirstElement, const Handle(MAT_BasicElt)& SecondElement);
  
  //! Returns the index of <me> in Graph.theArcs.
  Standard_EXPORT Standard_Integer Index() const;
  
  //! Returns  the index associated  of the  geometric
  //! representation of <me>.
  Standard_EXPORT Standard_Integer GeomIndex() const;
  
  //! Returns one of the BasicElt equidistant from <me>.
  Standard_EXPORT Handle(MAT_BasicElt) FirstElement() const;
  
  //! Returns the other BasicElt equidistant from <me>.
  Standard_EXPORT Handle(MAT_BasicElt) SecondElement() const;
  
  //! Returns one Node extremity of <me>.
  Standard_EXPORT Handle(MAT_Node) FirstNode() const;
  
  //! Returns the other Node extremity of <me>.
  Standard_EXPORT Handle(MAT_Node) SecondNode() const;
  
  //! an Arc has two Node, if <aNode> egal one
  //! Returns the other.
  //!
  //! if <aNode> is not oh <me>
  Standard_EXPORT Handle(MAT_Node) TheOtherNode (const Handle(MAT_Node)& aNode) const;
  
  //! Returnst True is there is an arc linked to
  //! the Node <aNode> located on the side <aSide> of <me>;
  //! if <aNode> is not on <me>
  Standard_EXPORT Standard_Boolean HasNeighbour (const Handle(MAT_Node)& aNode, const MAT_Side aSide) const;
  
  //! Returns the first arc linked to the Node <aNode>
  //! located on the side <aSide> of <me>;
  //! if HasNeighbour() returns FALSE.
  Standard_EXPORT Handle(MAT_Arc) Neighbour (const Handle(MAT_Node)& aNode, const MAT_Side aSide) const;
  
  Standard_EXPORT void SetIndex (const Standard_Integer anInteger);
  
  Standard_EXPORT void SetGeomIndex (const Standard_Integer anInteger);
  
  Standard_EXPORT void SetFirstElement (const Handle(MAT_BasicElt)& aBasicElt);
  
  Standard_EXPORT void SetSecondElement (const Handle(MAT_BasicElt)& aBasicElt);
  
  Standard_EXPORT void SetFirstNode (const Handle(MAT_Node)& aNode);
  
  Standard_EXPORT void SetSecondNode (const Handle(MAT_Node)& aNode);
  
  Standard_EXPORT void SetFirstArc (const MAT_Side aSide, const Handle(MAT_Arc)& anArc);
  
  Standard_EXPORT void SetSecondArc (const MAT_Side aSide, const Handle(MAT_Arc)& anArc);
  
  Standard_EXPORT void SetNeighbour (const MAT_Side aSide, const Handle(MAT_Node)& aNode, const Handle(MAT_Arc)& anArc);




  DEFINE_STANDARD_RTTIEXT(MAT_Arc,Standard_Transient)

protected:




private:


  Standard_Integer arcIndex;
  Standard_Integer geomIndex;
  Handle(MAT_BasicElt) firstElement;
  Handle(MAT_BasicElt) secondElement;
  Handle(MAT_Node) firstNode;
  Handle(MAT_Node) secondNode;
  Standard_Address firstArcLeft;
  Standard_Address firstArcRight;
  Standard_Address secondArcRight;
  Standard_Address secondArcLeft;


};







#endif // _MAT_Arc_HeaderFile
