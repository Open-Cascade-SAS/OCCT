// Created on: 1997-04-17
// Created by: Christophe MARION
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#ifndef No_Exception
#define No_Exception
#endif
#include <HLRBRep_FaceIterator.ixx>

//=======================================================================
//function : FaceIterator
//purpose  : 
//=======================================================================

HLRBRep_FaceIterator::HLRBRep_FaceIterator()
{}

//=======================================================================
//function : InitEdge
//purpose  : 
//=======================================================================

void HLRBRep_FaceIterator::InitEdge(HLRBRep_FaceData& fd)
{
  iWire = 0;
  myWires = fd.Wires();
  nbWires = myWires->NbWires();

  iEdge = 0;
  nbEdges = 0;
  NextEdge();
}

//=======================================================================
//function : NextEdge
//purpose  : 
//=======================================================================

void HLRBRep_FaceIterator::NextEdge()
{
  iEdge++;
  if (iEdge > nbEdges) {
    iWire++;
    if (iWire <= nbWires) {
      iEdge = 1;
      myEdges = myWires->Wire(iWire);
      nbEdges = myEdges->NbEdges();
    }
  }
}
