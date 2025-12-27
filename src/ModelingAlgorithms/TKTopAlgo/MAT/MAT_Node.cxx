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

#include <MAT_Node.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MAT_Node, Standard_Transient)

//=================================================================================================

MAT_Node::MAT_Node(const int GeomIndex,
                   const occ::handle<MAT_Arc>& LinkedArc,
                   const double    Distance)
    : nodeIndex(0),
      geomIndex(GeomIndex),
      distance(Distance)
{
  aLinkedArc = LinkedArc.get();
}

//=================================================================================================

int MAT_Node::GeomIndex() const
{
  return geomIndex;
}

//=================================================================================================

int MAT_Node::Index() const
{
  return nodeIndex;
}

//=================================================================================================

void MAT_Node::LinkedArcs(NCollection_Sequence<occ::handle<MAT_Arc>>& S) const
{
  S.Clear();
  occ::handle<MAT_Node> Me = this;
  occ::handle<MAT_Arc>  LA((MAT_Arc*)aLinkedArc);

  S.Append(LA);

  if (LA->HasNeighbour(Me, MAT_Left))
  {
    occ::handle<MAT_Arc> CA = LA->Neighbour(Me, MAT_Left);
    while (CA != LA)
    {
      S.Append(CA);
      CA = CA->Neighbour(Me, MAT_Left);
    }
  }
}

//=================================================================================================

void MAT_Node::NearElts(NCollection_Sequence<occ::handle<MAT_BasicElt>>& S) const
{
  S.Clear();

  occ::handle<MAT_Node> Me = this;
  occ::handle<MAT_Arc>  LA((MAT_Arc*)aLinkedArc);

  S.Append(LA->FirstElement());
  S.Append(LA->SecondElement());

  if (LA->HasNeighbour(Me, MAT_Left))
  {

    occ::handle<MAT_Arc>  CA   = LA->Neighbour(Me, MAT_Left);
    bool Pair = false;

    //---------------------------------------------------------
    // Recuperation des deux elements separes pour un arc sur
    // deux.
    //---------------------------------------------------------

    while (CA != LA)
    {
      if (Pair)
      {
        S.Append(CA->FirstElement());
        S.Append(CA->SecondElement());
      }
      else
      {
        Pair = true;
      }
      CA = CA->Neighbour(Me, MAT_Left);
    }
  }
}

//=================================================================================================

double MAT_Node::Distance() const
{
  return distance;
}

//=================================================================================================

bool MAT_Node::PendingNode() const
{
  occ::handle<MAT_Node> Me = this;
  return (!((MAT_Arc*)aLinkedArc)->HasNeighbour(Me, MAT_Left));
}

//=================================================================================================

bool MAT_Node::OnBasicElt() const
{
  return (Distance() == 0.0);
}

//=================================================================================================

bool MAT_Node::Infinite() const
{
  return (Distance() == Precision::Infinite());
}

//=================================================================================================

void MAT_Node::SetLinkedArc(const occ::handle<MAT_Arc>& LinkedArc)
{
  aLinkedArc = LinkedArc.get();
}

//=================================================================================================

void MAT_Node::SetIndex(const int anIndex)
{
  nodeIndex = anIndex;
}
