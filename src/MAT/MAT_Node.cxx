// Created on: 1993-05-06
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
#include <MAT_BasicElt.hxx>
#include <MAT_Node.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MAT_Node,Standard_Transient)

//=============================================================================
//function : 
//Purpose  :
//=============================================================================
MAT_Node::MAT_Node(const Standard_Integer     GeomIndex, 
		   const Handle(MAT_Arc)&     LinkedArc, 
		   const Standard_Real        Distance)
     : geomIndex(GeomIndex),
       distance(Distance)
{
  aLinkedArc = LinkedArc.operator->();
}

//=============================================================================
//function : GeomIndex 
//Purpose  :
//=============================================================================
Standard_Integer  MAT_Node::GeomIndex() const
{
  return geomIndex;
}

//=============================================================================
//function : Index
//Purpose  :
//=============================================================================
Standard_Integer  MAT_Node::Index() const
{
  return nodeIndex;
}

//=============================================================================
//function : LinkedArcs
//Purpose  :
//=============================================================================
void MAT_Node::LinkedArcs(MAT_SequenceOfArc& S) const
{
  S.Clear();
  Handle(MAT_Node) Me = this;
  Handle(MAT_Arc)  LA((MAT_Arc*)aLinkedArc);

  S.Append(LA);

  if (LA->HasNeighbour(Me, MAT_Left)) {
    Handle(MAT_Arc)  CA = LA->Neighbour(Me, MAT_Left);
    while (CA != LA) {
      S.Append(CA);
      CA = CA->Neighbour(Me, MAT_Left);
    }
  }
}

//=============================================================================
//function : NearElts 
//Purpose  :
//=============================================================================
void MAT_Node::NearElts(MAT_SequenceOfBasicElt& S) const
{
  S.Clear();

  Handle(MAT_Node) Me = this;
  Handle(MAT_Arc)  LA((MAT_Arc*)aLinkedArc);

  S.Append(LA->FirstElement());
  S.Append(LA->SecondElement());

  if (LA->HasNeighbour(Me, MAT_Left)) {

    Handle(MAT_Arc)  CA = LA->Neighbour(Me, MAT_Left);
    Standard_Boolean Pair = Standard_False;
    
    //---------------------------------------------------------
    // Recuperation des deux elements separes pour un arc sur
    // deux.
    //---------------------------------------------------------
    
    while (CA != LA) {
      if (Pair) {
	S.Append(CA->FirstElement());
	S.Append(CA->SecondElement());
      }
      else {
	Pair = Standard_True;
      }
      CA = CA->Neighbour(Me, MAT_Left);
    }
  }
}
  
//=============================================================================
//function : Distance
//Purpose  :
//=============================================================================
Standard_Real  MAT_Node::Distance() const
{
  return distance;
}


//=============================================================================
//function : PendingNode
//Purpose  :
//=============================================================================
Standard_Boolean  MAT_Node::PendingNode() const
{
  Handle(MAT_Node) Me = this;
  return (!((MAT_Arc*)aLinkedArc)->HasNeighbour(Me,MAT_Left));
}

//=============================================================================
//function : NodeOnFig
//Purpose  :
//=============================================================================
Standard_Boolean  MAT_Node::OnBasicElt() const
{
  return (Distance() == 0.0);
}

//=============================================================================
//function : NodeInfinite
//Purpose  :
//=============================================================================
Standard_Boolean  MAT_Node::Infinite() const
{
  return (Distance() == Precision::Infinite());
}

//=============================================================================
//function : SetLinkedArcs
//Purpose  :
//=============================================================================
void MAT_Node::SetLinkedArc (const Handle(MAT_Arc)& LinkedArc)
{
  aLinkedArc = LinkedArc.operator->();
}

//=============================================================================
//function : SetIndex
//Purpose  :
//=============================================================================
void MAT_Node::SetIndex (const Standard_Integer anIndex)
{
  nodeIndex = anIndex;
}




