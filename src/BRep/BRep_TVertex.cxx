// Created on: 1992-08-25
// Created by: Modelistation
// Copyright (c) 1992-1999 Matra Datavision
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



#include <BRep_TVertex.ixx>
#include <TopAbs.hxx>

//=======================================================================
//function : BRep_TVertex
//purpose  : 
//=======================================================================

BRep_TVertex::BRep_TVertex() :
       TopoDS_TVertex(),
       myTolerance(RealEpsilon())
{
}

//=======================================================================
//function : EmptyCopy
//purpose  : 
//=======================================================================

Handle(TopoDS_TShape) BRep_TVertex::EmptyCopy() const
{
  Handle(BRep_TVertex) TV = 
    new BRep_TVertex();
  TV->Pnt(myPnt);
  TV->Tolerance(myTolerance);
  return TV;
}

